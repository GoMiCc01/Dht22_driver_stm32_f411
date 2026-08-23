#pragma once
#define ERROR_TEMPERATURE -999

typedef struct
	{
		GPIO_TypeDef* data_port;
		uint16_t data_pin;
		uint64_t buffer;
		uint16_t humidity;
		uint16_t temperature;
		uint8_t checksum;
		TIM_HandleTypeDef* htim;
		bool is_data_valid;
	}dht_sensor_t;

void read_dht_data (dht_sensor_t* dht);

int16_t get_Temperature(dht_sensor_t* dht);
