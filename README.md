# DHT22 STM32F411 Driver

Robust, hardware-timer-based C driver for the DHT22 (AM2302) temperature and humidity sensor, designed for STM32 microcontrollers. 

## Hardware Setup
1. Connect the DHT22 DATA pin to an STM32 GPIO.
2. The GPIO pin must be configured as Open-Drain (not Push-Pull) in STM32CubeIDE.
3. An external 4.7kΩ - 10kΩ Pull-up resistor is required between the DATA pin and VCC (3.3V / 5V).  
   Note: If you are using a 3-pin DHT22 module, this resistor is usually already built into the PCB.

## Software Configuration
The driver requires a hardware timer (e.g., `TIM1`, `TIM2`) configured to tick exactly every 1 microsecond (1 MHz).
* **Prescaler (PSC):** Set to `(SystemCoreClock / 1000000) - 1`. For an STM32F411 running at 100 MHz, PSC = 99.
* **Counter Period (ARR):** Set to Maximum (`0xFFFF` for 16-bit or `0xFFFFFFFF` for 32-bit).

## Quick Start
```c
#include "dht22_driver.h"

/* 1. Allocate the sensor structure */
dht_sensor_t my_dht;
int16_t temperature = 0;

int main(void) {
    HAL_TIM_Base_Start(&htim1);

    /* 2. Initialize the driver */
    dht_init(&my_dht, GPIOA, GPIO_PIN_1, &htim1);

    while (1) {
        /* 3. Read data and check status */
        if (read_dht_data(&my_dht) == DHT_OK) {
            temperature = get_Temperature(&my_dht);
            // my_dht.humidity is available directly
        } else {
            // Handle error (e.g., DHT_TIMEOUT_ERROR)
        }
        
        HAL_Delay(2000); // DHT22 requires at least 2 seconds between readings
    }
}
