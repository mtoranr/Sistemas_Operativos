PROYECTO SISTEMAS OPERATIVOS 2025-2026
Simulador de memoria caché (MEMsym)

Integrantes del grupo:
- Miguel Torán Rodríguez

Repositorio Git:
- URL: https://github.com/mtoranr/Sistemas_Operativos

Descripción del proyecto:
Este proyecto implementa un simulador de memoria caché de correspondencia directa
para el sistema MEGATRONIX, con las siguientes características:

- Caché de 8 líneas, 16 bytes por línea.
- Direcciones de 12 bits, divididas en:
  * 4 bits de palabra
  * 3 bits de línea
  * 5 bits de etiqueta (ETQ)
- Simulación de memoria principal de 4096 bytes (CONTENTS_RAM.bin).
- Lectura de accesos desde accesos_memoria.txt.
- Gestión de aciertos y fallos de caché, con contabilización de:
  * tiempo global (globaltime)
  * número de fallos de caché (numfallos)
- Volcado periódico del contenido de la caché por pantalla.
- Escritura final de los 128 bytes de datos de la caché en CONTENTS_CACHE.bin.

Ficheros que se entregan:
- MEMsym.c          → Código fuente del simulador.
- logcache.txt      → Volcado de la ejecución del programa.
- README.txt        → Este fichero.
- [PDF nombre].pdf  → Informe del proyecto (integrantes, tabla de evaluación,
                      declaración de uso de IA y enlace al vídeo).

Cómo compilar:
- Compilación en entorno GCC (ejemplo bajo Windows/PowerShell):
  gcc -Wall -Wextra -o MEMsym MEMsym.c

Cómo ejecutar:
1) Asegurarse de que en el directorio actual están:
   - MEMsym.exe (o ./MEMsym en Linux)
   - CONTENTS_RAM.bin
   - accesos_memoria.txt

2) Ejecutar normalmente:
   ./MEMsym

3) Para guardar la salida en logcache.txt:
   ./MEMsym > logcache.txt
