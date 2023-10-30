#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/time.h"
#include <string.h>

#define IR_SENSOR_PIN 26
#define ADC_SAMPLE_INTERVAL_MS 100  // 25 ms
#define MAX_ARRAY_SIZE 100

int tempVal = 1;
char TotalVal[1000];

volatile uint16_t ir_sensor_value;

// Define an array to store the sensor values
uint16_t sensor_values[MAX_ARRAY_SIZE];
size_t sensor_values_count = 0;

void adc_setup() {
    adc_init();
    adc_gpio_init(IR_SENSOR_PIN);
}

bool timer_callback(struct repeating_timer *t) {
    // Perform ADC reading from the IR sensor
    adc_select_input(0); // Use ADC channel 0, you can change this if needed
    ir_sensor_value = adc_read();
    // Store the value in the array and print when it's full
    if (ir_sensor_value >1000){
        sensor_values[sensor_values_count++] = 1;
        if (tempVal < 0){
            if (tempVal < -2){
                strcat(TotalVal, "W");
                printf("W\n");
            }
            else{
                strcat(TotalVal, "w");
                printf("w\n");
            }
            tempVal = 1;
        }
        else{
            tempVal +=1;
        }
    }
    else{
        sensor_values[sensor_values_count++] = 0;
        if (tempVal > 0){
            if (tempVal > 2){
                strcat(TotalVal, "B");
                printf("B\n");
            }
            else{
                strcat(TotalVal, "b");
                printf("b\n");
            }
            tempVal = -1;
        }
        else{
            tempVal -=1;
        }
        
    }
    

    if (sensor_values_count == MAX_ARRAY_SIZE){
        // Print the array content and reset it
        char binaryString[101];
        for (int i = 0; i < 100; i++) {
            binaryString[i] = (sensor_values[i] == 0) ? '0' : '1';
        }
        binaryString[100] = '\0'; // Null-terminate the string

    // Output the binary string
    
    printf("Binary String: %s\n", binaryString);
    printf("Binary String: %s\n", TotalVal);
    sensor_values_count = 0;
    }

    // Print the current sensor value
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    uint32_t milliseconds = current_time / 1000;

    //printf("%02d:%02d:%02d.%03d -> IR Sensor Value: %d\n",
           //(int)(milliseconds / 3600000) % 24,
           //(int)((milliseconds / 60000) % 60),
           //(int)((milliseconds / 1000) % 60),
           //(int)(milliseconds % 1000),
           //ir_sensor_value);

    return true;
}

int main() {
    stdio_init_all();
    adc_setup();

    // Set up a timer interrupt for IR sensor sampling every 25 ms
    struct repeating_timer timer;
    add_repeating_timer_ms(ADC_SAMPLE_INTERVAL_MS, timer_callback, NULL, &timer);

    while (true) {
    }

    return 0;
}