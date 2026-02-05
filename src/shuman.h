#ifndef SHUMAN_H
#define SHUMAN_H

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

// Constant definitions
#define TABLE_SIZE 256        // Number of samples in the lookup table
#define PWM_PIN 9             // PWM pin (OC1A on Arduino)

// Output frequencies (Hz)
#define FREQ_1  7.83f  // First frequency (fundamental Schumann resonance)
#define FREQ_2  14.1f  // Second frequency (first harmonic)

// Sample rates calculated to generate the desired frequencies
// Sample Rate = Output Frequency × TABLE_SIZE
#define SAMPLE_RATE_7_83 2004  // 7.83 × 256 ≈ 2004 Hz
#define SAMPLE_RATE_14_1 3610  // 14.1 × 256 ≈ 3610 Hz

#ifndef F_CPU
#define F_CPU 16000000UL       // 16 MHz for Arduino Uno/Nano
#endif

/**
 * @brief Initializes the lookup table for sine wave generation.
 */
void initSineTable();

/**
 * @brief Initializes PWM for signal generation.
 */
void initPWM();

/**
 * @brief Sets the output signal frequency.
 *
 * @param freq Desired frequency in Hz.
 */
void setOutputFrequency(float freq);

#endif // SHUMAN_H
