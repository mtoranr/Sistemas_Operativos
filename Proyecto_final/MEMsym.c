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

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra,
                      int *linea, int *bloque) {
    /* 
     * Formato de la dirección de 12 bits:
     *  [ ETQ (5 bits) ][ linea (3 bits) ][ palabra (4 bits) ]
     *
     * palabra = bits [3:0]
     * linea   = bits [6:4]
     * ETQ     = bits [11:7]
     */

    // 4 bits menos significativos
    *palabra = addr & 0xF;          // 0xF = 0000 1111

    // Siguientes 3 bits (desplazamos 4 y nos quedamos con 0x7 = 0000 0111)
    *linea   = (addr >> 4) & 0x7;

    // 5 bits más altos (desplazamos 7 y nos quedamos con 0x1F = 0001 1111)
    *ETQ     = (addr >> 7) & 0x1F;

    // Número de bloque (cada bloque = 16 bytes)
    *bloque  = addr >> 4;           // addr / 16
}

void TratarFallo(T_CACHE_LINE *tbl, unsigned char *MRAM, int ETQ,
                 int linea, int bloque) {
    // Mensaje indicando qué bloque se carga y en qué línea
    printf("   Cargando bloque %02X en la linea %02X\n", bloque, linea);

    // Posición de inicio del bloque en RAM
    int base = bloque * TAM_LINEA;

    // Actualizamos la etiqueta de la línea
    tbl[linea].ETQ = (unsigned char)ETQ;

    // Copiamos TAM_LINEA bytes desde MRAM a la línea de caché
    for (int i = 0; i < TAM_LINEA; i++) {
        tbl[linea].Data[i] = MRAM[base + i];
    }

    // Penalización temporal por fallo de caché
    globaltime += 20;
}

/* -------- Programa principal -------- */

int main(void) {
    FILE *f_ram = NULL;
    FILE *f_acc = NULL;
    FILE *f_cache = NULL;

    unsigned int addr;                 // Dirección leída en hex
    int ETQ, palabra, linea, bloque;   // Campos de la dirección
    int num_accesos = 0;               // Nº total de accesos

    // Inicializamos variables globales relacionadas con la simulación
    globaltime = 0;
    numfallos  = 0;
    idx_texto  = 0;
    texto[0]   = '\0';

    // 1) Inicializar caché
    LimpiarCACHE(CACHE);

    // 2) Leer RAM desde CONTENTS_RAM.bin en Simul_RAM
    f_ram = fopen(FICH_RAM, "rb");
    if (f_ram == NULL) {
        perror("Error abriendo CONTENTS_RAM.bin");
        return -1;
    }

    size_t leidos = fread(Simul_RAM, 1, TAM_RAM, f_ram);
    fclose(f_ram);

    if (leidos != TAM_RAM) {
        fprintf(stderr,
                "Error: se han leido %zu bytes de RAM, se esperaban %d\n",
                leidos, TAM_RAM);
        return -1;
    }

    // 3) Abrir accesos_memoria.txt
    f_acc = fopen(FICH_ACCESOS, "r");
    if (f_acc == NULL) {
        perror("Error abriendo accesos_memoria.txt");
        return -1;
    }

    // 4) Bucle principal de simulación
    while (fscanf(f_acc, "%x", &addr) == 1) {
        num_accesos++;

        // Descomponer la dirección en ETQ, palabra, linea y bloque
        ParsearDireccion(addr, &ETQ, &palabra, &linea, &bloque);

        // Comprobación básica de rango (por si acaso)
        if (linea < 0 || linea >= NUM_FILAS || palabra < 0 || palabra >= TAM_LINEA) {
            fprintf(stderr, "Direccion fuera de rango: %04X\n", addr);
            continue;
        }

        int T = globaltime;  // Instante actual

        // ¿HIT o MISS?
        if (CACHE[linea].ETQ != (unsigned char)ETQ) {
            // FALLO DE CACHÉ
            numfallos++;

            printf("T: %d, Fallo de CACHE %u, ADDR %04X Label %X linea %02X palabra %02X bloque %02X\n",
                   T, numfallos, addr, ETQ, linea, palabra, bloque);

            // Cargar el bloque desde RAM y actualizar etiqueta + tiempo
            TratarFallo(CACHE, Simul_RAM, ETQ, linea, bloque);
        } else {
            // ACIERTO DE CACHÉ
            unsigned char dato = CACHE[linea].Data[palabra];

            printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X\n",
                   T, addr, ETQ, linea, palabra, dato);

            // Construimos el texto con los caracteres leídos
            if (idx_texto < 100) {
                texto[idx_texto++] = (char)dato;
                texto[idx_texto] = '\0';  // mantenemos terminación en '\0'
            }
        }

        // Volcado de la caché tras cada acceso
        VolcarCACHE(CACHE);

        // Sleep de 1 segundo
        sleep(1);
    }

    fclose(f_acc);

    // 5) Resumen final
    printf("\n===== RESUMEN FINAL =====\n");
    printf("Numero total de accesos: %d\n", num_accesos);
    printf("Numero de fallos de cache: %u\n", numfallos);

    double tiempo_medio = (num_accesos > 0)
                          ? (double)globaltime / (double)num_accesos
                          : 0.0;

    printf("Tiempo medio de acceso: %.2f\n", tiempo_medio);
    printf("Texto leido: %s\n", texto);

    // 6) Volcado de la caché a CONTENTS_CACHE.bin
    f_cache = fopen(FICH_CACHE, "wb");
    if (f_cache == NULL) {
        perror("Error abriendo CONTENTS_CACHE.bin para escritura");
        return -1;
    }

    // El byte 0 del fichero es el byte 0 de la línea 0, etc.
    for (int i = 0; i < NUM_FILAS; i++) {
        size_t escritos = fwrite(CACHE[i].Data, 1, TAM_LINEA, f_cache);
        if (escritos != TAM_LINEA) {
            fprintf(stderr, "Error escribiendo linea %d en CONTENTS_CACHE.bin\n", i);
            fclose(f_cache);
            return -1;
        }
    }

    fclose(f_cache);

    return 0;
}

