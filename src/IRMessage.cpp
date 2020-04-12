/*
 * IRMessage.cpp
 *
 *  Created on: 6 апр. 2020 г.
 *      Author: Garick
 */

#include "IRMessage.h"
using namespace IRNec;
using namespace std;

//static struct init func

//NECSequenceTypeDef initNECSequence();
//CapturedSequenceTypeDef initCapturedSequence();
//NECMessage initMessage();
//Var init
//Static fields initialization
	//uint16_t IRMessagePort::BuffCounter = 0;
	//uint16_t IRMessagePort::IRStatus = IRSTATUS_IDLE;
	//volatile uint32_t IRMessagePort::DecodedFrame = 0;
	//NECSequenceTypeDef IRMessagePort::NECSequence = initNECSequence();
	//CapturedSequenceTypeDef IRMessagePort::CapturedSequence = initCapturedSequence();
	//NECMessage IRMessagePort::Message = initMessage();

//Member-function definitions


/*---------Constructor of singleton--------------------------------------------*/

IRMessageSocket::IRMessageSocket()
{
	this->BuffCounter = 0;
	this->toIDLE();
}
/*-----------------------------------------------------------------------------*/
IRMessageSocket& IRMessageSocket::getInstance()
{
    static IRMessageSocket  instance;
    return instance;
}

/*-----------------------------------------------------------------*/
uint32_t IRMessageSocket::GetDecodedFrame()
{
    return this->DecodedFrame;
}

/*-----------------------------------------------------------------*/
uint16_t IRMessageSocket::getIRstatus()
{
    return this->IRStatus;
}

/*-----------------------------------------------------------------*/
void IRMessageSocket::setIRstatus(uint16_t irstatus)
{
  if((irstatus==IRSTATUS_IDLE)||(irstatus==IRSTATUS_RESUME))
  {
	this->IRStatus = irstatus;
  }
  else irstatus = 0;
}

/*-----------------------------------------------------------------*/
uint32_t IRMessageSocket::NextData(uint32_t Tim1, uint32_t Tim2)
{
  switch(BuffCounter)
	{
	  case 0://----------Check for correct start----------------------
             if(Check(Tim1,IR_NEC_START_WIDTH,IR_NEC_START_WIDTH_TOL)&&
				Check(Tim2,IR_NEC_START_PULSE,IR_NEC_START_PULSE_TOL))
               {this->toRESUME(); break;}
             else
               {this->toIDLE(); return false;}
	  case MAX_BUF_SIZE:
		     this->DecodedFrame = ParseIRSequence();
		     this->BuffCounter = 0;
		 	 this->toIDLE();
		     return true;
	  default:
		     if((Check(Tim1,IR_NEC_START_WIDTH,IR_NEC_START_WIDTH_TOL)&&
		 		 Check(Tim2,IR_NEC_START_PULSE,IR_NEC_START_PULSE_TOL))||
				(Check(Tim1,IR_REPEAT_WIDTH,IR_NEC_START_WIDTH_TOL)&&
				 Check(Tim2,IR_REPEAT_PULSE,IR_NEC_START_PULSE_TOL)))
		       {
		    	 this->BuffCounter = 0;
		    	 this->toIDLE();
		    	 return false;
		       }
	}
  this->CapturedSequence.CapturedSequence[this->BuffCounter*2] = Tim1;
  this->CapturedSequence.CapturedSequence[this->BuffCounter*2+1] = Tim2;
  this->BuffCounter++;
  return true;
}
/*-----------------------------------------------------------------*/
uint16_t IRMessageSocket::ParseIRSequence()
{
	     this->NECSequence.Frame = 0;

		 //check for correct start pulse/width
		 if (!Check(CapturedSequence.StartWidth, IR_NEC_START_WIDTH, IR_NEC_START_WIDTH_TOL))
		   {
		     return IR_NO_COMMAND;
		   }

		 if (!Check(CapturedSequence.StartPulse, IR_NEC_START_PULSE, IR_NEC_START_PULSE_TOL))
		   {
		     return IR_NO_COMMAND;
		   }

		 for(uint32_t i=0; i<32; i++)
		    {
		       if (   Check(CapturedSequence.CapturedSequence[(i+1)*2], IR_NEC_0_WIDTH, IR_NEC_0_WIDTH_TOL)
		    	   && Check(CapturedSequence.CapturedSequence[(i+1)*2+1], IR_NEC_0_PULSE, IR_NEC_0_PULSE_TOL))
		                 {
		    	            NECSequence.Frame = NECSequence.Frame  >> 1;
		    	            continue;
		                 }

		       if (   Check(CapturedSequence.CapturedSequence[(i+1)*2], IR_NEC_1_WIDTH, IR_NEC_1_WIDTH_TOL)
		           && Check(CapturedSequence.CapturedSequence[(i+1)*2+1], IR_NEC_1_PULSE, IR_NEC_1_PULSE_TOL))
		                 {
		    	            NECSequence.Frame  = (NECSequence.Frame  >> 1) | 0x80000000;
		    	            continue;
		                 }

		       return IR_NO_COMMAND;
		    }

		 if ( (NECSequence.Frame & 0xff000000) != ( (~(NECSequence.Frame) & 0x00ff0000) <<8 )  )
		    {
			   return IR_NO_COMMAND;
		    }

		 if ( (NECSequence.Frame & 0xff00) != ( ((~NECSequence.Frame) & 0x00ff) <<8 )  )
		    {
			   return IR_NO_COMMAND;
		    }
         this->Message.Adress = NECSequence.Adress;
         this->Message.Command = NECSequence.Command;
		 return ((NECSequence.Frame& 0xff000000) >> 16) | ((NECSequence.Frame & 0xff00)>>8);
}

/*-----------------------------------------------------------------*/
uint32_t IRMessageSocket::getAddress()
{
	return this->NECSequence.Adress;
	//return CapturedSequence.AdressWidth0;
}

/*-----------------------------------------------------------------*/
uint32_t IRMessageSocket::getCommand()
{
    return this->NECSequence.Command;
	//return CapturedSequence.AdressPulse0;
}

/*-----------------------------------------------------------------*/
NECMessage IRMessageSocket::getMessage()
{
	this->Message.Adress = this->NECSequence.Adress;
	this->Message.Command = this->NECSequence.Command;
    return Message;
}

/*-------Static struct init----------*/

NECSequenceTypeDef initNECSequence()
{
	NECSequenceTypeDef out;
	out.Frame = 0;
	return out;
}

CapturedSequenceTypeDef initCapturedSequence()
{
	CapturedSequenceTypeDef out;
	uint16_t i;
	for(i=0;i<66;i++)
	  {
		out.CapturedSequence[i] = 0;
	  }
	return out;
}

NECMessage initMessage()
{
	NECMessage out;
	out.Adress = 0;
	out.Command = 0;
	return out;
}
