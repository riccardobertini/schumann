#include "shuman.h"

// Table for sine function values
volatile uint16_t sineTable[TABLE_SIZE];
// Current index in the sine table
volatile uint16_t sineIndex;

// Initialize the sine value table
void initSineTable() {
    for (uint16_t i = 0; i < TABLE_SIZE; i++) {
        // Generate values from 0 to 1023 (10-bit PWM)
        // Offset of 512 + amplitude of 511 to center the waveform
        sineTable[i] = 512 + 511 * sin(2 * PI * i / TABLE_SIZE);
    }
}

// Initialize PWM
void initPWM() {
    pinMode(PWM_PIN, OUTPUT);
    // Reset Timer1 registers
    TCCR1A = 0;
    TCCR1B = 0;

    // Configure Fast PWM 10-bit (WGM13:0 = 0111)
    TCCR1A |= (1 << WGM11) | (1 << WGM10);
    TCCR1B |= (1 << WGM12);

    // Enable PWM on OC1A (pin 9) - non-inverting mode
    TCCR1A |= (1 << COM1A1);

    // Prescaler = 1 (maximum PWM frequency)
    TCCR1B |= (1 << CS10);
}

// Initialize the sampling timer
void initSampleTimer(uint16_t sampleRate) {
    cli(); // Disable global interrupts

    // Reset Timer2 registers
    TCCR2A = 0;
    TCCR2B = 0;

    // Configure CTC mode (Clear Timer on Compare Match)
    TCCR2A |= (1 << WGM21);

    // Calculate OCR2A value to obtain the desired sample rate
    // Formula: OCR2A = (F_CPU / (prescaler * sampleRate)) - 1
    // With prescaler = 64
    OCR2A = (F_CPU / 64 / sampleRate) - 1;

    // Set prescaler to 64
    TCCR2B |= (1 << CS22);

    // Enable interrupt on Compare Match A
    TIMSK2 |= (1 << OCIE2A);

    sei(); // Re-enable global interrupts
}

// Set the output frequency
void setOutputFrequency(float freq) {
    // Reset table index
    sineIndex = 0;

    // Configure the timer with the appropriate sample rate
    if (freq == FREQ_1) {
        initSampleTimer(SAMPLE_RATE_7_83);
    }
    else if (freq == FREQ_2) {
        initSampleTimer(SAMPLE_RATE_14_1);
    }
}

// Interrupt Service Routine for Timer2 Compare Match A
ISR(TIMER2_COMPA_vect) {
    // Update the PWM duty cycle with the current sine table value
    OCR1A = sineTable[sineIndex++];

    // Wrap around when we reach the end of the table
    if (sineIndex >= TABLE_SIZE) {
        sineIndex = 0;
    }
}
