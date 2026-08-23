#pragma once
#define ERROR_TEMPERATURE -999
#define TIMEOUT_LIMIT_US 5000
#define DHT22_START_SIGNAL_TIME 2000
#define DHT22_RECEIVED_DATA_SIZE 40
#define DHT22_BIT_DIFFERENCE_TIME 50

typedef enum
{
	DHT_NOT_INITIALIZED,
	DHT_OK,
	DHT_INVALID_PARAMETERS,
	DHT_TIMEOUT_ERROR,
	DHT_CHECKSUM_ERROR
}dht_status;

typedef struct
	{
		GPIO_TypeDef* data_port;
		uint16_t data_pin;
		uint64_t buffer;
		uint16_t humidity;
		uint16_t temperature;
		uint8_t checksum;
		TIM_HandleTypeDef* htim;
		dht_status status;
	}dht_sensor_t;

dht_status dht_init (dht_sensor_t* dht, GPIO_TypeDef* data_port, uint16_t data_pin, TIM_HandleTypeDef* htim);

dht_status read_dht_data (dht_sensor_t* dht);

int16_t get_Temperature(dht_sensor_t* dht);
