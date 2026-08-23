#include "main.h"
#include "dht22_driver.h"

static void delay_us(const uint16_t us , dht_sensor_t* dht)
{
	uint16_t start = __HAL_TIM_GetCounter(dht->htim);
	while((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - start) < us){}
}


void read_dht_data (dht_sensor_t* dht)
{
	if(NULL == dht)
	{}
	else
	{
		bool status_correct = true;
		uint16_t timeout = 0;
		HAL_GPIO_WritePin(dht->data_port, dht->data_pin, GPIO_PIN_RESET);
		delay_us(2000 , dht);
		HAL_GPIO_WritePin(dht->data_port, dht->data_pin, GPIO_PIN_SET);

		timeout = __HAL_TIM_GetCounter(dht->htim);
		while(GPIO_PIN_SET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
		{
			if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > 5000)
			{
				status_correct = false;
				break;
			}
		};

		if(true == status_correct)
		{
			timeout = __HAL_TIM_GetCounter(dht->htim);
			while(GPIO_PIN_RESET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
			{
				if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > 5000)
				{
					status_correct = false;
					break;
				}
			};
		}

		if(true == status_correct)
		{
			timeout = __HAL_TIM_GetCounter(dht->htim);
			while(GPIO_PIN_SET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
			{
				if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > 5000)
				{
					status_correct = false;
					break;
				}
			};
		}

		if(true == status_correct)
		{
		dht->buffer = 0;
		for(uint8_t i = 0 ; i < 40 ; i++)
			{
				if(true == status_correct)
				{
					timeout = __HAL_TIM_GetCounter(dht->htim);
					while(GPIO_PIN_RESET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
					{
						if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > 5000)
						{
							status_correct = false;
							break;
						}
					};
				}
				uint16_t start = __HAL_TIM_GetCounter(dht->htim);
				if(true == status_correct)
				{
					timeout = __HAL_TIM_GetCounter(dht->htim);
					while(GPIO_PIN_SET == HAL_GPIO_ReadPin(dht->data_port, dht->data_pin))
					{
						if((uint16_t)(__HAL_TIM_GetCounter(dht->htim) - timeout) > 5000)
						{
							status_correct = false;
							break;
						}
					};
				}
				uint16_t end = __HAL_TIM_GetCounter(dht->htim);
				dht->buffer <<= 1;
				if((uint16_t)(end - start)>50)
				{
					dht->buffer |= 1;
				}
			}
		}

		if(true == status_correct)
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
				dht->is_data_valid = false;
			}
		}
	}
}

int16_t get_Temperature(dht_sensor_t* dht)
{
	int16_t temperature = ERROR_TEMPERATURE;
	if(NULL == dht)
	{}
	else
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

