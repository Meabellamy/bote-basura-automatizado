# Bote de Basura Automatizado

Sistema embebido sobre Arduino UNO que abre y cierra la tapa de un bote de basura de forma autónoma al detectar una mano con sensor ultrasónico.

---

## ¿Qué hace?

Cuando acercas la mano a menos de **20 cm** del sensor, el servomotor levanta la tapa. Tres segundos después la cierra solo. Sin botones, sin contacto.

El flujo completo:

```
Detecta mano → Abre tapa (90°) → Espera 3 s → Cierra tapa (0°) → Repite
```

---

## Hardware necesario

| Componente | Cantidad |
|---|---|
| Arduino UNO (ATmega328P) | 1 |
| Sensor ultrasónico HC-SR04 | 1 |
| Servomotor SG-90 | 1 |
| Cables Dupont macho-hembra | ~8 |

---

## Conexiones

```
Arduino UNO        Componente
───────────        ──────────
D9  (OUT)    →     HC-SR04  TRIG
D10 (IN)     ←     HC-SR04  ECHO
D6  (PWM)    →     Servo    Signal
5V           →     HC-SR04 VCC  /  Servo VCC
GND          →     HC-SR04 GND  /  Servo GND
```

> Si el servo presenta vibraciones o inestabilidad, aliméntalo desde una fuente externa de 5 V y comparte el GND con el Arduino.

---

## Cómo cargarlo

1. Instala el **IDE de Arduino** (versión 1.8.x o 2.x).
2. Conecta el Arduino UNO por USB.
3. Abre `src/bote_basura.ino`.
4. Selecciona la placa: `Herramientas → Placa → Arduino UNO`.
5. Selecciona el puerto COM correspondiente.
6. Clic en **Subir** (→).

No requiere librerías externas — `Servo.h` viene incluida en el IDE.

---

## Ajustes configurables

Todos los parámetros relevantes están agrupados como constantes al inicio del archivo. No hay números mágicos en el código.

```cpp
DISTANCIA_UMBRAL_CM   = 20     // cm — distancia de activación
DELAY_TAPA_ABIERTA_MS = 3000   // ms — tiempo que la tapa permanece abierta
DELAY_MUESTREO_MS     = 100    // ms — frecuencia de lectura del sensor
ANGULO_ABIERTO        = 90     // °  — posición del servo (tapa abierta)
ANGULO_CERRADO        = 0      // °  — posición del servo (tapa cerrada)
```

---

## Estructura del proyecto

```
bote_basura/
├── src/
│   └── bote_basura.ino     # Código fuente principal (C++ / ANSI)
├── docs/
│   └── dossier.html        # Documentación técnica completa (exportable a PDF)
└── README.md
```

---

## Sobre el código

Escrito en C++/ANSI bajo el framework de Arduino, siguiendo buenas prácticas de sistemas embebidos:

- Tipos de ancho fijo (`uint8_t`, `uint16_t`, `uint32_t`) en lugar de `int` genérico.
- Constantes `static const` con sufijos tipados (`U`, `UL`, `f`) — sin números mágicos.
- Prototipos declarados antes de `setup()`.
- Estado inicial de pines definido explícitamente en `setup()`.
- Documentación Doxygen en cada función (`@brief`, `@param`, `@return`).
- Una sola variable global (`Servo servoTapa`) requerida por la librería.

---

## Depuración por Serial

Con el monitor serie abierto a **9600 baudios** el sistema reporta en tiempo real:

```
=== Bote de Basura Automatizado v1.0 ===
[SENSOR] Distancia: 14 cm  |  Tapa: ABIERTA
[SENSOR] Distancia: 14 cm  |  Tapa: CERRADA
```

---

## Autores

- **Meave Diaz Armando Abdiel**
- **Sandoval Juan Manuel**

Proyecto académico — Licencia MIT · 2026
