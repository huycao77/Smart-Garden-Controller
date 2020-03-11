#include <stdint.h>
#include "dwt_delay.h"
extern int dht22_sen1_status;
extern int dht22_sen2_status;
/* Define data port and pin*/
#define SEN1_PORT	GPIOE
#define SEN1_PIN	GPIO_PIN_2
#define SEN2_PORT	GPIOC
#define SEN2_PIN	GPIO_PIN_13

/* Define pin set*/
#define DHT22_SEN1_LOW	HAL_GPIO_WritePin(SEN1_PORT, SEN1_PIN, GPIO_PIN_RESET)
#define DHT22_SEN1_HIGH	HAL_GPIO_WritePin(SEN1_PORT, SEN1_PIN, GPIO_PIN_SET)
#define DHT22_SEN2_LOW	HAL_GPIO_WritePin(SEN2_PORT, SEN2_PIN, GPIO_PIN_RESET)
#define	DHT22_SEN2_HIGH HAL_GPIO_WritePin(SEN2_PORT, SEN2_PIN, GPIO_PIN_SET)

/* Define input reading */
#define DHT22_SEN1_SIGNAL HAL_GPIO_ReadPin(SEN1_PORT, SEN1_PIN)
#define DHT22_SEN2_SIGNAL HAL_GPIO_ReadPin(SEN2_PORT, SEN2_PIN)

/* Function prototype */
void dht22_sen1_start_signal(void);
void dht22_sen2_start_signal(void);
void dht22_sen1_response(void);
void dht22_sen2_response(void);
uint8_t dht22_sen1_data_transmission(void);
uint8_t dht22_sen2_data_transmission(void);
void sen1_pin_input(void);
void sen2_pin_input(void);
void sen1_pin_output(void);
void sen2_pin_output(void);
