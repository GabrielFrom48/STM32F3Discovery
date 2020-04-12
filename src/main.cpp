/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f30x.h"
#include "stm32f3_discovery.h"
//#include "IRMessage.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_it.h"
#include "stm32f30x_spi.h"
#include "ILI9341.h"
#include "stm32f30x_rcc.h"
#include "stdlib.h"
#include <functional>
#include <vector>
#include "Image.h"

#define DebugLedON   GPIOE->BSRR |= GPIO_BSRR_BS_15
#define DebugLedOFF  GPIOE->BSRR |= GPIO_BSRR_BR_15

using namespace IRNec;
using namespace ILI9341;
using namespace std;
//static variables
GPIO_InitTypeDef GPIOEInitData, GPIOAInitData;
SPI_InitTypeDef SPIinitData;
TIM_TimeBaseInitTypeDef TIM2InitData,TIM1InitData;
TIM_ICInitTypeDef TIM1ICInitData;
TIM_OCInitTypeDef TIM1OCInitData;

void ShowNecMessage(ILI9341::TFTdisplay& disp,uint32_t Adr, uint32_t Com, uint8_t radix);
char Pbuffer[50], Dbuffer[50];



uint32_t irPushButton(uint32_t buttonCode, function<void(int)> handler);


void DWT_Init(void);
void Delay(uint32_t us);

int main(void)
{
    //------------GPIO-----------------------------------------------------
	//---Blinking led(PE8), Debug led(PE15) Pause led (PE10) --------
	//RCC->AHBENR |= (uint32_t)RCC_AHBENR_GPIOEEN;  //Clocking enabled
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

    GPIOEInitData.GPIO_Mode = GPIO_Mode_OUT;
    GPIOEInitData.GPIO_OType = GPIO_OType_PP;
    GPIOEInitData.GPIO_Speed = GPIO_Speed_10MHz;
    GPIOEInitData.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOEInitData.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15 | GPIO_Pin_10;
    GPIO_Init(GPIOE,&GPIOEInitData);
    DebugLedON;
    //---------------

    //---Input capture pins---------
    //   TI1 (TIM1 Channel 1)
    GPIOAInitData.GPIO_Mode = GPIO_Mode_AF;
    GPIOAInitData.GPIO_OType = GPIO_OType_PP;
    GPIOAInitData.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOAInitData.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOAInitData.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA,&GPIOAInitData);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_6);

    //---SPI pins---------
    //   SCLK  PA5
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_5);
    //   MISO PA6
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_5);
    //   MOSI PA7
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_5);

    GPIOAInitData.GPIO_Mode = GPIO_Mode_AF;
    GPIOAInitData.GPIO_OType = GPIO_OType_PP;
    GPIOAInitData.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOAInitData.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOAInitData.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA,&GPIOAInitData);

    //   CS-PA4  D/C-PA3  RESET-PA2
    GPIOAInitData.GPIO_Mode = GPIO_Mode_OUT;
    GPIOAInitData.GPIO_OType = GPIO_OType_PP;
    GPIOAInitData.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOAInitData.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOAInitData.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOA,&GPIOAInitData);

    //------------SPI------------------------------------------------------
    RCC->APB2ENR |= (uint32_t)RCC_APB2ENR_SPI1EN; //Clocking enabled
    //     Init data
    SPIinitData.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPIinitData.SPI_CPHA = SPI_CPHA_1Edge;
	SPIinitData.SPI_CPOL = SPI_CPOL_Low;
	SPIinitData.SPI_CRCPolynomial = SPI_CRCLength_8b;
	SPIinitData.SPI_DataSize = SPI_DataSize_8b;
	SPIinitData.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPIinitData.SPI_FirstBit = SPI_FirstBit_MSB;
	SPIinitData.SPI_Mode = SPI_Mode_Master;
	SPIinitData.SPI_NSS = SPI_NSS_Soft;

	//     Write init data to SPI CTL registers
	SPI_Init(SPI1,&SPIinitData);
	//     Disable TI mode
	SPI_TIModeCmd(SPI1,DISABLE);
	//     Enable SPI peripheral
	SPI_Cmd(SPI1,ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);

	//------display init--------------------------------------------------
	TFTdisplay& display = TFTdisplay::getInstance(GPIOA);
	display.FullScreen();
	display.Flood(WHITE);

    //------------TIMER2---------------------------------------------------
	TIM2InitData.TIM_ClockDivision = TIM_CKD_DIV1; // no input clock division
	TIM2InitData.TIM_CounterMode = TIM_CounterMode_Up; // TIM2 counts up
	TIM2InitData.TIM_Period = 0xFFFF; //autoreload value
	TIM2InitData.TIM_Prescaler = 0xFF; //
	TIM_TimeBaseInit(TIM2,&TIM2InitData);
	TIM_Cmd(TIM2,ENABLE);


	//---TIMER1 in Input Capture mode---------
	//Basic data
	TIM1InitData.TIM_ClockDivision = TIM_CKD_DIV1; // no input clock division
	TIM1InitData.TIM_CounterMode = TIM_CounterMode_Up; // TIM1 counts up
	TIM1InitData.TIM_Period = 0xFFFF; //autoreload value
	TIM1InitData.TIM_Prescaler = 71; //
	TIM_TimeBaseInit(TIM1,&TIM1InitData);

    //IC1 mode data
	TIM1ICInitData.TIM_Channel = TIM_Channel_1; //
	TIM1ICInitData.TIM_ICSelection = TIM_ICSelection_DirectTI; //TI1 connected to CS1 for PWM input mode
	TIM1ICInitData.TIM_ICFilter = 0x00; // No filter
	TIM1ICInitData.TIM_ICPolarity = TIM_ICPolarity_Falling; // Measuring high level duty
	TIM1ICInitData.TIM_ICPrescaler = TIM_ICPSC_DIV1; //No prescaler
    TIM_ICInit(TIM1,&TIM1ICInitData);

    /* Эта функция настроит канал 2 для захвата заполнения. */
    TIM_PWMIConfig(TIM1,&TIM1ICInitData);
    /* Выбираем источник для триггера: вход 1 (PA6) */
    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    /* По событию от триггера счётчик будет сбрасываться. */
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    /* Включаем события от триггера */
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);

    //---TIMER1 in Output Compare mode---------
    /* Канал 3 таймера 1 для сравнения счётчика со значением в CCR3
       и выдачи прерывания по таймауту для фиксации паузы между передачами*/
    TIM1OCInitData.TIM_OCMode = TIM_OCMode_Timing; // Output channel is in timing mode to detect ir idle state
	TIM1OCInitData.TIM_OutputState = TIM_OutputState_Disable; // No output signals in channel 3
	TIM1OCInitData.TIM_OutputNState = TIM_OutputState_Disable; // No output signals in channel 3
	TIM1OCInitData.TIM_Pulse = IR_IDLE;// see IRParce.h (15 ms pause)
	TIM_OC3Init(TIM1,&TIM1OCInitData);

	TIM_ITConfig(TIM2,TIM_DIER_UIE,ENABLE); //Timer2 overload int

    TIM_ITConfig(TIM1,TIM_IT_CC1,ENABLE); //Timer1 Input Capture channel 1 and 2 interrupt enabled
    TIM_ITConfig(TIM1,TIM_IT_CC3,ENABLE); //Timer1 Output Compare channel 3 interrupt enabled
    TIM_Cmd(TIM1,ENABLE);

    DWT_Init();
	//------------Interrupts-----------------------------------------------
    //asm("cpsid i");
    asm("cpsie i");
	NVIC_EnableIRQ(TIM1_CC_IRQn);      //Timer1 СС Interrupt enabled
	NVIC_SetPriority(TIM1_CC_IRQn,1);  //Priority 1
	NVIC_EnableIRQ(TIM2_IRQn);       //Timer2 Interrupt enabled
	NVIC_SetPriority(TIM2_IRQn,2);     //Priority 2

	//The main cycle
	while(1)
	{
		ShowNecMessage(display,ReadNextMessage().Adress,ReadNextMessage().Command,10);
		Delay(50000);
	}
}

//----------------------------------------------------------------------------
void DWT_Init()
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}

//----------------------------------------------------------------------------
void Delay(uint32_t us)
{
	uint32_t us_count_tic =  us * (SystemCoreClock / 1000000); // получаем кол-во тактов за 1 мкс и умножаем на наше значение
	DWT->CYCCNT = 0U; // обнуляем счётчик
	while(DWT->CYCCNT < us_count_tic);

}

//----------------------------------------------------------------------------
void ShowNecMessage(ILI9341::TFTdisplay& disp,uint32_t Adr, uint32_t Com, uint8_t radix)
{
	   disp.WriteString(10,150,(char*)"ADDRESS:",Font_16x26,BLUE,WHITE);
	   disp.WriteString(150,150,(char*)"     ",Font_16x26,BLUE,WHITE);
	   disp.WriteString(150,150,utoa(Adr,Pbuffer,radix),Font_16x26,BLUE,WHITE);

	   disp.WriteString(10,200,(char*)"COMMAND:",Font_16x26,BLUE,WHITE);
	   disp.WriteString(150,200,(char*)"     ",Font_16x26,BLUE,WHITE);
	   disp.WriteString(150,200,utoa(Com,Dbuffer,radix),Font_16x26,BLUE,WHITE);
}

//----------------------------------------------------------------------------
uint32_t irPushButton(uint32_t buttonCode, function<void(int)> handler)
{
	handler(buttonCode);
	return 0;
}


