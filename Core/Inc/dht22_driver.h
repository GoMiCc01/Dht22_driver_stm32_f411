#pragma once

/**
 * @file dht22_driver.h
 * @brief DHT22 temperature and humidity sensor driver for STM32.
 *
 * This driver uses a hardware timer to measure microsecond pulses
 * on a single-wire interface.
 */

/** @brief Error value returned when temperature calculation fails */
#define ERROR_TEMPERATURE -999

/** @brief Timeout limit in microseconds for sensor response */
#define TIMEOUT_LIMIT_US 5000

/** @brief Start signal duration in microseconds sent by MCU */
#define DHT22_START_SIGNAL_TIME 2000

/** @brief Number of bits received in a complete DHT22 data packet */
#define DHT22_RECEIVED_DATA_SIZE 40

/** @brief Time threshold in microseconds to differentiate between bit '0' and bit '1' */
#define DHT22_BIT_DIFFERENCE_TIME 50

/**
 * @brief Sensor transaction status codes.
 */
typedef enum
{
    DHT_NOT_INITIALIZED,    /**< Sensor structure is not initialized */
    DHT_OK,                 /**< Data successfully read / Operation successful */
    DHT_INVALID_PARAMETERS, /**< Null pointer passed to the function */
    DHT_TIMEOUT_ERROR,      /**< Sensor is not responding or wire is disconnected */
    DHT_CHECKSUM_ERROR      /**< Data corruption detected during transmission */
} dht_status;

/**
 * @brief DHT22 sensor instance structure.
 */
typedef struct
{
    GPIO_TypeDef* data_port;  /**< Pointer to the GPIO port (e.g., GPIOA) */
    uint16_t data_pin;        /**< GPIO pin number (e.g., GPIO_PIN_1) */
    uint64_t buffer;          /**< Raw 40-bit data buffer received from the sensor */
    uint16_t humidity;        /**< Raw humidity data parsed from the buffer */
    uint16_t temperature;     /**< Raw temperature data parsed from the buffer */
    uint8_t checksum;         /**< Checksum byte received from the sensor */
    TIM_HandleTypeDef* htim;  /**< Pointer to the hardware timer used for delays */
    dht_status status;        /**< Current operational status of the sensor */
} dht_sensor_t;

/**
 * @brief Initializes the DHT22 sensor structure.
 *
 * @param dht Pointer to the DHT22 sensor instance structure.
 * @param data_port Pointer to the GPIO port where the sensor is connected.
 * @param data_pin GPIO pin number configured as Open-Drain.
 * @param htim Pointer to the hardware timer used for microsecond delays.
 * @return dht_status Returns DHT_OK if successful, or DHT_INVALID_PARAMETERS if any pointer is NULL.
 */
dht_status dht_init (dht_sensor_t* dht, GPIO_TypeDef* data_port, uint16_t data_pin, TIM_HandleTypeDef* htim);

/**
 * @brief Reads data from the DHT22 sensor over the single-wire bus.
 *
 * @param dht Pointer to the initialized DHT22 sensor instance.
 * @return dht_status Returns DHT_OK on success, or an error code on timeout/checksum failure.
 */
dht_status read_dht_data (dht_sensor_t* dht);

/**
 * @brief Calculates the actual temperature value including the sign.
 *
 * @param dht Pointer to the DHT22 sensor instance containing successfully read data.
 * @return int16_t Temperature in tenths of a degree Celsius (e.g., 253 means 25.3°C).
 *                 Returns ERROR_TEMPERATURE if the sensor instance is NULL.
 */
int16_t get_Temperature(dht_sensor_t* dht);
