/* 
    Project Scope     : This code is designed for controlling the car with two wheels, allowing it to move in various direction
    Ouput             : Motor will move forward, backward, left and right                               
    Last Updated Date : 31/October/2023
*/


// Include necessary libraries and header files
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Define GPIO pins for PWM signals and motor directions
#define motorPWMR 0
#define motorPWML 1

#define motorDirR01 2
#define motorDirR02 3
#define motorDirL01 5
#define motorDirL02 4


void turnRight(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B){
    // Turn Right
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500/2);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500/2);
        gpio_put(motorDirR01, 0);
        gpio_put(motorDirR02, 0);
        gpio_put(motorDirL01, 1);
        gpio_put(motorDirL02, 0);

        // Wait for a specified duration (2000 milliseconds)
        sleep_ms(2000);
}

void turnLeft(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B){
    // Turn Left
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500/2);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500/2);
        gpio_put(motorDirR01, 0);
        gpio_put(motorDirR02, 1);
        gpio_put(motorDirL01, 0);
        gpio_put(motorDirL02, 0);

        // Wait for a specified duration (2000 milliseconds)
        sleep_ms(2000);
}

void goBackwards(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B){
    // Set PWM and motor directions for backward motion
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500/2);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500/2);
        gpio_put(motorDirR01, 0);
        gpio_put(motorDirR02, 1);
        gpio_put(motorDirL01, 0);
        gpio_put(motorDirL02, 1);

        // Wait for a specified duration (2000 milliseconds)
        sleep_ms(2000);
}

void goForwards(int slice_num01, enum pwm_chan PWM_CHAN_A, uint slice_num02, enum pwm_chan PWM_CHAN_B){
    // Set PWM and motor directions for forward motion
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500/1);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500/1);
        gpio_put(motorDirR01, 1);
        gpio_put(motorDirR02, 0);
        gpio_put(motorDirL01, 1);
        gpio_put(motorDirL02, 0);

        // Wait for a specified duration (4000 milliseconds)
        sleep_ms(4000);
}

int main() {
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
    pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500);
    pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500);

    // Main control loop
    while(1) {
        //goForwards(slice_num01, PWM_CHAN_A, slice_num02, PWM_CHAN_B);
        //goBackwards(slice_num01, PWM_CHAN_A, slice_num02, PWM_CHAN_B);
        //turnLeft(slice_num01, PWM_CHAN_A, slice_num02, PWM_CHAN_B);
        //turnRight(slice_num01, PWM_CHAN_A, slice_num02, PWM_CHAN_B);

        // Turn Right
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500);
        gpio_put(motorDirR01, 0);
        gpio_put(motorDirR02, 0);
        gpio_put(motorDirL01, 1);
        gpio_put(motorDirL02, 0);

        // Turn Right max speed for 2s
        sleep_ms(2000);

        // Turn Left3
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500);
        gpio_put(motorDirR01, 0);
        gpio_put(motorDirR02, 1);
        gpio_put(motorDirL01, 0);
        gpio_put(motorDirL02, 0);

        // Turn Left max speed for 2s
        sleep_ms(2000);

        // Set PWM and motor directions for backward motion
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500);
        gpio_put(motorDirR01, 0);
        gpio_put(motorDirR02, 1);
        gpio_put(motorDirL01, 0);
        gpio_put(motorDirL02, 1);

        // backwards max speed for 2s
        sleep_ms(2000);

        // Set PWM and motor directions for forward motion
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500);
        gpio_put(motorDirR01, 1);
        gpio_put(motorDirR02, 0);
        gpio_put(motorDirL01, 1);
        gpio_put(motorDirL02, 0);

        // Forward max speed for 2s
        sleep_ms(2000);

        // Set PWM and motor directions for forward motion
        pwm_set_chan_level(slice_num01, PWM_CHAN_A, 12500/2);
        pwm_set_chan_level(slice_num02, PWM_CHAN_B, 12500/2);
        gpio_put(motorDirR01, 1);
        gpio_put(motorDirR02, 0);
        gpio_put(motorDirL01, 1);
        gpio_put(motorDirL02, 0);

        // Forward half speed for 2s
        sleep_ms(2000);
        
    }
}
