/**
 * @file main.c
 * @brief Main Program File
 *
 * This file contains the main program for controlling a robotic vehicle with various sensors,
 * including infrared sensor, ultrasonic sensor, motor, wheel encoder.
 *
 * @date October 27, 2023
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
#include "pico/time.h"
#include <string.h>
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "motor.h"
#include "infrared.h"
#include "ultrasonic_sensor.h"
#include "wifi.h"

// Define GPIO pin for wheel encoder
#define ENCODER_LEFT_PIN 2
#define ENCODER_RIGHT_PIN 3
// Define GPIO pin for infrared line and barcode sensors
#define LEFT_LINE_SENSOR_PIN 6
#define RIGHT_LINE_SENSOR_PIN 7
#define BARCODE_SENSOR_PIN 8
// Define GPIO pin for both motors
#define LEFT_MOTOR_PIN1 14
#define LEFT_MOTOR_PIN2 13
#define RIGHT_MOTOR_PIN1 11
#define RIGHT_MOTOR_PIN2 12
#define LEFT_MOTOR_PWM_PIN 15
#define RIGHT_MOTOR_PWM_PIN 10

uint32_t ultraval = 100;
bool count_notches = false;
uint32_t notch_arr[100];
uint32_t currentNotchCount = 0;
uint32_t tempNotchCount = 0;
const static char *START_COUNTING_NOTCHES = "g";
const static char *STOP_COUNTING_NOTCHES = "h";
int arr3D[40][40] = {0};
int currentLoc[2] = {0, 0};
int finalLoopVal = 0;

int leCounter = 0;

// WIFI Commands
const static char *MOVE_FORWARD = "w";
const static char *MOVE_BACKWARD = "s";
const static char *TURN_LEFT = "a";
const static char *TURN_RIGHT = "d";
const static char *STOP = "x";
const static char *START_SCAN = "p";
const static char *STOP_SCAN = "o";
const static char *SCAN_LEFT = "l";
const static char *SCAN_RIGHT = "r";

int currentDir = 1;

double front_heading = 0.0;
struct repeating_timer left_encoder_cool_down_timer;
struct repeating_timer right_encoder_cool_down_timer;
struct repeating_timer left_infrared_cool_down_timer;
struct repeating_timer right_infrared_cool_down_timer;

uint32_t countOfArr = 0;

volatile uint32_t left_last_pulse_time = 0;
volatile uint32_t right_last_pulse_time = 0;

bool left_line_tiggered = false;
bool right_line_tiggered = false;

// Function prototypes
bool ultrasonic_sensor_handler();
uint32_t runUltrasonic();
bool reset_left_encoder_cool_down(struct repeating_timer *t);
bool reset_right_encoder_cool_down(struct repeating_timer *t);
bool reset_left_infrared_cool_down(struct repeating_timer *t);
bool reset_right_infrared_cool_down(struct repeating_timer *t);
bool check_wifi_status(struct repeating_timer *t);

/**
 * @brief Control the robotic vehicle based on Wi-Fi commands.
 *
 * This function interprets Wi-Fi commands and triggers corresponding movements
 * or actions of the robotic vehicle. Supported commands include moving forward,
 * moving backward, turning left, turning right, stopping, and initiating/terminating scanning.
 *
 * @param recv_buffer A buffer containing the received Wi-Fi command.
 */
void motor_control(char *recv_buffer)
{
    // Print the command received from the client
    printf("Received command: %s\n", recv_buffer);

    // Move forward when command received is "w"
    if (recv_buffer[0] == MOVE_FORWARD[0])
    {
        printf("Moving forward\n");
        move_forward(6250);
    }
    // Move backward when command received is "s"
    else if (recv_buffer[0] == MOVE_BACKWARD[0])
    {
        printf("Moving backward\n");
        move_backward(6250);
    }
    // Turn left when command received is "a"
    else if (recv_buffer[0] == TURN_LEFT[0])
    {
        printf("Turning left\n");
        turn_left(6250, 90);
    }
    // Turn right when command received is "d"
    else if (recv_buffer[0] == TURN_RIGHT[0])
    {
        printf("Turning right\n");
        turn_right(6250, 90);
    }
    // Stop when command received is "x"
    else if (recv_buffer[0] == STOP[0])
    {
        printf("Stopping\n");
        stop_motors();
    }
    // Start scanning for barcode when command received is "p"
    else if (recv_buffer[0] == START_SCAN[0])
    {
        printf("Stopping\n");
        toggleBarcode = true;
        move_forward(6250);
    }
    // Stop scanning for barcode when command received is "o"
    else if (recv_buffer[0] == STOP_SCAN[0])
    {
        printf("Stopping\n");
        toggleBarcode = false;
        move_forward(6250);
    }
    // Start counting notches when the command received is "g"
    else if (recv_buffer[0] == START_COUNTING_NOTCHES[0])
    {
        count_notches = true;
        countOfArr = 0;
        currentNotchCount = left_encoder_count;
    }
    // Scan left when the command received is "l"
    else if (recv_buffer[0] == SCAN_LEFT[0])
    {
        tempNotchCount = left_encoder_count;

        notch_arr[countOfArr] = tempNotchCount - currentNotchCount;
        countOfArr++;
        notch_arr[countOfArr] = 1;
        countOfArr++;
        currentNotchCount = tempNotchCount;
    }
    // Scan right when the command received is "r"
    else if (recv_buffer[0] == SCAN_RIGHT[0])
    {
        tempNotchCount = left_encoder_count;
        notch_arr[countOfArr] = tempNotchCount - currentNotchCount;
        countOfArr++;
        notch_arr[countOfArr] = 2;
        countOfArr++;
        currentNotchCount = tempNotchCount;
    }
    // Stop counting notches when the command received is "h"
    else if (recv_buffer[0] == STOP_COUNTING_NOTCHES[0])
    {
        tempNotchCount = left_encoder_count;
        notch_arr[countOfArr] = tempNotchCount - currentNotchCount;
        countOfArr++;
        currentNotchCount = tempNotchCount;

        // Loop through the received notches and update the 2D array
        for (int i = 0; i < notch_arr[0]; i++)
        {
            arr3D[20][20 - i] = 1;

            finalLoopVal = 20 - i;
        }
        currentLoc[0] = 20;
        currentLoc[1] = finalLoopVal;
        finalLoopVal = 0;
        for (int i = 1; i < sizeof(notch_arr) / sizeof(notch_arr[0]); i++)
        {
            if (i % 2 != 0)
            {
                printf("notch_arr[i]=%d\n", notch_arr[i]);
                if (notch_arr[i] == 1)
                {
                    if (currentDir == 1)
                    {
                        currentDir = 2;
                    }
                    else if (currentDir == 2)
                    {
                        currentDir = 3;
                    }
                    else if (currentDir == 3)
                    {
                        currentDir = 4;
                    }
                    else
                    {
                        currentDir = 1;
                    }
                }
                else if (notch_arr[i] == 2)
                {
                    if (currentDir == 1)
                    {
                        currentDir = 4;
                    }
                    else if (currentDir == 2)
                    {
                        currentDir = 1;
                    }
                    else if (currentDir == 3)
                    {
                        currentDir = 2;
                    }
                    else
                    {
                        currentDir = 3;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                printf("%d,%d\n", currentLoc[0], currentLoc[1]);
                printf("%d\n", currentDir);
                if (currentDir == 1)
                {
                    for (int j = 0; j < notch_arr[i]; j++)
                    {
                        arr3D[currentLoc[0]][currentLoc[1] - j] = 1;
                        finalLoopVal = currentLoc[1] - j;
                    }
                    currentLoc[1] = finalLoopVal;
                    finalLoopVal = 0;
                }
                else if (currentDir == 2)
                {
                    for (int j = 0; j < notch_arr[i]; j++)
                    {
                        arr3D[currentLoc[0] - j][currentLoc[1]] = 1;
                        finalLoopVal = currentLoc[0] - j;
                    }
                    currentLoc[0] = finalLoopVal;
                    finalLoopVal = 0;
                }
                else if (currentDir == 4)
                {
                    for (int j = 0; j < notch_arr[i]; j++)
                    {
                        arr3D[currentLoc[0] + j][currentLoc[1]] = 1;
                        finalLoopVal = currentLoc[0] + j;
                    }
                    currentLoc[0] = finalLoopVal;
                    finalLoopVal = 0;
                }
                else
                {
                    for (int j = 0; j < notch_arr[i]; j++)
                    {
                        arr3D[currentLoc[0]][currentLoc[1] + j] = 1;
                        finalLoopVal = currentLoc[1] + j;
                    }

                    currentLoc[1] = finalLoopVal;
                    finalLoopVal = 0;
                    currentDir = 1;
                }
            }
        }

        for (int i = 0; i < sizeof(arr3D) / sizeof(arr3D[0]); i++)
        {
            for (int j = 0; j < sizeof(arr3D[i]) / sizeof(arr3D[i][0]); j++)
            {
                printf("%d", arr3D[i][j]);
            }
            printf("\n");
        }

        for (int i = 0; i < countOfArr; i++)
        {
            printf("%d", notch_arr[i]);
            notch_arr[i] = 0;
        }
        printf("\n");
    }

    for (int i = 0; i < sizeof(arr3D) / sizeof(arr3D[0]); i++)
    {
        for (int j = 0; j < sizeof(arr3D[i]) / sizeof(arr3D[i][0]); j++)
        {
            arr3D[i][j] = 0;
        }
    }
    currentDir = 1;
    finalLoopVal = 0;
}

/**
 * @brief Handle interrupts from various sensors.
 *
 * This function handles interrupts from encoders, line sensors, barcode sensor, and ultrasonic sensor.
 * Use only "if" as all the different types of interrupts are not mutually exclusive
 *
 * @param gpio The GPIO pin number generating the interrupt.
 * @param events The type of events triggering the interrupt.
 */
void interrupt_handler(uint gpio, uint32_t events)
{
    // Interrupt for the
    if (gpio == ENCODER_LEFT_PIN)
    {
        //  Left encoder triggered time
        uint32_t current_time = time_us_32();
        uint32_t time_since_last_pulse = current_time - left_last_pulse_time;
        
        // Check if it's a rising edge, indicating a pulse
        if (events == GPIO_IRQ_EDGE_RISE)
        {
            left_encoder_count++;
        }

        // Calculate the speed of the left encoder based on time between pulses
        if (time_since_last_pulse != 0)
        {
            left_encoder_speed = 1000000.0 / time_since_last_pulse;
        }
        
        // Update the time of the last pulse
        left_last_pulse_time = current_time;

        // Disable the interrupt for 100ms to prevent multiple interrupts from being triggered
        gpio_set_irq_enabled_with_callback(ENCODER_LEFT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, &interrupt_handler);
        // Add a repeating timer for 200ms to reset the left encoder cooldown
        add_repeating_timer_ms(200, reset_left_encoder_cool_down, NULL, &left_encoder_cool_down_timer);
    }

    if (gpio == ENCODER_RIGHT_PIN)
    {
        // Right encoder triggered time
        uint32_t current_time = time_us_32();
        uint32_t time_since_last_pulse = current_time - right_last_pulse_time;
        
        // Check if it's a rising edge, indicating a pulse
        if (events == GPIO_IRQ_EDGE_RISE)
        {
            // Increment the right encoder count for each rising edge
            right_encoder_count++;
        }
        
        // Calculate the speed of the right encoder based on time between pulses
        if (time_since_last_pulse != 0)
        {
            right_encoder_speed = 1000000.0 / time_since_last_pulse;
        }

        // Calculate the total degrees turned by the right encoder
        float degrees_turned = right_encoder_count * 4.6;

        // Check if the turn has reached the desired angle for the right movement
        if (degrees_turned > set_heading && movement_direction == 'd')
        {
            // Stop the motors if the desired angle is reached
            stop_motors();
        }
        
        // Update the time of the last pulse
        right_last_pulse_time = current_time;

        // Disable the interrupt for 100ms to prevent multiple interrupts from being triggered
        gpio_set_irq_enabled_with_callback(ENCODER_RIGHT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, &interrupt_handler);
        // Add a repeating timer for 200ms to reset the right encoder cooldown
        add_repeating_timer_ms(200, reset_right_encoder_cool_down, NULL, &right_encoder_cool_down_timer);
    }

    if (gpio == LEFT_LINE_SENSOR_PIN)
    {
        if (events == GPIO_IRQ_EDGE_RISE)
        {
            // Left line sensor triggered on a rising edge
            left_line_tiggered = true;

            // Check the current state of leCounter for the appropriate action
            if (leCounter == 1)
            {
            // First trigger: Set leCounter to 2 and perform left turn followed by forward movement   
                leCounter = 2;
                turn_left(6250, 90);
                move_forward(6250);
            }
            else if (leCounter == 2)
            {
            // Second trigger: Set leCounter to 3 and perform left turn followed by forward movement
                leCounter = 3;
                turn_left(6250, 90);
                move_forward(6250);
            }
            else
            {
            // Any other trigger: Perform right turn followed by forward movement
                turn_right(6250, 90);
                move_forward(6250);
            }
        }

        else if (events == GPIO_IRQ_EDGE_FALL)
        {
            left_line_tiggered = false;
        }

        // Disable the interrupt for 100ms to prevent multiple interrupts from being triggered
        gpio_set_irq_enabled_with_callback(LEFT_LINE_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, &interrupt_handler);
        // Add a repeating timer for 200ms to reset the left infrared cooldown
        add_repeating_timer_ms(200, reset_left_infrared_cool_down, NULL, &left_infrared_cool_down_timer);
    }

    if (gpio == RIGHT_LINE_SENSOR_PIN)
    {
        if (events == GPIO_IRQ_EDGE_RISE)
        {
            // The right line sensor signal has risen which mean it is black
            right_line_tiggered = true;
        }
        else if (events == GPIO_IRQ_EDGE_FALL)
        {
            // The right line sensor signal has fallen which mean it is white
            right_line_tiggered = false;
        }

        // Disable the interrupt for 100ms to prevent multiple interrupts from being triggered
        gpio_set_irq_enabled_with_callback(RIGHT_LINE_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, &interrupt_handler);
        // Add a repeating timer for 200ms to reset the right infrared cooldown
        add_repeating_timer_ms(200, reset_right_infrared_cool_down, NULL, &right_infrared_cool_down_timer);
    }

    if (gpio == BARCODE_SENSOR_PIN)
    {
         // If the event is on the barcode sensor pin, call the barcode sensor handler
        barcode_sensor_handler(gpio, events);
    }

    // Check if both left and right line sensors are triggered
    if (left_line_tiggered && right_line_tiggered)
    {
        // Print a message and stop the motors if both line sensors are triggered
        printf("Both line sensors triggered\n");
        stop_motors();
    }

    if (gpio == ECHO_PIN)
    {
        // If the event is on the echo pin, call the function to handle changes on that pin
        on_echo_pin_change(gpio, events);
    }
}

/**
 * @brief Handle ultrasonic sensor events.
 *
 * This function handles events related to the ultrasonic sensor.
 *
 * @return true if the ultrasonic sensor event is successfully handled.
 */
bool ultrasonic_sensor_handler()
{
    float pulse_duration = getPulseDuration(); // Measure the duration of the echo pulse
    if (pulse_duration != 0.0)
    {
        float distance_cm = calculateDistance(pulse_duration); // Calculate distance in centimeters
        if (distance_cm <= 5.0)
        {
            printf("Too close to a wall\n");
            // U-turn
            move_backward(LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2);
        }
    }

    return true;
}

/**
 * @brief Run the ultrasonic sensor and return the distance in centimeters.
 *
 * This function measures the pulse duration and calculates the distance in centimeters.
 *
 * @return The distance in centimeters.
 */
uint32_t runUltrasonic()
{
    uint32_t pulse_duration = measurePulse();
    // Measure distance in centimeters
    uint32_t distance_cm = calculateDistanceCm(pulse_duration);
    // Print the distance.
    printf("Distance: %d cm\n", distance_cm);

    return distance_cm;
}

/**
 * @brief Reset left encoder cooldown period.
 *
 * This function resets the cooldown period for the left encoder interrupt.
 *
 * @param t A pointer to the repeating timer structure.
 * @return true if the cooldown period is successfully reset.
 */
bool reset_left_encoder_cool_down(struct repeating_timer *t)
{
    gpio_set_irq_enabled_with_callback(ENCODER_LEFT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    // disable the timer
    cancel_repeating_timer(t);
    return true;
}

/**
 * @brief Reset right encoder cooldown period.
 *
 * This function resets the cooldown period for the right encoder interrupt.
 *
 * @param t A pointer to the repeating timer structure.
 * @return true if the cooldown period is successfully reset.
 */
bool reset_right_encoder_cool_down(struct repeating_timer *t)
{
    gpio_set_irq_enabled_with_callback(ENCODER_RIGHT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    cancel_repeating_timer(t);
    return true;
}

/**
 * @brief Reset left infrared sensor cooldown period.
 *
 * This function resets the cooldown period for the left infrared sensor interrupt.
 *
 * @param t A pointer to the repeating timer structure.
 * @return true if the cooldown period is successfully reset.
 */
bool reset_left_infrared_cool_down(struct repeating_timer *t)
{
    gpio_set_irq_enabled_with_callback(LEFT_LINE_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    cancel_repeating_timer(t);
    return true;
}

/**
 * @brief Reset right infrared sensor cooldown period.
 *
 * This function resets the cooldown period for the right infrared sensor interrupt.
 *
 * @param t A pointer to the repeating timer structure.
 * @return true if the cooldown period is successfully reset.
 */
bool reset_right_infrared_cool_down(struct repeating_timer *t)
{
    gpio_set_irq_enabled_with_callback(RIGHT_LINE_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    cancel_repeating_timer(t);
    return true;
}

/**
 * @brief Check the Wi-Fi status using a repeating timer.
 *
 * This function checks the Wi-Fi status and is called by a repeating timer.
 *
 * @param t A pointer to the repeating timer structure.
 * @return true if the Wi-Fi status is successfully checked.
 */
bool check_wifi_status(struct repeating_timer *t)
{
    cyw43_arch_poll(); // Poll for Wi-Fi driver or lwIP work
    return true;
}

/**
 * @brief Main function of the program.
 *
 * @return 0 upon successful execution.
 */
int main()
{
    // Initialize the stdio driver
    stdio_init_all();

    // Initialize the motor
    initialise_motors(LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2, LEFT_MOTOR_PWM_PIN, RIGHT_MOTOR_PWM_PIN, ENCODER_LEFT_PIN, ENCODER_RIGHT_PIN);

    // Initialize the Wi-Fi driver
    start_wifi();

    // Initialize the infrared sensors
    initialise_infrared(LEFT_LINE_SENSOR_PIN, RIGHT_LINE_SENSOR_PIN, BARCODE_SENSOR_PIN);

    // Initialize the ultrasonic sensor
    initialise_ultrasonic();

    // Enable interrupt on line sensor and barcode sensor
    gpio_set_irq_enabled_with_callback(LEFT_LINE_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    gpio_set_irq_enabled_with_callback(RIGHT_LINE_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    gpio_set_irq_enabled_with_callback(BARCODE_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);

    // Enable interrupt on encoder
    gpio_set_irq_enabled_with_callback(ENCODER_LEFT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);
    gpio_set_irq_enabled_with_callback(ENCODER_RIGHT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interrupt_handler);

    // Configure the repeating timer to poll the Wi-Fi driver
    struct repeating_timer check_wifi;
    add_repeating_timer_ms(-500, check_wifi_status, NULL, &check_wifi);

    // Configure the repeating timer to poll the PID_controller
    struct repeating_timer pid_timer;
    add_repeating_timer_ms(50, pid_control, NULL, &pid_timer);

    // struct repeating_timer ultrasonic_timer;
    // add_repeating_timer_ms(-50, &ultrasonic_sensor_handler, NULL, &ultrasonic_timer);

    while (1)
    {
        ultraval = runUltrasonic();
        if (ultraval < 20)
        {
            turn_right(LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2, 180);
        }
        sleep_ms(100);
        cyw43_arch_poll(); // Poll for Wi-Fi driver or lwIP work

        // TODO: Mapping algorithm
    }

    return 0;
}