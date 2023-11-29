/**
 * @file infrared.h
 * @brief Header file for barcode decoding and line sensing.
 * @details This file contains declaration for functions and variables relate
 * 
 * @date October 27, 2023
 */

// Define GPIO PIN for IR Sensors
#define LEFT_LINE_SENSOR_PIN 6
#define RIGHT_LINE_SENSOR_PIN 7
#define BARCODE_SENSOR_PIN 8

// Define timing and timeouts values
#define TICKS_PER_MICROSECOND 1
#define TIMEOUT_MICROSECONDS 5000000 // 5 seconds timeout

// Barcode definitions that are patterns to identify the different characters
#define BARCODE_A "111010100010111"
#define BARCODE_B "101110100010111"
#define BARCODE_C "111011101000101"
#define BARCODE_D "101011100010111"
#define BARCODE_E "111010111000101"
#define BARCODE_F "101110111000101"
#define BARCODE_G "101010001110111"
#define BARCODE_H "111010100011101"
#define BARCODE_I "101110100011101"
#define BARCODE_J "101011100011101"
#define BARCODE_K "111010101000111"
#define BARCODE_L "101110101000111"
#define BARCODE_M "111011101010001"
#define BARCODE_N "101011101000111"
#define BARCODE_O "111010111010001"
#define BARCODE_P "101110111010001"
#define BARCODE_Q "101010111000111"
#define BARCODE_R "111010101110001"
#define BARCODE_S "101110101110001"
#define BARCODE_T "101011101110001"
#define BARCODE_U "111000101010111"
#define BARCODE_V "100011101010111"
#define BARCODE_W "111000111010101"
#define BARCODE_X "100010111010111"
#define BARCODE_Y "111000101110101"
#define BARCODE_Z "100011101110101"
#define BARCODE_0 "101000111011101"
#define BARCODE_1 "111010001010111"
#define BARCODE_2 "101110001010111"
#define BARCODE_3 "111011100010101"
#define BARCODE_4 "101000111010111"
#define BARCODE_5 "111010001110101"
#define BARCODE_6 "101110001110101"
#define BARCODE_7 "101000101110111"
#define BARCODE_8 "111010001011101"
#define BARCODE_9 "101110001011101"
#define BARCODE_DASH "100010101110111"
#define BARCODE_PERIOD "111000101011101"
#define BARCODE_DOLLAR "100010001000101"
#define BARCODE_SLASH "100010001010001"
#define BARCODE_PLUS "100010100010001"
#define BARCODE_PERCENT "101000100010001"
#define BARCODE_SPACE "100011101011101"
#define BARCODE_ASTERISK "100010111011101"
#define BARCODE_A_B "111010001010111"
#define BARCODE_B_B "111010001011101"
#define BARCODE_C_B "101000101110111"
#define BARCODE_D_B "111010001110101"
#define BARCODE_E_B "101000111010111"
#define BARCODE_F_B "101000111011101"
#define BARCODE_G_B "111011100010101"
#define BARCODE_H_B "101110001010111"
#define BARCODE_I_B "101110001011101"
#define BARCODE_J_B "101110001110101"
#define BARCODE_K_B "111000101010111"
#define BARCODE_L_B "111000101011101"
#define BARCODE_M_B "100010101110111"
#define BARCODE_N_B "111000101110101"
#define BARCODE_O_B "100010111010111"
#define BARCODE_P_B "100010111011101"
#define BARCODE_Q_B "111000111010101"
#define BARCODE_R_B "100011101010111"
#define BARCODE_S_B "100011101011101"
#define BARCODE_T_B "100011101110101"
#define BARCODE_U_B "111010101000111"
#define BARCODE_V_B "111010101110001"
#define BARCODE_W_B "101010111000111"
#define BARCODE_X_B "111010111010001"
#define BARCODE_Y_B "101011101000111"
#define BARCODE_Z_B "101011101110001"
#define BARCODE_0_B "101110111000101"
#define BARCODE_1_B "111010100010111"
#define BARCODE_2_B "111010100011101"
#define BARCODE_3_B "101010001110111"
#define BARCODE_4_B "111010111000101"
#define BARCODE_5_B "101011100010111"
#define BARCODE_6_B "101011100011101"
#define BARCODE_7_B "111011101000101"
#define BARCODE_8_B "101110100010111"
#define BARCODE_9_B "101110100011101"
#define BARCODE_DASH_B "111011101010001"
#define BARCODE_PERIOD_B "101110101000111"
#define BARCODE_DOLLAR_B "101000100010001"
#define BARCODE_SLASH_B "100010100010001"
#define BARCODE_PLUS_B "100010001010001"
#define BARCODE_PERCENT_B "100010001000101"
#define BARCODE_SPACE_B "101110101110001"

// Variables declared for IR Line Sensors
bool is_left_line_black = false; 
bool is_right_line_black = false;

// Variables declared for IR and Barcode Sensors
uint32_t black_barcode_time_start = 0;      // Time the black barcode was first detected   
uint32_t black_barcode_time_stop = 0;       // Time the black barcode was no longer detected
uint32_t white_barcode_timer_start = 0;     // Time the white barcode was first detected 
uint32_t white_barcode_timer_stop = 0;      // Time the white barcode was no longer detected
uint32_t bar_detected_duration = 0;         // Time difference between first detected and no longer detected for the specific colour barcode

uint32_t barcode_array[9000] = {};          // Store barcode data
uint32_t barcode_counter = 0;               // To keep track of the length of the barcode
uint32_t bar_thickness_divider = 999999999; // Divider to determine the thickness of the barcode
uint32_t reset_barcode= 0;                  // Restting barcode to accept new barcode
bool barcode_scanning_started = false;      // Flag to check if barcode started scanning
bool toggleBarcode = false;                 // Flag to decide whether to decode barcode or not
char barcode_string[27] = {0};              // Store the binary for the barcode that was read
char decoded_characters[50] = " "; /        // Store the character that was decoded.
int count = 0;

// GPIO pin 
uint8_t left_sensor_pin = 6;                
uint8_t right_sensor_pin = 7;
uint8_t barcode_sensor_pin = 8;


// Function Prototypes
void decode_barcode();
void measure_barcode_reading(char color[]);
void handle_barcode_sensor_events(uint gpio, uint32_t events);
void handle_lines_sensor_events(uint gpio, uint32_t events);
void retrieve_line_sensor_value();
void initialise_infrared(int8_t left_line_sensor_pin, int8_t right_line_sensor_pin, int8_t barcode_sensor_pin);



/**
 * @brief Function to decode the barcode
 *
 * @details
 * This function decodes the binary representation of the barcode array and updates the character string and additional processing based on the decoded barcode string
 * 1) It will first print the barcode array and the current character decoded.
 * 2) When the barcode array is filled with 27 interger values, then it means the barcode has finished reading.
 * 3) It will start converting the barcode array's integer value to a binary string. 2 to "0" | 222 to "000" | 1 to "1" | 111 to "111"
 * 4) Clean up barcode array to use it for reading new values
 * 5) Check if the barcode contain the predefine barcode binary string, if yes, interpret it as the character associated
 */
void decode_barcode()
{
    // Print the barcode array and the string value
    for (int i = 0; i < 27; i++)
    {
        printf("%d", barcode_array[i]);
    }
    printf("\n");
    printf("\n%s\n", decoded_characters);

    // If barcode counter is 27, the barcode has finished reading.
    if (barcode_counter == 27)
    {
        // Convert the barcode array to a binary string
        for (int i = 0; i < 27; i++)
        {
            // Convert integer 2 to string 0
            if (barcode_array[i] == 2)
            {
                strcat(barcode_string, "0");
            }
            // Convert integer 222 to string 000
            else if (barcode_array[i] == 222)
            {
                strcat(barcode_string, "000");
            }
            // Convert integer 1 to string 1
            else if (barcode_array[i] == 1)
            {
                strcat(barcode_string, "1");
            }
            // Convert integer 111 to string 111
            else if (barcode_array[i] == 111)
            {
                strcat(barcode_string, "111");
            }
        }

        // Reset the barcode array by resetting all the elements value to 0 to prepare for reading new barcode
        for (int i = 0; i < 27; i++)
        {
            barcode_array[i] = 0;
        }
        // Reset the barcode counter to 0
        barcode_counter = 0;
        // Increment the reset counter for tracking of resets
        reset_barcode++;
        // Shift the barcode string to the left by 16 characters
        strcpy(barcode_string, barcode_string + 16);
        // Remove the last 9 characters from the barcode string
        barcode_string[strlen(barcode_string) - 9] = '\0';

        // Check if the barcode string contains the predefined barcode definition for 'A' (BARCODE_A) 
        if (strstr(barcode_string, BARCODE_A) != NULL)
        {
            // If BARCODE_A is found in the barcode_string, interpret it as 'A' and stored into decoded_character
            decoded_characters[count] = 'A';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'B' (BARCODE_B) 
        else if (strstr(barcode_string, BARCODE_B) != NULL)
        {
            // If BARCODE_B is found in the barcode_string, interpret it as 'B' and stored into decoded_character
            decoded_characters[count] = 'B';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'C' (BARCODE_C) 
        else if (strstr(barcode_string, BARCODE_C) != NULL)
        {
            // If BARCODE_C is found in the barcode_string, interpret it as 'C' and stored into decoded_character
            decoded_characters[count] = 'C';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'D' (BARCODE_D) 
        else if (strstr(barcode_string, BARCODE_D) != NULL)
        {
            // If BARCODE_D is found in the barcode_string, interpret it as 'D' and stored into decoded_character
            decoded_characters[count] = 'D';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'E' (BARCODE_E) 
        else if (strstr(barcode_string, BARCODE_E) != NULL)
        {
            // If BARCODE_E is found in the barcode_string, interpret it as 'E' and stored into decoded_character
            decoded_characters[count] = 'E';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'F' (BARCODE_F) 
        else if (strstr(barcode_string, BARCODE_F) != NULL)
        {
            // If BARCODE_F is found in the barcode_string, interpret it as 'F' and stored into decoded_character
            decoded_characters[count] = 'F';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'G (BARCODE_G) 
        else if (strstr(barcode_string, BARCODE_G) != NULL)
        {
            // If BARCODE_G is found in the barcode_string, interpret it as 'G' and stored into decoded_character
            decoded_characters[count] = 'G';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'H' (BARCODE_H) 
        else if (strstr(barcode_string, BARCODE_H) != NULL)
        {
            // If BARCODE_H is found in the barcode_string, interpret it as 'H' and stored into decoded_character
            decoded_characters[count] = 'H';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'I' (BARCODE_I) 
        else if (strstr(barcode_string, BARCODE_I) != NULL)
        {
            // If BARCODE_I is found in the barcode_string, interpret it as 'I' and stored into decoded_character
            decoded_characters[count] = 'I';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'J' (BARCODE_J) 
        else if (strstr(barcode_string, BARCODE_J) != NULL)
        {
            // If BARCODE_J is found in the barcode_string, interpret it as 'J' and stored into decoded_character
            decoded_characters[count] = 'J';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'K' (BARCODE_K) 
        else if (strstr(barcode_string, BARCODE_K) != NULL)
        {
            // If BARCODE_K is found in the barcode_string, interpret it as 'K' and stored into decoded_character
            decoded_characters[count] = 'K';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'L' (BARCODE_L) 
        else if (strstr(barcode_string, BARCODE_L) != NULL)
        {
            // If BARCODE_L is found in the barcode_string, interpret it as 'L' and stored into decoded_character
            decoded_characters[count] = 'L';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'M' (BARCODE_M) 
        else if (strstr(barcode_string, BARCODE_M) != NULL)
        {
            // If BARCODE_M is found in the barcode_string, interpret it as 'M' and stored into decoded_character
            decoded_characters[count] = 'M';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'N' (BARCODE_N) 
        else if (strstr(barcode_string, BARCODE_N) != NULL)
        {
            // If BARCODE_N is found in the barcode_string, interpret it as 'N' and stored into decoded_character
            decoded_characters[count] = 'N';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'O' (BARCODE_O) 
        else if (strstr(barcode_string, BARCODE_O) != NULL)
        {
            // If BARCODE_O is found in the barcode_string, interpret it as 'O' and stored into decoded_character
            decoded_characters[count] = 'O';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'P' (BARCODE_P) 
        else if (strstr(barcode_string, BARCODE_P) != NULL)
        {
            // If BARCODE_P is found in the barcode_string, interpret it as 'P' and stored into decoded_character
            decoded_characters[count] = 'P';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'Q' (BARCODE_Q) 
        else if (strstr(barcode_string, BARCODE_Q) != NULL)
        {
            // If BARCODE_Q is found in the barcode_string, interpret it as 'Q' and stored into decoded_character
            decoded_characters[count] = 'Q';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'R' (BARCODE_R) 
        else if (strstr(barcode_string, BARCODE_R) != NULL)
        {n
            // If BARCODE_R is found in the barcode_string, interpret it as 'R' and stored into decoded_character
            decoded_characters[count] = 'R';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'S' (BARCODE_S) 
        else if (strstr(barcode_string, BARCODE_S) != NULL)
        {
            // If BARCODE_S is found in the barcode_string, interpret it as 'S' and stored into decoded_character
            decoded_characters[count] = 'S';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'T' (BARCODE_T) 
        else if (strstr(barcode_string, BARCODE_T) != NULL)
        {
            // If BARCODE_T is found in the barcode_string, interpret it as 'T' and stored into decoded_character
            decoded_characters[count] = 'T';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'U' (BARCODE_U) 
        else if (strstr(barcode_string, BARCODE_U) != NULL)
        {
            // If BARCODE_U is found in the barcode_string, interpret it as 'U' and stored into decoded_character
            decoded_characters[count] = 'U';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'V' (BARCODE_V) 
        else if (strstr(barcode_string, BARCODE_V) != NULL)
        {
            // If BARCODE_V is found in the barcode_string, interpret it as 'V' and stored into decoded_character
            decoded_characters[count] = 'V';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'W' (BARCODE_W) 
        else if (strstr(barcode_string, BARCODE_W) != NULL)
        {
            // If BARCODE_W is found in the barcode_string, interpret it as 'W' and stored into decoded_character
            decoded_characters[count] = 'W';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'X' (BARCODE_X) 
        else if (strstr(barcode_string, BARCODE_X) != NULL)
        {
            // If BARCODE_X is found in the barcode_string, interpret it as 'X' and stored into decoded_character
            decoded_characters[count] = 'X';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for 'Y' (BARCODE_Y) 
        else if (strstr(barcode_string, BARCODE_Y) != NULL)
        {
            // If BARCODE_Y is found in the barcode_string, interpret it as 'Y' and stored into decoded_character
            decoded_characters[count] = 'Y';
            count++;
        }
        // If barcode string contains predefined barcode(BARCODE_Z), interpret it as 'Z' and stored into decoded_character
        else if (strstr(barcode_string, BARCODE_Z) != NULL)
        {
            // If BARCODE_Z is found in the barcode_string, interpret it as 'Z' and stored into decoded_character
            decoded_characters[count] = 'Z';
            count++;
        }

        // Check if the barcode string contains the predefined barcode definition for '0' (BARCODE_0) 
        else if (strstr(barcode_string, BARCODE_0) != NULL)
        {
            // If BARCODE_0 is found in the barcode_string, interpret it as '0' and stored into decoded_character
            decoded_characters[count] = '0';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '1' (BARCODE_1) 
        else if (strstr(barcode_string, BARCODE_1) != NULL)
        {
            // If BARCODE_1 is found in the barcode_string, interpret it as '1' and stored into decoded_character
            decoded_characters[count] = '1';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '2' (BARCODE_2) 
        else if (strstr(barcode_string, BARCODE_2) != NULL)
        {
            // If BARCODE_2 is found in the barcode_string, interpret it as '2' and stored into decoded_character
            decoded_characters[count] = '2';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '3' (BARCODE_3) 
        else if (strstr(barcode_string, BARCODE_3) != NULL)
        {
            // If BARCODE_3 is found in the barcode_string, interpret it as '3' and stored into decoded_character
            decoded_characters[count] = '3';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '4' (BARCODE_4) 
        else if (strstr(barcode_string, BARCODE_4) != NULL)
        {
            // If BARCODE_4 is found in the barcode_string, interpret it as '4' and stored into decoded_character
            decoded_characters[count] = '4';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '5' (BARCODE_5) 
        else if (strstr(barcode_string, BARCODE_5) != NULL)
        {
            // If BARCODE_5 is found in the barcode_string, interpret it as '5' and stored into decoded_character
            decoded_characters[count] = '5';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '6' (BARCODE_6) 
        else if (strstr(barcode_string, BARCODE_6) != NULL)
        {
            // If BARCODE_6 is found in the barcode_string, interpret it as '6' and stored into decoded_character
            decoded_characters[count] = '6';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '7' (BARCODE_7) 
        else if (strstr(barcode_string, BARCODE_7) != NULL)
        {
            // If BARCODE_7 is found in the barcode_string, interpret it as '7' and stored into decoded_character
            decoded_characters[count] = '7';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '8' (BARCODE_8) 
        else if (strstr(barcode_string, BARCODE_8) != NULL)
        {
            // If BARCODE_8 is found in the barcode_string, interpret it as '8' and stored into decoded_character
            decoded_characters[count] = '8';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '9' (BARCODE_9) 
        else if (strstr(barcode_string, BARCODE_9) != NULL)
        {
            // If BARCODE_9 is found in the barcode_string, interpret it as '9' and stored into decoded_character
            decoded_characters[count] = '9';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '-' (BARCODE_DASH) 
        else if (strstr(barcode_string, BARCODE_DASH) != NULL)
        {
            // If BARCODE_DASH is found in the barcode_string, interpret it as '-' and stored into decoded_character
            decoded_characters[count] = '-';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '.' (BARCODE_PERIOD) 
        else if (strstr(barcode_string, BARCODE_PERIOD) != NULL)
        {
            // If BARCODE_PERIOD is found in the barcode_string, interpret it as '.' and stored into decoded_character
            decoded_characters[count] = '.';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '$' (BARCODE_DOLLAR) 
        else if ((strstr(barcode_string, BARCODE_DOLLAR) != NULL))
        {
            // If BARCODE_DOLLAR is found in the barcode_string, interpret it as '$' and stored into decoded_character
            decoded_characters[count] = '$';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '/' (BARCODE_SLASH) 
        else if ((strstr(barcode_string, BARCODE_SLASH) != NULL))
        {
            // If BARCODE_SLASH is found in the barcode_string, interpret it as '/' and stored into decoded_character
            decoded_characters[count] = '/';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '+' (BARCODE_PLUS) 
        else if ((strstr(barcode_string, BARCODE_PLUS) != NULL))
        {
            // If BARCODE_PLUS is found in the barcode_string, interpret it as '+' and stored into decoded_character
            decoded_characters[count] = '+';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for '%' (BARCODE_PERCENT) 
        else if ((strstr(barcode_string, BARCODE_PERCENT) != NULL))
        {
            // If BARCODE_PERCENT is found in the barcode_string, interpret it as '%' and stored into decoded_character
            decoded_characters[count] = '%';
            count++;
        }
        // Check if the barcode string contains the predefined barcode definition for ' ' (BARCODE_SPACE) 
        else if ((strstr(barcode_string, BARCODE_SPACE) != NULL))
        {
            // If BARCODE_SPACE is found in the barcode_string, interpret it as ' ' and stored into decoded_character
            decoded_characters[count] = ' ';
            count++;
        }
    }
}

/**
 * @brief Measures and records barcode data.
 *
 * @details
 * This function assesses the thickness of the barcode and updates the barcode_array based on the provided color.
 * If the barcode_array is full, it shifts the values to make space for new data.
 *
 * @param color The color of the barcode being measured ("black" or "white").
 */
void measure_barcode_reading(char color[])
{
    // Shift the array to the left if the barcode is complete to make space for new barcode
    if (barcode_counter == 27)
    {
        for (int i = 0; i < 8; i++)
        {
            barcode_array[i] = barcode_array[i + 1];
        }
        barcode_counter--;
    }

    // When the barcode is black, measure it using 0
    if (strcmp(color, "black") == 0)
    {

        // Calculate the time difference to get the total time the barcode is black
        bar_detected_duration = black_barcode_time_stop - black_barcode_time_start;

        // Set the divider used to determine whether the barcode is thick or thin. 
        if (barcode_counter == 0 && reset_barcode == 0)
        {
            bar_thickness_divider = bar_detected_duration + 20000;
        }

        // Determine the thickness of the barcode
        // When the barcode is thick
        if (bar_detected_duration / bar_thickness_divider > 0)
        {

            barcode_array[barcode_counter] = 111;
            barcode_counter++;
        }
        // When the barcode is thin
        else
        {
            barcode_array[barcode_counter] = 1;
            barcode_counter++;
        }
    }
    // When the barcode is white, measure it using 2
    else if (strcmp(color, "white") == 0)
    {
        // Calculate the time difference to get the total time the barcode is white
        bar_detected_duration = white_barcode_timer_stop - white_barcode_timer_start;

        // When the barcode has not started. 
        if (barcode_counter == 0)
        {
            // Barcode has not started yet
        }
        else
        {
            // Determine the thickness of the barcode
            // When the barcode is thick
            if (bar_detected_duration / bar_thickness_divider > 0)
            {
                barcode_array[barcode_counter] = 222;
                barcode_counter++;
            }
            // When the barcode is thin
            else
            {
                barcode_array[barcode_counter] = 2;
                barcode_counter++;
            }
        }
    }
}

/**
 * @brief Function to Barcode Sensor Interrupt Handler
 *
 * @details
 * If events is GPIO_IRQ_EDGE_RISE, then the barcode is black.
 * if the barcode is black, then measure the previous white barcode and decode it.
 * 
 * If events is GPIO_IRQ_EDGE_FALL, then the barcode is white.
 * If the barcode is white, then measure the previous black barcode and decode it.
 * 
 * @param gpio The GPIO pin.
 * @param events The type of GPIO event.
 */
void handle_barcode_sensor_events(uint gpio, uint32_t events)
{
    if (toggleBarcode == true)
    {
        // When the barcode detected is black
        if (events == GPIO_IRQ_EDGE_RISE && gpio == BARCODE_SENSOR_PIN)
        {
            // Start the timer for black by storing the current time as this is when the black is first detected
            black_barcode_time_start = time_us_32();

            // Stop the timer for white by storing the current time as this is when white is no logner detected
            white_barcode_timer_stop = time_us_32();

            // Start scanning for barcode
            if (barcode_scanning_start == false)
            {
                barcode_scanning_start = true;
            }

            //Measure the previous white barcode
            measure_barcode_reading("white");
        }
        // When the barcode detected is white
        else if (events == GPIO_IRQ_EDGE_FALL && gpio == BARCODE_SENSOR_PIN)
        {
            // Start the timer for white by storing the current time as this is when the white is first detected
            white_barcode_time_start = time_us_32();
            
            // Stop the timer for black because barcode detected is no longer black.
            black_barcode_time_stop = time_us_32();

            // Measure the previous black barcode
            measure_barcode_reading("black");
        }

        // Try to decode the barcode based on what is read.
        decode_barcode();
    }
}

/**
 * @brief Function to update isLeftLineBlack and isRightLineBlack boolean value based on the event that occured. 
 *
 * @details
 * If events is GPIO_IRQ_EDGE_RISE, then the line is black
 * If events is GPIO_IRQ_EDGE_FALL, then the line is white
 * Update the isLeftLineBlack and isRightLineBlack variables
 */
void handle_lines_sensor_events(uint gpio, uint32_t events)
{
    // When the right line is black
    if (events == GPIO_IRQ_EDGE_RISE && gpio == RIGHT_LINE_SENSOR_PIN)
    {
        isRightLineBlack = true;
    }
    // When the left line is black
    else if (events == GPIO_IRQ_EDGE_RISE && gpio == LEFT_LINE_SENSOR_PIN)
    {
        isLeftLineBlack = true;
    }
    // When the right line is white
    else if (events == GPIO_IRQ_EDGE_FALL && gpio == RIGHT_LINE_SENSOR_PIN)
    {
        isRightLineBlack = false;
    }
    // When the left line is white
    else if (events == GPIO_IRQ_EDGE_FALL && gpio == LEFT_LINE_SENSOR_PIN)
    {
        isLeftLineBlack = false;
    }
}

/**
 * @brief Function to prints value for left and right to see if it detects black
 *
 * @details
 * This function prints the values of whether the left and right line sensors
 * is detecting a black line or not to the serial monitor.
 *
 */
void retrieve_line_sensor_value()
{
    printf("Left Line Sensor: %s\n", isLeftLineBlack ? "true" : "false");
    printf("Right Line Sensor: %s\n", isRightLineBlack ? "true" : "false");
}

/**
 * @brief Function to initialize the infrared sensors.
 *
 * @details
 * This function initialises the GPIO pins for the left line sensor,
 * right line sensor and barcode sensor.
 *
 * @param left_line_sensor_pin GPIO pin for the left line sensor.
 * @param right_line_sensor_pin GPIO pin for the right line sensor.
 * @param barcode_sensor_pin GPIO pin for the barcode sensor.
 */
void initialise_infrared(int8_t left_line_sensor_pin, int8_t right_line_sensor_pin, int8_t barcode_sensor_pin)
{
    // Set the GPIO pins
    left_sensor_pin = left_line_sensor_pin;
    right_sensor_pin = right_line_sensor_pin;
    barcode_sensor_pin = barcode_sensor_pin;

    // Initialize GPIO pins
    gpio_init(left_line_sensor_pin);
    gpio_init(right_line_sensor_pin);
    gpio_init(barcode_sensor_pin);

    // Set GPIO pins to pull down
    gpio_set_dir(left_line_sensor_pin, GPIO_IN);
    gpio_set_dir(right_line_sensor_pin, GPIO_IN);
    gpio_set_dir(barcode_sensor_pin, GPIO_IN);
}