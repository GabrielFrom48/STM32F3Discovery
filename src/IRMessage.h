/*
 * IRMessage.h
 *
 *  Created on: 6 апр. 2020 г.
 *      Author: Garick
 */

#ifndef IRMESSAGE_H_
#define IRMESSAGE_H_

#include "stm32f30x.h"
#include "stdlib.h"

/* */

/*          ___________       _______
 *         |           |     |
 *         |           |     |
 *  _______|<--Pulse-->|_____|
 *
 *         |<-----Width----->|
 */


#define IR_NO_COMMAND   0

#define IR_NEC_START_PULSE      9000
#define IR_NEC_START_WIDTH      13500

#define IR_NEC_0_PULSE          562
#define IR_NEC_0_WIDTH          1125

#define IR_NEC_1_PULSE          562
#define IR_NEC_1_WIDTH          2250

#define IR_IDLE                 25000

#define IR_REPEAT_PULSE         9000
#define IR_REPEAT_WIDTH         11250


#define IR_NEC_TOL              20
#define IR_NEC_ABS_TOL          150

#define IR_NEC_START_PULSE_TOL  100
#define IR_NEC_START_WIDTH_TOL  500

#define IR_NEC_1_PULSE_TOL      100
#define IR_NEC_1_WIDTH_TOL      100

#define IR_NEC_0_PULSE_TOL      100
#define IR_NEC_0_WIDTH_TOL      100

#define IRSTATUS_RESUME        9
#define IRSTATUS_IDLE          12

/* Parsing return codes */
#define BAD_IR_START_PULSE     80
#define BAD_IR_START_WIDTH     81
#define BAD_IR_0_WIDTH         82
#define BAD_IR_1_WIDTH         83
#define BAD_IR_BIT             1
#define GOOD_IR_BIT            0

#define MAX_BUF_SIZE    33   // Start pulse/width             1bit
                             //         V
                             // Address pulse/width           8bit
                             //         V
                             // NAddress pulse/width          8bit
                             //         V
                             // Command pulse/width           8bit
                             //         V
                             // NCommand pulse/width          8bit
// этои дефайны просто сравнивает значения с заданной точностью...
// в процентах
#define checkVal(var,val,tol) (var>(val*(100-tol)/100) && var<(val*(100+tol)/100))
// в абсолютном значении
#define Check(var,val,tol)    (var>(val-tol) && var<(val+tol))

/*----------------------------------------------------------------------------*/
using namespace std;

namespace IRNec
{

/*---------------- NECSequenceTypeDef ----------------*/
typedef union
{
    uint32_t Frame;
	struct
	{
     uint32_t  Adress:8;
     uint32_t  NAdress:8;
     uint32_t  Command:8;
     uint32_t  NCommand:8;
	};
} NECSequenceTypeDef;

typedef struct
{
	uint32_t  Adress;
	uint32_t  Command;
}NECMessage;
/*---------------- CapturedSequenceTypeDef ----------------*/
typedef union
{
	uint16_t CapturedSequence[66];
	struct
	{
		uint16_t StartWidth;  uint16_t StartPulse;

		uint16_t AdressWidth0;uint16_t AdressPulse0;
		uint16_t AdressWidth1;uint16_t AdressPulse1;
		uint16_t AdressWidth2;uint16_t AdressPulse2;
		uint16_t AdressWidth3;uint16_t AdressPulse3;
		uint16_t AdressWidth4;uint16_t AdressPulse4;
		uint16_t AdressWidth5;uint16_t AdressPulse5;
		uint16_t AdressWidth6;uint16_t AdressPulse6;
		uint16_t AdressWidth7;uint16_t AdressPulse7;

		uint16_t NAdressWidth0;uint16_t NAdressPulse0;
		uint16_t NAdressWidth1;uint16_t NAdressPulse1;
		uint16_t NAdressWidth2;uint16_t NAdressPulse2;
		uint16_t NAdressWidth3;uint16_t NAdressPulse3;
		uint16_t NAdressWidth4;uint16_t NAdressPulse4;
		uint16_t NAdressWidth5;uint16_t NAdressPulse5;
		uint16_t NAdressWidth6;uint16_t NAdressPulse6;
		uint16_t NAdressWidth7;uint16_t NAdressPulse7;

		uint16_t CommandWidth0;uint16_t CommandPulse0;
		uint16_t CommandWidth1;uint16_t CommandPulse1;
		uint16_t CommandWidth2;uint16_t CommandPulse2;
		uint16_t CommandWidth3;uint16_t CommandPulse3;
		uint16_t CommandWidth4;uint16_t CommandPulse4;
		uint16_t CommandWidth5;uint16_t CommandPulse5;
		uint16_t CommandWidth6;uint16_t CommandPulse6;
		uint16_t CommandWidth7;uint16_t CommandPulse7;

		uint16_t NCommandWidth0;uint16_t NCommandPulse0;
		uint16_t NCommandWidth1;uint16_t NCommandPulse1;
		uint16_t NCommandWidth2;uint16_t NCommandPulse2;
		uint16_t NCommandWidth3;uint16_t NCommandPulse3;
		uint16_t NCommandWidth4;uint16_t NCommandPulse4;
		uint16_t NCommandWidth5;uint16_t NCommandPulse5;
		uint16_t NCommandWidth6;uint16_t NCommandPulse6;
		uint16_t NCommandWidth7;uint16_t NCommandPulse7;
	};
} CapturedSequenceTypeDef;

//Singleton class
class IRMessageSocket
{
  private:
	    IRMessageSocket();
	    IRMessageSocket(const IRMessageSocket&) = delete;
	    IRMessageSocket& operator=(IRMessageSocket&) = delete;
	    volatile uint32_t DecodedFrame;
	    volatile NECSequenceTypeDef NECSequence;
        volatile uint16_t BuffCounter;
        volatile CapturedSequenceTypeDef CapturedSequence;
	    volatile uint16_t IRStatus;
	    NECMessage Message;
        uint32_t GetDecodedFrame();
        uint32_t GetIRStatus();
        uint16_t ParseIRSequence();
        void     setIRstatus(uint16_t irstatus);
  public:
        uint32_t    getAddress();
        uint32_t    getCommand();
        NECMessage  getMessage();
        uint16_t    getIRstatus();
        uint32_t    NextData(uint32_t Tim1, uint32_t Tim2);
        inline void toIDLE()  {this->IRStatus = IRSTATUS_IDLE;};
        inline void toRESUME(){this->IRStatus = IRSTATUS_RESUME;};
        static IRMessageSocket& getInstance();
};
}

#endif /* IRMESSAGE_H_ */
