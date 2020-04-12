/*
 * SDcardspi.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: Garick
 */

#ifndef SDCARDSPI_H_
#define SDCARDSPI_H_

#include "stm32f30x.h"
#include "stm32f30x_spi.h"

namespace SDcards
{

  class SDcard_spi
   {
    public:
	   SDcard_spi();
	   virtual ~SDcard_spi();

    private:
	protected:
   };

} /* namespace SDcards */

#endif /* SDCARDSPI_H_ */
