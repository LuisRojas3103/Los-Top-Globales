#include <bits/stdc++.h>
using namespace std;

using ll = long long;

#ifdef LOCAL
#define dbg(x) cerr << #x << " = " << (x) << '\n'
#else
#define dbg(x)
#endif

// Problema BF "Ventas masivas". Reglas descubiertas con la caja negra:
//  - Línea válida: exactamente "AAAA-MM-DD,id_venta,producto_id,cantidad" (se tolera UN '\r' final).
//    Fecha real del calendario (bisiestos) con año 2000..2099; id 1..4 000 000; producto 1..100 000;
//    cantidad != 0 y |cantidad| <= 10 000. Números solo con dígitos (cantidad con '-' opcional),
//    de 1 a 12 dígitos (ceros a la izquierda permitidos). Cualquier otro carácter o espacio la invalida.
//  - Repetidas: cuenta la PRIMERA línea válida de cada id_venta, aunque su fecha caiga fuera de [D1, D2].
//  - Se reportan solo productos con total > 0 (máx. 3, mayor total; empate: menor id). Si no hay: SIN DATOS.
//  - Se leen exactamente N líneas (las vacías cuentan); si el archivo trae menos, se procesa lo que haya.

static const int MAX_ID = 4000000, MAX_PROD = 100000, MAX_CANT = 10000, MAX_DIG = 12;
static const int CAP = 64;  // la línea válida más larga mide 51 caracteres

// Lectura en streaming por bloques de 64 KB: la entrada (~90 MB) no cabe en 64 MB de RAM.
static char BUF[1 << 16];
static size_t BLEN = 0, BPOS = 0;

static inline bool rellenar() {
    BLEN = fread(BUF, 1, sizeof(BUF), stdin);
    BPOS = 0;
    return BLEN > 0;
}

static inline int leer_char() {
    if (BPOS == BLEN && !rellenar()) return -1;
    return (unsigned char)BUF[BPOS++];
}

// Copia en L hasta CAP bytes de la siguiente línea (sin '\n') y devuelve su longitud real,
// acotada a CAP + 1 (más larga = inválida). Devuelve -1 si ya no quedan datos.
static int leer_linea(char* L) {
    if (BPOS == BLEN && !rellenar()) return -1;
    size_t n = 0;
    while (true) {
        const char* ini = BUF + BPOS;
        size_t resto = BLEN - BPOS;
        const char* nl = (const char*)memchr(ini, '\n', resto);
        size_t k = nl ? (size_t)(nl - ini) : resto;
        if (n < (size_t)CAP) memcpy(L + n, ini, min(k, (size_t)CAP - n));
        n = min((size_t)CAP + 1, n + k);
        if (nl) { BPOS += k + 1; return (int)n; }
        if (!rellenar()) return (int)n;  // última línea sin '\n'
    }
}

// Lee un token del encabezado (salta espacios). Devuelve el carácter que lo terminó (-1 = fin).
static int leer_token(string& s) {
    s.clear();
    int c = leer_char();
    while (c == ' ' || c == '\t' || c == '\r' || c == '\n') c = leer_char();
    while (c != -1 && c != ' ' && c != '\t' && c != '\r' && c != '\n') { s += (char)c; c = leer_char(); }
    return c;
}

static int a_fecha(const string& s) {  // "AAAA-MM-DD" -> AAAAMMDD
    int v = 0, k = 0;
    for (char c : s)
        if (c >= '0' && c <= '9' && k < 8) { v = v * 10 + (c - '0'); ++k; }
    return v;
}

static inline int dias_mes(int y, int m) {
    static const int D[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return D[m];
}

// Número sin signo de 1..12 dígitos desde L[i] hasta la siguiente coma o hasta `fin`.
static inline bool numero(const char* L, int& i, int fin, ll& v) {
    int ini = i;
    v = 0;
    while (i < fin && L[i] != ',') {
        unsigned d = (unsigned char)L[i] - '0';
        if (d > 9 || i - ini >= MAX_DIG) return false;
        v = v * 10 + d;
        ++i;
    }
    return i > ini;
}

static bool validar(const char* L, int n, int& fecha, int& id, int& prod, int& cant) {
    if (n > CAP) return false;
    if (n > 0 && L[n - 1] == '\r') --n;
    if (n < 16 || L[4] != '-' || L[7] != '-' || L[10] != ',') return false;
    static const int POS[8] = {0, 1, 2, 3, 5, 6, 8, 9};
    int dg[8];
    for (int k = 0; k < 8; ++k) {
        unsigned d = (unsigned char)L[POS[k]] - '0';
        if (d > 9) return false;
        dg[k] = (int)d;
    }
    int y = dg[0] * 1000 + dg[1] * 100 + dg[2] * 10 + dg[3], m = dg[4] * 10 + dg[5], d = dg[6] * 10 + dg[7];
    if (y < 2000 || y > 2099 || m < 1 || m > 12 || d < 1 || d > dias_mes(y, m)) return false;
    fecha = y * 10000 + m * 100 + d;

    int i = 11;
    ll v;
    if (!numero(L, i, n, v) || i >= n || v < 1 || v > MAX_ID) return false;    // i queda en la coma
    id = (int)v;
    ++i;
    if (!numero(L, i, n, v) || i >= n || v < 1 || v > MAX_PROD) return false;
    prod = (int)v;
    ++i;
    bool neg = (i < n && L[i] == '-');
    if (neg) ++i;
    if (!numero(L, i, n, v) || i != n || v < 1 || v > MAX_CANT) return false;  // sin 4.ª coma; 0 inválido
    cant = neg ? -(int)v : (int)v;
    return true;
}

static uint64_t visto[(MAX_ID >> 6) + 1];  // bitset de id_venta ya usados (~0.5 MB)
static ll total[MAX_PROD + 1];               // venta neta por producto (~0.8 MB)

int main() {
    string s1, s2, sn;
    leer_token(s1);
    leer_token(s2);
    int t = leer_token(sn);
    if (sn.empty()) return 0;                      // sin línea N la caja no imprime nada
    while (t != '\n' && t != -1) t = leer_char();  // resto de la línea de N
    const int D1 = a_fecha(s1), D2 = a_fecha(s2);
    ll N = 0;
    for (char c : sn)
        if (c >= '0' && c <= '9' && N < (ll)1e15) N = N * 10 + (c - '0');

    static char L[CAP + 1];
    for (ll k = 0; k < N; ++k) {
        int n = leer_linea(L);
        if (n < 0) break;
        int f, id, p, q;
        if (!validar(L, n, f, id, p, q)) continue;
        uint64_t bit = 1ULL << (id & 63);
        if (visto[id >> 6] & bit) continue;  // repetida: ya contó la primera válida
        visto[id >> 6] |= bit;
        if (f >= D1 && f <= D2) total[p] += q;
    }

    int top[3], cnt = 0;  // orden: total desc, producto asc (p crece, así que el empate conserva al menor)
    for (int p = 1; p <= MAX_PROD; ++p) {
        ll v = total[p];
        if (v <= 0 || (cnt == 3 && v <= total[top[2]])) continue;
        int j = (cnt < 3) ? cnt++ : 2;
        while (j > 0 && total[top[j - 1]] < v) { top[j] = top[j - 1]; --j; }
        top[j] = p;
    }
    if (cnt == 0) printf("SIN DATOS\n");
    for (int j = 0; j < cnt; ++j) printf("%d %lld\n", top[j], total[top[j]]);
    return 0;
}
