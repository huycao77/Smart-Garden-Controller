/* Functions for communicating with 2 DHT22 sensors
	 Written by Huy Cao, 2019													*/

#include "stm32f4xx.h"
#include "dht22.h"

GPIO_InitTypeDef GPIO_InitStruct;
int dht22_sen1_status = 0;
int dht22_sen2_status = 0;

void dht22_sen1_start_signal(void)
{
	sen1_pin_output();
	DHT22_SEN1_LOW;
	DWT_Delay(10000);
	DHT22_SEN1_HIGH;
	sen1_pin_input();
}

void dht22_sen2_start_signal(void)
{
	sen2_pin_output();
	DHT22_SEN2_LOW;
	DWT_Delay(10000);
	DHT22_SEN2_HIGH;
	sen2_pin_input();
}

void dht22_sen1_response(void)
{
	DWT_Delay(40);

	if (!(DHT22_SEN1_SIGNAL))
	{
		DWT_Delay(80);
		if (DHT22_SEN1_SIGNAL)
		{
			dht22_sen1_status = 1;
		}
	}
	else
	{
		dht22_sen1_status = 0;
	}

	while (DHT22_SEN1_SIGNAL);
}

void dht22_sen2_response(void)
{
	DWT_Delay(40);

	if (!(DHT22_SEN2_SIGNAL))
	{
		DWT_Delay(80);
		if (DHT22_SEN2_SIGNAL)
		{
			dht22_sen2_status = 1;
		}
	}
	else
	{
		dht22_sen2_status = 0;
	}

	while (DHT22_SEN2_SIGNAL);
}

uint8_t dht22_sen1_data_transmission(void)
{
	uint8_t data_bits, i;

	for (i = 0; i < 8; i++)
	{
		while (!DHT22_SEN1_SIGNAL);
		DWT_Delay(50);
		if (DHT22_SEN1_SIGNAL == 1)
		{
			data_bits |= 1 << (7 - i);
		}
		else
		{
			data_bits &= ~(1 << (7 - i));
		}

		while (DHT22_SEN1_SIGNAL);
	}

	return data_bits;
}

uint8_t dht22_sen2_data_transmission(void)
{
	uint16_t data_bits, i;

	for (i = 0; i < 8;i++)
	{
		while (!DHT22_SEN2_SIGNAL);
		DWT_Delay(50);
		if (DHT22_SEN2_SIGNAL == 1)
		{
			data_bits |= 1 << (7 - i);
		}
		else
		{
			data_bits &= ~(1 << (7 - i));
		}

		while (DHT22_SEN2_SIGNAL);
	}

	return data_bits;
}

void sen1_pin_input(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void sen2_pin_input(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void sen1_pin_output(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void sen2_pin_output(void)
{
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}