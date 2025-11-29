#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Constantes del sistema MEGATRONIX */
#define NUM_FILAS   8       // Número de líneas de la caché
#define TAM_LINEA   16      // Bytes por línea de caché
#define TAM_RAM     4096    // 2^12 direcciones de memoria

/* Nombres de ficheros */
#define FICH_RAM    "CONTENTS_RAM.bin"
#define FICH_ACCESOS "accesos_memoria.txt"
#define FICH_CACHE  "CONTENTS_CACHE.bin"

/* Definición de la línea de caché (según enunciado) */
typedef struct {
    unsigned char ETQ;              // Etiqueta (Label)
    unsigned char Data[TAM_LINEA];  // Datos de la línea
} T_CACHE_LINE;

/* Variables globales */
T_CACHE_LINE CACHE[NUM_FILAS];
unsigned char Simul_RAM[TAM_RAM];

unsigned int globaltime = 0;   // Instante global de tiempo
unsigned int numfallos  = 0;   // Número de fallos de caché

char texto[101];               // Texto leído carácter a carácter (máx. 100 chars)
int  idx_texto = 0;            // Posición actual en texto

/* Prototipos de las funciones obligatorias */
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra,
                      int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ,
                 int linea, int bloque);

/* -------- Implementación de funciones -------- */

/* Inicializa la caché:
 * - ETQ = 0xFF
 * - Datos = 0x23
 */
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]) {
    for (int i = 0; i < NUM_FILAS; i++) {
        tbl[i].ETQ = 0xFF;  // Valor "inválido" de etiqueta
        for (int j = 0; j < TAM_LINEA; j++) {
            tbl[i].Data[j] = 0x23;  // Relleno según enunciado
        }
    }
}

/* Vuelca el contenido de la caché por pantalla.
 * Los datos se imprimen de izquierda a derecha de mayor a menor peso:
 * byte 15 ... byte 0
 */
void VolcarCACHE(T_CACHE_LINE *tbl) {
    printf("\n---------------- CONTENIDO DE LA CACHE ----------------\n");
    printf("Linea | ETQ | Datos (byte15 ... byte0)\n");
    printf("-------------------------------------------------------\n");

    for (int i = 0; i < NUM_FILAS; i++) {
        printf(" %2d   | %02X | ", i, tbl[i].ETQ);
        for (int j = TAM_LINEA - 1; j >= 0; j--) {
            printf("%02X ", tbl[i].Data[j]);
        }
        printf("\n");
    }
    printf("-------------------------------------------------------\n\n");
}

/* De momento dejamos solo el esqueleto de estas funciones;
 * las completaremos en el siguiente paso.
 */
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra,
                      int *linea, int *bloque) {
    // TODO: completar en el siguiente paso
}

void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ,
                 int linea, int bloque) {
    // TODO: completar en el siguiente paso
}

/* -------- Programa principal -------- */

int main(void) {
    /* Más adelante:
     * - Inicializaremos la caché
     * - Leeremos CONTENTS_RAM.bin
     * - Leeremos accesos_memoria.txt
     * - Implementaremos el bucle de simulación
     * - Volcaremos CONTENTS_CACHE.bin
     */

    // De momento solo inicializamos la caché y la volcamos para probar.
    LimpiarCACHE(CACHE);
    VolcarCACHE(CACHE);

    return 0;
}
