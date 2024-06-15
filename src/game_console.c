/*************************************************************************
Title   : sketch game_console
Author  : Sangchul Lee
Software: AVR-GCC 
Hardware: ATMEGA16 @ 8Mhz 

DESCRIPTION:
	Main and functions for Game Console basic sample code

*************************************************************************/

#include "game_console.h" 
#include <util/delay.h>

// Variables to be used in the interrupt
volatile char up_flag, down_flag, left_flag, right_flag, action1_flag, action2_flag;

// Variables for drawing configuration 
unsigned char width_factor = 0;
unsigned char invert_increment = 0;

// Enable basic snake game
unsigned char game = 0;

// PWM duty cycle
uint8_t duty_cycle = 0;

// Variables related to LCD  
byte row, page, column, pixel;
	
int main(void)
{	
	initialize_IOs();		// Set I/Os
	enable_SPI();			// Set up serial pheripheral interface
	enable_PWM();
	sei();					// Enable global interrupt (required for all interrupts)
	enable_interrupt();		// Enable interrupt for INT1
	enable_ADC();			// Enable ADC
	clear_frame_buffer();	// Initialize frame buffer
	clear_flags();			// Clear flags used in interrupt
	initialise_LCD();		// Initialize LCD
	center_cursor();		// Places cursor at the center of the screen

	// Etch A Sketch
	while (game)			// Master loop always true so always loop
	{
		unsigned char i,k;
		if (up_flag == 1)
		{
			for(k = 0; k <= STEP; k++)
			{
				if (row == 0)
					row = MAX_ROW - 1;
				else
					row--;
				for(i = 0; i <= width_factor; i++)
				{
					draw_pixel(row,column);
					draw_pixel(row,column + i);
					draw_pixel(row,column - i);
				}
			}
			up_flag = 0;
		}
		if (down_flag == 1)
		{
			for(k = 0; k <= STEP; k++)
			{
				if (row == MAX_ROW)
					row = 0;
				else
					row++;
				for(i = 0; i <= width_factor; i++)
				{	
					draw_pixel(row,column);
					draw_pixel(row,column + i);
					draw_pixel(row,column - i);
				}
			}
			down_flag = 0;
		}
		if (left_flag == 1)
		{
			for(k = 0; k <= STEP; k++)
			{
				if (column == 0)
					column = MAX_COLUMN;
				else
					column--;
				for(i = 0; i <= width_factor; i++)
				{
					draw_pixel(row,column);
					draw_pixel(row + i,column);
					draw_pixel(row - i,column);
				}
			}
			left_flag = 0;
		}
		if (right_flag == 1)
		{
			for(k = 0; k <= STEP; k++)
			{
				if (column == MAX_COLUMN)
					column = 0;
				else
					column++;
				for(i = 0; i <= width_factor; i++)
				{
					draw_pixel(row,column);
					draw_pixel(row + i,column);
					draw_pixel(row - i,column);
				}
			}
			right_flag = 0;
		}
		if (action1_flag == 1)
		{
			if(width_factor == MAX_WIDTH + 1)
			{
				invert_increment = 1;
				width_factor--;
			}
			else if((width_factor > 1) && (width_factor < MAX_WIDTH + 1) && (invert_increment == 1))
				width_factor--;
			else if (width_factor == 1 && invert_increment == 1)
			{
				invert_increment = 0;
				width_factor++;
			}
			else
				width_factor++;
			action1_flag = 0;
		}
		if (action2_flag == 1)
		{
			clear_LCD();
			action2_flag = 0;
		}
		check_bat_lvl();
	}

	// Snake
	while(!(game))
	{
		if (up_flag == 1)
		{
			if (row == 0)
				row = MAX_ROW - 1;
			else
				row--;
			draw_pixel(row,column);
			_delay_ms(50);
		}
		if (down_flag == 1)
		{
			if (row == MAX_ROW)
				row = 0;
			else
				row++;
			draw_pixel(row,column);
			_delay_ms(50);
		}
		if (left_flag == 1)
		{
			if (column == 0)
				column = MAX_COLUMN;
			else
				column--;
			draw_pixel(row,column);
			_delay_ms(50);
		}
		if (right_flag == 1)
		{
			if (column == MAX_COLUMN)
				column = 0;
			else
				column++;
			draw_pixel(row,column);
			_delay_ms(50);
		}
		if (action2_flag == 1)
		{
			clear_LCD();
		}
		if (action1_flag == 1)
		{
			OCR0 = duty_cycle;
			// Halts the snake
		}
		check_bat_lvl();
	}
	return 0;
}

// Important!: It may be neccessary to implement nested interrupts to have more interrupts working
ISR(TIMER2_COMP_vect)
{
	// OCR0 = duty_cycle; Coudnt get this to work. The system 
	// Timer interrupt occurs when output compare match flag is set (OCF0)
	// Note: Enable OCIE0.
}

ISR(TIMER2_OVF_vect)
{
	// Implement overflow interrupt
	// Must enable TIMER interrupt (TOIE0 bit in TIMSK - initiated by ovf flag TOV0)
	// Note: Interrupt occurs only when TOV0 flag is set due to overflow.
	// Note2: Interrupt can also be initiated by compare match (use OCIE0 instead of TOIE0)
}

ISR(SPI_STC_vect)
{
	// Implement SPI interrupt
	// Must enable SPI interrupt first (SPIE bit in SPCR)
	// Note: Interrupt occurs only when SPIF flag is set due to serial transfer completion.
}

ISR(ADC_vect)
{
	// Implement ADC interrupt - executes after conversion followed by ADIF bit
	// Must enable ADC interrupt first (ADIE bit in ADCSRA)
	// Note: Interrupt occurs only when ADCF flag is set due to conversion completion
}


ISR(INT1_vect)
{
	_delay_ms(50);		// Software debouncing
	if(!(UP_BUTTON))
	{
		clear_flags();
		up_flag = 1;
	}
		else if(!(DOWN_BUTTON))
		{
		clear_flags();
		down_flag = 1;
		}
		else if(!(RIGHT_BUTTON))
		{
		clear_flags();
		right_flag = 1;
		}
		else if(!(LEFT_BUTTON))
		{
		clear_flags();
		left_flag = 1;
		}
		else if(!(ACTION1_BUTTON))
		{
			clear_flags();
			action1_flag = 1;
			if (duty_cycle == 200)
				duty_cycle = 0;
			else
				duty_cycle = duty_cycle + 50;
		}
		else if(!(ACTION2_BUTTON))
		{
			clear_flags();
			action2_flag = 1;
		}
}

void initialize_IOs()
{
	UP_BUTTON_DIR(IN);		// Set UP_BUTTON I/Os as input.
	DOWN_BUTTON_DIR(IN);	// Set DOWN_BUTTON I/Os as input.
	LEFT_BUTTON_DIR(IN);	// Set LEFT_BUTTON I/Os as input.
	RIGHT_BUTTON_DIR(IN);	// Set RIGHT_BUTTON I/Os as input.
	ACTION1_BUTTON_DIR(IN);	// Set ACTION1_BUTTON I/Os as input.
	ACTION2_BUTTON_DIR(IN);	// Set ACTION2_BUTTON I/Os as input.
	MISO_DIR(IN);
	BAT_LOW_LED_DIR(OUT);	// Set BATTERY LED I/Os as outputs.	
	MOSI_DIR(OUT);
	SLAVE_SELECT_DIR(OUT);		
	SCK_DIR(OUT);
	LCD_CS0_DIR(OUT);
	LCD_PWM_DIR(OUT);
	LCD_RST_DIR(OUT);
	LCD_CD_DIR(OUT);
	RAM_CS_DIR(OUT);
	RAM_WP_DIR(OUT);
	PULL_UP_UP_BUTTON(HIGH);
	PULL_UP_DOWN_BUTTON(HIGH);
	PULL_UP_LEFT_BUTTON(HIGH);
	PULL_UP_RIGHT_BUTTON(HIGH);
	PULL_UP_ACTION1_BUTTON(HIGH);
	PULL_UP_ACTION2_BUTTON(HIGH);
	PULL_UP_LCD_PWM(HIGH);
	SLAVE_SELECT(HIGH);
	LCD_RST(HIGH);			// Disable reset
	//LCD_CS0(LOW);
	LCD_PWM(ON);			// The PWM will turn it on with output compare
	BAT_LOW_LED(OFF);		// Turn off low battery LED
}

void enable_SPI()
{
	SET(SPCR,(_BV(SPE)|_BV(MSTR)),HIGH); 	// SPI control register
	//SET(SPCR,(_BV(CPHA)|_BV(CPOL)),HIGH);	// Do I need them? No!
	//SET(SPSR,_BV(SPI2X),HIGH); 			// SPI speed x 2. Dont need it either.
}

void enable_interrupt()
{
	SET(GICR,_BV(INT1),HIGH);					// Enable INT1
	SET(MCUCR,(_BV(ISC11)|_BV(ISC10)),HIGH);	// Rising edge trigger
}

void enable_ADC()
{
	SET(ADCSRA,_BV(ADEN),HIGH);					// Enable ADC
	//SET(ADCSRA,(_BV(ADEN)|_BV(ADIF)),HIGH);	// Enable ADC and the interrupt
	SET(ADMUX,(_BV(REFS1)|_BV(REFS0)),HIGH);	// Internal 2.56V reference 
}

void clear_frame_buffer()
{
	unsigned char i,j;
	for(i = 0;i < MAX_PAGE; i++)
	{
		for(j = 0;j < MAX_COLUMN; j++)
		{
			frame_buffer[i][j]=0;
		}
	}
}

void clear_flags()
{
	up_flag = 0;
	down_flag = 0;
	left_flag = 0;
	right_flag = 0;
	action1_flag = 0;
	action2_flag = 0;
}

byte initialise_LCD(void)
{	
	LCD_command_tx(0x40);	//Display start line 0
	LCD_command_tx(0xA1);	//SEG reverse
	LCD_command_tx(0xC0);	//Normal COM0~COM63
	LCD_command_tx(0xA4);	//Disable -> Set All Pixel to ON
	LCD_command_tx(0xA6);	//Display inverse off
	_delay_ms(120);
	LCD_command_tx(0xA2);	//Set LCD Bias Ratio A2/A3
	LCD_command_tx(0x2F);	//Set Power Control 28...2F
	LCD_command_tx(0x27);	//Set VLCD Resistor Ratio 20...27
	LCD_command_tx(0x81);	//Set Electronic Volume
	LCD_command_tx(0x10);	//Set Electronic Volume 00...3F
	LCD_command_tx(0xFA);	//Set Adv. Program Control
	LCD_command_tx(0x90);	//Set Adv. Program Control x00100yz yz column wrap x Temp Comp
	LCD_command_tx(0xAF);	//Display on
	return(TRUE);
}

void center_cursor()
{
	row = MAX_ROW/2;
	page = row/MAX_PAGE;
	column = MAX_COLUMN/2;
	pixel = row%MAX_PAGE;
	draw_pixel(row,column);	// Draw reference pixel
}

void draw_pixel(byte row,byte column)
{
	page = ((row/MAX_PAGE))%MAX_PAGE;
    pixel = row%MAX_PAGE;
    select_column(column);
    select_page(page);
	frame_buffer[page][column] = (1<<pixel)|frame_buffer[page][column];
	LCD_data_tx(_BV(pixel)|frame_buffer[page][column]);
	//_delay_ms(10);
}

void clear_LCD()
{
	byte i,j;
	for(i = 0;i < MAX_PAGE; i++)
	{
		select_page(i);
		for(j = 0; j < MAX_COLUMN; j++)
		{
			select_column(j);
			LCD_data_tx(0x00);
		}
	}
	clear_frame_buffer();
	center_cursor();
	width_factor = 0;			// Default line width
	invert_increment = 0;
}

void LCD_command_tx(byte cData)
{
	LCD_CS0(LOW);
	LCD_CD(COMMAND);
	SPDR = cData;	
	while(!(SPSR & (1<<SPIF)));	// Wait for transmission complete
	LCD_CS0(HIGH);
	return;
}

void LCD_data_tx(byte cData)
{
	LCD_CS0(LOW);
	LCD_CD(DATA);
	RAM_CS(LOW);
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));	// Wait for transmission complete
	LCD_CS0(HIGH);
	return;
}

int select_page (byte page)
{
	byte page_cmd_address;
	page_cmd_address =(CMD_PAGE | page);
	LCD_command_tx(page_cmd_address );
	return(TRUE);
}

int select_column (byte column)
{
	byte page_cmd_address_MSB;
	byte page_cmd_address_LSB;
	page_cmd_address_LSB =(CMD_COL_LSB | (column & 0x0F));
	page_cmd_address_MSB =(CMD_COL_MSB | (column >> 4));
	LCD_command_tx(page_cmd_address_LSB);
	LCD_command_tx(page_cmd_address_MSB);
	return(TRUE);
}

void check_bat_lvl()
{
	short batt_threshold;
	short adc_reading;

	SET(ADMUX,(_BV(MUX2)|_BV(MUX1)),HIGH);	// Set channel ADC6 (PA6) as input 
	SET(ADCSRA,_BV(ADSC),HIGH);				// Start conversion

	while(ADCSRA & (1<<ADSC));				// Wait for conversion to finish
	
	adc_reading = ADC;						// Store analog conversion result

	batt_threshold = (VLOW/VREF)*1024;		// Calculate the low battery level
			
	//Compare voltage
	if (adc_reading < batt_threshold)	
		BAT_LOW_LED(ON);
	else
		BAT_LOW_LED(OFF);
}

void enable_PWM()
{
	TCCR0 = 1<<COM01 | 1<<WGM01 | 1<<WGM00; // Fast PWM 
	TCCR0 |= 1<<CS00;						// No prescaler
	//SET(TIMSK,_BV(OCIE0),HIGH);			// Enable interrupt for output compare
}
