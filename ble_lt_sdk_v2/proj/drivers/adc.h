/*
 * adc.h
 *
 *  Created on: 2015-12-10
 *      Author: Telink
 */

#ifndef ADC_H_
#define ADC_H_

#if(__TL_LIB_8266__ || MCU_CORE_TYPE == MCU_CORE_8266)
#include "../mcu_spec/adc_8266.h"
#elif(__TL_LIB_MSSOC__ || (MCU_CORE_TYPE == MCU_CORE_MSSOC))
#include "../mcu_spec/adc_mssoc.h"
#endif



#endif /* ADC_H_ */
