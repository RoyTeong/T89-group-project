#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

// Define pins for ultrasonic sensor
#define TRIG_PIN 0
#define ECHO_PIN 1

int timeout = 26100;

volatile bool echoReceived = false;
volatile absolute_time_t pulseStartTime;
volatile absolute_time_t pulseEndTime;

void setupUltrasonicPins(uint trigPin, uint echoPin) {
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
}

void echo_isr(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        pulseEndTime = get_absolute_time();
        echoReceived = true;
    }
}

uint64_t getPulse(uint trigPin, uint echoPin) {
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);

    echoReceived = false;

    // Enable the interrupt on the falling edge of the echo signal
    gpio_set_irq_enabled_with_callback(echoPin, GPIO_IRQ_EDGE_FALL, true, echo_isr);

    // Wait for the echo signal to be received
    while (!echoReceived) {
        tight_loop_contents();
    }

    // Calculate the pulse length
    return absolute_time_diff_us(pulseStartTime, pulseEndTime);
}

uint64_t getCm(uint trigPin, uint echoPin) {
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    return pulseLength / 29 / 2; // Convert to centimeters
}

int main() {
    stdio_init_all();

    uint trigPin = 0; // Replace with your trigger pin number
    uint echoPin = 1; // Replace with your echo pin number

    setupUltrasonicPins(trigPin, echoPin);

    // Initialize pulseStartTime and pulseEndTime to the current time
    pulseStartTime = get_absolute_time();
    pulseEndTime = pulseStartTime;

    while (1) {
        uint64_t distanceCm = getCm(trigPin, echoPin);

        printf("Distance: %lld cm ", distanceCm);

        sleep_ms(1000); // Wait for 1 second before the next measurement
    }

    return 0;
}
