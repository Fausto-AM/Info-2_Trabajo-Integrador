# Trabajo Práctico Integrador - Máquinas de Estado

Repositorio personal para la entrega del Trabajo Práctico Integrador de Informática II para la UTNFRA.

## Profesores

- Ing. Gustavo Viard.
- Mg. Ing. Damian Ruben Corbalan.

## Generador de Funciones con ESP32

### Resumen

El proyecto consiste en el diseño e implementación de un generador de funciones digital basado en un microcontrolador ESP32-WROOM.

El sistema permite seleccionar la forma de onda y modificar sus principales parámetros mediante un único encoder rotativo con pulsador. La información correspondiente al estado del generador, la función seleccionada y los parámetros configurables se presenta en una pantalla OLED SSD1306 de 128 × 64 píxeles.

La señal de salida es generada digitalmente mediante tablas de consulta (_Look-Up Tables_ [LUT]) y posteriormente convertida a una señal analógica mediante el DAC interno del ESP32. La generación de muestras se realiza a una frecuencia de muestreo de 40 kHz.

El firmware se encuentra organizado en módulos independientes y utiliza una máquina de estados finitos para gestionar la interfaz de usuario y los diferentes modos de operación del generador.

### Objetivos

- Implementar un generador de funciones digital utilizando el DAC interno del ESP32.
- Generar diferentes formas de onda: seno, cuadrada, triangular y sierra.
- Permitir la modificación de frecuencia, amplitud, offset y ciclo de trabajo.
- Permitir la modificación de los parámetros mientras la señal se encuentra siendo generada.
- Implementar una interfaz de usuario utilizando únicamente un encoder rotativo con pulsador.
- Visualizar el estado del sistema y los parámetros configurables mediante una pantalla OLED.
- Implementar una máquina de estados finitos que gestione la navegación y las acciones del usuario.
- Separar las diferentes funciones del firmware en módulos independientes.
- Generar las muestras de salida mediante una frecuencia de muestreo fija de 40 kHz.

### Diseño de la FSM

La interfaz de usuario se implementa mediante una máquina de estados finitos (FSM) contenida en `generator.c` y `generator.h`.

Los eventos producidos por el encoder son:

- `ENC_CW`: giro horario.
- `ENC_CCW`: giro antihorario.
- `ENC_SHORT_PRESS`: pulsación corta.
- `ENC_LONG_PRESS`: pulsación larga.

#### Estados

- `STATE_STANDBY`

    Es el estado inicial de operación del generador.

    La salida se encuentra detenida y el usuario puede desplazarse mediante el encoder entre las siguientes opciones:

    * Parámetros
    * Función
    * RUN

    Una pulsación corta confirma la opción seleccionada.

    Al seleccionar _RUN_, el sistema pasa a `STATE_GENERATING`.

- `STATE_GENERATING`

    En este estado el generador se encuentra funcionando y el DAC recibe las muestras generadas.

    El menú principal pasa a mostrar:

    * Parámetros
    * Función
    * STOP

    Desde este estado también es posible ingresar a los menús de parámetros y selección de función sin detener la generación de la señal.

    Al seleccionar _STOP_, el sistema vuelve a `STATE_STANDBY`.

- `STATE_SELECT_PARAM`

    Permite seleccionar uno de los cuatro parámetros configurables:

    * Frecuencia
    * Amplitud
    * Offset
    * Duty

    Una pulsación corta sobre un parámetro lleva a `STATE_EDIT_PARAM`.

    Una pulsación larga abandona el menú y retorna al estado desde el cual se ingresó.

- `STATE_EDIT_PARAM`

    Permite modificar el parámetro seleccionado mediante el giro del encoder.

    Cada parámetro posee:

    * un valor actual;
    * un límite mínimo;
    * un límite máximo;
    * un tamaño de paso;
    * una función setter encargada de aplicar el nuevo valor.

    Los valores se limitan automáticamente a los rangos definidos en config.h.

    Una pulsación corta o larga finaliza la edición y retorna a `STATE_SELECT_PARAM`.

- `STATE_SELECT_FUNC`

    Permite seleccionar la forma de onda utilizada por el generador:

    * Seno
    * Cuadrada
    * Triangular
    * Sierra

    Una pulsación corta aplica inmediatamente la función seleccionada. Esto permite recorrer las diferentes formas de onda y observar el cambio sin abandonar el menú.

    Una pulsación larga retorna al estado desde el cual se ingresó.

### Descripción de archivos

| Archivo | Responsabilidad |
|---|---|
| main.c | Punto de entrada de la aplicación. Inicializa el sistema y ejecuta el bucle principal de _polling_.
| sys.c/.h | Inicialización general de los módulos y procesamiento de eventos del sistema.
| generator.c/.h | Implementación de la máquina de estados, navegación de menús, selección de parámetros y control `RUN/STOP`.
| encoder.c/.h | Lectura del encoder y del pulsador. Genera los eventos utilizados por la FSM.
| display.c/.h | Control y actualización de la pantalla OLED mediante u8g2 e I2C.
| waveform.c/.h | Generación de las LUT, selección de formas de onda, acumulador de fase y cálculo de muestras.
| dac.c/.h | Abstracción del DAC interno del ESP32 mediante dac_oneshot.
| sample_timer.c/.h | Temporización de la generación de muestras mediante `esp_timer`.
| config.h | Configuración de GPIO, límites de parámetros, frecuencia de muestreo y constantes del sistema.

### Diagrama de la Máquina de Estado

![Diagrama de la Máquina de Estado](Assets/Dia_Maq_Est.svg)

### Esquemático

![Esquemático](Assets/Sch_v03.svg)

### PCB Finalizado

![PCB](Assets/PCB.png)

### Proyecto Funcionando

![Proyecto en Funcionamiento](Assets/Proyecto_en_Funcionamiento.mp4)