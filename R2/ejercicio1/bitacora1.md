# Bitácora — Ronda 2 · Ejercicio 1 (BF «Ventas masivas»)

**Participante:** Luis Rojas · **Categoría:** Básica · **IA:** Claude Code (claude-opus-5-5) · **Fecha:** 2026-11-05

Notas de lo que probamos con la caja negra y lo que descubrimos. La conversación completa está en `conversacion1.md`.
Como ahí las salidas largas de los comandos aparecen recortadas, al final de esta bitácora va el registro **completo** de los experimentos.

## Reglas descubiertas con la caja

| Pregunta del enunciado | Lo que hace la caja |
|---|---|
| **Qué hace inválida una línea** | Debe ser exactamente `AAAA-MM-DD,id_venta,producto_id,cantidad`. La invalida cualquier espacio o tabulador (al inicio, al final o entre campos), un `+`, un decimal, campos de más o de menos, un campo vacío o otro separador. Una línea vacía es inválida pero **cuenta dentro de N**. Se tolera **un solo `\r` final** (CRLF). |
| Fecha | Debe existir en el calendario (bisiestos gregorianos: 2026-02-29 y 2026-04-31 se rechazan) y el año debe estar entre **2000 y 2099**. |
| id_venta / producto_id | Solo dígitos, con valores 1–4 000 000 y 1–100 000. Se permiten ceros a la izquierda, con un máximo de **12 dígitos**. |
| cantidad | `-` opcional y luego de 1 a 12 dígitos. El valor debe ser **distinto de 0** y estar entre **−10 000 y 10 000**. |
| **Cuál repetida cuenta** | La **primera línea válida** de cada `id_venta` en el orden del archivo, sin importar su fecha ni su cantidad. Las líneas inválidas no ocupan el id. Una línea válida **fuera de [D1, D2] sí lo ocupa**, aunque no suma. `01` y `1` son el mismo id. |
| **Qué productos se reportan** | Solo los de **total neto > 0**. Hasta 3, de mayor a menor total; en empate va primero el `producto_id` menor. Si no hay ninguno: `SIN DATOS`. |
| Encabezado y N | [D1, D2] incluye ambos extremos; si D1 > D2, la respuesta es `SIN DATOS`. Se leen exactamente N líneas: las que sobran se ignoran y, si faltan, se procesa lo que haya. |

## Cómo se probó
- La caja solo acepta **N ≤ 300** (lote `limite`), así que todos los experimentos son entradas pequeñas hechas a mano con `exp.py`.
- **Validez de una línea:** se manda sola con cantidad positiva. Si la caja reporta el producto, la línea es válida; si responde `SIN DATOS`, es inválida.
- **Cantidades negativas:** como los totales negativos no se reportan, se suma primero una línea positiva conocida. En el lote `negativas`, las filas con −10 000 o más dan `SIN DATOS` porque la línea positiva de apoyo (> 10 000) también era inválida. El lote `negativas2` repite esa prueba bien y confirma el límite: −10 000 es válida y −10 001 no.
- **Repetidas:** pares con el mismo id donde la primera aparición tiene menor o mayor fecha y cantidad que la segunda, o es inválida, o queda fuera de rango.

## Validación de la solución (`codigo1.cpp`)

| Prueba | Resultado |
|---|---|
| Ejemplos 1 y 2 del enunciado + caso de reglas (salida tomada de la caja) | 3/3 AC |
| Estrés contra la caja con un generador aleatorio: 34 tipos de línea inválida, muchas repetidas, empates, CRLF, N distinto del número de líneas | **3000/3000** casos iguales |
| Referencia independiente en Python (`bruta.py`) contra la caja | 1000/1000 casos iguales |
| 6 casos máximos con N = 3 000 000 (hasta 148 MB): todo válido, ceros a la izquierda, mezcla con CRLF, ids únicos con totales de 10¹⁰, todo repetido, todo inválido | 6/6 iguales a `bruta.py` · **CPU máx. 0.17 s de 16 s · memoria máx. 2.9 MB de 64 MB** |

Algoritmo: lectura en streaming por bloques de 64 KB (90 MB no caben en 64 MB), validación manual de cada línea,
bitset de 4 M ids ya usados (0.5 MB), totales `long long` por producto (0.8 MB) y top 3 en un solo recorrido. Complejidad O(bytes).

## Registro completo de experimentos con la caja

Salida de `python3 exp.py`, que corre todos los lotes y vuelve a ejecutar la caja. Se regeneró completo a las 13:05.
Cada fila muestra el experimento, las líneas de datos (`¦` separa líneas) y la respuesta de la caja
(encabezado por defecto `2026-01-01 2026-12-31`; N = número de líneas, salvo que la fila diga otra cosa).

```text

=== lote limite
N = 100     -> 1 45 / 2 45 / 3 44
N = 1000    -> ERROR: la caja negra solo acepta N <= 300
N = 2000    -> ERROR: la caja negra solo acepta N <= 300
N = 5000    -> ERROR: la caja negra solo acepta N <= 300
N = 10000   -> ERROR: la caja negra solo acepta N <= 300
N = 100000  -> ERROR: la caja negra solo acepta N <= 300

=== lote formato
control válido                     | 2026-05-10,1,7,10                                                                          | 7 10
espacio al final                   | 2026-05-10,1,7,10                                                                          | SIN DATOS
espacio al inicio                  |  2026-05-10,1,7,10                                                                         | SIN DATOS
espacio tras coma                  | 2026-05-10, 1,7,10                                                                         | SIN DATOS
espacio antes de coma              | 2026-05-10 ,1,7,10                                                                         | SIN DATOS
espacio dentro de cantidad         | 2026-05-10,1,7,1 0                                                                         | SIN DATOS
tab al final                       | 2026-05-10,1,7,10\t                                                                        | SIN DATOS
CRLF (\r al final)                 | 2026-05-10,1,7,10\r                                                                        | 7 10
coma final extra                   | 2026-05-10,1,7,10,                                                                         | SIN DATOS
5 campos                           | 2026-05-10,1,7,10,1                                                                        | SIN DATOS
3 campos                           | 2026-05-10,1,7                                                                             | SIN DATOS
campo id vacío                     | 2026-05-10,,7,10                                                                           | SIN DATOS
campo producto vacío               | 2026-05-10,1,,10                                                                           | SIN DATOS
cantidad vacía                     | 2026-05-10,1,7,                                                                            | SIN DATOS
separador ;                        | 2026-05-10;1;7;10                                                                          | SIN DATOS
línea vacía                        |                                                                                            | SIN DATOS
solo espacios                      |                                                                                            | SIN DATOS

=== lote numeros
id 0                               | 2026-05-10,0,7,10                                                                          | SIN DATOS
id 1                               | 2026-05-10,1,7,10                                                                          | 7 10
id 4000000                         | 2026-05-10,4000000,7,10                                                                    | 7 10
id 4000001                         | 2026-05-10,4000001,7,10                                                                    | SIN DATOS
id negativo                        | 2026-05-10,-3,7,10                                                                         | SIN DATOS
id con +                           | 2026-05-10,+3,7,10                                                                         | SIN DATOS
id con cero a la izq.              | 2026-05-10,03,7,10                                                                         | 7 10
id decimal                         | 2026-05-10,3.0,7,10                                                                        | SIN DATOS
producto 0                         | 2026-05-10,1,0,10                                                                          | SIN DATOS
producto 1                         | 2026-05-10,1,1,10                                                                          | 1 10
producto 100000                    | 2026-05-10,1,100000,10                                                                     | 100000 10
producto 100001                    | 2026-05-10,1,100001,10                                                                     | SIN DATOS
producto negativo                  | 2026-05-10,1,-7,10                                                                         | SIN DATOS
producto con +                     | 2026-05-10,1,+7,10                                                                         | SIN DATOS
producto con cero a la izq.        | 2026-05-10,1,07,10                                                                         | 7 10
producto con letra                 | 2026-05-10,1,7a,10                                                                         | SIN DATOS
cantidad con +                     | 2026-05-10,1,7,+10                                                                         | SIN DATOS
cantidad con cero a la izq.        | 2026-05-10,1,7,010                                                                         | 7 10
cantidad decimal                   | 2026-05-10,1,7,10.0                                                                        | SIN DATOS
cantidad '-' sola                  | 2026-05-10,1,7,-                                                                           | SIN DATOS
cantidad '--10'                    | 2026-05-10,1,7,--10                                                                        | SIN DATOS
cantidad '10-'                     | 2026-05-10,1,7,10-                                                                         | SIN DATOS
cantidad con letra                 | 2026-05-10,1,7,1x                                                                          | SIN DATOS
cantidad 0                         | 2026-05-10,1,7,0                                                                           | SIN DATOS
cantidad -0                        | 2026-05-10,1,7,-0                                                                          | SIN DATOS
cantidad 2147483648                | 2026-05-10,1,7,2147483648                                                                  | SIN DATOS
cantidad 10^12                     | 2026-05-10,1,7,1000000000000                                                               | SIN DATOS
cantidad 10^18                     | 2026-05-10,1,7,1000000000000000000                                                         | SIN DATOS
cantidad 9223372036854775807       | 2026-05-10,1,7,9223372036854775807                                                         | SIN DATOS
cantidad 9223372036854775808       | 2026-05-10,1,7,9223372036854775808                                                         | SIN DATOS
cantidad 10^20                     | 2026-05-10,1,7,100000000000000000000                                                       | SIN DATOS

=== lote fechas
control 2026-05-10                 | 2026-05-10,1,7,10                                                                          | 7 10
2024-02-29 (bisiesto)              | 2024-02-29,1,7,10                                                                          | 7 10
2026-02-29 (no bisiesto)           | 2026-02-29,1,7,10                                                                          | SIN DATOS
2000-02-29 (bisiesto /400)         | 2000-02-29,1,7,10                                                                          | 7 10
1900-02-29 (no bisiesto /100)      | 1900-02-29,1,7,10                                                                          | SIN DATOS
2026-04-31                         | 2026-04-31,1,7,10                                                                          | SIN DATOS
2026-12-31                         | 2026-12-31,1,7,10                                                                          | 7 10
2026-13-01 (mes 13)                | 2026-13-01,1,7,10                                                                          | SIN DATOS
2026-00-10 (mes 0)                 | 2026-00-10,1,7,10                                                                          | SIN DATOS
2026-05-00 (día 0)                 | 2026-05-00,1,7,10                                                                          | SIN DATOS
2026-05-32 (día 32)                | 2026-05-32,1,7,10                                                                          | SIN DATOS
2026-5-10 (sin ceros)              | 2026-5-10,1,7,10                                                                           | SIN DATOS
2026-05-1 (día 1 dígito)           | 2026-05-1,1,7,10                                                                           | SIN DATOS
26-05-10 (año 2 dígitos)           | 26-05-10,1,7,10                                                                            | SIN DATOS
2026/05/10                         | 2026/05/10,1,7,10                                                                          | SIN DATOS
20260510                           | 20260510,1,7,10                                                                            | SIN DATOS
0000-05-10 (año 0)                 | 0000-05-10,1,7,10                                                                          | SIN DATOS
0001-01-01                         | 0001-01-01,1,7,10                                                                          | SIN DATOS
1899-12-31                         | 1899-12-31,1,7,10                                                                          | SIN DATOS
1970-01-01                         | 1970-01-01,1,7,10                                                                          | SIN DATOS
9999-12-31                         | 9999-12-31,1,7,10                                                                          | SIN DATOS
10000-01-01 (año 5 dígitos)        | 10000-01-01,1,7,10                                                                         | SIN DATOS
2026-05-10x                        | 2026-05-10x,1,7,10                                                                         | SIN DATOS
+2026-05-10                        | +2026-05-10,1,7,10                                                                         | SIN DATOS

=== lote cantidades
cantidad 1                         | 2026-05-10,1,7,1                                                                           | 7 1
cantidad 999                       | 2026-05-10,1,7,999                                                                         | 7 999
cantidad 1000                      | 2026-05-10,1,7,1000                                                                        | 7 1000
cantidad 1001                      | 2026-05-10,1,7,1001                                                                        | 7 1001
cantidad 9999                      | 2026-05-10,1,7,9999                                                                        | 7 9999
cantidad 10000                     | 2026-05-10,1,7,10000                                                                       | 7 10000
cantidad 10001                     | 2026-05-10,1,7,10001                                                                       | SIN DATOS
cantidad 99999                     | 2026-05-10,1,7,99999                                                                       | SIN DATOS
cantidad 100000                    | 2026-05-10,1,7,100000                                                                      | SIN DATOS
cantidad 100001                    | 2026-05-10,1,7,100001                                                                      | SIN DATOS
cantidad 999999                    | 2026-05-10,1,7,999999                                                                      | SIN DATOS
cantidad 1000000                   | 2026-05-10,1,7,1000000                                                                     | SIN DATOS
cantidad 1000001                   | 2026-05-10,1,7,1000001                                                                     | SIN DATOS
cantidad 10000000                  | 2026-05-10,1,7,10000000                                                                    | SIN DATOS
cantidad 100000000                 | 2026-05-10,1,7,100000000                                                                   | SIN DATOS
cantidad 1000000000                | 2026-05-10,1,7,1000000000                                                                  | SIN DATOS
cantidad 2147483647                | 2026-05-10,1,7,2147483647                                                                  | SIN DATOS
cantidad 0010 (ceros)              | 2026-05-10,1,7,0010                                                                        | 7 10
cantidad 20 dígitos = 10           | 2026-05-10,1,7,00000000000000000010                                                        | SIN DATOS

=== lote negativas
+6 y -1                            | 2026-05-10,1,7,6 ¦ 2026-05-10,2,7,-1                                                       | 7 5
+1005 y -1000                      | 2026-05-10,1,7,1005 ¦ 2026-05-10,2,7,-1000                                                 | 7 5
+1006 y -1001                      | 2026-05-10,1,7,1006 ¦ 2026-05-10,2,7,-1001                                                 | 7 5
+10005 y -10000                    | 2026-05-10,1,7,10005 ¦ 2026-05-10,2,7,-10000                                               | SIN DATOS
+100005 y -100000                  | 2026-05-10,1,7,100005 ¦ 2026-05-10,2,7,-100000                                             | SIN DATOS
+1000005 y -1000000                | 2026-05-10,1,7,1000005 ¦ 2026-05-10,2,7,-1000000                                           | SIN DATOS
+5 y -010 (cero izq.)              | 2026-05-10,1,7,15 ¦ 2026-05-10,2,7,-010                                                    | 7 5

=== lote anios
año 1000                           | 1000-06-15,1,7,10                                                                          | SIN DATOS
año 1899                           | 1899-06-15,1,7,10                                                                          | SIN DATOS
año 1900                           | 1900-06-15,1,7,10                                                                          | SIN DATOS
año 1969                           | 1969-06-15,1,7,10                                                                          | SIN DATOS
año 1970                           | 1970-06-15,1,7,10                                                                          | SIN DATOS
año 1989                           | 1989-06-15,1,7,10                                                                          | SIN DATOS
año 1990                           | 1990-06-15,1,7,10                                                                          | SIN DATOS
año 1999                           | 1999-06-15,1,7,10                                                                          | SIN DATOS
año 2000                           | 2000-06-15,1,7,10                                                                          | 7 10
año 2001                           | 2001-06-15,1,7,10                                                                          | 7 10
año 2025                           | 2025-06-15,1,7,10                                                                          | 7 10
año 2030                           | 2030-06-15,1,7,10                                                                          | 7 10
año 2049                           | 2049-06-15,1,7,10                                                                          | 7 10
año 2050                           | 2050-06-15,1,7,10                                                                          | 7 10
año 2069                           | 2069-06-15,1,7,10                                                                          | 7 10
año 2070                           | 2070-06-15,1,7,10                                                                          | 7 10
año 2099                           | 2099-06-15,1,7,10                                                                          | 7 10
año 2100                           | 2100-06-15,1,7,10                                                                          | SIN DATOS
año 2101                           | 2101-06-15,1,7,10                                                                          | SIN DATOS
año 2199                           | 2199-06-15,1,7,10                                                                          | SIN DATOS
año 2999                           | 2999-06-15,1,7,10                                                                          | SIN DATOS
año 3000                           | 3000-06-15,1,7,10                                                                          | SIN DATOS

=== lote retornos
todo con CRLF                      | 2026-01-01 2026-12-31\r\n2\r\n2026-05-10,1,7,10\r\n2026-05-11,2,7,20\r\n                   | 7 30
doble CR al final                  | 2026-05-10,1,7,10\r\r                                                                      | SIN DATOS
CR en medio de la cantidad         | 2026-05-10,1,7,1\r0                                                                        | SIN DATOS
CR solo como separador, N=2        | 2026-01-01 2026-12-31\n2\n2026-05-10,1,7,10\r2026-05-11,2,7,20\n                           | SIN DATOS
ultima linea sin salto             | 2026-01-01 2026-12-31\n2\n2026-05-10,1,7,10\n2026-05-11,2,7,20                             | 7 30

=== lote repetidas
1ª fecha menor, cant. menor        | 2026-05-01,1,1,5 ¦ 2026-05-20,1,2,7                                                        | 1 5
1ª fecha mayor, cant. menor        | 2026-05-20,1,1,5 ¦ 2026-05-01,1,2,7                                                        | 1 5
1ª fecha menor, cant. mayor        | 2026-05-01,1,1,9 ¦ 2026-05-20,1,2,3                                                        | 1 9
misma fecha                        | 2026-05-10,1,1,5 ¦ 2026-05-10,1,2,7                                                        | 1 5
tres apariciones                   | 2026-05-10,1,1,5 ¦ 2026-05-11,1,2,7 ¦ 2026-05-12,1,3,9                                     | 1 5
1ª inválida (5 campos)             | 2026-05-01,1,1,5,9 ¦ 2026-05-02,1,2,7                                                      | 2 7
1ª inválida (fecha 02-30)          | 2026-02-30,1,1,5 ¦ 2026-05-02,1,2,7                                                        | 2 7
1ª inválida (cantidad 20000)       | 2026-05-01,1,1,20000 ¦ 2026-05-02,1,2,7                                                    | 2 7
1ª inválida (producto 0)           | 2026-05-01,1,0,5 ¦ 2026-05-02,1,2,7                                                        | 2 7
1ª inválida (año 1999)             | 1999-05-01,1,1,5 ¦ 2026-05-02,1,2,7                                                        | 2 7
1ª inválida (espacio final)        | 2026-05-01,1,1,5  ¦ 2026-05-02,1,2,7                                                       | 2 7
1ª válida FUERA de [D1,D2]         | 2025-05-01,1,1,5 ¦ 2026-05-02,1,2,7                                                        | SIN DATOS
1ª dentro, 2ª fuera de [D1,D2]     | 2026-05-02,1,1,5 ¦ 2025-05-01,1,2,7                                                        | 1 5
1ª con cantidad 0                  | 2026-05-01,1,1,0 ¦ 2026-05-02,1,2,7                                                        | 2 7
1ª devolución (-5)                 | 2026-05-01,1,1,-5 ¦ 2026-05-02,1,2,7                                                       | SIN DATOS
id '01' y luego '1'                | 2026-05-01,01,1,5 ¦ 2026-05-02,1,2,7                                                       | 1 5

=== lote reporte
solo negativo                      | 2026-05-01,1,7,-5                                                                          | SIN DATOS
total cero (+5 -5)                 | 2026-05-01,1,7,5 ¦ 2026-05-02,2,7,-5                                                       | SIN DATOS
positivo + negativo                | 2026-05-01,1,7,5 ¦ 2026-05-02,2,8,-5                                                       | 7 5
positivo, cero, negativo           | 2026-05-01,1,7,5 ¦ 2026-05-01,2,8,3 ¦ 2026-05-01,3,8,-3 ¦ 2026-05-01,4,9,-2                | 7 5
2 positivos + 1 negativo           | 2026-05-01,1,7,5 ¦ 2026-05-01,2,8,3 ¦ 2026-05-01,3,9,-2                                    | 7 5 / 8 3
empates (prod 50,3,7,9,1)          | 2026-05-01,1,50,10 ¦ 2026-05-01,2,3,10 ¦ 2026-05-01,3,7,20 ¦ 2026-05-01,4,9,5 ¦ 2026-05-01 | 7 20 / 1 10 / 3 10
neg. mayor que otros neg.          | 2026-05-01,1,7,-1 ¦ 2026-05-01,2,8,-9 ¦ 2026-05-01,3,9,-4                                  | SIN DATOS

=== lote encabezado
D1 > D2                            | 2026-05-10,1,7,10                                                                          | SIN DATOS
D1 = D2 = fecha                    | 2026-05-10,1,7,10                                                                          | 7 10
bordes 09|10|20|21 (1,2,4,8)       | 2026-05-09,1,7,1 ¦ 2026-05-10,2,7,2 ¦ 2026-05-20,3,7,4 ¦ 2026-05-21,4,7,8                  | 7 6
N=1 con 2 líneas                   | 2026-05-10,1,7,10 ¦ 2026-05-11,2,7,20                                                      | 7 10
N=5 con 2 líneas                   | 2026-05-10,1,7,10 ¦ 2026-05-11,2,7,20                                                      | 7 30
N=2: vacía + válida                |  ¦ 2026-05-10,1,7,10                                                                       | 7 10
N=2: válida, vacía, válida         | 2026-05-10,1,7,10 ¦  ¦ 2026-05-11,2,7,20                                                   | 7 10
N=0                                | 2026-05-10,1,7,10                                                                          | SIN DATOS
D1/D2 fuera de 2000-2099           | 2026-05-10,1,7,10                                                                          | 7 10
dos espacios entre D1 y D2         | 2026-01-01  2026-12-31\n1\n2026-05-10,1,7,10\n                                             | 7 10
espacio final en línea N           | 2026-01-01 2026-12-31\n1 \n2026-05-10,1,7,10\n                                             | 7 10

=== lote largo
cantidad '10' (2 car.)             | 2026-05-10,1,7,10                                                                          | 7 10
cantidad '010' (3 car.)            | 2026-05-10,1,7,010                                                                         | 7 10
cantidad '0010' (4 car.)           | 2026-05-10,1,7,0010                                                                        | 7 10
cantidad '00010' (5 car.)          | 2026-05-10,1,7,00010                                                                       | 7 10
cantidad '000010' (6 car.)         | 2026-05-10,1,7,000010                                                                      | 7 10
cantidad '0000010' (7 car.)        | 2026-05-10,1,7,0000010                                                                     | 7 10
cantidad '00000010' (8 car.)       | 2026-05-10,1,7,00000010                                                                    | 7 10
cantidad '000000010' (9 car.)      | 2026-05-10,1,7,000000010                                                                   | 7 10
cantidad '0000000010' (10 car.)    | 2026-05-10,1,7,0000000010                                                                  | 7 10
+20 y '-10' (3 car.)               | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-10                                                     | 7 10
+20 y '-010' (4 car.)              | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-010                                                    | 7 10
+20 y '-0010' (5 car.)             | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-0010                                                   | 7 10
+20 y '-00010' (6 car.)            | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-00010                                                  | 7 10
+20 y '-000010' (7 car.)           | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-000010                                                 | 7 10
+20 y '-0000010' (8 car.)          | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-0000010                                                | 7 10
producto '7' (1 car.)              | 2026-05-10,1,7,10                                                                          | 7 10
producto '07' (2 car.)             | 2026-05-10,1,07,10                                                                         | 7 10
producto '007' (3 car.)            | 2026-05-10,1,007,10                                                                        | 7 10
producto '0007' (4 car.)           | 2026-05-10,1,0007,10                                                                       | 7 10
producto '00007' (5 car.)          | 2026-05-10,1,00007,10                                                                      | 7 10
producto '000007' (6 car.)         | 2026-05-10,1,000007,10                                                                     | 7 10
producto '0000007' (7 car.)        | 2026-05-10,1,0000007,10                                                                    | 7 10
producto '00000007' (8 car.)       | 2026-05-10,1,00000007,10                                                                   | 7 10
id '1' (1 car.)                    | 2026-05-10,1,7,10                                                                          | 7 10
id '01' (2 car.)                   | 2026-05-10,01,7,10                                                                         | 7 10
id '001' (3 car.)                  | 2026-05-10,001,7,10                                                                        | 7 10
id '0001' (4 car.)                 | 2026-05-10,0001,7,10                                                                       | 7 10
id '00001' (5 car.)                | 2026-05-10,00001,7,10                                                                      | 7 10
id '000001' (6 car.)               | 2026-05-10,000001,7,10                                                                     | 7 10
id '0000001' (7 car.)              | 2026-05-10,0000001,7,10                                                                    | 7 10
id '00000001' (8 car.)             | 2026-05-10,00000001,7,10                                                                   | 7 10
id '000000001' (9 car.)            | 2026-05-10,000000001,7,10                                                                  | 7 10

=== lote negativas2
10000+5 y -9999                    | 2026-05-10,1,7,10000 ¦ 2026-05-10,2,7,5 ¦ 2026-05-10,3,7,-9999                             | 7 6
10000+5 y -10000                   | 2026-05-10,1,7,10000 ¦ 2026-05-10,2,7,5 ¦ 2026-05-10,3,7,-10000                            | 7 5
10000+5 y -10001                   | 2026-05-10,1,7,10000 ¦ 2026-05-10,2,7,5 ¦ 2026-05-10,3,7,-10001                            | 7 10005
10000+5 y -20000                   | 2026-05-10,1,7,10000 ¦ 2026-05-10,2,7,5 ¦ 2026-05-10,3,7,-20000                            | 7 10005

=== lote largo2
cantidad 1 con 10 car. (línea 25)  | 2026-05-10,1,7,0000000001                                                                  | 7 1
cantidad 1 con 11 car. (línea 26)  | 2026-05-10,1,7,00000000001                                                                 | 7 1
cantidad 1 con 12 car. (línea 27)  | 2026-05-10,1,7,000000000001                                                                | 7 1
cantidad 1 con 13 car. (línea 28)  | 2026-05-10,1,7,0000000000001                                                               | SIN DATOS
cantidad 1 con 14 car. (línea 29)  | 2026-05-10,1,7,00000000000001                                                              | SIN DATOS
cantidad 1 con 15 car. (línea 30)  | 2026-05-10,1,7,000000000000001                                                             | SIN DATOS
cantidad 1 con 16 car. (línea 31)  | 2026-05-10,1,7,0000000000000001                                                            | SIN DATOS
cantidad 1 con 17 car. (línea 32)  | 2026-05-10,1,7,00000000000000001                                                           | SIN DATOS
cantidad 1 con 18 car. (línea 33)  | 2026-05-10,1,7,000000000000000001                                                          | SIN DATOS
cantidad 1 con 19 car. (línea 34)  | 2026-05-10,1,7,0000000000000000001                                                         | SIN DATOS
cantidad 1 con 20 car. (línea 35)  | 2026-05-10,1,7,00000000000000000001                                                        | SIN DATOS
id 1 con 9 car. (línea 25)         | 2026-05-10,000000001,7,10                                                                  | 7 10
id 1 con 10 car. (línea 26)        | 2026-05-10,0000000001,7,10                                                                 | 7 10
id 1 con 11 car. (línea 27)        | 2026-05-10,00000000001,7,10                                                                | 7 10
id 1 con 12 car. (línea 28)        | 2026-05-10,000000000001,7,10                                                               | 7 10
id 1 con 13 car. (línea 29)        | 2026-05-10,0000000000001,7,10                                                              | SIN DATOS
id 1 con 14 car. (línea 30)        | 2026-05-10,00000000000001,7,10                                                             | SIN DATOS
id 1 con 15 car. (línea 31)        | 2026-05-10,000000000000001,7,10                                                            | SIN DATOS
id 1 con 16 car. (línea 32)        | 2026-05-10,0000000000000001,7,10                                                           | SIN DATOS
id 1 con 17 car. (línea 33)        | 2026-05-10,00000000000000001,7,10                                                          | SIN DATOS
id 1 con 18 car. (línea 34)        | 2026-05-10,000000000000000001,7,10                                                         | SIN DATOS
id 1 con 19 car. (línea 35)        | 2026-05-10,0000000000000000001,7,10                                                        | SIN DATOS
id 1 con 20 car. (línea 36)        | 2026-05-10,00000000000000000001,7,10                                                       | SIN DATOS
producto 7 con 8 car. (línea 24)   | 2026-05-10,1,00000007,10                                                                   | 7 10
producto 7 con 9 car. (línea 25)   | 2026-05-10,1,000000007,10                                                                  | 7 10
producto 7 con 10 car. (línea 26)  | 2026-05-10,1,0000000007,10                                                                 | 7 10
producto 7 con 11 car. (línea 27)  | 2026-05-10,1,00000000007,10                                                                | 7 10
producto 7 con 12 car. (línea 28)  | 2026-05-10,1,000000000007,10                                                               | 7 10
producto 7 con 13 car. (línea 29)  | 2026-05-10,1,0000000000007,10                                                              | SIN DATOS
producto 7 con 14 car. (línea 30)  | 2026-05-10,1,00000000000007,10                                                             | SIN DATOS
producto 7 con 15 car. (línea 31)  | 2026-05-10,1,000000000000007,10                                                            | SIN DATOS
producto 7 con 16 car. (línea 32)  | 2026-05-10,1,0000000000000007,10                                                           | SIN DATOS
producto 7 con 17 car. (línea 33)  | 2026-05-10,1,00000000000000007,10                                                          | SIN DATOS
producto 7 con 18 car. (línea 34)  | 2026-05-10,1,000000000000000007,10                                                         | SIN DATOS
producto 7 con 19 car. (línea 35)  | 2026-05-10,1,0000000000000000007,10                                                        | SIN DATOS
producto 7 con 20 car. (línea 36)  | 2026-05-10,1,00000000000000000007,10                                                       | SIN DATOS
campos máximos sin ceros           | 2026-05-10,4000000,100000,-10000                                                           | SIN DATOS
id y producto con 8 ceros cada uno | 2026-05-10,000000001,000000007,10                                                          | 7 10

=== lote bordes_anio
fecha 1999-12-31                   | 1999-12-31,1,7,10                                                                          | SIN DATOS
fecha 2000-01-01                   | 2000-01-01,1,7,10                                                                          | 7 10
fecha 2099-12-31                   | 2099-12-31,1,7,10                                                                          | 7 10
fecha 2100-01-01                   | 2100-01-01,1,7,10                                                                          | SIN DATOS
fecha 2096-02-29                   | 2096-02-29,1,7,10                                                                          | 7 10
fecha 2100-02-29                   | 2100-02-29,1,7,10                                                                          | SIN DATOS
encabezado con fecha inválida 02-30 | 2026-03-01,1,7,10                                                                          | 7 10
fecha vacía                        | ,1,7,10                                                                                    | SIN DATOS
cantidad -0 sola tras +10          | 2026-05-10,1,7,-0                                                                          | SIN DATOS
1ª con cantidad -0, luego válida   | 2026-05-01,1,1,-0 ¦ 2026-05-02,1,2,7                                                       | 2 7
1ª con id 0000 (=0), luego id 1    | 2026-05-01,0000,1,5 ¦ 2026-05-02,1,2,7                                                     | 2 7

=== lote ultimos
+20 y '-0000000001' (11 car., 10 díg.) | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-0000000001                                             | 7 19
+20 y '-00000000001' (12 car., 11 díg.) | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-00000000001                                            | 7 19
+20 y '-000000000001' (13 car., 12 díg.) | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-000000000001                                           | 7 19
línea basura de 50 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ¦ 2026-05-10,1,7,10                     | 7 10
línea basura de 64 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ¦ 2026-05-10,1,7,10       | 7 10
línea basura de 100 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | 7 10
línea basura de 128 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | 7 10
línea basura de 256 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | 7 10
línea basura de 300 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | 7 10
línea basura de 1000 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | 7 10
línea basura de 5000 car. + válida, N=2 | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | 7 10
fecha '2026-5-010' (10 car.)       | 2026-5-010,1,7,10                                                                          | SIN DATOS
fecha '2026-05-1a'                 | 2026-05-1a,1,7,10                                                                          | SIN DATOS
fecha con mes '-1'                 | 2026--1-10,1,7,10                                                                          | SIN DATOS
línea solo con \r                  | \r                                                                                         | SIN DATOS

=== lote digitos_neg
+20 y '-000000000001' (12 díg.)    | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-000000000001                                           | 7 19
+20 y '-0000000000001' (13 díg.)   | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-0000000000001                                          | 7 20
+20 y '-00000000000001' (14 díg.)  | 2026-05-10,1,7,20 ¦ 2026-05-10,2,7,-00000000000001                                         | 7 20
cantidad '000000010000' (12 díg.)  | 2026-05-10,1,7,000000010000                                                                | 7 10000
cantidad '0000000010000' (13 díg.) | 2026-05-10,1,7,0000000010000                                                               | SIN DATOS
```
