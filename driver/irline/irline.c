/*
        Project Scope : Reading values from an IR sensor at a regular intervals 
                      : Print those values along with timestamps
               Output : hh:mm:ss:sss - Print the time in hour, miniute, second, microsecond and the IR Sensor Value
    Last Updated Date : 31/October/2023
*/

// Import the required libraries 
#include <stdio.h>          // Standard C library for I/O Operations
#include "pico/stdlib.h"    // Pico SDK standard library for initialisation and I/O
#include "hardware/adc.h"   // Pico SDK hardware library for ADC (Analog-to-Digital Conversion)
#include "pico/time.h"      // Pico SDK library for time-related functions 

// Define constant value 
#define IR_SENSOR_PIN 26           // GPIO pin connected to the IR sensor
#define ADC_SAMPLE_INTERVAL_MS 25  // Sampling interval for IR sensor (25ms)

/* 
    ir_sensor_value Store the value read from IR sensor.
    Volatile - indicate that the value can change asynchronously during the timer interrupt
*/
volatile uint16_t ir_sensor_value;


// Function to set up the ADC for reading the IR sensor
void adc_setup() {
    adc_init();
    adc_gpio_init(IR_SENSOR_PIN);
}

// The function reads the IR sensor value and prints it along with a timestamp
bool read_and_print_IR_sensory_value(struct repeating_timer *t) {
    
    // Perform ADC reading from the IR sensor using ADC channel 0
    adc_select_input(0); 
    ir_sensor_value = adc_read();

    // Calculate the current time in miliseconds since Pico booted
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    uint32_t current_time_in_ms = current_time / 1000;

    // Print the time in the format hh:mm:ss:sss" and IR sensor value 
    // ---------------------------------------------------------------
    // Format the output for the time
    printf("%02d:%02d:%02d.%03d -> IR Sensor Value: %d\n", 
           // hours - Converts total miliseconds to hours, mod 24 to ensure that the result is limited to 0-23
           (int)(current_time_in_ms / 3600000) % 24,      
           // minutes - Converting the total miliseconds to minutes, %60 to ensure that the result is limited to 0-59
           (int)((current_time_in_ms / 60000) % 60),      
           // seconds - Converts total miliseconds to seconds, %60 - to ensure that it is within 0-59
           (int)((current_time_in_ms / 1000) % 60),       
           // miliseconds - %1000 to isolate the miliseconds from the seconds, minutes and hours
           (int)(current_time_in_ms % 1000),              
           ir_sensor_value);

    return true;
}

int main() {
    // Initialise standard I/O for console output
    stdio_init_all(); 
    // Set up the ADC for IR Sensor readings
    adc_setup();      
    
    // Set up a timer interrupt for IR sensor sampling every 25 ms
    struct repeating_timer timer;
     // Run the read_and_print_IR_sensory_value function every 25ms interval
    add_repeating_timer_ms(ADC_SAMPLE_INTERVAL_MS, read_and_print_IR_sensory_value, NULL, &timer);

    while (true) {
    }

    return 0;
}
