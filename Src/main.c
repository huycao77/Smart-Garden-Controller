/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;

RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};
RTC_AlarmTypeDef sAlarm = {0};

struct time
{
	uint8_t hour;
	uint8_t min;
};
struct time set_time = {0,0};
int TIM3_flag = 0;
int TIM2_flag = 0;
int RTC_AlarmA_flag = 0;
uint16_t TIM4_counter = 1;
float T = 0; 
float RH = 0;
uint8_t duration = 0;
int t_threshold = 0;
int h_threshold = 0;
char temp[16] = { 'T',':' };
char hum[16] = {' ', 'H',':' };
char time[16] = { 'h','h',':','m','m',':','s','s' };
char calendar[16] = {' ',' ','d','d','/','m','m',' '};
char time_set[16] = {'h','h',':','m','m'};
char date_set[16] = {'d','d','/','m','m'};
char alarm[16] = {'h','h',':','m','m',':','s','s'};
char dur[16];
char temp_set[16];
char hum_set[16];
uint8_t rundate_water = 0;
uint8_t rundate_mist = 0;
//For Timer 2
uint16_t count1s = 0;
uint8_t ablink = 0;
//For Timer 3
uint16_t count2s = 0;
// For key pad in Timer 4
uint8_t ccount=0; 
uint8_t cvarkey=0,cbounce=0,ctmrpt=50;   // for detect keypad press
uint8_t u8page=1, y=1;
unsigned char bstatus=0,ucBuzz=1;

//For Timer 4
uint8_t b_flagtim4=0;
uint16_t tim4_count =0, tim4_count2=0;
uint8_t count_button=0;
uint8_t b_flag1s=0;
uint8_t submenu2=0;
uint8_t blink = 0;
uint8_t settime_count = 0;
uint8_t date_count = 0;
uint8_t alarm_count = 0;
uint8_t set_flag = 0;
uint8_t dur_count = 0;
uint8_t temp_count = 0;
uint8_t longbuzz = 0;
uint8_t h_count = 0;
#define sw1_but  HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3)
#define sw2_but  HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)
#define sw3_but  HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5)
#define sw4_but  HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6)

//For alarm
uint8_t water = 0;
uint8_t mist = 0;
uint8_t dur_water = 0;
uint8_t dur_mist = 0;

//For timer 6
uint16_t count10s = 0;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
//====================Scan Key Board============
unsigned char localkey(void) {
   volatile unsigned char ckey;
  // static unsigned int8 but, count;
  // volatile unsigned char bret;
   if (sw1_but == 0) {ckey = 1;}                         // exit
    else if (sw2_but == 0) {ckey = 2;  }                 // dn -
         else if (sw3_but == 0) {ckey = 3; }            // up + 
						else if (sw4_but == 0) {ckey = 4;  }           // set 
             else ckey=0;            
    return(ckey);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Timer ISRs */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	unsigned char i;
	
	//TIM2 ISR. Occur every 1s
	if (htim->Instance == TIM2)
	{
		TIM2_flag = 1;
		count1s++;
		
		//After 10s, check for water and mist status
		if (count1s%10==0)
		{
			//Check for water status. Trigger the water valve if the status is set
			if (water==1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			
			//Check for mist status. Trigger the pump for mist if the status is set
			if (mist==1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
		}
		
	}
	
	//TIM3 ISR. Occur every 2s
	if (htim->Instance == TIM3)
	{
		TIM3_flag = 1;
		count2s++;
		
		
		if (count2s%5==0)
		{
				ablink ^= 1;
		}
	}
	
	//TIM6 ISR. Occur every 10s
	if (htim->Instance == TIM6)
	{
		count10s++;
		if ((count10s % (6*dur_water)) == 0)
		{
			if (water == 1) 
			{
				water = 0;
			}		
		}
		
		if ((count10s % (6*dur_mist)) == 0)
		{
			if (mist == 1)
			{
				mist = 0;
			}
		}
	}
	
	//TIM4 ISR. Occur every 10ms. USed for button debouncing and button tasks
	if (htim->Instance == TIM4)
	{
		
   if (++ccount == 200) ccount = 0;
   //------------------------------------ begin TASK BUTTON input
   
   i = localkey();
   if ((cvarkey == i) && (cvarkey != 0)) {
      if (cbounce > ctmrpt) { 
         cbounce = 0;    
         ctmrpt = 5; // the second char will be repeated at 80ms (Old value = 50ms)
      }
      else cbounce++;
   }
   else { 
      cbounce = 0; ctmrpt = 50;   // the first char will be repeated at 1000ms (Old value = 500ms)
   }         
   if (cbounce == 1) {
   //*********************************************************** STAR  Keypads Input
    
    
//SET button - single press      
			if (i == 4){//SET  
        submenu2=0;//reset the second submenu status

				//SET button tasks when being at the main setting menu
				if (bstatus==1){
					
				/*Check the current main setting menu page
				Also check the cursor position (y)
				Then show the submenu accordingly
					*/
				switch (u8page){
					case 1:
					 if (y==1){
						 submenu2=1;
					 }
					 else if (y==2){
						 submenu2=2;
					 }
					 break;
				 case 2:
					   if (y==1){
						 submenu2=3;
					 }
					 else if (y==2){
						 submenu2=4;
					 }
					 break;
				 case 3:
					  if (y==1){
						 submenu2=5;
					 }
					 else if (y==2){
						 submenu2=6;
					 }
					 break;
			 } 
		 }

				//Check the submenu status and show the submenu accordingly
				switch (submenu2)
				{
					//Show the main setting menu when showing the main screen
					case 0: 
						u8page=1;
						y=1;
						break;
					
					//Set time menu. Blinking the hour, minute and second respectively after pressing SET
					case 1:
						settime_count++;
						if (settime_count > 3) settime_count = 1;
						break;
					
					//Set date menu. Blinking the date, month, year respectively after pressing SET
					case 2:
						date_count++;
						if (date_count > 3) date_count = 1;
						break;
					
					//Set alarm menu. Blinking the hour, minute and second respectively after pressing SET
					case 3:
						alarm_count++;
						if (alarm_count > 4) alarm_count = 1;
						break;
					
					//Set water and mist duration menu. Blinking the time in minute after pressing SET
					case 4:
						dur_count++;
						if (dur_count > 2) dur_count = 1;
						break;
					
					//Set temp threshold. Blinking the temp after pressing SET
					case 5:
						temp_count++;
						if (temp_count > 2) temp_count = 1;
						break;
					
					//Set humidity threshold. Blinking the hum after pressing SET
					case 6:
						h_count++;
						if (h_count > 2) h_count = 1;
						break;
				}
				
			  bstatus=1;
			  ucBuzz=1;//buzzer for button press
     }
//DOWN button - single press     
    else if (i==2){ 
			
			//Move the cursor when in main setting menu
			if (submenu2 == 0)
			{
				y++; 
				if (y>2) {
					y=1;  
					u8page++;				
					if(u8page>3){	u8page=1;	}
				}
			}
			
			//Decrease the parameter setting when in sub menu
			else {
			if (settime_count == 2) { 
				if (sTime.Hours <= 0) sTime.Hours = 24;
				sTime.Hours--;
			}
			else if (settime_count == 3) 
			{
				if (sTime.Minutes <= 0) sTime.Minutes = 60;
				sTime.Minutes--;
			}
			if (date_count == 2) {
				switch (sDate.Month)
				{
					case 1:case 3: case 5: case 7: case 8: case 10: case 12:
						if (sDate.Date <= 1) sDate.Date  = 32;
						break;
					case 4: case 6: case 9: case 11:
						if (sDate.Date <= 1) sDate.Date = 31;
						break;
					case 2:
						if (sDate.Date <= 1)
						{
							if (sDate.Year % 4 == 0) sDate.Date = 30;
							else sDate.Date = 29;
						}
						break;						
				}
				sDate.Date--;
			}
			else if (date_count == 3) {
				if (sDate.Month <= 1) sDate.Month = 13;
				sDate.Month--;
			}
			if (alarm_count == 2) {
				if (sAlarm.AlarmTime.Hours <= 0) sAlarm.AlarmTime.Hours = 24;
				sAlarm.AlarmTime.Hours--;}
			else if (alarm_count == 3) {
				if (sAlarm.AlarmTime.Minutes <=0) sAlarm.AlarmTime.Minutes = 60;
				sAlarm.AlarmTime.Minutes--; }
			else if (alarm_count == 4) {
				if (sAlarm.AlarmTime.Seconds <= 0) sAlarm.AlarmTime.Seconds = 60;
				sAlarm.AlarmTime.Seconds--;}
			if (dur_count == 2)
			{
				duration--;
				if (duration <= 0) duration = 0;
			}
			
			if (temp_count == 2)
			{
				if (t_threshold <=-40) t_threshold = -39;
				t_threshold--;
			}
			if (h_count == 2) 
			{
				if (h_threshold <= 0) h_threshold = 1;
				h_threshold--;
			}
			
     } 
			ucBuzz=1; //Buzzer for button press
	 }
		
//UP button - single press     
    else if (i==3 ){
			
			//Move the cursor when in main setting menu
			if (submenu2 == 0)
			{
				y--; 
				if (y<1) {
						y=2; 			
						u8page--;
						if(u8page<1){	u8page=3;	}
				}
			}
			
			//Increase the parameter setting when in sub menu
			else {
				if (settime_count == 2) {
					sTime.Hours++;
					if (sTime.Hours >= 24) sTime.Hours = 0;}
				else if (settime_count == 3) {
					sTime.Minutes++;
					if (sTime.Minutes >= 60) sTime.Minutes = 0;}
				if (date_count == 2) 
				{
					sDate.Date++;
					switch (sDate.Month)
					{
						case 1: case 3: case 5: case 7: case 8: case 10: case 12:
							if (sDate.Date > 31) sDate.Date = 1;
							break;
						case 4: case 6: case 9: case 11:
							if (sDate.Date > 30) sDate.Date = 1;
							break;
						case 2:
							if (sDate.Year % 4 == 0)
							{
								if (sDate.Date > 29) sDate.Date = 1;
							}
							else{
								if (sDate.Date > 28) sDate.Date = 1;
							}
							break;
					}
				}
				else if (date_count == 3) {
					sDate.Month++;
					if (sDate.Month > 12) sDate.Month = 1;}
				if (alarm_count == 2) 
				{
					sAlarm.AlarmTime.Hours++;
					if (sAlarm.AlarmTime.Hours >= 24) sAlarm.AlarmTime.Hours = 0;}
				else if (alarm_count == 3) {
					sAlarm.AlarmTime.Minutes++;
					if (sAlarm.AlarmTime.Minutes >= 60) sAlarm.AlarmTime.Minutes = 0;}
				else if (alarm_count == 4) { 
					sAlarm.AlarmTime.Seconds++; 
					if (sAlarm.AlarmTime.Seconds >= 60) sAlarm.AlarmTime.Seconds = 0;}
				if (dur_count == 2) 
				{
					duration++;
					if (duration > 31) duration = 30;
				}
				if (temp_count == 2)
				{
					t_threshold++;
					if (t_threshold > 80) t_threshold = 80;
				}
				if (h_count == 2)
				{
					h_threshold++;
					if (h_threshold > 100) h_threshold = 100;
				}
				
			}  
			ucBuzz=1;//buzzer for button press
		}			
		
//EXIT button - single press    
    else if (i==1 ){
			
			//back to the main setting menu when in sub menu
			if (submenu2!=0){
				submenu2=0;
				u8page=1;
				y=1;
			}
			
			//back to the main screen when in main setting menu
			else{
			 bstatus=0;
			 set_flag = 0;
			 u8page=0;
			 y=0;			 
      }	
      ucBuzz=1;			
    }          
    //*********************************************************** STOP  Keypads Input 
   }
	 
	 if (cbounce >= 20) //button long press
	 {
		 
		 //Hotkey - trigger water valve when long-pressing UP
		 if (i==3) 
		 {
			 if (bstatus == 0)
			 {
			 dur_water = duration;
			 water = 1;
			 count10s = 1;
			 longbuzz = 1;
			 }
		 }
		 
		 //Hotkey - trigger mist pump when long-pressing DOWN
		 else if (i==2) //down
		 {
			 if (bstatus == 0)
			 {
			 dur_mist = duration;
			 mist = 1;
			 count10s = 1;
			 longbuzz = 1;
			 }
		 }
		 
		 //Hotkey - stop water or mist when long-pressing EXIT
		 else if (i==1) //exit
		 {
			 bstatus = 0;
			 set_flag = 0;
			 u8page = 0;
			 y = 0;
			 water = 0;
			 mist = 0;
			 longbuzz = 1;
		 }
	 }
   cvarkey = i; 
   //====================================== end TASK BUTTON input
   
 
   if ( ccount%20 == 0) {
   //------------------------------------------------------------------------------ call every 200ms
		
		//trigger the buzzer
    if (ucBuzz==1){
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
			ucBuzz=0;
		}
		else{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
		} 
   }//End timing 200ms 
	 
	 //trigger long buzzer
	 if (ccount%150==0){
		//----------------------------------------------------------------------------- call every 700ms
		 if (longbuzz == 1)
		 {
			 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
			 longbuzz = 0;
		 }
		 else HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
	 }
	 
	 //Get the RTC time and blink status every 1s
   if ( ccount%100 == 0) {
   //------------------------------------------------------------------------------ call every 1s
     if (bstatus==0){
			 HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			 HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		 }

		 b_flag1s=1;       		 
		 set_flag = 0;
		 blink ^= 1;
    } //End timing 1s  
	}
}
//RTC Alarm ISR
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_AlarmA_flag = 1;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int button = 0;
	int state = 0;
	int menu_page = 1;
	int y = 0;
	
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
	lcd_init();
	DWT_Init();
	TM_BKPSRAM_Init();
  /* USER CODE BEGIN 2 */
	lcd_puts(0,0,(int8_t*) "Welcome");
	HAL_Delay(1000);
	lcd_clear();
	lcd_puts(0,0,(int8_t*) "Starting...");
	HAL_Delay(1000);
	lcd_clear();
	
	/* Read parameters from SRAM */
	duration = TM_BKPSRAM_Read8(0x00);
	t_threshold = TM_BKPSRAM_Read8(0x01);
	h_threshold = TM_BKPSRAM_Read8(0x02);
	sAlarm.AlarmTime.Hours = TM_BKPSRAM_Read8(0x04);
  sAlarm.AlarmTime.Minutes = TM_BKPSRAM_Read8(0x05);
  sAlarm.AlarmTime.Seconds = TM_BKPSRAM_Read8(0x06);
	rundate_water = TM_BKPSRAM_Read8(0x10);
	rundate_mist = TM_BKPSRAM_Read8(0x12);
	
	/* Initialize timers in interrupt mode */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim6);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		output_control(T,RH);//Check the temp and hum for automatic watering and misting
		
		//Update the time every 1s
		if (b_flag1s==1){			
			if (bstatus==0 && set_flag == 0){
					//Display water indication if watering
					if (water==1)
					{
						if (ablink==1) lcd_puts(0,0,(int8_t*)"   Dang tuoi    ");
						else displayRTC();
					}
					else displayRTC();
			}
			
			//Display main setting menu and submenu
			else {
				switch (submenu2){
					case 0:
						menu(u8page);
						break;
					case 1:
						setTime_menu();
						switch (settime_count)
						{
							case 1:
								lcd_puts(1,6,(int8_t*) time_set);
								break;
							case 2:
								if (blink == 1) lcd_puts(1,6,(int8_t*) "  ");
								else lcd_puts(1,6,(int8_t*) time_set);
								break;
							case 3:
								if (blink == 1) lcd_puts(1,9,(int8_t*) "  ");
								else lcd_puts(1,6,(int8_t*) time_set);
								break;
						}
						break;
					case 2:
						setDate_menu();
						switch (date_count)
						{
							case 1:
								lcd_puts(1,6,(int8_t*) date_set);
								break;
							case 2:
								if (blink == 1) lcd_puts(1,6,(int8_t*) "  ");
								else lcd_puts(1,6,(int8_t*) date_set);
								break;
							case 3:
								if (blink == 1) lcd_puts(1,9,(int8_t*) "  ");
								else lcd_puts(1,6,(int8_t*) date_set);
								break;
						}
						break;
					case 3:
						setAlarm_menu();
					
						switch (alarm_count)
						{
							case 1:
								lcd_puts(1,4,(int8_t*) alarm);
								break;
							case 2:
								if (blink == 1) lcd_puts(1,4,(int8_t*) "  ");
								else lcd_puts(1,4,(int8_t*) alarm);
								break;
							case 3:
								if (blink == 1) lcd_puts(1,7,(int8_t*) "  ");
								else lcd_puts(1,4,(int8_t*) alarm);
								break;
							case 4:
								if (blink == 1) lcd_puts(1,10,(int8_t*) "  ");
								else lcd_puts(1,4,(int8_t*) alarm);
								break;
						}
						
						//Write the current Alarm info into SRAM 
						TM_BKPSRAM_Write8(0x04, sAlarm.AlarmTime.Hours);
						TM_BKPSRAM_Write8(0x05, sAlarm.AlarmTime.Minutes);
						TM_BKPSRAM_Write8(0x06, sAlarm.AlarmTime.Seconds);
						
						break;
					case 4:
						setDuration_menu(duration);
						switch (dur_count)
						{
							case 1:
								lcd_puts(1,3,(int8_t*) dur);
								break;
							case 2:
								if (blink == 1) lcd_puts(1,3,(int8_t*) "   ");
								else lcd_puts(1,3,(int8_t*) dur);
								break;
						}

						//Write water duration info into SRAM
						TM_BKPSRAM_Write8(0x00, duration);
					break;
					case 5:
						setTemp_menu(t_threshold);
						switch (temp_count)
						{
							case 1:
								lcd_puts(1,6,(int8_t*) temp_set);
								break;
							case 2:
								if (blink == 1) lcd_puts (1,6,(int8_t*) "  ");
								else lcd_puts(1,6,(int8_t*) temp_set);
								break;
						}
						
						//Write temp info into SRAM
						TM_BKPSRAM_Write8(0x01, t_threshold);
					break;
					case 6:
						setH_menu(h_threshold);
						switch (h_count)
						{
							case 1:
								lcd_puts(1,5,(int8_t*) hum_set);
								break;
							case 2:
								if (blink == 1) lcd_puts (1,5,(int8_t*) "   ");
								else lcd_puts (1,5,(int8_t*) hum_set);
								break;
						}
						
						//Write hum info into SRAM
						TM_BKPSRAM_Write8(0x02, h_threshold);
					break;
				}					
			}	
			b_flag1s=0;
			
			
		}
		
		//Update the temp and hum display every 2s
		if ((TIM3_flag == 1)&&(bstatus==0))
				{
					TIM3_flag = 0;
					if (bstatus==0&&set_flag==0){
						
						//Mist indication when mist pump is triggered
						if (mist==1) 
						{
							if (ablink==1) lcd_puts(1,0,(int8_t*) "  Dang phun am  ");
							else dht22_display();
						}
						else dht22_display();
					}
				}
		
		//Trigger the automatic water and mist when the alarm is triggered
		if (RTC_AlarmA_flag == 1)
		{
			
			/* Compare the current temp with the set threshold temp
				 If lower, the water duration follows a preset value
				 If higher, the water duration is the set duration	*/
			if (T <= t_threshold)
			{
				if (T <= 20) 								dur_water = DUR_OFF;
				else if ((T>20) && (T<=25)) dur_water = DUR_1;
				else if ((T>25) && (T<=30)) dur_water = DUR_2;
				else if ((T>30) && (T<=35)) dur_water = DUR_3;
				else if ((T>35) && (T<=40)) dur_water = DUR_4;
				else if ((T>40))						dur_water = DUR_5;
			}
			else
			{
				dur_water = duration;
			}
			
			/* Compare the current hum with the set threshold hum
				 If higher, the water duration follows a preset value
				 If lower, the water duration is the set duration	*/
			if (RH <= h_threshold)
			{
				if (RH >= 80) 								dur_mist = DUR_OFF;
				else if ((RH<80) && (RH>=75)) dur_mist = DUR_1;
				else if ((RH<75) && (RH>=70)) dur_mist = DUR_2;
				else if ((RH<70) && (RH>=65)) dur_mist = DUR_3;
				else if ((RH<65) && (RH>=60)) dur_mist = DUR_4;
				else if ((RH<60))							dur_mist = DUR_5;
			}
			else 
			{
				dur_mist = duration;
			}
			water = 1;
			mist = 1;
			count10s = 1;
		}
    /* USER CODE END WHILE */
		
    /* USER CODE BEGIN 3 */
		}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 11;
  sTime.Minutes = 6;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

  sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
  sDate.Month = RTC_MONTH_AUGUST;
  sDate.Date = 1;
  sDate.Year = 19;

  /** Enable the Alarm A 
  */
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 12;
  sAlarm.Alarm = RTC_ALARM_A;
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 41999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 41999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 3999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 4199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 199;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 62999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 13332;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE3 PE4 PE5 PE6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB7 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

//Get and display data from the DHT22 sensors
void dht22_display(void)
{
	uint16_t sen1_rh = 0;
	uint8_t sen1rh_in = 0;
	uint8_t sen1rh_de = 0;
	uint16_t sen1_t = 0;
	uint8_t sen1t_de = 0;
	uint8_t sen1t_in = 0;
	uint8_t sen1_checksum = 0;
	uint16_t sen2_rh = 0;
	uint8_t sen2rh_de = 0;
	uint8_t sen2rh_in = 0;
	uint16_t sen2_t = 0;
	uint8_t sen2t_de = 0;
	uint8_t sen2t_in = 0;
	uint8_t sen2_checksum = 0;
	char sign[16] = {(char)223};
	
	//Check sensor 1
	dht22_sen1_start_signal();
	dht22_sen1_response();

	if (dht22_sen1_status == 1)
	{
		sen1rh_in = dht22_sen1_data_transmission();
		sen1rh_de = dht22_sen1_data_transmission();
		sen1t_in = dht22_sen1_data_transmission();
		sen1t_de = dht22_sen1_data_transmission();
		sen1_checksum = dht22_sen1_data_transmission();
		
		//Converting data into decimals
		sen1_rh = (sen1rh_in << 8)  + sen1rh_de;
		sen1_t = (sen1t_in << 8) + sen1t_de;

	}

	else
	{
		lcd_puts(0, 0, (int8_t*) "Sensor 1 error");
	}

	//Check sensor 2 
	dht22_sen2_start_signal();
	dht22_sen2_response();

	if (dht22_sen2_status == 1)
	{
		sen2rh_in = dht22_sen2_data_transmission();
		sen2rh_de = dht22_sen2_data_transmission();
		sen2t_in = dht22_sen2_data_transmission();
		sen2t_de = dht22_sen2_data_transmission();
		sen2_checksum = dht22_sen2_data_transmission();

		//Converting data into decimals
		sen2_rh = (sen2rh_in << 8) + sen2rh_de;
		sen2_t = (sen2t_in << 8) + sen2t_de;
	}

	else
	{
		lcd_puts(1, 0, (int8_t*) "Sensor 2 error");
	}

	//Averaging data from 2 sensors for final result
	if ((dht22_sen1_status == 1) || (dht22_sen2_status == 1))
	{
		T = (((float)sen1_t/10) + ((float)sen2_t/10)) / 2;
		RH = (((float)sen1_rh/10) + ((float)sen2_rh/10)) / 2;
		
		lcd_puts(1,0,(int8_t*) temp);
		lcd_puts(1,8,(int8_t*) hum); 
		
		LCD1602_PrintFloat(1,2,T,1);
		LCD1602_PrintFloat(1,11,RH,1);
		
		lcd_puts(1,6,(int8_t*) sign);
		lcd_puts(1,7,(int8_t*) "C");
		lcd_puts(1,15,(int8_t*) "%");
	}
	
}

//Get and display time from the STM32F407VET6 internal RTC
void displayRTC (void)
{
	uint8_t second = 0;
	uint8_t minute = 0;
	uint8_t hour = 0;
	uint8_t date = 0;
	uint8_t month = 0;
	uint8_t year = 0;	
	second = sTime.Seconds;
	minute = sTime.Minutes;
	hour = sTime.Hours;
	date = sDate.Date;
	month = sDate.Month;
	year = sDate.Year;
			
		//Converting time and date
		time[0] = (hour/10) + 48;
		time[1] = (hour%10) + 48;
		time[3] = (minute/10) + 48;
		time[4] = (minute%10) + 48;
		time[6] = (second/10) + 48;
		time[7] = (second%10) + 48;
		
		calendar[2] = (date/10) + 48;
		calendar[3] = (date%10) + 48;
		calendar[5] = (month/10) + 48;
		calendar[6] = (month%10) + 48;

		//Display time and date
		lcd_puts(0,0,(int8_t*) time);
		lcd_puts(0,8,(int8_t*) calendar);
}

//Function for displaying the main setting menu
void menu (uint8_t page)
{
	//cursor 
	if (y == 1){
			lcd_puts(0,0,(int8_t*) ">>");
			lcd_puts(1,0,(int8_t*) "    ");
		}
    else if (	y == 2){
			lcd_puts(0,0,(int8_t*) "  ");
			lcd_puts(1,0,(int8_t*) ">>");
		}

	//menu pages
	switch (page)
	{
		case 1:	
			lcd_puts(0,2,(int8_t*) "Set Time       ");
			lcd_puts(1,2,(int8_t*) "Set Date       ");		
			break;
		case 2:
			lcd_puts(0,2,(int8_t*) "Set Alarm      ");
			lcd_puts(1,2,(int8_t*) "Set Duration   ");
			break;
		case 3:
			lcd_puts(0,2,(int8_t*) "T Threshold      ");
			lcd_puts(1,2,(int8_t*) "H Threshold      ");
			break;
	}
}

//Function for the Set Time submenu
void setTime_menu (void)
{
	lcd_puts(0,0,(int8_t*) "  Set Time      ");
	
	time_set[0] = (sTime.Hours / 10) + 48;
	time_set[1] = (sTime.Hours % 10) + 48;
	time_set[3] = (sTime.Minutes / 10) + 48;
	time_set[4] = (sTime.Minutes % 10) + 48;
	
	lcd_puts(1,0,(int8_t*) "      ");
	lcd_puts(1,6,(int8_t*) time_set);	
	lcd_puts(1,11,(int8_t*)"     ");
	
	//Modify time in the RTC module
	if (settime_count == 1) 
	{
		HAL_RTC_SetTime(&hrtc,&sTime, RTC_FORMAT_BIN);
		set_flag = 1;
	}
}

//Function for the SetDate submenu
void setDate_menu (void)
{
	lcd_puts(0,0,(int8_t*) "  Set Date      ");

	date_set[0] = (sDate.Date	/ 10) + 48;
	date_set[1] = (sDate.Date % 10) + 48;
	date_set[3] = (sDate.Month / 10) + 48;
	date_set[4] = (sDate.Month % 10) + 48;
	
	lcd_puts(1,0,(int8_t*) "      ");
	lcd_puts (1,6,(int8_t*) date_set);
	lcd_puts(1,11,(int8_t*)"     ");
	
	//Modify date in the RTC module
	if (date_count == 1)
	{
		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		set_flag = 1;
	}
}

//Function for the Set Alarm submenu
void setAlarm_menu (void)
{
	lcd_puts(0,0,(int8_t*) "  Set Alarm");
	
	alarm[0] = (sAlarm.AlarmTime.Hours / 10) + 48;
	alarm[1] = (sAlarm.AlarmTime.Hours % 10) + 48;
	alarm[3] = (sAlarm.AlarmTime.Minutes / 10) + 48;
	alarm[4] = (sAlarm.AlarmTime.Minutes % 10) + 48;
	alarm[6] = (sAlarm.AlarmTime.Seconds / 10) + 48;
	alarm[7] = (sAlarm.AlarmTime.Seconds % 10) + 48;
	
	lcd_puts(1,0,(int8_t*) "    ");
	lcd_puts(1,12,(int8_t*) "    ");
	
	//Modify the alarm time in the RTC module
	if (alarm_count == 1)
	{
		HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
		set_flag = 1;
	}
}

//Function for the Duration submenu
void setDuration_menu (int min)
{
	lcd_puts(0,0,(int8_t*) "  Set Duration");
	sprintf(dur, "%2d", min);
	lcd_puts(1,0,(int8_t*) "   ");
	lcd_puts(1,3,(int8_t*) dur);
	lcd_puts(1,6,(int8_t*) " Min(s)");
	lcd_puts(1,13,(int8_t*) "   ");
	
	if (dur_count == 1) 
	{
		set_flag = 1;
	}
}

//Function for the T Threshold submenu
void setTemp_menu (int temp)
{
	char sign[16] = {(char)223};
	
	lcd_puts(0,0,(int8_t*) "  T Threshold");
	sprintf(temp_set,"%2d", temp);
	lcd_puts(1,0,(int8_t*) "      ");
	lcd_puts(1,6,(int8_t*) temp_set);
	lcd_puts(1,8,(int8_t*) sign);
	lcd_puts(1,9,(int8_t*) "C");
	lcd_puts(1,10,(int8_t*) "      ");
	
	if (temp_count == 1) 
	{
		set_flag = 1;
	}
}

//Function for the H Threshold submenu
void setH_menu (int hum)
{
	lcd_puts(0,0,(int8_t*) "  H Threshold");
	sprintf(hum_set,"%3d", hum);
	lcd_puts(1,0, (int8_t*) "      ");
	lcd_puts(1,8, (int8_t*) " %");
	lcd_puts(1,10, (int8_t*) "      ");
	
	if (h_count == 1) 
	{
		set_flag = 1;
	}
}

/* Function for triggering the automatic water and mist without the alarm
	 Input parameters: current temperature, current humidity
	 Work accordingly to the set alarm. Also check the water and mist date 
	 to prevent triggering twice on the same day															*/
void output_control (float temp, float hum)
{
	/* If alarm is set in the morning, the system will be triggered 
		 in the evening between 15 and 19 at least 8 hours after the set alarm  */
	if ((sAlarm.AlarmTime.Hours>=0) && (sAlarm.AlarmTime.Hours<=12))
	{
		if ((sTime.Hours>=15) && (sTime.Hours<=19) && (sDate.Date != rundate_water) && (sDate.Date != rundate_mist) 
			&& ((sTime.Hours-sAlarm.AlarmTime.Hours) >= 8))
		{
			if (temp >= t_threshold)
			{
				if (temp <= 20) 									dur_water = DUR_OFF;
				else if ((temp>20) && (temp<=25)) dur_water = DUR_1;
				else if ((temp>25) && (temp<=30)) dur_water = DUR_2;
				else if ((temp>30) && (temp<=35)) dur_water = DUR_3;
				else if ((temp>35) && (temp<=40)) dur_water = DUR_4;
				else if ((temp>40))								dur_water = DUR_5;
				
				water = 1;
				count10s = 0;
				rundate_water = sDate.Date;
			}
			else water = 0;
			if (hum <= h_threshold)
			{
				if (hum >= 80) 									dur_mist = DUR_OFF;
				else if ((hum<80) && (hum>=75)) dur_mist = DUR_1;
				else if ((hum<75) && (hum>=70)) dur_mist = DUR_2;
				else if ((hum<70) && (hum>=65)) dur_mist = DUR_3;
				else if ((hum<65) && (hum>=60)) dur_mist = DUR_4;
				else if ((hum<60))							dur_mist = DUR_5;
				
				mist = 1;
				count10s = 0;
				rundate_mist = sDate.Date;
			}
			else mist = 0;
		}
	}
	
	/* If alarm is set in the evening, the system will be triggered 
		 in the morning between 5 and 10 at least 8 hours before the set alarm  */
	else if ((sAlarm.AlarmTime.Hours>12) && (sAlarm.AlarmTime.Hours<=23))
	{
		if ((sTime.Hours>=5) && (sTime.Hours<=10) && (sDate.Date != rundate_water) && (sDate.Date != rundate_mist) 
			&& (sAlarm.AlarmTime.Hours-sTime.Hours >= 8))
		{
			if (temp >= t_threshold)
			{
				if (temp <= 20) 									dur_water = DUR_OFF;
				else if ((temp>20) && (temp<=25)) dur_water = DUR_1;
				else if ((temp>25) && (temp<=30)) dur_water = DUR_2;
				else if ((temp>30) && (temp<=35)) dur_water = DUR_3;
				else if ((temp>35) && (temp<=40)) dur_water = DUR_4;
				else if ((temp>40))								dur_water = DUR_5;
				
				water = 1;
				count10s = 1;
				rundate_water = sDate.Date;
			}
			else water = 0;
			if (hum <= h_threshold)
			{
				if (hum >= 80) 									dur_mist = DUR_OFF;
				else if ((hum<80) && (hum>=75)) dur_mist = DUR_1;
				else if ((hum<75) && (hum>=70)) dur_mist = DUR_2;
				else if ((hum<70) && (hum>=65)) dur_mist = DUR_3;
				else if ((hum<65) && (hum>=60)) dur_mist = DUR_4;
				else if ((hum<60))							dur_mist = DUR_5;
				
				mist = 1;
				count10s = 1;
				rundate_mist = sDate.Date;
			}
			else mist = 0;
		}
	}
	
	//Write the water and mist date to the SRAM
	TM_BKPSRAM_Write8(0x10, rundate_water);
	TM_BKPSRAM_Write8(0x12, rundate_mist);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
