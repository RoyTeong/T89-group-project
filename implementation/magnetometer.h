/**
 * @file magnetometer.h
 * @brief magnetometer Interface Header File
 *
 * This file contains declarations for interfacing with a magnetometer using I2C.
 *
 * @date October 27, 2023
 */

#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "math.h"

#define MAGNETOMETER_ADDRESS 0x1E           // Address of the magnetometer
#define MAGNETOMETER_CONFIG_REGISTER_A 0x00 // CRA_REG_M
#define MAGNETOMETER_CONFIG_REGISTER_B 0x01 // CRB_REG_M
#define MAGNETOMETER_MODE_REGISTER 0x02     // MR_REG_M
#define MAGNETOMETER_DATA_REGISTER 0x03     // Starting address of data registers OUT_X_H_M

bool initialise_i2c_bus();
bool initialise_magnetometer();
uint16_t *get_magnetometer_data();
float get_heading();

#endif // MAGNETOMETER_H

/**
 * @brief Initialize the I2C bus for communication.
 *
 * This function initializes the I2C bus with a baud rate of 400,000 bps.
 *
 * @return true if initialization is successful, false otherwise.
 */
bool initialise_i2c_bus()
{
    // Initialize the I2C bus
    i2c_init(i2c0, 400000);
   
    // Make the I2C pins available to picotool
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); // GPIO 4
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C); // GPIO 5
   
    // Enable pull up on SDA/SCL lines
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    return true;
}

/**
 * @brief Initialize the magnetometer.
 *
 * This function initializes the magnetometer by configuring its mode and registers.
 *
 * @return true if initialization is successful, false otherwise.
 */
bool initialise_magnetometer()
{
    // Initialize I2C bus
    initialise_i2c_bus();

    // Initialize magnetometer
    uint8_t magnetometer_config[] = {MAGNETOMETER_MODE_REGISTER, MAGNETOMETER_CONFIG_REGISTER_A}; // 0x02, 0x00
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, magnetometer_config, 2, false);
    return true;
}

/**
 * @brief Get the heading in degrees based on magnetometer data.
 *
 * This function reads the magnetometer data, calculates the heading, and returns the result.
 *
 * @return The heading in degrees.
 */
float get_heading()
{

    float heading = 0;

    // Write to the magnetometer to tell it to read the data
    uint8_t magnetometer_reg[1] = {MAGNETOMETER_DATA_REGISTER};
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, magnetometer_reg, 1, true);

    // Read the data from the magnetometer
    uint8_t magnetometer_data[6];
    i2c_read_blocking(i2c0, MAGNETOMETERER_ADDRESS, magnetometer_data, 6, false);

    // Convert the data to 16-bit signed values
    int16_t x = (magnetometer_data[0] << 8) | magnetometer_data[1];
    int16_t y = (magnetometer_data[4] << 8) | magnetometer_data[5];

    // Calculate the heading
    heading = atan2(y, x) * 180.0 / M_PI;

    // Normalize to 0-360
    if (heading < 0)
    {
        heading += 360;
    }

    return heading;
}

/**
 * @brief Get the raw magnetometer data.
 *
 * This function reads the raw magnetometer data and returns a pointer to an array of three uint16_t values (X, Y, Z).
 * The memory is dynamically allocated, and the caller is responsible for freeing it.
 *
 * @return A pointer to an array of three uint16_t values (X, Y, Z).
 */
uint16_t *get_magnetometer_data()
{
    uint8_t magnetometer_reg[1] = {MAGNETOMETER_DATA_REGISTER};
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, magnetometer_reg, 1, true);

    // Read the data from the magnetometer
    uint8_t magnetometer_data[6];
    i2c_read_blocking(i2c0, MAGNETOMETER_ADDRESS, magnetometer_data, 6, false);

    // Convert the data to 16-bit signed values
    int16_t x = (magnetometer_data[0] << 8) | magnetometer_data[1];
    int16_t z = (magnetometer_data[2] << 8) | magnetometer_data[3];
    int16_t y = (magnetometer_data[4] << 8) | magnetometer_data[5];

    // put the data into a 16 bit unsigned int
    uint16_t *data = malloc(3 * sizeof(uint16_t));
    data[0] = x;
    data[1] = y;
    data[2] = z;

    return data;
}