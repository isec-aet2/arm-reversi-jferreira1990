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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_lcd.h"
#include "stm32f769i_discovery_ts.h"
#include "stdio.h"
#include <stdlib.h>
#include <time.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMP_REFRESH_PERIOD   1000    /* Internal temperature refresh period */
#define MAX_CONVERTED_VALUE   4095    /* Max converted value */
#define AMBIENT_TEMP            25    /* Ambient Temperature */
#define VSENS_AT_AMBIENT_TEMP  760    /* VSENSE value (mv) at ambient temperature */
#define AVG_SLOPE               25    /* Avg_Solpe multiply by 10 */
#define VREF                  3300
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DMA2D_HandleTypeDef hdma2d;

DSI_HandleTypeDef hdsi;

LTDC_HandleTypeDef hltdc;

SD_HandleTypeDef hsd2;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */

TS_StateTypeDef TS_State;

uint32_t ConvertedValue;
long int JTemp;
char desc[100];
char clockTime[20];
char timerString[20];
char timeOutString[20];

int startSoft=0;
int startARMSoft=0;

int sec=0;
int min=0;

int turnFlag=0;
int startFlag=0;
int startARMFlag=0;
int flagClock=0;
int downTimer=20;

char board[8][8]={"........",
                  "........",
                  "........",
                  "...xo...",
                  "...ox...",
                  "........",
                  "........",
                  "........"};

char symbPlayer;
char symbAdv;

	int newPlayer=0;
	int gameON=0;
	int gameARMon=0;
	int gameEnd=0;
	int firstMove=0;

	uint16_t casaX=0;
	uint16_t casaY=0;
	uint16_t jogadaX=0;
	uint16_t jogadaY=0;

	uint32_t colorP1 =  LCD_COLOR_LIGHTGREEN ;
	uint32_t colorP2 =  LCD_COLOR_LIGHTRED ;
	uint32_t colorPlayer;
	//uint32_t colorAdv;

	int timOutFlag=0;
	int timOutP1=0;
	int timOutP2=0;

	int j=0;

	int JogPossiveisX[20]={0};
	int JogPossiveisY[20]={0};
	int indx=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA2D_Init(void);
static void MX_DSIHOST_DSI_Init(void);
static void MX_FMC_Init(void);
static void MX_LTDC_Init(void);
static void MX_TIM6_Init(void);
static void MX_ADC1_Init(void);
static void MX_SDMMC2_SD_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */

static void LCD_Config();

static void startGame();

void gameOver();

void playARM();

void jogadasPossiveis(char symbPlayer, char symbAdv);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){



	if(GPIO_Pin == GPIO_PIN_13){

		BSP_TS_GetState(&TS_State);

		if(TS_State.touchDetected){

		   if( (int)TS_State.touchX[0] <480 && (int)TS_State.touchY[0] <480  ){

			   HAL_Delay(100);

			   turnFlag=1;


            }

		   if( (int)TS_State.touchX[0] >500 &&  (int)TS_State.touchX[0] <780 && (int)TS_State.touchY[0] >180 && (int)TS_State.touchY[0] < 230  ){

			   if(startSoft==0  && startARMSoft==0){
				   startFlag=1;
				   startSoft++;

				   BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
				   BSP_LCD_DrawRect(500, 180, 280, 50);
			   }


		   }

		   if( (int)TS_State.touchX[0] >500 &&  (int)TS_State.touchX[0] <780 && (int)TS_State.touchY[0] >100 && (int)TS_State.touchY[0] < 150  ){

		   			   if(startSoft==0  && startARMSoft==0){
		   				   startARMFlag=1;
		   				    startARMSoft++;

		   				BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
		   				BSP_LCD_DrawRect(500, 100, 280, 50);

		   			   }
        		   }

	}
  }

}

void  HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM6){           // 2 em 2 seg
					HAL_StatusTypeDef status=HAL_ADC_PollForConversion(&hadc1,TEMP_REFRESH_PERIOD);
			  	  	  if(status==HAL_OK)
			  	  	  {
			  	  		  ConvertedValue=HAL_ADC_GetValue(&hadc1); //get value

			  	  		  JTemp = ((((ConvertedValue * VREF)/MAX_CONVERTED_VALUE) - VSENS_AT_AMBIENT_TEMP) * 10 / AVG_SLOPE) + AMBIENT_TEMP;
			  	  		  /* Display the Temperature Value on the LCD */
			  	  		  sprintf(desc, "T= %ld C", JTemp);
			  	  		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			  	  	      BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			  	  	      BSP_LCD_SetFont(&Font16);
			  	  		  BSP_LCD_DisplayStringAt(650, 435, (uint8_t *)desc, LEFT_MODE);
			  	  	  }

	}

	if(htim->Instance == TIM7){
		flagClock=1;

		}


}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_DMA2D_Init();
  MX_DSIHOST_DSI_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_SDMMC2_SD_Init();
  MX_TIM7_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
    BSP_LED_Init(LED_RED);
    BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);

    LCD_Config();

    BSP_TS_Init(800,480);
    BSP_TS_ITConfig();

    HAL_ADC_Start(&hadc1);     //start do adc

    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start_IT(&htim7);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
/*  PUSH-BUTTON  */
	  if(BSP_PB_GetState(BUTTON_WAKEUP)==1){
		  LCD_Config();
		  gameON=0;
		  gameARMon=0;
		  startSoft=0;
		  startARMSoft=0;
		  newPlayer=0;

		  //volta a escrever a matriz
		        for (int i=0;i<8;i++){
		                    for (int j=0;j<8;j++){
		                   board[i][j]='.';
		                  }
		                }
		                board[3][3]='x';
		                board[3][4]='o';
		                board[4][3]='o';
		                board[4][4]='x';
	  }




/*    Play game against Human     */
if(startFlag==1 ){
	  startFlag=0;

	  gameON=1;
	  startGame();

}

if (gameON==1){

//Player1

	  if(newPlayer%2 == 0){

	  	 BSP_LCD_SetTextColor(colorP1);
	  	 BSP_LCD_DisplayStringAt(590, 250, (uint8_t *)"PLAYER 1", LEFT_MODE);

	  	 symbPlayer='x';
	  	 symbAdv='o';
	  	 colorPlayer=colorP1;
	  	 //colorAdv=colorP2;

	  	 if(turnFlag==1){
	  		 downTimer=20;
	  	 }

	  	 if(timOutFlag == 1){
	  		timOutFlag=0;
	  		timOutP1++;

	  		sprintf(timeOutString, "Timeout P1: %d", timOutP1);
	  		BSP_LCD_DisplayStringAt(550, 320, (uint8_t *)timeOutString, LEFT_MODE);

	  	 }

	  	 if(timOutP1 == 3){
	  		gameON = 0;
	  		gameEnd=1;
	  	 }


	    }

//Player2

	 else{

		 //newPlayer++;

	  	BSP_LCD_SetTextColor(colorP2);
	  	 BSP_LCD_DisplayStringAt(590, 250, (uint8_t *)"PLAYER 2", LEFT_MODE);

	  	symbPlayer='o';
	    symbAdv='x';
	    colorPlayer=colorP2;
	    //colorAdv=colorP1;

	    if(turnFlag==1){
	    	downTimer=20;
	    }

	    if(timOutFlag == 1){
	       timOutFlag=0;
    		timOutP2++;

    		sprintf(timeOutString, "Timeout P2: %d", timOutP2);
    		BSP_LCD_DisplayStringAt(550, 340, (uint8_t *)timeOutString, LEFT_MODE);
	    }

	    if(timOutP2 == 3){
	  		gameON = 0;
	  		gameEnd=1;
	    }

	   }

//----colocar as peças-------------------------------------------------------------------------------------




	  casaX = (int)TS_State.touchX[0]/60;
	  casaY = (int)TS_State.touchY[0]/60;
	  jogadaX = casaX*60+30;
	  jogadaY = casaY*60+30;

	  BSP_LCD_SetTextColor(colorPlayer);

	  //verificar q a casa está vazia
	  if(board[casaY][casaX] == '.'  ){


		  //verificar se existe peça adversaria na casa adjacente
		  //verfica linha-----------------------------------------------------------------------------

			  if( board[casaY+1][casaX]==symbAdv ){


				  BSP_LCD_FillCircle(jogadaX,jogadaY,25);

				  board[casaY][casaX]=symbPlayer;

				  //troca as peças
				  for (int i = casaY ; i<8 ; i++){
				  	 if( board [i][casaX] == symbPlayer){
				  		  j=i;
				  		  for(int i = casaY; i<=j; i++){
				  			 board[i][casaX]= symbPlayer;

				  			  BSP_LCD_FillCircle(jogadaX,  (i*60+30)  ,25);
				  			  }
				  		  }
				  }



			  }
			  else if(board[casaY-1][casaX]==symbAdv ){

			  				  BSP_LCD_FillCircle(jogadaX,jogadaY,25);

			  				  board[casaY][casaX]=symbPlayer;

			  				 //troca as peças
			  				for (int i = casaY ; i>0 ; i--){
			  				      if( board [i][casaX] == symbPlayer){
			  						  j=i;
			  						 for(int i = casaY; i>=j; i--){
			  						  board[i][casaX]= symbPlayer;

			  						  BSP_LCD_FillCircle(jogadaX,  (i*60+30)  ,25);

			  						  }
			  					  }
			  				 }


			  }

		  //verfica coluna-----------------------------------------------------------------------

			  if( board[casaY][casaX+1] == symbAdv ){

				  BSP_LCD_FillCircle(jogadaX,jogadaY,25);

				board[casaY][casaX]=symbPlayer;

				  //troca as peças
				 for (int i = casaX ; i<8 ; i++){
					 if( board [casaY][i] == symbPlayer){
					  j=i;
					  for(int i = casaX; i<=j; i++){
						 board[casaY][i]= symbPlayer;

						BSP_LCD_FillCircle((i*60+30),  jogadaY  ,25);
						}
					  }
		        }



			  }
			  else if(  board[casaY][casaX-1]==symbAdv ){

			  				 BSP_LCD_FillCircle(jogadaX,jogadaY,25);

			  				board[casaY][casaX]=symbPlayer;

			  				//troca as peças
			  				for (int i = casaX ; i>0 ; i--){
			  				    if( board [casaY][i] == symbPlayer){
			  					 j=i;
			  					 for(int i = casaX; i>=j; i--){
			  						 board[casaY][i]= symbPlayer;

			  					      BSP_LCD_FillCircle((i*60+30),  jogadaY  ,25);

			  						 }
			  					 }
			  				 }



				  }



	}

	  if(turnFlag==1){
	  		turnFlag=0;
	  	   newPlayer++;
	  	}

}

/* Play against ARM   */
if(startARMFlag==1){
	startARMFlag=0;

	 startGame();
	 gameARMon=1;

}

if(gameARMon ==1){
	playARM();

}


/*Game over */

if(gameEnd==1){
	gameEnd=0;

	gameOver();

}

/*   Clock     */
if(flagClock==1){
		  flagClock=0;

		  sec++;
		  if(sec==60){
			  sec=0;
			  min++;
		  }

		  BSP_LCD_SetFont(&Font16);
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  sprintf(clockTime, "Time: %2d:%2d", min,sec);
		  BSP_LCD_DisplayStringAt(650, 400, (uint8_t *)clockTime, LEFT_MODE);

		  //Temporizador de 20 seg
		  if(gameON ==1 || gameARMon ==1){
			  downTimer--;
			  sprintf(timerString, "Time Left: %2d s", downTimer);
			  BSP_LCD_DisplayStringAt(550, 300, (uint8_t *)timerString, LEFT_MODE);

			  if (downTimer <= 0){
			  	turnFlag=1;
			  	timOutFlag=1;

			  }
		  }

	  }


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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SDMMC2
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 3;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc2ClockSelection = RCC_SDMMC2CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief DSIHOST Initialization Function
  * @param None
  * @retval None
  */
static void MX_DSIHOST_DSI_Init(void)
{

  /* USER CODE BEGIN DSIHOST_Init 0 */

  /* USER CODE END DSIHOST_Init 0 */

  DSI_PLLInitTypeDef PLLInit = {0};
  DSI_HOST_TimeoutTypeDef HostTimeouts = {0};
  DSI_PHY_TimerTypeDef PhyTimings = {0};
  DSI_LPCmdTypeDef LPCmd = {0};
  DSI_CmdCfgTypeDef CmdCfg = {0};

  /* USER CODE BEGIN DSIHOST_Init 1 */

  /* USER CODE END DSIHOST_Init 1 */
  hdsi.Instance = DSI;
  hdsi.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
  hdsi.Init.TXEscapeCkdiv = 4;
  hdsi.Init.NumberOfLanes = DSI_ONE_DATA_LANE;
  PLLInit.PLLNDIV = 20;
  PLLInit.PLLIDF = DSI_PLL_IN_DIV1;
  PLLInit.PLLODF = DSI_PLL_OUT_DIV1;
  if (HAL_DSI_Init(&hdsi, &PLLInit) != HAL_OK)
  {
    Error_Handler();
  }
  HostTimeouts.TimeoutCkdiv = 1;
  HostTimeouts.HighSpeedTransmissionTimeout = 0;
  HostTimeouts.LowPowerReceptionTimeout = 0;
  HostTimeouts.HighSpeedReadTimeout = 0;
  HostTimeouts.LowPowerReadTimeout = 0;
  HostTimeouts.HighSpeedWriteTimeout = 0;
  HostTimeouts.HighSpeedWritePrespMode = DSI_HS_PM_DISABLE;
  HostTimeouts.LowPowerWriteTimeout = 0;
  HostTimeouts.BTATimeout = 0;
  if (HAL_DSI_ConfigHostTimeouts(&hdsi, &HostTimeouts) != HAL_OK)
  {
    Error_Handler();
  }
  PhyTimings.ClockLaneHS2LPTime = 28;
  PhyTimings.ClockLaneLP2HSTime = 33;
  PhyTimings.DataLaneHS2LPTime = 15;
  PhyTimings.DataLaneLP2HSTime = 25;
  PhyTimings.DataLaneMaxReadTime = 0;
  PhyTimings.StopWaitTime = 0;
  if (HAL_DSI_ConfigPhyTimer(&hdsi, &PhyTimings) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DSI_ConfigFlowControl(&hdsi, DSI_FLOW_CONTROL_BTA) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DSI_SetLowPowerRXFilter(&hdsi, 10000) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DSI_ConfigErrorMonitor(&hdsi, HAL_DSI_ERROR_NONE) != HAL_OK)
  {
    Error_Handler();
  }
  LPCmd.LPGenShortWriteNoP = DSI_LP_GSW0P_DISABLE;
  LPCmd.LPGenShortWriteOneP = DSI_LP_GSW1P_DISABLE;
  LPCmd.LPGenShortWriteTwoP = DSI_LP_GSW2P_DISABLE;
  LPCmd.LPGenShortReadNoP = DSI_LP_GSR0P_DISABLE;
  LPCmd.LPGenShortReadOneP = DSI_LP_GSR1P_DISABLE;
  LPCmd.LPGenShortReadTwoP = DSI_LP_GSR2P_DISABLE;
  LPCmd.LPGenLongWrite = DSI_LP_GLW_DISABLE;
  LPCmd.LPDcsShortWriteNoP = DSI_LP_DSW0P_DISABLE;
  LPCmd.LPDcsShortWriteOneP = DSI_LP_DSW1P_DISABLE;
  LPCmd.LPDcsShortReadNoP = DSI_LP_DSR0P_DISABLE;
  LPCmd.LPDcsLongWrite = DSI_LP_DLW_DISABLE;
  LPCmd.LPMaxReadPacket = DSI_LP_MRDP_DISABLE;
  LPCmd.AcknowledgeRequest = DSI_ACKNOWLEDGE_DISABLE;
  if (HAL_DSI_ConfigCommand(&hdsi, &LPCmd) != HAL_OK)
  {
    Error_Handler();
  }
  CmdCfg.VirtualChannelID = 0;
  CmdCfg.ColorCoding = DSI_RGB888;
  CmdCfg.CommandSize = 640;
  CmdCfg.TearingEffectSource = DSI_TE_EXTERNAL;
  CmdCfg.TearingEffectPolarity = DSI_TE_RISING_EDGE;
  CmdCfg.HSPolarity = DSI_HSYNC_ACTIVE_LOW;
  CmdCfg.VSPolarity = DSI_VSYNC_ACTIVE_LOW;
  CmdCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
  CmdCfg.VSyncPol = DSI_VSYNC_FALLING;
  CmdCfg.AutomaticRefresh = DSI_AR_ENABLE;
  CmdCfg.TEAcknowledgeRequest = DSI_TE_ACKNOWLEDGE_DISABLE;
  if (HAL_DSI_ConfigAdaptedCommandMode(&hdsi, &CmdCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DSI_SetGenericVCID(&hdsi, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DSIHOST_Init 2 */

  /* USER CODE END DSIHOST_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 7;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 14;
  hltdc.Init.AccumulatedVBP = 5;
  hltdc.Init.AccumulatedActiveW = 654;
  hltdc.Init.AccumulatedActiveH = 485;
  hltdc.Init.TotalWidth = 660;
  hltdc.Init.TotalHeigh = 487;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 0;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 0;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 0;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 0;
  pLayerCfg.ImageHeight = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 0;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 0;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief SDMMC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC2_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC2_Init 0 */

  /* USER CODE END SDMMC2_Init 0 */

  /* USER CODE BEGIN SDMMC2_Init 1 */

  /* USER CODE END SDMMC2_Init 1 */
  hsd2.Instance = SDMMC2;
  hsd2.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd2.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd2.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd2.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd2.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd2.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDMMC2_Init 2 */

  /* USER CODE END SDMMC2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 14141;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 14141;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 9999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 9999;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 16;
  SdramTiming.ExitSelfRefreshDelay = 16;
  SdramTiming.SelfRefreshTime = 16;
  SdramTiming.RowCycleDelay = 16;
  SdramTiming.WriteRecoveryTime = 16;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 16;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /*Configure GPIO pin : PI13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PI15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
static void LCD_Config(void)
{
 int Xpos=0;
 int Space=60;
 int j=0;

  uint32_t  lcd_status = LCD_OK;

  /* Initialize the LCD */
  lcd_status = BSP_LCD_Init();
  while(lcd_status != LCD_OK);

  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);

  /* Clear the LCD */
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Set the Board  */
   //Draw vertical lines
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

  j=0;
  for(int i=0; i<9; i++){

	  	  BSP_LCD_DrawVLine(Xpos+j, 0, 480 );
	  	  j=j+Space;
  }
  //Draw horizontal lines
    BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

    j=0;
    for(int i=0; i<9; i++){

  	  	  BSP_LCD_DrawHLine(0, j, 480 );
  	  	  j=j+Space;
    }

    /*Game menu*/
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
    BSP_LCD_FillRect(490, 0, 300, 60);
    BSP_LCD_DrawRect(490, 0, 300, 470);


    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
      BSP_LCD_SetFont(&Font24);
      BSP_LCD_DisplayStringAt(580, 15, (uint8_t *)"REVERSI", LEFT_MODE);


      /*Start button*/

      BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
      BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
      BSP_LCD_FillRect(500, 100, 280, 50);
      BSP_LCD_FillRect(500, 180, 280, 50);

      BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
       BSP_LCD_SetFont(&Font24);
       BSP_LCD_DisplayStringAt(595, 115, (uint8_t *)"Play ARM", LEFT_MODE);
       BSP_LCD_DisplayStringAt(595, 195, (uint8_t *)"Play Human", LEFT_MODE);


}

static void startGame(){
    int Xpos=0;
    int Space=60;
	int j=0;

	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_FillRect(0, 0, 480, 480);

	/* Set the Board  */
	   //Draw vertical lines
	  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

	  j=0;
	  for(int i=0; i<9; i++){

		  	  BSP_LCD_DrawVLine(Xpos+j, 0, 480 );
		  	  j=j+Space;
	  }
	  //Draw horizontal lines
	    BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

	    j=0;
	    for(int i=0; i<9; i++){

	  	  	  BSP_LCD_DrawHLine(0, j, 480 );
	  	  	  j=j+Space;
	    }

	/*Draw initial symbols*/

		 BSP_LCD_SetTextColor(LCD_COLOR_LIGHTRED);
		 BSP_LCD_FillCircle(210,270,25);
		 BSP_LCD_FillCircle(270,210,25);

		  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
		  BSP_LCD_FillCircle(210,210,25);
		  BSP_LCD_FillCircle(270,270,25);

    /*Draw window for communication */
		  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
		  BSP_LCD_FillRect(500, 280, 281, 180);


}

void gameOver(){
unsigned int nBytes;

int cnt_x =0;
int cnt_o=0;

char score_x[20];
char score_o[20];

//conta o numero de peças dos jogadores
 for (int i=0; i<8; i++){
    for (int j=0; j<8; j++){
         if (board[i][j]=='x'){
             cnt_x++;
        	  }
         if (board[i][j]=='o'){
        	  cnt_o++;
        	  }
   }
 }

 if(cnt_x > cnt_o){
	 BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	 BSP_LCD_SetFont(&Font24);
	 BSP_LCD_DisplayStringAt(520, 115, (uint8_t *)"PLAYER 1 WINS" , LEFT_MODE);
 }

 else if(cnt_x < cnt_o){
	 BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	 BSP_LCD_SetFont(&Font24);
	 BSP_LCD_DisplayStringAt(520, 115, (uint8_t *)"PLAYER 2 WINS" , LEFT_MODE);
 }

 if(cnt_x == cnt_o){
	 BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	 BSP_LCD_SetFont(&Font24);
	 BSP_LCD_DisplayStringAt(520, 115, (uint8_t *)"IT'S A TIE" , LEFT_MODE);
 }


//desenha os menus
     BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
     BSP_LCD_FillRect(0, 0, 800, 60);
     BSP_LCD_FillRect(0, 180, 800, 120);

     BSP_LCD_SetBackColor(LCD_COLOR_DARKBLUE);
     BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
     BSP_LCD_SetFont(&Font24);
     BSP_LCD_DisplayStringAt(0, 15, (uint8_t *)"GAME OVER", CENTER_MODE);

     BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
      BSP_LCD_SetFont(&Font24);
      BSP_LCD_DisplayStringAt(0, 185, (uint8_t *)"SCORE", CENTER_MODE);

      sprintf(score_x, "Player 1: %d", cnt_x);
      BSP_LCD_DisplayStringAt(0, 215, (uint8_t *)score_x, CENTER_MODE);
      sprintf(score_o, "Player 2: %d", cnt_o);
      BSP_LCD_DisplayStringAt(0, 235, (uint8_t *)score_o, CENTER_MODE);


//volta a escrever a matriz
      for (int i=0;i<8;i++){
                  for (int j=0;j<8;j++){
                 board[i][j]='.';
                }
              }
              board[3][3]='x';
              board[3][4]='o';
              board[4][3]='o';
              board[4][4]='x';

//Escreve p cartao SD

if(f_mount(&SDFatFS,SDPath,0)!=FR_OK)
       Error_Handler();

if(f_open(&SDFile,"score.txt",FA_WRITE | FA_CREATE_ALWAYS)!=FR_OK)
       Error_Handler();

 if(f_write(&SDFile, score_x, strlen(score_x), &nBytes)!=FR_OK)
    	Error_Handler();

 if(f_write(&SDFile, score_o, strlen(score_o), &nBytes)!=FR_OK)
    	Error_Handler();

 f_close (&SDFile);

}

void playARM(){

//Player1

		  if(newPlayer%2 == 0){


		  	 BSP_LCD_SetTextColor(colorP1);
		  	 BSP_LCD_DisplayStringAt(590, 250, (uint8_t *)"PLAYER 1", LEFT_MODE);

		  	 symbPlayer='x';
		  	 symbAdv='o';
		  	 colorPlayer=colorP1;
		  	 //colorAdv=colorP2;

		  	if(turnFlag==1){
		  		downTimer=20;
		    }

		  	 if(timOutFlag == 1){
		  		timOutFlag=0;
		  		timOutP1++;
		  	 }

		  	 if(timOutP1 == 3){
		  		BSP_LCD_DisplayStringAt(570, 118, (uint8_t *)"PLAYER 1 LOOSES" , LEFT_MODE);
		  		gameARMon = 0;
		  		gameEnd=1;
		  	 }

		  	 //--Jogada do Player-----------------------------------------------------------

		  	 casaX = (int)TS_State.touchX[0]/60;
		  	 casaY = (int)TS_State.touchY[0]/60;

		  	BSP_LCD_SetTextColor(colorPlayer);

		  	jogadaX = casaX*60+30;
		  	jogadaY = casaY*60+30;

		  BSP_LCD_FillCircle(jogadaX,jogadaY,25);

		 board[casaY][casaX]=symbPlayer;

		 /*
		  	jogadasPossiveis(symbPlayer, symbAdv);

		  	 //verificar se a jogada e possivel
		  	*/



		  }


//Player2

		 else{

			newPlayer++;         //qd e o ARM a jogar tenho q incrementar a variavel

		  	BSP_LCD_SetTextColor(colorP2);
		  	 BSP_LCD_DisplayStringAt(590, 250, (uint8_t *)"   ARM   ", LEFT_MODE);

		  	symbPlayer='o';
		    symbAdv='x';
		    colorPlayer=colorP2;
		    //colorAdv=colorP1;

		    if(turnFlag==1){
		    	downTimer=20;
		    }

		    if(timOutFlag == 1){
		       timOutFlag=0;
	    		timOutP2++;
		    }

		    if(timOutP2 == 3){
		  		BSP_LCD_DisplayStringAt(570, 118, (uint8_t *)"ARM LOOSES" , LEFT_MODE);
		  		gameARMon = 0;
		  		gameEnd=1;
		    }

		    //--Jogada do ARM---------------------------

		    jogadasPossiveis(symbPlayer, symbAdv);

		    srand(time(NULL));

		    int random_number = rand() %20;

		    casaX = JogPossiveisX[random_number];
		    casaY = JogPossiveisY[random_number];

		    //----colocar as peças-------------------------------------------------------------------------------------

		    		 BSP_LCD_SetTextColor(colorPlayer);

		    		 jogadaX = casaX*60+30;
		    		 jogadaY = casaY*60+30;

		    		 BSP_LCD_FillCircle(jogadaX,jogadaY,25);

		    		board[casaY][casaX]=symbPlayer;

	 }





   if(turnFlag==1){
		 turnFlag=0;
		   newPlayer++;
       }

}

void jogadasPossiveis(char symbPlayer, char symbAdv){


	//reinicia as matrizes de jogadas possiveis
	for(int i=0; i<=20; i++){
		JogPossiveisX[i]=JogPossiveisY[i]=0;
	}

	for(casaX=0; casaX<=7; casaX++){

			for(casaY=0; casaY<=7; casaY++){

				//verificar q a casa está vazia
				if(board[casaY][casaX] == '.'  ){


				//verificar se existe peça adversaria na casa adjacente
			    //verifica linha-----------------------------------------------------------------------------

				 if( board[casaY+1][casaX]==symbAdv ){
					 JogPossiveisX[indx]=casaX;
					 JogPossiveisY[indx]=casaY;
				  	 indx++;
			    }
				else if(board[casaY-1][casaX]==symbAdv ){

					  JogPossiveisX[indx]=casaX;
					  JogPossiveisY[indx]=casaY;
					  indx++;
				}

			//verfica coluna-----------------------------------------------------------------------

				if( board[casaY][casaX+1] == symbAdv ){
					  JogPossiveisX[indx]=casaX;
					  JogPossiveisY[indx]=casaY;
				    	 indx++;
				 }
				else if(  board[casaY][casaX-1]==symbAdv ){
					JogPossiveisX[indx]=casaX;
			    	JogPossiveisY[indx]=casaY;
		          	indx++;
				}

			}
		}
	   }

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

 BSP_LED_On(LED_RED);

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
