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
			.pinN = 14,
		},
	hall_start = {
			.port = GPIOA,
			.pinN = 13,
		};
		
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
		
/** itterate state machine (version A) */
void itt_stateA();
		
/** itterate state machine (version B) */
void itt_stateB();

bool getHallStart() {
	return GPIO_getIn(&hall_start);
}
bool getHallEnd() {
	return GPIO_getIn(&hall_end);
}	
		
		
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
	
	initStepperA();
	setSTEP(&stepperA, STEP_1);
	initPWM(); // conveyor belt 
	//initSPI1(); // beam sensor
	//initMCP(); // beam sensor
	initFsrHW();
	initLED_Driver();
	
	{ // init hall gpio sensor, DI
		hall_end.port->MODER &= ~	(0b11 << (2 * hall_end.pinN));
		hall_end.port->MODER |= 	(0b00 << (2 * hall_end.pinN));
		
		hall_start.port->MODER &= ~	(0b11 << (2 * hall_start.pinN));
		hall_start.port->MODER |= 	(0b00 << (2 * hall_start.pinN));
	
		GPIO_setBiasPD(&hall_end);
		GPIO_setBiasPD(&hall_end);
	}
	
	GPIO_setOut(&EB_LED, 0);
	delaymS(250);
	
//	while(1)
//				{
//					GPIO_setOut(&stepperA.dir, 0);
//					if(!stepperIsStepping(&stepperA)) 
//					{
//						stepperSetSteps(&stepperA, 100);
//						stepperStart(&stepperA);
//					}
//				} 
	
	/*** main loop ******************************************/
	
	Set_Color_RGB(0U,   255U, 0U);    // Purple
	delaymS(1000);
	Set_Color_RGB(255U, 255U, 255U);  //Off
	delaymS(1000);
	Set_Color_RGB(0U,   255U, 0U);    // Purple
	delaymS(1000);
	Set_Color_RGB(255U, 255U, 255U);  //Off
	delaymS(1000);
	Set_Color_RGB(0U,   255U, 0U);    // Purple
	delaymS(1000);
	Set_Color_RGB(255U, 255U, 255U);  //Off
	delaymS(1000);
	
	// full range sweep
	while (0)
	{
		if (getHallStart()) {
			Set_Color_RGB(0U, 255U, 255U);
			GPIO_setOut(&stepperA.dir, 0);
		} else if (getHallEnd()) {
			Set_Color_RGB(255, 0U, 255U);
			GPIO_setOut(&stepperA.dir, 1);
		} else
			Set_Color_RGB(255, 255, 255U);
		
		if(!stepperIsStepping(&stepperA)) 
		{
			stepperSetSteps(&stepperA, 100);
			stepperStart(&stepperA);
		}
	}
	
	// test beam
	while(0) {
		if (beam_check()) {
				Set_Color_RGB(0U, 255U, 255U);
		} else 
			Set_Color_RGB(255, 255, 255U);
	}
	
	while(0) {
		PWM_on();
		delaymS(5e3);
		PWM_off();
		delaymS(5e3);
	}
	
	// home swepper
	GPIO_setOut(&stepperA.dir, 1);
	while (!getHallStart())
	{
		if(!stepperIsStepping(&stepperA)) 
		{
			stepperSetSteps(&stepperA, 200);
			stepperStart(&stepperA);
		}
	}
	
	while(1) {
			//itt_stateA();
			itt_stateB();
	}
				Set_Color_RGB(0U,   180U, 255U);  // Orange  
}

void itt_stateA() {
	
	
	typedef enum {
		IDLE,
		BEAM,
		SWEEPING,
		RUN_CONV,
		RETURNING,
	} State_e;
		
	static State_e state = IDLE;
	static int FSR_Check = 0;
	
	FSR_Check = FsrWeightCheck();
		
		if (FSR_Check == FSRC_CHICKEN)
		{
			state = IDLE;
		}
		
		else if (state == IDLE && FSR_Check == FSRC_EGGONLY)
		{
			state = BEAM;
			delaymS(1000);
		}
		
		switch (state)
		{
			case IDLE:
			{
				// Return to start position if not there already
				Set_Color_RGB(0U, 255U, 255U); // RED for IDLE
				while(GPIO_getIn(&hall_start))
				{
					GPIO_setOut(&stepperA.dir, 1);
					if(!stepperIsStepping(&stepperA)) 
					{
						stepperSetSteps(&stepperA, 100);
						stepperStart(&stepperA);
					}
				}
				
				//stepperStop(&stepperA);
				
				break;
			}
			
			case BEAM:
			{
				Set_Color_RGB(0U,   0U,   255U); // Yellow for BEAM
				if (beam_integrity())
					state = SWEEPING;
				else
					state = IDLE;
				break;
			}
			
			case SWEEPING:
			{
				Set_Color_RGB(255U, 0U, 255U);  // Green for SWEEPING
				while (GPIO_getIn(&hall_end))
				{
					GPIO_setOut(&stepperA.dir, 0);
					if(!stepperIsStepping(&stepperA)) 
					{
						stepperSetSteps(&stepperA, 100);
						stepperStart(&stepperA);
					}
				}
				stepperStop(&stepperA);
				state = RUN_CONV;
				
				break;
			}
			
			case RUN_CONV:
			{
				Set_Color_RGB(255U, 255U, 0U); // Blue for RUN_CONV
				PWM_on();
				delaymS(10000);
				PWM_off();
				state = RETURNING;
			}
			
			case RETURNING:
			{
				Set_Color_RGB(0U,   180U, 255U);  // Orange    
				// Return to start position if not there already
				while (GPIO_getIn(&hall_start))
				{
					GPIO_setOut(&stepperA.dir, 1);
					if(!stepperIsStepping(&stepperA)) 
					{
						stepperSetSteps(&stepperA, 100);
						stepperStart(&stepperA);
					}
				}
				
				stepperStop(&stepperA);
				
				// Stay here until chicken returns
				if (FSR_Check == FSRC_CHICKEN)
					state = IDLE;
				
				break;
			}
		}
}

void itt_stateB() {
	typedef enum {
		IDLE,
		CHICKEN_PRES,
		SWEEPING,
		RUN_CONV,
	} State_e;
		
	static State_e state = IDLE;
	
	int volatile fsr_raw = getFSRRaw();
	static volatile int fsr_zero = -1;
	if(fsr_zero < 0)
		fsr_zero = getFSRRaw();
	
	static bool init_state = true;
	
	switch(state) {
		case IDLE: {
			/* wait for chicken to appear
					transistion to CHICKEN_PRES when detected 
			*/
			Set_Color_RGB(0,255,255);
			
			bool chick_pres = false;
			static bool chick_pres_former = false;
			
			if(fsr_raw > fsr_zero + 300) {
				chick_pres = true;
			}
			
			if(init_state) {
				init_state = false;
				chick_pres = false;
				chick_pres_former = !chick_pres;
			}
			
			// value remain same for X time
			{
				static uint32_t time = 0;
				if(chick_pres_former != chick_pres) {
					time = tick;
					chick_pres_former = chick_pres;
					break;
				}
				if(tick - time < 1000)
					break;
			}
			
			if(chick_pres) {
				state = CHICKEN_PRES;
				init_state = true;
			}
			
		} break;
		
		case CHICKEN_PRES: {
			/* wait for chicken to dissipear
				if totally empty, 
			*/
			Set_Color_RGB(255,0,255);
			
			static bool chick_pres_former = false;
			bool chick_pres = chick_pres_former;
			
			if(fsr_raw < fsr_zero - 150) {
				if(beam_integrity())
					chick_pres = false;
			}
			
			if(init_state) {
				init_state = false;
				chick_pres = false;
				chick_pres_former = !chick_pres;
			}
			
			// value remain same for X time
			{
				static uint32_t time = 0;
				if(chick_pres_former != chick_pres) {
					time = tick;
					chick_pres_former = chick_pres;
					break;
				}
				if(tick - time < 1000)
					break;
			}
			
			if(!chick_pres) {
				if(fsr_raw < fsr_zero + 50)
					state = IDLE;
				else 
					state = SWEEPING;
				init_state = true;
			}
			
		} break;
		
		case SWEEPING: {
			Set_Color_RGB(255,255,0);
			
			static bool sweepOut;
			if(init_state) {
				init_state = false;
				sweepOut = true;
			}
			
			if(sweepOut)
				GPIO_setOut(&stepperA.dir, 0);
			else 
				GPIO_setOut(&stepperA.dir, 1);
			
			if(!stepperIsStepping(&stepperA) && beam_check()) 
			{
				stepperSetSteps(&stepperA, 100);
				stepperStart(&stepperA);
			}
			
			if(sweepOut) {
				if(getHallEnd())
					sweepOut = false;
			} else {
				if(getHallStart()) {
					state = RUN_CONV;
					init_state = true;
				}
			}
			
		} break;
		
		case RUN_CONV: {
			Set_Color_RGB(0,0,0);
			
			static uint32_t time;
			
			if(init_state) {
				init_state = false;
				time = tick;
			}
			
			if(tick - time < 10e3) {
				PWM_on();
				break;
			}
			
			PWM_off();
			
			init_state = true;
			state = IDLE;
			
		} break;
	}
}


void SysTick_Handler(void) {
	tick++;
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

