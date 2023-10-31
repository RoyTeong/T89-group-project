/* 
        Project Scope : An ultrasonic distance measurement system using a Pico microcontroller and ultrasonic sensor
                      : Initialising the ultrasonic sensor, 
                      : Mesuring the duration of ultrasonic pulse
                      : Calculating the distance based on the pulse duration. 
                      : Program continuously measures and displahy the distance in centimeters. 
               
                Ouput : Continuous stream of distance measurements in centimeters printed to serial console.
                      : Disance: X cm  --> X is the distance in centimeters
                    
    Last Updated Date : 31/October/2023
*/


// Import the neccessary libraries
#include <stdio.h>          // Standard C library for I/O Operations
#include "pico/stdlib.h"    // Pico Standard library for hardware access
#include "hardware/gpio.h"  // To work with GPIO pins on Raspberry Pi Pico.
#include "hardware/dma.h"   // Provide access to the Direct Memory Access (DMA) controller on the pico.
#include "hardware/timer.h" // Give access to the timer hardware on the PIco. Can be used for various timing and interval-based tasks.

//
#define TRIGGER_GPIO_PIN 15
#define ECHO_GPIO_PIN 14
#define TIMEOUT 26100 //

void gpio_init() {

    //Initialised the the GPIO pin for Ultrasonic sensor
    gpio_init(TRIGGER_GPIO_PIN);
    gpio_init(ECHO_GPIO_PIN);

    // Set TRIGGER_GPIO_PIN, GPIO pin 15 as output
    gpio_set_dir(TRIGGER_GPIO_PIN, GPIO_OUT);

    // Set ECHO_GPIO_PIN, GPIO pin 14 as input
    gpio_set_dir(ECHO_GPIO_PIN, GPIO_IN);

    // Initialize the Pico SDK
    stdio_init_all();
}

// Measure the duration of the ultrasonic pulse and return in microseconds
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

// Calculate the distance in cm based on the pulse duration
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

int main() {
    // Initialize gpio pin for the ultrasonic sensor
    gpio_init()

    // 
    while (1) {
        // Measure the pulse duration
        uint32_t pulse_duration = measurePulse(); 
        // Measure distance in centimeters
        uint32_t distance_cm = calculateDistanceCm(pulse_duration); 
        // Print the distance. 
        printf("Distance: %d cm\n", distance_cm);
        // Sleep 1 second before measuring again
        sleep_ms(1000); 
    }

    return 0;
}
