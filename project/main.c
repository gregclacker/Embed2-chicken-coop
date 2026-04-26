#include "common.h"
#include "stepper.h"
#include "fsr.h"
#include "p9813.h"
//#include "MCP23S08.h"
#include "conveyor_belt.h"
#include "light_beam.h"

A4988_t stepperA = {
	.ms1 = {
		.port	= GPIOA,
		.pinN = 7,
	},
	.ms2 = {
		.port = GPIOB,
		.pinN	= 6,
	},
	.ms3 = {
		.port = GPIOC,
		.pinN	= 7,
	},
	.reset_n = {
		.port = GPIOA,
		.pinN	= 9,
	},
	.step = {
		.port = GPIOA,
		.pinN	= 8,
	},
	.dir = {
		.port = GPIOB,
		.pinN	= 3,
	},
	.sleep_n = {
		.port = GPIOA,
		.pinN	= 10,
	},
	.enable_n = {
		.port = GPIOA,
		.pinN	= 6,
	},
	.driveTimer = TIM1,
};

void initStepperA();

const GPIO_Pin_t 
		cvb_dir = { // TODO
			.port = GPIOA,
			.pinN = 9,
		},
		EB_LED = {
			.port = GPIOA,
			.pinN	= 5,
		},
		EB_USR_BTN = {
			.port = GPIOC,
			.pinN	= 13,
		}
		;

const GPIO_Pin_t 
	ledDriver_clk = {
			.port = GPIOA,
			.pinN = 3,
		},
	ledDriver_data = {
			.port = GPIOA,
			.pinN = 3,
		}
	;

const GPIO_Pin_t 
	hall_end = {
			.port = GPIOA,
			.pinN = 6,
		},
	hall_start = {
			.port = GPIOA,
			.pinN = 7,
		}
	;
		
const GPIO_Pin_t 
	MCPCS = {
			.port = GPIOA,
			.pinN = 10,
		};

		
/* MCP23S08_t mcp = (MCP23S08_t){
		.cs = MCPCS,
		.spi= SPI1,
		.addr_ext = 0b11,
	};*/
		
#include "stm32f4xx.h"
#include "conveyor_belt.h"
		
		void main_womp();
		
int main(){
	SysTick->LOAD = FCPU/1000 - 1;
	SysTick->VAL	= 0;
	SysTick->CTRL |= BV(2) | BV(1); // BV(1) enable interrupt, BV(2)internal clock.  https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-timer--systick/systick-control-and-status-register
	SysTick->CTRL |= BV(0); 	// enable SysTick
	
	// enable clock
	RCC->AHB1ENR |= BV(0); // PA
	RCC->AHB1ENR |= BV(1); // PB
	RCC->AHB1ENR |= BV(2); // PC
	RCC->AHB1ENR |= BV(3); // PD
	RCC->AHB1ENR |= BV(4); // PE
	RCC->AHB1ENR |= BV(5); // PF
	RCC->AHB1ENR |= BV(6); // PG
	RCC->AHB1ENR |= BV(7); // PH
	
	/*** init ***********************************************/
	
	// PA5 mode EB LED : DO
	EB_LED.port->MODER &= ~	(0b11 << (2 * EB_LED.pinN));
	EB_LED.port->MODER |= 	(0b01 << (2 * EB_LED.pinN));
	
	GPIO_setOut(&EB_LED, 1);
	delaymS(250);
	
	// PC13 mode EB usr btn : DI
	EB_USR_BTN.port->MODER &= ~	(0b11 << (2 * EB_USR_BTN.pinN));
	EB_USR_BTN.port->MODER |= 	(0b00 << (2 * EB_USR_BTN.pinN));
	GPIO_setBiasPU(&EB_USR_BTN);
	
	//initStepperA();
	//setSTEP(&stepperA, STEP_16);
	//initPWM(); // conveyor belt 
	//initSPI1(); // beam sensor
	//initMCP(); // beam sensor
	//initFsrHW();
	initLED_Driver();
	
	{ // init hall gpio sensor, DI
		hall_end.port->MODER &= ~	(0b11 << (2 * hall_end.pinN));
		hall_end.port->MODER |= 	(0b00 << (2 * hall_end.pinN));
		
		hall_start.port->MODER &= ~	(0b11 << (2 * hall_start.pinN));
		hall_start.port->MODER |= 	(0b00 << (2 * hall_start.pinN));
	}
	
	GPIO_setOut(&EB_LED, 0);
	delaymS(250);
	int FSR_Check = 0;
	
	/*** main loop ******************************************/
	
	/*
	const GPIO_Pin_t 
			pin = {
					.port = GPIOB,
					.pinN = 13,
				};
	pin.port->MODER &= ~	(0b11 << (2 * pin.pinN));
	pin.port->MODER |= 	(0b01 << (2 * pin.pinN));
				*/
	
	while(0){
		PWM_on();
		//GPIO_setOut(&pin, 1);
		delaymS(1e3);
		PWM_off();
		//GPIO_setOut(&pin, 0);
		delaymS(1e3);
	}
	
	/*{
		{
			RCC->AHB1ENR |= 0x1; // Enable GPIOA clock
			GPIOA->MODER &= ~0xF000; // Clear PA6 and PA7 mode
		}

		volatile int i = 0;
			
	while(1)
	{
		if (!(GPIOA->IDR & 0x40)) {
			GPIO_toggleOut(&EB_LED);
			delaymS
		}
		
		else if (!(GPIOA->IDR & 0x80))
			i = 2;
	}
	
	}*/
	
	// test sanity
	while(0) {
		GPIO_toggleOut(&EB_LED);
		delaymS(500);
	}
	
	//Set_Color_RGB(0,0,0);
	
	// test LEDs
	while(1) {
			main_womp();
	}
	
	// test beam sensor
	while(0) {
		//if(!beam_check()) {
			GPIO_setOut(&EB_LED, beam_integrity());
		//}
	}
	
	
	// test conveyor motor
	/* const GPIO_Pin_t 
			pin = {
				.port = GPIOA,
				.pinN = 8,
			};
	pin.port->MODER &= ~	(0b11 << (2 * pin.pinN));
	pin.port->MODER |= 	(0b01 << (2 * pin.pinN));
	*/
	while(0) {
		/*
		GPIO_setOut(&pin, 0);
		delaymS(1e3);
		GPIO_setOut(&pin, 1);
		delaymS(1e3);
		*/
		
		PWM_on();
		delaymS(5e3);
		PWM_off();
		delaymS(10e3);
	}
	
	// test hall sensors
	while(0){
		if(!GPIO_getIn(&hall_end)) {
			GPIO_toggleOut(&EB_LED);
			delaymS(100);
		} else if(!GPIO_getIn(&hall_start)) {
			GPIO_toggleOut(&EB_LED);
			delaymS(500);
		} else {
			GPIO_setOut(&EB_LED, 0);
		}
	}
	
	
	// test stepper
	GPIO_setOut(&stepperA.dir, 0);
	//setSTEP(&stepperA, STEP_8);
	while(1) {
		if(!stepperIsStepping(&stepperA)) {
			stepperSetSteps(&stepperA, 100);
			stepperStart(&stepperA);
		}	
		
		static uint32_t time = 0;
		if(tick - time >= 3000) {
			GPIO_toggleOut(&stepperA.dir);
			time = tick;
			GPIO_toggleOut(&EB_LED);
		}
		
		
		/*
		if(!GPIO_getIn(&hall_end)) {
			GPIO_setOut(&stepperA.dir, 0);
		} else if(!GPIO_getIn(&hall_start)) {
			GPIO_setOut(&stepperA.dir, 1);
		}
		*/
	}
	
	// test stepper & hall
	while(0) {
		static bool dir = false;
		if(GPIO_getIn(&hall_end)) {
			GPIO_toggleOut(&EB_LED);
			dir = false;
		} else if(GPIO_getIn(&hall_start)) {
			GPIO_toggleOut(&EB_LED);
			dir = true;
		}
		
		// set motor direction
		GPIO_setOut(&stepperA.dir, dir);
		
		if(stepperIsStepping(&stepperA))
			break;
		
		stepperSetSteps(&stepperA, 100);
		stepperStart(&stepperA);
		
		GPIO_toggleOut(&EB_LED);
	}
	
	
	while(1) {
		FSR_Check = FsrWeightCheck();
		switch(FSR_Check){
				default:
				case FSRC_EMPTY: 
				case FSRC_EGGONLY:
				case FSRC_CHICKEN:
					break;
			}
	}
	
	typedef enum {
		IDLE,
		SWEEPING,
		CHICKEN_present,
		RUN_CONV,
	} State_e;
		
	State_e state = IDLE;
	
	while(1) {
		//GPIO_toggleOut(&EB_LED);
		FSR_Check = FsrWeightCheck();
		
		switch(state) {
			default:
			case IDLE: {
					// stop everything, do nothing
					// if weight or beam sensor triggered, -> CHICKEN_present
					
					{ // weight sensor must be consistent for 1S before any state transistion
						static int FSR_previous = 12309123; // init with some bogus value
						static uint32_t startTime_mS = 0;
				
						if(FSR_previous != FSR_Check) {	// reset timer on weight change
							startTime_mS = tick;
							FSR_previous = FSR_Check;
							break;
						} else if(startTime_mS - tick < 1000) { // wait for state to remain the same for X mS
							break;
						}
					}
					
					switch(FSR_Check){
						default:
						case FSRC_EMPTY: 		break;
						case FSRC_EGGONLY: 
						case FSRC_CHICKEN:
								state = CHICKEN_present;
							break;
					}
				}break;
			
			case SWEEPING: {
					// sweeper is moving, watch for chicken
					// check beam sensor and weight for chicken, stop if detected
					// 	-> run convayer belt afterwards
				
					if(!beam_integrity()) { // if beam sensor go off assume chicken is around
						FSR_Check = FSRC_CHICKEN; 
					}
				
					static bool sweeping_out = true;
					
					switch(FSR_Check){
						default:
						case FSRC_EGGONLY: 
						case FSRC_EMPTY:
								
								if(!sweeping_out && GPIO_getIn(&hall_start)) { // motor has returned to start
									stepperStop(&stepperA);
									sweeping_out = true;
									state = RUN_CONV;
								}
								if(sweeping_out && GPIO_getIn(&hall_end)) { // motor has reached range limit
									stepperStop(&stepperA);
									sweeping_out = false;
								}
								
								// set motor direction
								GPIO_setOut(&stepperA.dir, !sweeping_out); // 0 : push away from motor, 1 : pull towards motor
								
								if(stepperIsStepping(&stepperA))
									break;
								
								stepperSetSteps(&stepperA, 100);
								stepperStart(&stepperA);
								
						case FSRC_CHICKEN:
								// stop the sweep
								stepperStop(&stepperA);
								delaymS(1000);
							break;
					}
				}break;
			
			case CHICKEN_present: {
					// wait for chicken to leave
					// check weight for egg, -> sweeping
					
					{ // weight sensor must be consistent for 1S before any state transistion
						static int FSR_previous = 12309123; // init with some bogus value
						static uint32_t startTime_mS = 0;
				
						if(FSR_previous != FSR_Check) {	// reset timer on weight change
							startTime_mS = tick;
							FSR_previous = FSR_Check;
							break;
						} else if(!beam_integrity() || startTime_mS - tick < 1000) { // wait for state to remain the same for X mS
							break;
						}
					}
					
					switch(FSR_Check){
						default:
						case FSRC_EMPTY: 
						case FSRC_EGGONLY: 
							state = SWEEPING;
						case FSRC_CHICKEN:
							break;
					}
					
				}break;
			
			case RUN_CONV: {
					// run the conveyer for like 10 seconds
					// -> idle
					PWM_on();
					delaymS(10e3);
					PWM_off();
				}break;
		}
		
		switch(FSR_Check){
			case FSRC_EMPTY:
				//Set_Color_RGB(0U,   0U,   255U);  // Yellow
				GPIO_setOut(&EB_LED, 0);
			break;
			
			case FSRC_EGGONLY:
				//Set_Color_RGB(255U, 0U, 255U);    // Green
				GPIO_setOut(&EB_LED, 1);
			break;
			
			case FSRC_CHICKEN:
				//Set_Color_RGB(0U, 255U, 255U);    // Red 
				GPIO_toggleOut(&EB_LED);			
				delaymS(100);
			break;
		}
	}
}

void SysTick_Handler(void) {
	tick++;
}

void initDCMotor() {
	// TODO
}

void initLEDDriver() {
	// TODO
}

void initStrainGuage() {
	// TODO
}

void initStepperA() {
	/*
		ms1, 			D3				gpio
		ms2,			D4				gpio
		ms3,			D5				gpio
		reset_n,	D6				gpio
		step,			D7	PA8		TIM1_CH1
		dir,			D2				gpio
		sleep_n,	D1				gpio
		enable;		D0				gpio
	*/

	stepperA.ms1.port->MODER &= ~	(0b11 << (2 * stepperA.ms1.pinN));
	stepperA.ms1.port->MODER |= 	(0b01 << (2 * stepperA.ms1.pinN));

	stepperA.ms2.port->MODER &= ~	(0b11 << (2 * stepperA.ms2.pinN));
	stepperA.ms2.port->MODER |= 	(0b01 << (2 * stepperA.ms2.pinN));

	stepperA.ms3.port->MODER &= ~	(0b11 << (2 * stepperA.ms3.pinN));
	stepperA.ms3.port->MODER |= 	(0b01 << (2 * stepperA.ms3.pinN));

	stepperA.reset_n.port->MODER &= ~	(0b11 << (2 * stepperA.reset_n.pinN));
	stepperA.reset_n.port->MODER |= 	(0b01 << (2 * stepperA.reset_n.pinN));

	stepperA.dir.port->MODER &= ~	(0b11 << (2 * stepperA.dir.pinN));
	stepperA.dir.port->MODER |= 	(0b01 << (2 * stepperA.dir.pinN));

	stepperA.sleep_n.port->MODER &= ~	(0b11 << (2 * stepperA.sleep_n.pinN));
	stepperA.sleep_n.port->MODER |= 	(0b01 << (2 * stepperA.sleep_n.pinN));

	stepperA.enable_n.port->MODER &= ~	(0b11 << (2 * stepperA.enable_n.pinN));
	stepperA.enable_n.port->MODER |= 	(0b01 << (2 * stepperA.enable_n.pinN));
	
	GPIO_setOut(&stepperA.enable_n, 1); // disable outputs
	GPIO_setOut(&stepperA.reset_n, 0); 	// enable reset
	
	{ // setup A8 as TIM1 output
			// must be a advanced timer, need the featuer to automatically step X times
		/*
		_Static_assert(stepperA.step.pinN == 8, "update timer configuration");
		_Static_assert(stepperA.step.port == GPIOA, "update timer configuration");
		_Static_assert(stepperA.driveTimer == TIM1, "update timer configuration");
		*/
		
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // pg148
		
		// alternative function 1 : TIM1_CH1
		stepperA.step.port->MODER &= ~(0b11 << (2 * stepperA.step.pinN));
		stepperA.step.port->MODER |= 0b10 << (2 * stepperA.step.pinN);
		stepperA.step.port->AFR[1] &= ~0xF;
		stepperA.step.port->AFR[1] |= 1;
		
		// timer setup
		stepperA.driveTimer->PSC = FCPU/1e6 - 1; // 1M
		stepperA.driveTimer->ARR = 2e3 - 1; // 1mS period
		stepperA.driveTimer->CR1 &= ~TIM_CR1_DIR; // up coutning
		stepperA.driveTimer->CR1 |= TIM_CR1_OPM;	// oneshot mode
		stepperA.driveTimer->CCR1 = stepperA.driveTimer->ARR / 2; // %50 duty, CH1
		stepperA.driveTimer->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
		stepperA.driveTimer->CCMR1 |= 0b110 << TIM_CCMR1_OC1M_Pos; // pwm mode 1
		stepperA.driveTimer->CCER |= TIM_CCER_CC1E; // enable CH1
		
		stepperA.driveTimer->BDTR |= TIM_BDTR_MOE; // enable main output
	}
	
	// make ready
	{
		// defaults
		GPIO_setOut(&stepperA.dir, 0);
		setSTEP(&stepperA, STEP_1);
		
		GPIO_setOut(&stepperA.sleep_n, 1); // exit sleep
		delaymS(1); // wait 1mS before step after existing sleep. hardware requirement.	
		GPIO_setOut(&stepperA.enable_n, 0); // enable outputs
		GPIO_setOut(&stepperA.reset_n, 1); 	// disable reset
	}
}

