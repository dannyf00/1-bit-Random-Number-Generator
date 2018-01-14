//use two independent clocks (one of them has large phase noise) to generate truly random numbers
//clock #1: coretick driven by the instruction clock HSI/HSE
//clock #2: RTC driven by the LSI (internal RC oscillator)
//for this demo, RTC is set up to pull the 2nd to the last bit

#include "gpio.h"					//we use F_CPU + gpio functions
#include "delay.h"					//we use software delays
#include "coretick.h"				//we use dwt
#include "rtc.h"

//hardware configuration
#define LEDG_PORT		GPIOC
#define LEDG			(1<<9)		//green led on PC9 on Discovery board

#define LEDB_PORT		GPIOC
#define LEDB			(1<<8)		//blue led on PC8 on Discovery board

#define LED_PORT		LEDG_PORT
#define LED				LEDG

#define RTC_PR			(LSI_VALUE / 16)	//rtc interval - at 1/16th of a second when RTC is fed LSI
//end hardware configuration

//global defines

//global variables

int main(void) {
	uint32_t tmp1, tmp=0, tmp2;

	mcu_init();									//reset the  mcu

	//initialize the pins to outout
	IO_CLR(LEDG_PORT, LEDG); IO_OUT(LEDG_PORT, LEDG);					//ledg/b as output
	IO_CLR(LEDB_PORT, LEDB); IO_OUT(LEDB_PORT, LEDB);

	//initialize the timer / clocks
	coretick_init();							//reset coretick
	rtc_init(RTC_PR);							//initialize rtc - LSI_VALUE = 40Khz -> half a second
	//rtc_act(ledb_flp);						//install user handler
	tmp = tmp1 = rtc_get();						//initialize tmp

	ei();										//enable global interrupts
	while (1) {
		//poll coreticks' 2nd to the last bit upon RTC time-out
		if ((rtc_get() - tmp) >= 1) {			//if rtc has timed-out
			tmp += 1;							//increment rtc counter
			//LSB tends to be odd numbers
			if (coreticks() & (1<<1)) IO_SET(LEDB_PORT, LEDB); else IO_CLR(LEDB_PORT, LEDB);
		}

		//optional - flip LEDG periodically
		if ((rtc_get() - tmp1) >= 1) {
			tmp1 += 1;
			IO_FLP(LEDG_PORT, LEDG);				//flip ledg
		}
		//delay_ms(100);							//waste sometime
	};
}
