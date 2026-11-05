// B1 - Racha de sensores
// Reglas descubiertas con la caja negra:
//  * Los sensores forman un CIRCULO: despues del sensor N sigue el 1 (una racha puede dar la vuelta).
//  * Una racha (de 1 a N sensores) es estable si max - min de sus lecturas conocidas <= K (no estricto).
//  * '?' es comodin: cuenta en la longitud y no restringe (una racha solo de '?' es estable).
//  * Respuesta: longitud maxima L y la menor posicion inicial P (1..N).
// Algoritmo: dos punteros sobre el circulo con colas monotonas de max y min. O(N) por caso.
// Aritmetica exacta: lecturas y K como enteros escalados x10^6 (sin double).
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using ull = unsigned long long;

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
    while (c != '-' && (c < '0' || c > '9')) c = leer_char();
    bool neg = (c == '-');
    if (neg) c = leer_char();
    ll r = 0;
    while (c >= '0' && c <= '9') { r = r * 10 + (c - '0'); c = leer_char(); }
    return neg ? -r : r;
}

// Lee un decimal (hasta 6 decimales) como valor absoluto escalado x10^6 y su signo.
// Devuelve false si la lectura es '?'.
static inline bool leer_decimal(bool &neg, ull &mag) {
    int c = leer_char();
    while (c == ' ' || c == '\n' || c == '\r' || c == '\t') c = leer_char();
    if (c == '?') return false;
    neg = false;
    if (c == '-' || c == '+') { neg = (c == '-'); c = leer_char(); }
    ull ent = 0, fr = 0;
    int nd = 0;
    while (c >= '0' && c <= '9') { ent = ent * 10 + (c - '0'); c = leer_char(); }
    if (c == '.') {
        c = leer_char();
        while (c >= '0' && c <= '9') {
            if (nd < 6) { fr = fr * 10 + (c - '0'); ++nd; }
            c = leer_char();
        }
    }
    while (nd < 6) { fr *= 10; ++nd; }
    mag = ent * 1000000ULL + fr;   // |lectura| <= 9e18 y K <= 1.8e19: caben en ull
    return true;
}

int main() {
    int T = (int)leer();
    vector<ll> v;
    vector<char> ok;
    vector<int> qmx, qmn;   // colas monotonas con indices del recorrido doble [0, 2N)
    string out;
    out.reserve((size_t)T * 16);
    char tmp[32];

    while (T--) {
        int n = (int)leer();
        bool kneg;
        ull K;
        leer_decimal(kneg, K);
        v.assign(n, 0);
        ok.assign(n, 0);
        if ((int)qmx.size() < 2 * n) { qmx.resize(2 * n); qmn.resize(2 * n); }
        for (int i = 0; i < n; ++i) {
            bool neg;
            ull m;
            if (leer_decimal(neg, m)) { ok[i] = 1; v[i] = neg ? -(ll)m : (ll)m; }
        }

        int hmx = 0, tmx = 0, hmn = 0, tmn = 0;
        int j = 0, bestL = 0, bestP = 1;   // ventana actual: [i, j) en indices dobles
        for (int i = 0; i < n; ++i) {
            while (hmx < tmx && qmx[hmx] < i) ++hmx;
            while (hmn < tmn && qmn[hmn] < i) ++hmn;
            while (j < i + n) {
                int r = j < n ? j : j - n;
                if (ok[r]) {
                    ll x = v[r];
                    ll mx = x, mn = x;
                    if (hmx < tmx) { int q = qmx[hmx]; ll y = v[q < n ? q : q - n]; if (y > mx) mx = y; }
                    if (hmn < tmn) { int q = qmn[hmn]; ll y = v[q < n ? q : q - n]; if (y < mn) mn = y; }
                    if ((ull)mx - (ull)mn > K) break;   // resta sin signo: diferencia real en [0, 1.8e19]
                    while (hmx < tmx) { int q = qmx[tmx - 1]; if (v[q < n ? q : q - n] <= x) --tmx; else break; }
                    qmx[tmx++] = j;
                    while (hmn < tmn) { int q = qmn[tmn - 1]; if (v[q < n ? q : q - n] >= x) --tmn; else break; }
                    qmn[tmn++] = j;
                }
                ++j;
            }
            if (j - i > bestL) { bestL = j - i; bestP = i + 1; }
        }
        int len = snprintf(tmp, sizeof(tmp), "%d %d\n", bestL, bestP);
        out.append(tmp, len);
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
