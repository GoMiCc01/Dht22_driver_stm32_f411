#include "main.h"
#include "dht22_driver.h"


static void delay_us(const uint16_t us , dht_sensor_t* dht)
{
	uint16_t start = __HAL_TIM_GetCounter(dht->htim);
	while((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - start) < us){}
}

dht_status dht_init (dht_sensor_t* dht, GPIO_TypeDef* data_port, uint16_t data_pin, TIM_HandleTypeDef* htim)
{
	dht_status retcode = DHT_NOT_INITIALIZED;
	if(NULL == dht)
	{
		retcode = DHT_INVALID_PARAMETERS;
	}
	else if(NULL == data_port || NULL == htim)
	{
		dht->status = DHT_INVALID_PARAMETERS;
		retcode = dht->status;
	}
	else
	{
		dht->status = DHT_OK;
		dht->data_port = data_port;
		dht->data_pin = data_pin;
		dht->htim = htim;
		retcode = dht->status;
	}
	return retcode;
}


dht_status read_dht_data (dht_sensor_t* dht)
{
	dht_status retcode = DHT_OK;
	if(NULL == dht)
	{
		retcode = DHT_INVALID_PARAMETERS;
	}
	else if(DHT_NOT_INITIALIZED != dht->status && DHT_INVALID_PARAMETERS != dht->status)
	{
		dht->status = DHT_OK;
		uint16_t timeout = 0;
		HAL_GPIO_WritePin(dht->data_port, dht->data_pin, GPIO_PIN_RESET);
		delay_us(DHT22_START_SIGNAL_TIME , dht);
		HAL_GPIO_WritePin(dht->data_port, dht->data_pin, GPIO_PIN_SET);

		timeout = __HAL_TIM_GetCounter(dht->htim);
		while(GPIO_PIN_SET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
		{
			if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > TIMEOUT_LIMIT_US)
			{
				dht->status = DHT_TIMEOUT_ERROR;
				break;
			}
		};

		if(DHT_OK == dht->status)
		{
			timeout = __HAL_TIM_GetCounter(dht->htim);
			while(GPIO_PIN_RESET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
			{
				if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > TIMEOUT_LIMIT_US)
				{
					dht->status = DHT_TIMEOUT_ERROR;
					break;
				}
			};
		}

		if(DHT_OK == dht->status)
		{
			timeout = __HAL_TIM_GetCounter(dht->htim);
			while(GPIO_PIN_SET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
			{
				if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > TIMEOUT_LIMIT_US)
				{
					dht->status = DHT_TIMEOUT_ERROR;
					break;
				}
			};
		}

		if(DHT_OK == dht->status)
		{
		dht->buffer = 0;
		for(uint8_t i = 0 ; i < DHT22_RECEIVED_DATA_SIZE ; i++)
			{
				if(DHT_OK == dht->status)
				{
					timeout = __HAL_TIM_GetCounter(dht->htim);
					while(GPIO_PIN_RESET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
					{
						if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > TIMEOUT_LIMIT_US)
						{
							dht->status = DHT_TIMEOUT_ERROR;
							break;
						}
					};
				}
				uint16_t start = __HAL_TIM_GetCounter(dht->htim);
				if(DHT_OK == dht->status)
				{
					timeout = __HAL_TIM_GetCounter(dht->htim);
					while(GPIO_PIN_SET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
					{
						if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > TIMEOUT_LIMIT_US)
						{
							dht->status = DHT_TIMEOUT_ERROR;
							break;
						}
					};
				}
				uint16_t end = __HAL_TIM_GetCounter(dht->htim);
				dht->buffer <<= 1;
				if((uint16_t)(end - start)>DHT22_BIT_DIFFERENCE_TIME)
				{
					dht->buffer |= 1;
				}
			}
		}

		if(DHT_OK == dht->status)
		{
			dht->humidity = (uint16_t)(dht->buffer >> (sizeof(dht->checksum)+sizeof(dht->temperature))*8);
			dht->temperature = (uint16_t) (dht->buffer >> sizeof(dht->checksum)*8);
			dht->checksum = (uint8_t)dht->buffer;

			uint8_t calculated_checksum =
					(uint8_t)(dht->humidity>>8)+
					(uint8_t)(dht->humidity&0xFF)+
					(uint8_t)(dht->temperature>>8)+
					(uint8_t)(dht->temperature&0xFF);
			if(calculated_checksum != dht->checksum)
			{
				dht->status = DHT_CHECKSUM_ERROR;
			}
		}
		retcode = dht->status;
	}
	else
	{
		retcode = dht->status;
	}
	return retcode;
}

int16_t get_Temperature(dht_sensor_t* dht)
{
	int16_t temperature = ERROR_TEMPERATURE;
	if(NULL != dht)
	{
		if((1<<(sizeof(dht->temperature)*8-1)) & dht->temperature)
		{
			temperature = -(int16_t)(1<<(sizeof(dht->temperature)*8-1)^dht->temperature);
		}
		else
		{
			temperature = (int16_t)(dht->temperature);
		}
	}
	return temperature;
}

