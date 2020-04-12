#include "stm32f30x_it.h"
#include "ILI9341.h"


bool LedColorChange;

#define PauseLedON   GPIOE->BSRR |= GPIO_BSRR_BS_10
#define PauseLedOFF  GPIOE->BSRR |= GPIO_BSRR_BR_10

using namespace IRNec;
using namespace ILI9341;
using namespace std;

IRMessageSocket& IRSocket = IRMessageSocket::getInstance();

//-------Timer1 Capture/Compare interrupt handler-------
 void TIM1_CC_IRQHandler()
 {
	 // Input capture interrupt handling
	 if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
	 	{
		 //disable interrupt for a handle time
		 //NVIC_DisableIRQ(TIM1_CC_IRQn);
		  /* Reset flag */
		  TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
		  PauseLedOFF;
		  /* */
		  switch(IRSocket.getIRstatus())
		  {
		    case  IRSTATUS_RESUME:IRSocket.NextData((uint32_t)(TIM1->CCR1),(uint32_t)(TIM1->CCR2));
		    	  break;
		    case  IRSTATUS_IDLE:  IRSocket.toRESUME();
		    	  break;
		  }
	 	}

	 /* over-capture */
	 if (TIM_GetFlagStatus(TIM1, TIM_FLAG_CC2OF) != RESET)
	 	{
	 	  TIM_ClearFlag(TIM1, TIM_FLAG_CC2OF);
	 	}

	 /* Output compare interrupt handling.
	    Timeout is over without any signal. Set Idle status */
     if(TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
        {
    	  IRSocket.toIDLE();
    	 /* Reset flag */
    	  TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
    	  PauseLedON;
        }
 	//enable interrupt
 	//NVIC_EnableIRQ(TIM1_CC_IRQn);
 }


 //-------Timer2 overload interrupt handler-------
  void TIM2_IRQHandler()
  {
 	 LedColorChange = !LedColorChange;
 	 if(LedColorChange)
 		 {
 		  GPIOE->BSRR |= GPIO_BSRR_BS_15;
 		 }
 	 else
 		 {
 		  GPIOE->BSRR |= GPIO_BSRR_BR_15;
 		 }
 	 //Reset interrupt flag
 	TIM_ClearFlag(TIM2, TIM_IT_Update);
  }

  //---------Global variable reading functions-----------------------
  NECMessage ReadNextMessage()
  {
    return IRSocket.getMessage();
  }

  uint32_t ReadAddress()
  {
	return IRSocket.getAddress();
  }

  uint32_t ReadCommand()
  {
    return IRSocket.getCommand();
  }

