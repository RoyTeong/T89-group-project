/* 
        Project Scope : Uses an infrared (IR) sensor to sense the distance between white and black bar on a barcode.
                      : Pico's ADC reads the analog output from the IR sensor connected to GPIO Pin 26
                      : and generates a binary representation of the barcode pattern
                Ouput : Individual Character - 'W', 'w', 'B', 'b' (refer to Barcode pattern comment)
                      :        Binary String - 0 is black, 1 is white
                      :      Barcode Pattern - 'W' and 'w' represent white, 'B' and 'b' represent black.
                      :                      - 'W' and 'B' represent bigger block, 'w' and 'b' represent smaller block
    Last Updated Date : 30/October/2023
*/

// Import the required libraries
#include <stdio.h>        // Standard C library for I/O Operations
#include "pico/stdlib.h"  // Pico Standard library for hardware access
#include "hardware/adc.h" // Pico ADC hardware access for Analog-to-Digital Converter
#include "pico/time.h"    // Pico time-related functions 
#include <string.h>       // Standard C library for string manipulation

// Define constant
#define IR_SENSOR_PIN 26            // GPIO pin connected to the IR sensor
#define ADC_SAMPLE_INTERVAL_MS 100 // Sampling interval for IR sensor (100ms)
#define MAX_ARRAY_SIZE 100         // Maximum size for the sensor valus array

// Variables to track and store the letter of the barcode (W,w,B,b)
char barcode_letter_data_array[1000];        // Array to store the barcode pattern
int current_barcode_pattern_count = 1;       // Variable to track the number of each barcode bar type (white or black) up to now
                                             // -ve for white, +ve for black
volatile uint16_t ir_sensor_value;           // Variable to store the IR sensor value

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
    // Perform ADC reading from the IR sensor using ADC channel 0
    adc_select_input(0); 
    ir_sensor_value = adc_read();

    // Update the sensor_values array with the 1 for black and update the white and based on the previous count
    if (ir_sensor_value > 1000)   // If ir_sensor_value is more than 1000, it is considered as part of a barcode "black" region
    {
        // Replace the corresponding index position in the sensor_values array with 1 to represent barcode is now black
        sensor_values[sensor_values_count++] = 1;

        // If the current_barcode_pattern_count is less than 0, then the previous colour detected is white
        if (current_barcode_pattern_count < 0)
        {
            // If more than 2 consecutive white line (based on sensor_values array)
            if (current_barcode_pattern_count < -2)
            {
                // This mean it is a big white block, so append "W" to barcode_letter_data_array
                strcat(barcode_letter_data_array, "W");
                // Print the "W"
                printf("W\n");
            }
            // If less than 2 consecutive white line (based on sensor_values array), 
            else
            {
                // This mean it is a small white block, so append "w" to barcode_letter_data_array
                strcat(barcode_letter_data_array, "w");
                // Print the "w" 
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
    
   // Update the sensor_values array with the 0 for white and update the black and based on the previous count
    else // Else ir_sensor_vale is less than 1000, it is considered as part of a barcode "white" region
    {
        // Replace the corresponding index position in the sensor_values array with 1 to represent barcode is now black
        sensor_values[sensor_values_count++] = 0;

        // If the current_barcode_pattern_count is more than 0, then the previous colour detected is black
        if (current_barcode_pattern_count > 0)
        {
            // If more than 2 consecutive black line (based on sensor_values array), 
            if (current_barcode_pattern_count > 2)
            {
                // This mean it is a big black block, so append "B" to barcode_letter_data_array
                strcat(barcode_letter_data_array, "B");
                // Print the "B"
                printf("B\n");
            }
            // If less than 2 consecutive black line (based on sensor_values array), 
            else
            {
                // This mean it is a small black block, so append "b" to barcode_letter_data_array
                strcat(barcode_letter_data_array, "b");
                // Print the "b"
                printf("b\n");
            }
            //Reset current_barcode_pattern_count to -1
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
            // Converting the sensor_values array from int to char, to create the string. 
            binaryString[i] = (sensor_values[i] == 0) ? '0' : '1';
        }
        binaryString[100] = '\0'; // Null-terminate the string

        // Output the binary string
        printf("Binary String: %s\n", binaryString);                    // Print the binary String (0, 1)
        printf("Binary String: %s\n", barcode_letter_data_array);       // Print the letter string (W, w, B, b)

        //Reset sensor_values_count to 0 to reset the array
        sensor_values_count = 0;
    }

    // Print the current sensor value
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    uint32_t milliseconds = current_time / 1000;

    return true;
}

int main() {
    // Initialise standard I/O for console output
    stdio_init_all(); 
    // Set up the ADC for IR Sensor readings
    adc_setup();      
    
    // Set up a timer interrupt for IR sensor sampling every 100 ms
    struct repeating_timer timer;

    // Run the ir_sensor_reading_for_barcode function every 100ms interval
    add_repeating_timer_ms(ADC_SAMPLE_INTERVAL_MS,  ir_sensor_reading_for_barcode, NULL, &timer);

    while (true) {
    }

    return 0;
}
