// B2 · Cola de impresión
// Reglas descubiertas con la caja negra:
//  - Empate de prioridad: sale primero el que entró antes (FIFO).
//  - BOOST suma x y además manda el trabajo al final de la fila entre los empatados
//    (como si acabara de llegar); conserva el envejecimiento acumulado.
//  - Cada PRINT suma +1 a la prioridad de todos los trabajos que siguen pendientes.
//    ADD, CANCEL y BOOST no envejecen a nadie.
//  - Aritmética exacta: p + suma de x llega a 10^24 -> __int128.
// Clave fija por trabajo: p + sum(x) - (PRINTs antes de su ADD). Ordenar por ella equivale
// a ordenar por la prioridad envejecida. Heap con borrado perezoso: O(Q log Q).
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128;

static char BUF[1 << 16];
static size_t BUF_LEN = 0, BUF_POS = 0;
static inline int leer_char() {
    if (BUF_POS == BUF_LEN) {
        BUF_LEN = fread(BUF, 1, sizeof(BUF), stdin);
        BUF_POS = 0;
        if (BUF_LEN == 0) return -1;
    }
    return BUF[BUF_POS++];
}
static inline ll leer() {
    int c = leer_char();
    while (c != -1 && c != '-' && (c < '0' || c > '9')) c = leer_char();
    bool neg = (c == '-');
    if (neg) c = leer_char();
    ll r = 0;
    while (c >= '0' && c <= '9') { r = r * 10 + (c - '0'); c = leer_char(); }
    return neg ? -r : r;
}
// Lee la palabra de la orden y devuelve su primera letra (A, B, C o P).
static inline int leer_orden() {
    int c = leer_char();
    while (c != -1 && (c < 'A' || c > 'Z')) c = leer_char();
    int primera = c;
    while (c >= 'A' && c <= 'Z') c = leer_char();
    return primera;
}

// Salida con búfer propio.
static char OUT[1 << 16];
static size_t OUT_LEN = 0;
static inline void flush_out() { fwrite(OUT, 1, OUT_LEN, stdout); OUT_LEN = 0; }
static inline void put_str(const char* s, size_t n) {
    if (OUT_LEN + n > sizeof(OUT)) flush_out();
    memcpy(OUT + OUT_LEN, s, n);
    OUT_LEN += n;
}
static inline void put_int(int v) {
    char t[12];
    int n = 0;
    do { t[n++] = char('0' + v % 10); v /= 10; } while (v);
    if (OUT_LEN + n > sizeof(OUT)) flush_out();
    while (n) OUT[OUT_LEN++] = t[--n];
}
static inline void put_char(char c) {
    if (OUT_LEN == sizeof(OUT)) flush_out();
    OUT[OUT_LEN++] = c;
}

struct Entrada {
    i128 clave;  // p + sum(x) - PRINTs antes del ADD
    int sello;   // número de orden del último ADD/BOOST (desempate: menor primero)
    int id;
};
// Mayor clave primero; con empate, menor sello primero.
struct Menor {
    bool operator()(const Entrada& a, const Entrada& b) const {
        if (a.clave != b.clave) return a.clave < b.clave;
        return a.sello > b.sello;
    }
};

const int MAXID = 2000000;
static i128 clave_de[MAXID + 1];
static int sello_de[MAXID + 1];  // 0 = no pendiente

int main() {
    int q = (int)leer();
    vector<Entrada> almacen;
    almacen.reserve(min(q, 1000000) + 1);
    priority_queue<Entrada, vector<Entrada>, Menor> heap(Menor(), std::move(almacen));
    ll prints = 0;  // PRINTs ejecutados hasta ahora

    for (int t = 1; t <= q; t++) {
        int op = leer_orden();
        if (op == 'A') {
            int id = (int)leer();
            ll p = leer();
            clave_de[id] = (i128)p - prints;
            sello_de[id] = t;
            heap.push({clave_de[id], t, id});
        } else if (op == 'B') {
            int id = (int)leer();
            ll x = leer();
            if (sello_de[id]) {
                clave_de[id] += x;
                sello_de[id] = t;  // pasa al final de su grupo de empate
                heap.push({clave_de[id], t, id});
                put_str("OK\n", 3);
            } else {
                put_str("NO\n", 3);
            }
        } else if (op == 'C') {
            int id = (int)leer();
            if (sello_de[id]) {
                sello_de[id] = 0;
                put_str("OK\n", 3);
            } else {
                put_str("NO\n", 3);
            }
        } else {  // PRINT
            ll k = leer();
            ll impresos = 0;
            while (impresos < k && !heap.empty()) {
                Entrada e = heap.top();
                heap.pop();
                if (sello_de[e.id] != e.sello) continue;  // entrada vieja
                sello_de[e.id] = 0;
                if (impresos) put_char(' ');
                put_int(e.id);
                impresos++;
            }
            if (impresos) put_char('\n');
            else put_str("VACIA\n", 6);
            prints++;  // los que quedan envejecen +1
        }
    }
    flush_out();
    return 0;
}
