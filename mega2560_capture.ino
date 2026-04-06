// =============================================================================
// mega2560_capture.ino
// 32-pin logic capture with rising-edge trigger for Arduino Mega2560
//
// Pin assignments:
//   Trigger : Pin 2  (PE4, INT4) — rising edge triggers capture
//   Port A  : Pins 22–29 (PA0–PA7) — data bits  0– 7
//   Port C  : Pins 37–30 (PC0–PC7) — data bits  8–15
//   Port L  : Pins 49–42 (PL0–PL7) — data bits 16–23
//   Port K  : Pins A8–A15 (PK0–PK7) — data bits 24–31
//
// Serial output (115200 baud), one line per capture:
//   T:<timestamp_ms>,0x<32-bit hex>\n
//   e.g. T:00001234,0xABCD1234
// =============================================================================

#define TRIGGER_PIN 2

volatile bool     g_triggered  = false;
volatile uint8_t  g_portA      = 0;
volatile uint8_t  g_portC      = 0;
volatile uint8_t  g_portL      = 0;
volatile uint8_t  g_portK      = 0;
volatile uint32_t g_timestamp  = 0;

// ISR: fires on rising edge of trigger pin.
// Reads all four ports as close together as possible.
ISR(INT4_vect) {
    g_portA     = PINA;
    g_portC     = PINC;
    g_portL     = PINL;
    g_portK     = PINK;
    g_timestamp = millis();
    g_triggered = true;
}

void setup() {
    Serial.begin(115200);

    // --- Configure data ports as inputs, no pull-ups ---
    DDRA  = 0x00;  PORTA = 0x00;  // Port A: pins 22-29
    DDRC  = 0x00;  PORTC = 0x00;  // Port C: pins 37-30
    DDRL  = 0x00;  PORTL = 0x00;  // Port L: pins 49-42
    DDRK  = 0x00;  PORTK = 0x00;  // Port K: A8-A15

    // --- Configure trigger pin (pin 2 = PE4) ---
    pinMode(TRIGGER_PIN, INPUT);

    // Set up INT4 manually for rising edge (attachInterrupt uses INT4 for pin 2
    // on the Mega, but we configure EICRB directly for clarity)
    EICRB |= (1 << ISC41) | (1 << ISC40);  // rising edge on INT4
    EIMSK |= (1 << INT4);                   // enable INT4

    sei(); // enable global interrupts

    Serial.println(F("# Mega2560 32-pin capture ready."));
    Serial.println(F("# Waiting for rising edge on pin 2 (trigger)..."));
    Serial.println(F("# Format: T:<ms>,0x<32-bit hex>"));
}

void loop() {
    if (g_triggered) {
        // Copy volatile data with interrupts disabled to avoid a torn read
        uint8_t  a, c, l, k;
        uint32_t ts;

        noInterrupts();
        a  = g_portA;
        c  = g_portC;
        l  = g_portL;
        k  = g_portK;
        ts = g_timestamp;
        g_triggered = false;
        interrupts();

        // Pack into a single 32-bit word
        // bit  0– 7 : Port A (pins 22-29, PA0-PA7)
        // bit  8–15 : Port C (pins 37-30, PC0-PC7)
        // bit 16–23 : Port L (pins 49-42, PL0-PL7)
        // bit 24–31 : Port K (pins A8-A15, PK0-PK7)
        uint32_t state = ((uint32_t)a)        |
                         ((uint32_t)c << 8)   |
                         ((uint32_t)l << 16)  |
                         ((uint32_t)k << 24);

        char buf[32];
        snprintf(buf, sizeof(buf), "T:%08lu,0x%08lX", ts, state);
        Serial.println(buf);
    }
}
