/**
 * @file ultrasonic_sensor.h
 * @brief Header file for ultrasonic sensor .
 *
 * @details
 * This module provides functions to interface with an ultrasonic sensor for distance measurement using pulse-width measurement
 *
 * @date October 27, 2023
 */

#include <stdio.h>          // Include standard input-output library
#include "pico/stdlib.h"    // Include the Pico standard library
#include "hardware/gpio.h"  // Include the GPIO hardware library
#include "hardware/timer.h" // Include the timer hardware library

// Define the GPIO pin for ultrasonic
#define TRIGGER_PIN 0       // Define the GPIO pin for the ultrasonic sensor trigger
#define ECHO_PIN 1          // Define the GPIO pin for the ultrasonic sensor echo
#define TRIGGER_GPIO_PIN 0  // Define the GPIO pin for the ultrasonic sensor trigger
#define ECHO_GPIO_PIN 1     // Define the GPIO pin for the ultrasonic sensor echo

// Define the timeout for pulse measurement in microseconds
#define TIMEOUT 26100 

// Variables to measure the pulse
uint32_t pulse_start_time = 0; // Variable to store the start time of the echo pulse
uint32_t pulse_end_time = 0;   // Variable to store the end time of the echo pulse
bool pulse_received = false;   // Flag to indicate if an echo pulse has been received

// Function prototypes
void interrupt_handler(uint gpio, uint32_t events);
bool checkPulseTimes(uint32_t start_time, uint32_t end_time);
void on_echo_pin_change(uint gpio, uint32_t events);
uint32_t getPulseDuration();
void initialise_ultrasonic();
float calculateDistance(float pulse_duration);


/**
 * @brief Check if the start time is less than the end time.
 *
 * @param start_time The start time of the pulse.
 * @param end_time The end time of the pulse.
 * @return true if start time is less than end time, false otherwise.
 */
bool checkPulseTimes(uint32_t start_time, uint32_t end_time)
{
    // Check if start time is less than end time
    if (start_time < end_time)
    {
        // Valid pulse times
        return true; 
    }
    else
    {
        // Invalid pulse times
        return false; 
    }
}

/**
 * @brief Callback function for handling changes on the ECHO pin.
 *
 * @param gpio The GPIO pin number.
 * @param events The type of events that triggered the callback.
 */
void on_echo_pin_change(uint gpio, uint32_t events)
{
    if (gpio_get(ECHO_PIN))
    {
        // Record the start time of the echo pulse
        pulse_start_time = time_us_32(); 
    }
    else
    {
        // Record the end time of the echo pulse
        pulse_end_time = time_us_32();
        // Set the flag to indicate that the echo pulse has been received                                      
        pulse_received = checkPulseTimes(pulse_start_time, pulse_end_time); 
    }
}

/**
 * @brief Initialize the ultrasonic sensor GPIO pins.
 */
void initialise_ultrasonic()
{
    gpio_init(TRIGGER_PIN);              // Initialize the trigger pin
    gpio_init(ECHO_PIN);                 // Initialize the echo pin
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT); // Set trigger pin as output
    gpio_set_dir(ECHO_PIN, GPIO_IN);     // Set echo pin as input
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    // Enable interrupt on both rising and falling edges of the echo pulse
}

/**
 * @brief Shoot a trigger pulse to activate the ultrasonic sensor.
 */
void shoot_pulse()
{
    gpio_put(TRIGGER_PIN, true);  // Send a trigger pulse
    sleep_us(10);                 // Wait for 10 microseconds
    gpio_put(TRIGGER_PIN, false); // Stop the trigger pulse
}

/**
 * @brief Get the duration of the received pulse.
 *
 * @return The pulse duration in microseconds.
 */
uint32_t getPulseDuration()
{
    pulse_received = false;       // Reset the pulse received flag
    gpio_put(TRIGGER_PIN, true);  // Send a trigger pulse
    sleep_us(10);                 // Wait for 10 microseconds
    gpio_put(TRIGGER_PIN, false); // Stop the trigger pulse

    // Wait for the echo pulse to be received or timeout after 1 second
    uint32_t timeout_start = time_us_32(); // Record the start time of the timeout
    while (!pulse_received && (time_us_32() - timeout_start) < 1000000)
    {
        tight_loop_contents(); // Wait without consuming CPU cycles
    }

    return pulse_received ? pulse_end_time - pulse_start_time : 0.0;
}

/**
 * @brief Calculate the distance from the measured pulse duration.
 *
 * @param pulse_duration The duration of the ultrasonic pulse in microseconds.
 * @return The distance in centimeters.
 */
float calculateDistance(float pulse_duration)
{
    // Speed of sound at sea level is approximately 343 meters per second or 34300 centimeters per second
    // Distance = (Speed of sound * Pulse duration) / 2
    return (float)(pulse_duration * 34300) / (2 * 1000000); // Convert microseconds to seconds (10^6) and calculate distance in centimeters
}

/**
 * @brief Measure the duration of the ultrasonic pulse and return in microseconds.
 *
 * @return The pulse duration in microseconds.
 */
uint64_t measurePulse()
{
    // Activate the ultrasonice sensor by sending a brief HIGH signal (pulse) on the trigger pin 
    gpio_put(TRIGGER_GPIO_PIN, 1); // Set TRIGGER pin to HIGH
    sleep_us(10);                  // Keep it High for 10 microseconds
    gpio_put(TRIGGER_GPIO_PIN, 0); // Set TRIGGER pin back to LOW

    // Initialised the pulse width measurement variable
    uint64_t pulse_width = 0;

    // Wait for the ECHO pin to go HIGH, indicating the start of the ultrasonic pulse
    while (gpio_get(ECHO_GPIO_PIN) == 0) {
        tight_loop_contents();
    }
    
    //Record the start time
    absolute_time_t startTime = get_absolute_time(); 

    // Measure the duration of the ECHO pulse (high signal )
    while (gpio_get(ECHO_GPIO_PIN) == 1) 
    {
        // Increment the pulse width measurement
        pulse_width++;

        // Delay for 1 microsecond
        sleep_us(1);

        //
        if (pulse_width > TIMEOUT) {
            printf("Pulse Width, %lld\n", pulse_width);
            return 0;
        }
    }
    // Record the end time
    absolute_time_t endTime = get_absolute_time();
    
    // Calculateand return the pulse duration in microseconds.
    return absolute_time_diff_us(startTime, endTime);
}

/**
 * @brief Calculate the distance in centimeters based on the pulse duration.
 *
 * @param pulseLength The duration of the ultrasonic pulse in microseconds.
 * @return The distance in centimeters.
 */
uint64_t calculateDistanceCm(uint64_t pulseLength)
{
    /*
        Distance (in centimeter) = Speed of sound * 
        pulseLength - time duration measured by the ultrasonic sensor 
        29 - speed of sound in centimeter per microsecond
        2  - to account the two-way travel of the pulse ( travel to the object and back to the sensor)
    */
    return pulseLength / 29 / 2;
}


