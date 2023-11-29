/**
 * @file motor.h
 * @brief Header file for wheel and encoder 
 * @details This file contains declaration for functions and variables relate
 * 
 * @date October 27, 2023
 */


#ifndef MOTOR_H
#define MOTOR_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <stdbool.h>
#include "pico/time.h"
#include <math.h>

#include "magnetometer.h"

// Global variables for GPIO Pin
uint8_t left_encoder_pin = 2; 
uint8_t right_encoder_pin = 3;
uint8_t motor_enable_pin_A = 15;
uint8_t motor_enable_pin_B = 10;
uint8_t input_1 = 14;
uint8_t input_2 = 13;
uint8_t input_3 = 12;
uint8_t input_4 = 11;

// Function prototypes
void move_forward(float speed);
void move_backward(float speed);
void turn_left(float speed, float angle);
void turn_right(float speed, float angle);
void stop_motors();
void initialise_motors(uint8_t left_motor_pin1, uint8_t left_motor_pin2, uint8_t right_motor_pin1, uint8_t right_motor_pin2, uint8_t left_motor_pwm_pin, uint8_t right_motor_pwm_pin, uint8_t encoder_left_pin, uint8_t encoder_right_pin);

float start_heading = 0.0;
float current_heading = 0.0;
float target_heading = 0.0;
float set_heading = 0.0;
volatile int left_encoder_count = 0;
volatile int right_encoder_count = 0;
double left_encoder_speed = 0.0;
double right_encoder_speed = 0.0;

volatile char movement_direction = 'x'; // w = forward, s = backward, a = left, d = right, x = stop

float kp = 0.1;  // Proportional gain
float ki = 0.01; // Integral gain
float kd = 0.01; // Derivative gain
float I = 0.0;   // Integral term
float P = 0.0;   // Proportional term
float D = 0.0;   // Derivative term

#define SPEED 6250
#endif // MOTOR_H

/**
 * @brief Function to set the speed of the left and right motors.
 * @param left_motor_speed Speed of the left motor.
 * @param right_motor_speed Speed of the right motor.
 */
void set_speed(float left_motor_speed, float right_motor_speed)
{
    // Set the PWM channels
    pwm_set_gpio_level(motor_enable_pin_A, left_motor_speed);
    pwm_set_gpio_level(motor_enable_pin_B, right_motor_speed);
}

/**
 * @brief Function to calculate a new heading after a turn.
 * @param current_heading Current heading.
 * @param angle Angle of the turn.
 * @param is_left_turn Boolean indicating if it's a left turn.
 * @return The new heading after the turn.
 */
float calculate_new_heading(float current_heading, float angle, bool is_left_turn)
{
    // Check if it's a left turn
    if (!is_left_turn)
    {
        // Calculate the new heading after a right turn
        float new_heading = current_heading + angle;

        // Normalize the heading to be within [0, 360) degrees
        if (new_heading > 360)
        {
            new_heading = new_heading - 360;
        }

        // Return the new heading after a right turn
        return new_heading;
    }
    else
    {
        // Calculate the new heading after a left turn
        float new_heading = current_heading - angle;
        
        // Normalize the heading to be within [0, 360) degrees
        if (new_heading < 0)
        {
            new_heading = 360 + new_heading;
        }
        // Return the new heading after a left turn
        return new_heading;
    }
}

/**
 * @brief Function for PID control of motor movement.
 * @return True if the control is successful, false otherwise.
 */
bool pid_control()
{
    if (movement_direction == 'w')
    {
        // Get current heading
        current_heading = get_heading();

        // Calculate the error
        float error = 0;
        float left_wheel_speed = left_encoder_speed;
        float right_wheel_speed = right_encoder_speed;
        float wheel_speed_error = left_wheel_speed - right_wheel_speed;

        // Calculate the PID
        P = kp * error;
        I = I + ki * error;
        D = kd * wheel_speed_error;

        float PID = P + I + D;

        // Calculate the new motor speeds
        if (PID > 0)
        {
            // Turn left
            float left_motor_speed = SPEED + PID;
            float right_motor_speed = SPEED;
            
            // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else if (PID < 0)
        {
            // update pid to positive
            PID = fabs(PID);
            float left_motor_speed = SPEED;
            float right_motor_speed = SPEED + PID;

             // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else
        {
            // Maintain the same heading by moving straight
            set_speed(SPEED, SPEED);
        }
        
        // Indicate successful PID control
        return true;
    }
    else if (movement_direction == 's')
    {
        // Get current heading
        current_heading = get_heading();

        // Calculate the error
        float error = 0;
        float left_wheel_speed = left_encoder_speed;
        float right_wheel_speed = right_encoder_speed;
        float wheel_speed_error = left_wheel_speed - right_wheel_speed;

        // Calculate the PID
        P = kp * error;
        I = I + ki * error;
        D = kd * wheel_speed_error;

        float PID = P + I + D;

        // Print diagnostic information
        printf("PID: %f\n", PID);
        printf("Left wheel speed: %f\n", left_wheel_speed);
        printf("Right wheel speed: %f\n", right_wheel_speed);
        printf("heading: %f\n", current_heading);

        // Calculate the new motor speeds
        if (PID < 0)
        {
            // Turn left
            float left_motor_speed = SPEED + PID;
            float right_motor_speed = SPEED;

            // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else if (PID > 0)
        {
            // Change pid to positive and turn right
            PID = fabs(PID);
            float left_motor_speed = SPEED;
            float right_motor_speed = SPEED + PID;
            
            // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else
        {
            // Maintain the same heading
            set_speed(SPEED, SPEED);
        }

        return true;
    }
    else if (movement_direction == 'a')
    {
        // Get current heading
        current_heading = get_heading();

        // Calculate the error
        // float error = target_heading - current_heading;
        float error = 0;
        float left_wheel_speed = left_encoder_speed;
        float right_wheel_speed = right_encoder_speed;
        float wheel_speed_error = left_wheel_speed - right_wheel_speed;

        // Calculate the PID
        P = kp * error;
        I = I + ki * error;
        D = kd * wheel_speed_error;

        float PID = P + I + D;

        printf("PID: %f\n", PID);
        printf("Left wheel speed: %f\n", left_wheel_speed);
        printf("Right wheel speed: %f\n", right_wheel_speed);
        printf("heading: %f\n", current_heading);
        printf("target heading: %f\n", target_heading);

        // Calculate the new motor speeds
        if (PID > 0)
        {
            // Turn left
            float left_motor_speed = SPEED + PID;
            float right_motor_speed = SPEED;
            
            // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else if (PID < 0)
        {
            // change pid to positive
            PID = fabs(PID);
            float left_motor_speed = SPEED;
            float right_motor_speed = SPEED + PID;
            
            // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else
        {
            // Maintain the same heading
            set_speed(SPEED, SPEED);
        }

        float heading_difference = fabs(current_heading - target_heading);
        if (heading_difference < 20.0)
        {
            stop_motors();
        }

        return true;
    }
    else if (movement_direction == 'd')
    {
        // Get current heading
        current_heading = get_heading();

        // Calculate the error
        float error = 0;
        float left_wheel_speed = left_encoder_speed;
        float right_wheel_speed = right_encoder_speed;
        float wheel_speed_error = left_wheel_speed - right_wheel_speed;

        // Calculate the PID
        P = kp * error;
        I = I + ki * error;
        D = kd * wheel_speed_error;

        float PID = P + I + D;

        printf("PID: %f\n", PID);
        printf("Left wheel speed: %f\n", left_wheel_speed);
        printf("Right wheel speed: %f\n", right_wheel_speed);
        printf("heading: %f\n", current_heading);
        printf("target heading: %f\n", target_heading);

        // Calculate the new motor speeds
        if (PID < 0)
        {
            // Turn left
            float left_motor_speed = SPEED + PID;
            float right_motor_speed = SPEED;
            
            // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else if (PID > 0)
        {
            // Update PID value to positive
            PID = fabs(PID);
            float left_motor_speed = SPEED;
            float right_motor_speed = SPEED + PID;
            
            // Set the new motor speeds
            set_speed(left_motor_speed, right_motor_speed);
        }
        else
        {
            // Maintain the same heading
            set_speed(SPEED, SPEED);
        }

        float heading_difference = fabs(current_heading - target_heading);
        if (heading_difference < 5.0)
        {
            stop_motors();
        }

        return true;
    }
    return true;
}

/**
 * @brief Function to reset various values (encoder counts, PID variables, heading, etc.).
 */
void reset_values()
{
    // Reset the encoder counts
    left_encoder_count = 0;
    right_encoder_count = 0;

    // Reset the encoder speeds
    left_encoder_speed = 0.0;
    right_encoder_speed = 0.0;

    // Reset the PID variables
    I = 0.0;
    P = 0.0;
    D = 0.0;

    // Reset the heading variables
    start_heading = 0.0;
    current_heading = 0.0;
    target_heading = 0.0;
}

/**
 * @brief Function to move the robot forward.
 * @param speed Speed of the movement.
 */
void move_forward(float speed)
{
    // Reset Values
    reset_values();

    // Set the GPIO pins to high
    gpio_put(input_1, 1);
    gpio_put(input_2, 0);
    gpio_put(input_3, 0);
    gpio_put(input_4, 1);

    set_speed(speed, speed);

    // Set the PWM channels
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_A), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_B), true);

    // Get the current heading
    start_heading = get_heading();
    target_heading = calculate_new_heading(start_heading, 0, false); // Maintain the same heading
    movement_direction = 'w';
}

/**
 * @brief Function to move the robot backward.
 * @param speed Speed of the movement.
 */
void move_backward(float speed)
{
    // Reset Values
    reset_values();

    // Set the GPIO pins to high
    gpio_put(input_1, 0);
    gpio_put(input_2, 1);
    gpio_put(input_3, 1);
    gpio_put(input_4, 0);

    set_speed(speed, speed);

    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_A), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_B), true);

    // Get the current heading
    start_heading = get_heading();
    target_heading = calculate_new_heading(start_heading, 0, false); // Maintain the same heading
    movement_direction = 's';
}

/**
 * @brief Function to turn the robot left.
 * @param speed Speed of the turn.
 * @param angle Angle of the turn.
 */
void turn_left(float speed, float angle)
{
    // Reset Values
    reset_values();

    // Set the GPIO pins to high
    gpio_put(input_1, 0);
    gpio_put(input_2, 1);
    gpio_put(input_3, 0);
    gpio_put(input_4, 1);

    set_speed(speed, speed);

    // Set the PWM channels
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_A), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_B), true);

    // Get the current heading
    start_heading = get_heading();
    target_heading = calculate_new_heading(start_heading, angle, true);
    set_heading = angle;
    movement_direction = 'a';
}

/**
 * @brief Function to turn the robot right.
 * @param speed Speed of the turn.
 * @param angle Angle of the turn.
 */
void turn_right(float speed, float angle)
{
    // Reset Values
    reset_values();

    // Set the GPIO pins to high
    gpio_put(input_1, 1);
    gpio_put(input_2, 0);
    gpio_put(input_3, 1);
    gpio_put(input_4, 0);

    set_speed(speed, speed);

    // Set the PWM channels
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_A), true);
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_B), true);

    // Get the current heading
    start_heading = get_heading();
    target_heading = calculate_new_heading(start_heading, angle, false);
    set_heading = angle;
    movement_direction = 'd';
}

/**
 * @brief Function to stop the motors.
 */
void stop_motors()
{
    // Set the GPIO pins to low
    gpio_put(input_1, 0);
    gpio_put(input_2, 0);
    gpio_put(input_3, 0);
    gpio_put(input_4, 0);

    set_speed(0, 0);

    // Set the PWM channels
    pwm_set_chan_level(pwm_gpio_to_slice_num(motor_enable_pin_A), PWM_CHAN_A, 0);
    pwm_set_chan_level(pwm_gpio_to_slice_num(motor_enable_pin_B), PWM_CHAN_A, 0);

    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_A), false);
    pwm_set_enabled(pwm_gpio_to_slice_num(motor_enable_pin_B), false);

    reset_values();

    // Reset the movement direction
    movement_direction = 'x';
}

/**
 * @brief Function to initialize the motors and encoder pins.
 * @param left_motor_pin1 Pin 1 of the left motor.
 * @param left_motor_pin2 Pin 2 of the left motor.
 * @param right_motor_pin1 Pin 1 of the right motor.
 * @param right_motor_pin2 Pin 2 of the right motor.
 * @param left_motor_pwm_pin PWM pin for the left motor.
 * @param right_motor_pwm_pin PWM pin for the right motor.
 * @param encoder_left_pin Encoder pin for the left motor.
 * @param encoder_right_pin Encoder pin for the right motor.
 */
void initialise_motors(uint8_t left_motor_pin1, uint8_t left_motor_pin2, uint8_t right_motor_pin1, uint8_t right_motor_pin2, uint8_t left_motor_pwm_pin, uint8_t right_motor_pwm_pin, uint8_t encoder_left_pin, uint8_t encoder_right_pin)
{
    // change the global variables
    left_encoder_pin = encoder_left_pin;
    right_encoder_pin = encoder_right_pin;
    motor_enable_pin_A = left_motor_pwm_pin;
    motor_enable_pin_B = right_motor_pwm_pin;
    input_1 = left_motor_pin1;
    input_2 = left_motor_pin2;
    input_3 = right_motor_pin1;
    input_4 = right_motor_pin2;

    // Initialize the GPIO pins for the motors
    gpio_init(left_encoder_pin);
    gpio_init(right_encoder_pin);
    gpio_init(motor_enable_pin_A);
    gpio_init(motor_enable_pin_B);
    gpio_init(input_1);
    gpio_init(input_2);
    gpio_init(input_3);
    gpio_init(input_4);

    // Set the GPIO pins to output
    gpio_set_dir(left_encoder_pin, GPIO_IN);
    gpio_set_dir(right_encoder_pin, GPIO_IN);
    gpio_set_dir(input_1, GPIO_OUT);
    gpio_set_dir(input_2, GPIO_OUT);
    gpio_set_dir(input_3, GPIO_OUT);
    gpio_set_dir(input_4, GPIO_OUT);

    // Configure PWM
    gpio_set_function(motor_enable_pin_A, GPIO_FUNC_PWM);
    gpio_set_function(motor_enable_pin_B, GPIO_FUNC_PWM);

    uint slice_A = pwm_gpio_to_slice_num(motor_enable_pin_A);
    uint slice_B = pwm_gpio_to_slice_num(motor_enable_pin_B);

    pwm_set_clkdiv(slice_A, 100.0); // set the clock divider to 100
    pwm_set_clkdiv(slice_B, 100.0); // set the clock divider to 100

    pwm_set_wrap(slice_A, 12500); // set the wrap to 12500
    pwm_set_wrap(slice_B, 12500); // set the wrap to 12500

    pwm_set_chan_level(slice_A, PWM_CHAN_A, 12500 / 2);
    pwm_set_chan_level(slice_B, PWM_CHAN_A, 12500 / 2);

    pwm_set_enabled(slice_A, false);
    pwm_set_enabled(slice_B, false);

    // initialize magnetometer
    initialise_magnetometer();
}