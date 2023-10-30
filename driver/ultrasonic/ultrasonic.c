#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

// Define pins for ultrasonic sensor
#define TRIG_PIN 0
#define ECHO_PIN 1

// Define a timeout value (in microseconds) for the ultrasonic sensor
int timeout = 26100;

// Variables to handle the ultrasonic sensor's echo signal
volatile absolute_time_t pulseStartTime;
volatile absolute_time_t pulseEndTime;

// Function to set up the GPIO pins for the ultrasonic sensor
void setupUltrasonicPins(uint trigPin, uint echoPin) {
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

uint64_t getPulse(uint trigPin, uint echoPin)
{
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);

    uint64_t width = 0;

    while (gpio_get(echoPin) == 0) tight_loop_contents();
    absolute_time_t startTime = get_absolute_time();
    while (gpio_get(echoPin) == 1) 
    {
        width++;
        sleep_us(1);
        if (width > timeout) return 0;
    }
    absolute_time_t endTime = get_absolute_time();
    
    return absolute_time_diff_us(startTime, endTime);
}

// Function to calculate distance in centimeters
uint64_t getCm(uint trigPin, uint echoPin) {
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    // Convert pulse length to distance in centimeters
    return pulseLength / 29 / 2;
}

int main() {
    stdio_init_all();

    // Define the TRIG and ECHO pins
    uint trigPin = TRIG_PIN;
    uint echoPin = ECHO_PIN;

    // Initialize the GPIO pins for the ultrasonic sensor
    setupUltrasonicPins(trigPin, echoPin);

    // Initialize pulseStartTime and pulseEndTime to the current time
    pulseStartTime = get_absolute_time();
    pulseEndTime = pulseStartTime;

    while (1) {
        // Get the distance in centimeters
        uint64_t distanceCm = getCm(trigPin, echoPin);

        // Print the measured distance
        printf("Distance: %lld cm\n", distanceCm);

        // Wait for 1 second before the next measurement
        sleep_ms(1000);
    }

    return 0;
}
