
/**	made for nucleo-f446re/nucleo-f446ze
 * this is the first lab of robotics programming
 *  it include the pipeline of memory register access
 *  we will define the structure of GPIO how it locate in memory
 *  and by changing some bits we will set pins as output or input
 *
 *  keywords: p - page, RM - reference manual, UM - user manual, PM - programming manual
 * */

#include <stdint.h>

// ------------ task ------------
//	Peripheral base address in the alias region;
//	Peripheral base address in the bit-band region;
//	Advanced Hardware Bus 1 Peripheral base address region in the bit-band region;
//	GPIOA Peripheral base address in the region in the bit-band region;
//	GPIOC Peripheral base address in the region in the bit-band region;

// ------------ note ------------
// bit-band alias means 32 bit region where one word (32 bit register)
// corresponds one bit in main memory region (bit-band region)

// p56-57 RM (2 Memory organization)
#define APB2_ALIAS 0x42000000 // check p28 PM
#define PEREF 0x40000000 // APB1
#define APB2 0x40010000 //  usart is here
#define AHB1  0x40020000 // GPIO is here
#define GPIOA 0x40020000
#define GPIOC 0x40020800

// make analog of CMSIS lib
// p 193-194 RM
typedef struct
{
   uint32_t MODER; // mode: input/output analog ....
   uint32_t OTYPER;
   uint32_t OSPEEDR;
   uint32_t PUPDR;

   uint32_t IDR; // read state of GPIO
   uint32_t ODR; // write state to GPIO

   uint32_t BSRR;
   uint32_t LCKR;
   uint32_t AFRL;
   uint32_t AFRH;
} GPIO;

// take address of GPIOA to easy set required register
// p57 RM
#define PIN_A 	((GPIO *) GPIOA)
#define PIN_C 	((GPIO *) GPIOC)

// ------------ note ------------
// reg |= 0b10; - set 1 to second bit of register
// reg &= ~0b10; - set 0 to second bit of register
// for more info check binary masking

#define LED_R_HIGH_REG (1 << 8)
#define LED_B_HIGH_REG (1 << 9)

#define BUTT_R_PRESSED_REG (1 << 5)
#define BUTT_B_PRESSED_REG (1 << 6)

#define SET_LED_R_HIGH (PIN_C->ODR |= LED_R_HIGH_REG)
#define SET_LED_R_LOW (PIN_C->ODR &= ~LED_R_HIGH_REG)

#define BUTT_R_IS_PRESSED ((PIN_C->IDR) & (BUTT_R_PRESSED_REG))
#define BUTT_B_IS_PRESSED ((PIN_C->IDR) & (BUTT_B_PRESSED_REG))

// ------------ user fnc ------------
void display(uint8_t digit);
void my_delay(uint16_t ms);

int main(void)
{
	// p 144, 57
	*((uint32_t *) 0x40023830) |= 1 | 4; // set rcc for gpio
	// config leds and buttons p 177 187
	// all pins are inputs by default
	PIN_C->MODER |= 0b0101<<16; // B=8 R=9  pin-led-gnd

	uint8_t counter = 0;

	// blink just for fun
	SET_LED_R_HIGH;
	my_delay(100);
	SET_LED_R_LOW;
	my_delay(100);


    /* Loop forever */
	for(;;){
		if(counter>0){
			if(BUTT_B_IS_PRESSED==0){
				counter--;
				my_delay(500);
			}
		}
		if(counter<3){
			if(BUTT_R_IS_PRESSED==0){
				counter++;
				my_delay(500);
			}
		}
		display(counter);
	}
}


// ------------ user fnc ------------
void display(uint8_t digit){
	switch(digit){
	case 0:// R - LOW, B - LOW
		PIN_C->ODR &= ~LED_R_HIGH_REG;
		PIN_C->ODR &= ~LED_B_HIGH_REG;
		break;
	case 1:// R - HIGH, B - LOW
		PIN_C->ODR |= LED_R_HIGH_REG;
		PIN_C->ODR &= ~LED_B_HIGH_REG;
		break;
	case 2: // R - LOW, B - HIGH
		PIN_C->ODR &= ~LED_R_HIGH_REG;
		PIN_C->ODR |= LED_B_HIGH_REG;
		break;
	case 3: // R - HIGH, B - HIGH
		PIN_C->ODR |= LED_R_HIGH_REG;
		PIN_C->ODR |= LED_B_HIGH_REG;
		break;
	}
}

void my_delay(uint16_t ms){ // Inaccurate delay
	for(uint64_t i =0; i<= ms*800; i++){
		;
	}

}
