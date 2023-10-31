/*


*/

//#include "ultrasonic_sensor.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/timer.h"

#define TRIGGER_GPIO_PIN 15
#define ECHO_GPIO_PIN 14
#define TIMEOUT 26100

void ultrasonic_init() {

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
    //
    return pulseLength / 29 / 2;
}

int main() {
    // Initialize ultrasonic sensor
    ultrasonic_init(); 


    // 
    while (1) {
        // Measure the pulse duration
        uint32_t pulse_duration = measurePulse(); 
        // Measure distance in centimeters
        uint32_t distance_cm = calculateDistanceCm(pulse_duration); 
        // Print the distance. 
        printf("Distance: %d cm\n", distance_cm);
        sleep_ms(1000); // Wait for 1 second before measuring again
    }

    return 0;
}
