/**
 * @file        bote_basura.ino
 * @project     Dossier — Bote de Basura Automatizado
 * @version     1.0.0
 * @date        2026-10-24
 * @standard    ANSI / ISO C++11
 *
 * @authors
 *   - Meave Diaz Armando Abdiel
 *   - Sandoval Juan Manuel
 *
 * @brief
 *   Sistema embebido para la apertura y cierre automatizado
 *   de la tapa de un bote de basura mediante la detección de
 *   proximidad con sensor ultrasónico HC-SR04 y un servomotor.
 *
 * @description
 *   El microcontrolador sondea continuamente la distancia frente
 *   al sensor.  Cuando un objeto (mano) se aproxima a menos del
 *   umbral configurado, el servo abre la tapa, la mantiene
 *   abierta durante DELAY_CIERRE_MS milisegundos y la cierra de
 *   forma automática.  El puerto serie emite trazas de depuración
 *   a 9 600 baudios.
 *
 * @pinout
 *   ┌─────┬───────────┬───────────────────┬──────────────────────┐
 *   │ Pin │ Dirección │ Componente        │ Función              │
 *   ├─────┼───────────┼───────────────────┼──────────────────────┤
 *   │  6  │ PWM OUT   │ Servomotor SG-90  │ Control posición tapa│
 *   │  9  │ OUT       │ HC-SR04 TRIG      │ Disparo de pulso US  │
 *   │ 10  │ IN        │ HC-SR04 ECHO      │ Recepción de eco US  │
 *   │ 5 V │ PWR       │ HC-SR04 / Servo   │ Alimentación         │
 *   │ GND │ GND       │ HC-SR04 / Servo   │ Tierra común         │
 *   └─────┴───────────┴───────────────────┴──────────────────────┘
 *
 * @dependencies
 *   - Servo.h  (incluida en el IDE de Arduino)
 *
 * @license     MIT
 * ===========================================================================
 */

#include <Servo.h>

/* ===========================================================================
 *  SECCIÓN 1 — CONSTANTES DE CONFIGURACIÓN DE HARDWARE
 * =========================================================================== */

/** @defgroup Pines Definición de pines de E/S */
/** @{ */
static const uint8_t PIN_TRIG  = 9U;   ///< Pin TRIGGER del HC-SR04 (SALIDA digital)
static const uint8_t PIN_ECHO  = 10U;  ///< Pin ECHO    del HC-SR04 (ENTRADA digital)
static const uint8_t PIN_SERVO = 6U;   ///< Pin PWM del servomotor  (SALIDA PWM)
/** @} */

/** @defgroup Servo Parámetros angulares del servomotor */
/** @{ */
static const uint8_t ANGULO_CERRADO = 90U;  ///< Ángulo para tapa cerrada (grados) — invertido por montaje físico del servo
static const uint8_t ANGULO_ABIERTO = 0U;   ///< Ángulo para tapa abierta (grados) — invertido por montaje físico del servo
/** @} */

/** @defgroup Sensor Parámetros del sensor ultrasónico */
/** @{ */
static const uint16_t DISTANCIA_UMBRAL_CM  = 20U;      ///< Distancia máxima de detección (cm)
static const uint32_t TIMEOUT_ECHO_US      = 30000UL;  ///< Tiempo máximo espera eco (µs)
static const float    VELOCIDAD_SONIDO_CmUs = 0.034f;  ///< Velocidad del sonido (cm/µs) a ~20 °C
/** @} */

/** @defgroup Tiempos Configuración de tiempos del sistema */
/** @{ */
static const uint32_t DELAY_TAPA_ABIERTA_MS = 3000U;  ///< Tiempo que la tapa permanece abierta (ms)
static const uint32_t DELAY_MUESTREO_MS      = 100U;  ///< Período de muestreo del sensor (ms)
/** @} */

/* ===========================================================================
 *  SECCIÓN 2 — OBJETOS GLOBALES
 * =========================================================================== */

Servo servoTapa;  ///< Instancia que controla el servomotor de la tapa

/* ===========================================================================
 *  SECCIÓN 3 — PROTOTIPOS DE FUNCIONES
 * =========================================================================== */

uint16_t medirDistancia(void);
void     abrirTapa(void);
void     cerrarTapa(void);
void     imprimirEstado(uint16_t distanciaCm, bool tapaAbierta);

/* ===========================================================================
 *  SECCIÓN 4 — INICIALIZACIÓN DEL SISTEMA
 * =========================================================================== */

/**
 * @brief   Configura pines, adjunta el servo y abre el puerto serie.
 *          Se ejecuta una única vez al encender o reiniciar la placa.
 */
void setup(void)
{
    /* — Configuración de pines del sensor ultrasónico — */
    pinMode(PIN_TRIG, OUTPUT);
    digitalWrite(PIN_TRIG, LOW);   // Estado inicial: reposo
    pinMode(PIN_ECHO, INPUT);

    /* — Adjuntar servomotor y colocar tapa en posición cerrada — */
    servoTapa.attach(PIN_SERVO);
    cerrarTapa();

    /* — Inicialización del puerto serie para depuración — */
    Serial.begin(9600);
    Serial.println(F("============================================"));
    Serial.println(F("  Bote de Basura Automatizado  v1.0.0"));
    Serial.println(F("  Autores: Meave Diaz A.A. / Sandoval J.M."));
    Serial.println(F("============================================"));
    Serial.println(F("Sistema inicializado. En espera..."));
}

/* ===========================================================================
 *  SECCIÓN 5 — CICLO PRINCIPAL
 * =========================================================================== */

/**
 * @brief   Ciclo continuo de detección y control de la tapa.
 *
 *          Flujo:
 *            1. Medir distancia con el sensor.
 *            2. Si objeto detectado → abrir tapa.
 *            3. Esperar DELAY_TAPA_ABIERTA_MS.
 *            4. Cerrar tapa automáticamente.
 *            5. Esperar DELAY_MUESTREO_MS y repetir.
 */
void loop(void)
{
    uint16_t distanciaCm = medirDistancia();

    if (distanciaCm > 0U && distanciaCm <= DISTANCIA_UMBRAL_CM)
    {
        /* Objeto / mano detectada dentro del umbral */
        imprimirEstado(distanciaCm, true);
        abrirTapa();
        delay(DELAY_TAPA_ABIERTA_MS);
        cerrarTapa();
        imprimirEstado(distanciaCm, false);
    }
    else
    {
        /* Sin detección — reportar solo cada muestreo */
        imprimirEstado(distanciaCm, false);
    }

    delay(DELAY_MUESTREO_MS);
}

/* ===========================================================================
 *  SECCIÓN 6 — IMPLEMENTACIÓN DE FUNCIONES
 * =========================================================================== */

/**
 * @brief   Activa el HC-SR04 y calcula la distancia al objeto más cercano.
 *
 * @details Genera un pulso HIGH de 10 µs en PIN_TRIG, luego mide el tiempo
 *          que tarda el pulso en regresar al PIN_ECHO.  Aplica la fórmula:
 *            distancia (cm) = (tiempo_eco µs × 0.034) / 2
 *
 * @return  Distancia en centímetros; 0 si no hubo eco (fuera de rango).
 */
uint16_t medirDistancia(void)
{
    /* Garantizar línea TRIG en bajo antes de disparar */
    digitalWrite(PIN_TRIG, LOW);
    delayMicroseconds(2U);

    /* Pulso de disparo: 10 µs en HIGH */
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10U);
    digitalWrite(PIN_TRIG, LOW);

    /* Esperar el eco y medir su duración */
    unsigned long duracionUs = pulseIn(PIN_ECHO, HIGH, TIMEOUT_ECHO_US);

    /* Sin eco → fuera de rango */
    if (duracionUs == 0UL)
    {
        return 0U;
    }

    /* Convertir tiempo a distancia */
    uint16_t distancia = static_cast<uint16_t>(
        (static_cast<float>(duracionUs) * VELOCIDAD_SONIDO_CmUs) / 2.0f
    );

    return distancia;
}

/**
 * @brief   Mueve el servomotor a la posición de tapa abierta (ANGULO_ABIERTO).
 */
void abrirTapa(void)
{
    servoTapa.write(ANGULO_ABIERTO);
}

/**
 * @brief   Mueve el servomotor a la posición de tapa cerrada (ANGULO_CERRADO).
 */
void cerrarTapa(void)
{
    servoTapa.write(ANGULO_CERRADO);
}

/**
 * @brief   Emite una línea de estado por el puerto serie.
 *
 * @param   distanciaCm   Distancia medida en centímetros.
 * @param   tapaAbierta   true si la tapa acaba de abrirse, false si está cerrada.
 */
void imprimirEstado(uint16_t distanciaCm, bool tapaAbierta)
{
    Serial.print(F("[SENSOR] Distancia: "));
    Serial.print(distanciaCm);
    Serial.print(F(" cm  |  Tapa: "));
    Serial.println(tapaAbierta ? F("ABIERTA") : F("CERRADA"));
}

/* =========================================================================== */
/*  FIN DE ARCHIVO — bote_basura.ino                                           */
/* =========================================================================== */
