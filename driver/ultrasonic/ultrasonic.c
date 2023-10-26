#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Define pins for ultrasonic sensor
#define TRIG_PIN 0
#define ECHO_PIN 1

int timeout = 26100;

void setupUltrasonicPins(uint trigPin, uint echoPin) {
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

volatile absolute_time_t pulseStartTime;
volatile absolute_time_t pulseEndTime;

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

uint64_t getCm(uint trigPin, uint echoPin) {
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    return pulseLength / 29 / 2; // Convert to centimeters
}

uint64_t getInch(uint trigPin, uint echoPin) {
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    return pulseLength / 74.f / 2.f; // Convert to inches
}

int main() {
    stdio_init_all();

    uint trigPin = 0; // Replace with your trigger pin number
    uint echoPin = 1; // Replace with your echo pin number

    setupUltrasonicPins(trigPin, echoPin);

    while (1) {
        uint64_t distanceCm = getCm(trigPin, echoPin);
        uint64_t distanceInch = getInch(trigPin, echoPin);

        printf("Distance: %lld cm, %lld inches\n", distanceCm, distanceInch);

        sleep_ms(1000); // Wait for 1 second before the next measurement
    }

    return 0;
}
