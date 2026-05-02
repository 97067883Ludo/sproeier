# Sproeier – Arduino Irrigation System

Arduino OPTA firmware for an automated garden irrigation system. Controlled via MQTT over Ethernet.

## Target Platform

**Arduino OPTA** (ARM Cortex-M4, industrial PLC form factor).  
Use OPTA-specific pin constants: `D0`–`D3` (digital outputs), `I1` (analog input), `BTN_USER`, `LEDG`, `LEDR`.

## Architecture

All classes are **header-only** — declaration and implementation both live in `.h` files. There is no separate `.cpp`.

| File | Responsibility |
|------|---------------|
| `SproeiInstallatie.ino` | Entry point: `setup()`, `loop()`, `AcutateValve()` callback |
| `Valve.h` | Controls a single solenoid valve (digital output + status LED) |
| `MqttClient.h` | Ethernet + MQTT via PubSubClient; subscribes to zone topics |
| `AlarmService.h` | Alarm state machine; drives LEDG/LEDR; reset via BTN_USER |
| `PressureSensor.h` | Analog pressure sensor (0–10 V → 0–5 bar); publishes over MQTT |

## Zone Mapping

| MQTT topic | Valve # | Zone |
|------------|---------|------|
| `huis/sproeiers/achterhuis` | 1 | Back garden |
| `huis/sproeiers/naasthuis` | 2 | Side garden |
| `huis/sproeiers/voorhuis` | 3 | Front garden |
| *(auto)* | 4 | Frequency inverter / pump |

**Valve 4** (pump/frequency inverter) opens automatically when any zone valve opens and closes when all zone valves are closed. Do not expose valve 4 as a user-facing MQTT topic.

Pressure data is published to: `huis/sproeisysteem/druk`

## Key Conventions

- MQTT payload `"1"` → `HIGH` (open); anything else → `LOW` (close).
- Alarm code `1` = MQTT disconnected. Alarms block all valve operation until reset.
- Static IP for the OPTA: `192.168.178.180`; MQTT broker: `192.168.178.209:1883`.
- Pressure publishing: every 60 s, or when change exceeds 0.06 bar threshold.
- Sensor calibration: ADC max ≈ 918 counts → 10 V → 5 bar.
