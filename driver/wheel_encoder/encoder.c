// Include necessary libraries and header files
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Define GPIO pins for PWM signals, motor directions, etc.
#define motorPWMR 0     // Right Motor's PWM pin
#define motorPWML 1     // Left Motor's PWM pin

#define motorDirR01 2   // Right motor's direction pin 1
#define motorDirR02 3   // Right motor's direction pin 2
#define motorDirL01 5   // Left motor's direction pin 1
#define motorDirL02 4   // Left motor's direction pin 2

#define ENCODER_PIN 6   // GPIO Pin for encoder input

#define PPR 20                                // Example: 360 pulses per wheel revolution
#define WHEEL_CIRCUMFERENCE 3.141592654 * 7.0 // Example: 20 cm

volatile uint32_t pulse_count = 0;
volatile uint32_t last_pulse_time = 0;

void encoder_isr()
{
    pulse_count++;
    uint32_t current_time = time_us_32();
    uint32_t time_since_last_pulse = current_time - last_pulse_time;

    if (time_since_last_pulse > 0)
    {
        float speed_hz = 1.0 / (time_since_last_pulse * 1e-6);                // Speed in Hz
        float speed_cm_per_sec = speed_hz * (WHEEL_CIRCUMFERENCE / PPR);      // Speed in cm/s
        float distance_cm = (float)pulse_count * (WHEEL_CIRCUMFERENCE / PPR); // Distance in cm
        printf("Distance: %.2f cm, Speed: %.2f cm/s\n", distance_cm, speed_cm_per_sec);
    }

    last_pulse_time = current_time;
}

void turnRight(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B)
{
    // Turn Right
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 2);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 2);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);

    // Wait for a specified duration (2000 milliseconds)
    sleep_ms(2000);
}

void turnLeft(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B)
{
    // Turn Left
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 2);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 2);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 1);
    gpio_put(motorDirL01, 0);
    gpio_put(motorDirL02, 0);

    // Wait for a specified duration (2000 milliseconds)
    sleep_ms(2000);
}

void goBackwards(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B)
{
    // Set PWM and motor directions for backward motion
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 2);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 2);
    gpio_put(motorDirR01, 0);
    gpio_put(motorDirR02, 1);
    gpio_put(motorDirL01, 0);
    gpio_put(motorDirL02, 1);

    // Wait for a specified duration (2000 milliseconds)
    sleep_ms(2000);
}

void goForwards(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B)
{
    // Set PWM and motor directions for forward motion
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 1);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 1);
    gpio_put(motorDirR01, 1);
    gpio_put(motorDirR02, 0);
    gpio_put(motorDirL01, 1);
    gpio_put(motorDirL02, 0);

    // Wait for a specified duration (4000 milliseconds)
    sleep_ms(4000);
}

int main()
{

    gpio_init(ENCODER_PIN);
    gpio_set_dir(ENCODER_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &encoder_isr);
    // Initialize standard I/O
    stdio_init_all();

    // Initialize GPIO pins for motor directions
    gpio_init(motorDirR01);
    gpio_init(motorDirR02);
    gpio_init(motorDirL01);
    gpio_init(motorDirL02);

    // Set GPIO directions to output
    gpio_set_dir(motorDirR01, GPIO_OUT);
    gpio_set_dir(motorDirR02, GPIO_OUT);
    gpio_set_dir(motorDirL01, GPIO_OUT);
    gpio_set_dir(motorDirL02, GPIO_OUT);

    // Set GPIO functions for PWM
    gpio_set_function(motorPWMR, GPIO_FUNC_PWM);
    gpio_set_function(motorPWML, GPIO_FUNC_PWM);

    // Get PWM slice numbers for the specified GPIO pins
    // Here I am creating 2 PWM, that way we can control both wheels at
    // different speed. Example for turning.
    uint slice_num01 = pwm_gpio_to_slice_num(motorPWMR);
    uint slice_num02 = pwm_gpio_to_slice_num(motorPWML);

    // Configure PWM settings (clock divider and wrap value)

    pwm_set_clkdiv(slice_num01, 100);
    pwm_set_wrap(slice_num01, 12500);
    pwm_set_clkdiv(slice_num02, 100);
    pwm_set_wrap(slice_num02, 12500);

    // Enable PWM slices
    pwm_set_enabled(slice_num01, true);
    pwm_set_enabled(slice_num02, true);

    // Set initial PWM duty cycle levels
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 2);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 2);

    // Main control loop
    while (1)
    {
        //  Turn Right
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500 / 2);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500 / 2);
        gpio_put(motorDirR01, 0);
        gpio_put(motorDirR02, 0);
        gpio_put(motorDirL01, 1);
        gpio_put(motorDirL02, 0);

        // Turn right for 2s
        sleep_ms(2000);

        // Max speed for 4s
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500/1);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500/1);
        sleep_ms(4000);
    }
}
