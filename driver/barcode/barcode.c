/* 
    Project Scope   : Uses an infrared (IR) sensor to sense the distance between white and black bars on a barcode.
                    : Pico's ADC reads the analog output from the IR sensor connected to GPIO Pin 26
                    : and generates a binary representation of the barcode pattern
                    : White using the -ve value, Black using the +ve value
    Ouput           : Individual Character - 'W', 'w', 'B', 'b' (refer to Barcode pattern comment)
                    :        Binary String - 0 is black, 1 is white
                    :      Barcode Pattern - 'W' and 'w' represent white, 'B' and 'b' represent black.
                    :                      - 'W' and 'B' represent bigger block, 'w' and 'b' represent smaller block
                                
    Last Update Date: 30/October/2023
*/

// Import the required libraries
#include <stdio.h>        // Standard C library for I/O Operations
#include "pico/stdlib.h"  // Pico Standard library for hardware access
#include "hardware/adc.h" // Pico ADC hardware access for Analog-to-Digital Converter
#include "pico/time.h"    // Pico time-related functions 
#include <string.h>       // Standard C library for string manipulation

// Define constant
#define IR_SENSOR_PIN 26           // GPIO pin connected to the IR sensor
#define ADC_SAMPLE_INTERVAL_MS 100 // Sampling interval for IR sensor (100ms)
#define MAX_ARRAY_SIZE 100         // Maximum size for the sensor valus array

// Global variable
int current_barcode_pattern_count_count = 1; // Variable to track the number of each barcode bar type (white or black). -ve for white, +ve for black
char barcode_letter_data_array[1000]; // Array to store the barcode pattern
volatile uint16_t ir_sensor_value; // Variable to store the IR sensor value

// Define an array to store the sensor values
uint16_t sensor_values[MAX_ARRAY_SIZE]; // Variable to track the barcode bar type (0 or 1)
size_t sensor_values_count = 0;         // Count number of elements in the sensor_values array

// Function to set up the ADC for reading the IR sensor
void adc_setup()
{
    adc_init();                         //Initialise ADC
    adc_gpio_init(IR_SENSOR_PIN);       //Associate with the specific GPIO pin used for IR sensor
}

// Timer callback function for IR sensor sampling
bool ir_sensor_reading_for_barcode (struct repeating_timer *t)
{
    // Perform ADC reading from the IR sensor
    adc_select_input(0); // Use ADC channel 0, you can change this if needed
    ir_sensor_value = adc_read();

    // Store the value in the array and print when it's full

    // If ir_sensor_value is more than 1000, it is considered as part of a barcode "black" region
    // Update the sensor_values array with the 1 for black and update the white and based on the previous count 
    if (ir_sensor_value > 1000)
    {
        // Replace the corresponding position in the sensor_values arraywith 1 to represent black
        sensor_values[sensor_values_count++] = 1;

        // Check if the number of 
        if (current_barcode_pattern_count_count < 0)
        {
            // If more than 2 consecutive white line (based on sensor_values array), 
            if (current_barcode_pattern_count_count < -2)
            {
                // When the value is big block white, append W to the barcode_letter_data_array
                strcat(barcode_letter_data_array, "W");
                printf("W\n");
            }
            // If less than 2 consecutive black line (based on sensor_values array), 
            else
            {
                // When the value is small block white, append w to the barcode_letter_data_array
                strcat(barcode_letter_data_array, "w");
                printf("w\n");
            }
            //Reset current_barcode_pattern_count to 1
            current_barcode_pattern_count = 1;
        }
        // Switch to black 
        else
        {
            current_barcode_pattern_count += 1;
        }
    }
    // Else if ir_sensor_vale is less than 1000, it is considered as part of a barcode "white" region
    // Update the sensor_values array with the 0 for white and update the black and based on the previous count 
    else
    {
        // Replace the corresponding position in the sensor_values array with 0 to represent black
        sensor_values[sensor_values_count++] = 0;
        // 
        if (current_barcode_pattern_count > 0)
        {
            // If more than 2 consecutive black line (based on sensor_values array), 
            if (current_barcode_pattern_count > 2)
            {
                // When the value is big block black, append B to the barcode_letter_data_array
                strcat(barcode_letter_data_array, "B");
                printf("B\n");
            }
            // If less than 2 consecutive black line (based on sensor_values array), 
            else
            {
                // When the value is small block black, append b to the barcode_letter_data_array
                strcat(barcode_letter_data_array, "b");
                printf("b\n");
            }
            //Reset current_barcode_pattern_count to 0-1
            current_barcode_pattern_count = -1;
        }
        // Switch to white
        else
        {
            current_barcode_pattern_count -= 1;
        }
    }

    // When the sensor_values_count reaches MAX_ARRAY_SIZE
    if (sensor_values_count == MAX_ARRAY_SIZE)
    {
        // Print the array content and reset it
        char binaryString[101];
        for (int i = 0; i < 100; i++)
        {
            // Converting the sensor_values
            binaryString[i] = (sensor_values[i] == 0) ? '0' : '1';
        }
        binaryString[100] = '\0'; // Null-terminate the string

        // Output the binary string
        printf("Binary String: %s\n", binaryString);
        printf("Binary String: %s\n", barcode_letter_data_array);

        //Reset sensor_values_count to 0 to reset the array
        sensor_values_count = 0;
    }

    // Print the current sensor value
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    uint32_t milliseconds = current_time / 1000;

    return true;
}

int main() {
    stdio_init_all(); // Initialise standard I/O for console output
    adc_setup();      // Set up the ADC for IR Sensor readings
    

    // Set up a timer interrupt for IR sensor sampling every 100 ms
    struct repeating_timer timer;
    add_repeating_timer_ms(ADC_SAMPLE_INTERVAL_MS,  ir_sensor_reading_for_barcode, NULL, &timer);

    while (true) {
    }

    return 0;
}