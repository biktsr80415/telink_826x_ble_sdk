/*
 * app_gpio_irq.c
 *
 *  Created on: 2017-8-25
 *      Author: Administrator
 */

#if (DRIVER_TEST_MODE == TEST_GPIO_IRQ)

#include "app_gpio_irq.h"
#include "../../proj/drivers/adc.h"
#include "../../proj/drivers/uart.h"
#include "../../proj/drivers/i2c.h"
#include "../../proj/drivers/spi.h"

unsigned char gpio_irq_test_mode = 0;

void app_gpio_irq_test_init(unsigned int pin,enum gpio_pol polarity,enum gpio_irqmode mode){
	/***step1. set pin as gpio and enable input,open 10k pullup********/
	gpio_set_func(pin, AS_GPIO);           //enable GPIO func
	gpio_set_input_en(pin, 1);             //enable input
	gpio_set_output_en(pin, 0);            //disable output
	gpio_setup_up_down_resistor(pin, PM_PIN_PULLUP_10K);  //open pull up resistor

	/***step2.      set the polarity  ***/
	gpio_set_interrupt_pol(pin, pol_falling);

	/************************************************************
	 *  step3. config gpio irq mode. contain three mode: irq_gpio
	 *  gpio2risc0 / gpio2risc1 / gpio2risc2
	 *  open the relevant irq
	 */
	switch(mode){

	gpio_irq_test_mode = mode;

	case irq_gpio:
		gpio_en_interrupt(pin, 1);
		gpio_core_irq_enable_all(1);    //for irq_gpio, it's necessary.
		reg_irq_mask |= FLD_IRQ_GPIO_EN;
		break;

	case gpio2risc0:
		gpio_en_interrupt_risc0(pin, 1);
		reg_irq_mask |= FLD_IRQ_GPIO_EN;
		break;
		//////////////////////////
	case gpio2risc1:
		gpio_en_interrupt_risc1(pin, 1);
		reg_irq_mask |= FLD_IRQ_GPIO_EN;
		break;
	case gpio2risc2:
		gpio_en_interrupt_risc2(pin, 1);
		reg_irq_mask |= FLD_IRQ_GPIO_EN;
		break;
	default:
		break;
	}
}

//init the LED pin,for indication
void app_led_init(void){

	gpio_set_func(GPIO_LED_PIN, AS_GPIO);

	gpio_set_output_en(GPIO_LED_PIN, 1);//enable output

	gpio_set_input_en(GPIO_LED_PIN, 0); //disable input

	gpio_write(GPIO_LED_PIN, 0);
}


void app_gpio_irq_test_proc(void){
	unsigned char bit = (GPIO_TEST_PIN & 0xff);
	/************ irq gpio ***************/
	if(gpio_irq_test_mode == irq_gpio){
		reg_irq_src = FLD_IRQ_GPIO_EN;        // clear irq_gpio irq flag

		if (gpio_read(GPIO_TEST_PIN) & bit == 0){
			WaitMs(10);   //delay for eliminate jitter
			if (gpio_read(GPIO_TEST_PIN) & bit == 0){

				gpio_toggle(GPIO_LED_PIN);
			}
		}
	}
	/************* gpio2risc0 *************/
	else if(gpio_irq_test_mode == gpio2risc0){
		reg_irq_src = FLD_IRQ_GPIO_RISC0_EN;  // clear gpio2risc0 irq flag

		if (gpio_read(GPIO_TEST_PIN) & bit == 0){
			WaitMs(10);   //delay for eliminate jitter
			if (gpio_read(GPIO_TEST_PIN) & bit == 0){

				gpio_toggle(GPIO_LED_PIN);
			}
		}
	}
	/************* gpio2risc1 *************/
	else if (gpio_irq_test_mode ==  gpio2risc1){
		reg_irq_src = FLD_IRQ_GPIO_RISC1_EN;  // clear gpio2risc1 irq flag

		if (gpio_read(GPIO_TEST_PIN) & bit == 0){
			WaitMs(10);   //delay for eliminate jitter
			if (gpio_read(GPIO_TEST_PIN) & bit == 0){

				gpio_toggle(GPIO_LED_PIN);

			}
		}

	}
	/************* gpio2risc2 *************/
	else if (gpio_irq_test_mode ==  gpio2risc2){
		reg_irq_src = FLD_IRQ_GPIO_RISC2_EN;  // clear gpio2risc2 irq flag

		if (gpio_read(GPIO_TEST_PIN) & bit == 0){
			WaitMs(10);   //delay for eliminate jitter
			if (gpio_read(GPIO_TEST_PIN) & bit == 0){

				gpio_toggle(GPIO_LED_PIN);
			}
		}

	}
}



#endif
