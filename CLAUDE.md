# MEGA2560 32x Signal Capture

## Project overview
Arduino Mega2560 sketch that captures the state of 32 digital I/O pins on a rising-edge trigger and sends the result over serial to an external computer.

## Hardware
- **Board:** Arduino Mega2560
- **Trigger pin:** Pin 2 (PE4, INT4) — rising edge triggers capture
- **Data pins (32 total):**
  | Port | Pins | Arduino Mega Pins | Data bits |
  |------|------|-------------------|-----------|
  | Port A | PA0–PA7 | 22–29 | 0–7 |
  | Port C | PC0–PC7 | 37–30 | 8–15 |
  | Port L | PL0–PL7 | 49–42 | 16–23 |
  | Port K | PK0–PK7 | A8–A15 | 24–31 |

## Serial interface
- **Baud rate:** 115200
- **Output format:** `T:<timestamp_ms>,0x<32-bit hex>\n`
- **Example:** `T:00001234,0xABCD5678`

## Implementation notes
- Port registers (`PINA`, `PINC`, `PINL`, `PINK`) are read directly in the ISR for near-simultaneous capture
- INT4 is configured manually via `EICRB`/`EIMSK` for rising-edge detection
- Volatile ISR data is copied to local variables with interrupts disabled before serial output

## Build
```bash
arduino-cli compile --fqbn arduino:avr:mega mega2560_capture
```

## Upload (when board is connected)
```bash
arduino-cli upload --fqbn arduino:avr:mega --port <port> mega2560_capture
```

## Repository
https://github.com/maxnil/MEGA2560_32x_signal_capture
