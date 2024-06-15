/*************************************************************************
Title   : sketch game_console Header File
Author  : Sangchul Lee
Software: AVR-GCC 
Hardware: ATMEGA16 @ 8Mhz 

DESCRIPTION:
	Macros for Game Console

*************************************************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define byte unsigned char
#define CMD_PAGE 0xB0
#define CMD_COL_LSB 0x00
#define CMD_COL_MSB 0x10
#define MAX_PAGE 8
#define MAX_COLUMN 102
#define MAX_ROW 64
#define VLOW 1.2
#define VREF 2.56
#define MAX_WIDTH 5
#define STEP 4

byte frame_buffer[MAX_PAGE][MAX_COLUMN];

/*ON OFF*/
#define ON 0xFF
#define OFF 0x00
#define HIGH 0xFF
#define LOW 0x00
#define IN 0x00
#define OUT 0xFF
#define ALL 0xFF
#define TRUE 1
#define FALSE 0
#define FORWARD 0x00
#define BACK 0xFF 
#define COMMAND 0x00
#define DATA 0xFF
#define TRUE 1

/*SET and GET MACRO*/
#define SET(PORT,MASK,VALUE) PORT = ((MASK & VALUE) | (PORT & ~MASK))
#define GET(PORT,MASK) PORT & MASK

//Devices Direction MACROs
//#define MULTI_LINES_DIR(DIR) SET(DDR?,(_BV(P?#)|_BV(P?#)|_BV(P?#)),DIR)
#define BAT_LOW_LED_DIR(DIR) 	SET(DDRC,_BV(PC0),DIR)
#define UP_BUTTON_DIR(DIR) 		SET(DDRA,_BV(PA0),DIR)
#define DOWN_BUTTON_DIR(DIR) 	SET(DDRA,_BV(PA1),DIR)
#define LEFT_BUTTON_DIR(DIR) 	SET(DDRA,_BV(PA2),DIR)
#define RIGHT_BUTTON_DIR(DIR) 	SET(DDRA,_BV(PA5),DIR)
#define ACTION1_BUTTON_DIR(DIR) SET(DDRA,_BV(PA4),DIR)
#define ACTION2_BUTTON_DIR(DIR) SET(DDRA,_BV(PA3),DIR)

// SPI_DIR
#define MISO_DIR(DIR) 		  SET(DDRB,_BV(PB6),DIR)
#define MOSI_DIR(DIR) 		  SET(DDRB,_BV(PB5),DIR)
#define SCK_DIR(DIR) 		  SET(DDRB,_BV(PB7),DIR)
#define SLAVE_SELECT_DIR(DIR) SET(DDRB,_BV(PB4),DIR)

// LCD_DIR
#define LCD_CS0_DIR(DIR) SET(DDRB,_BV(PB2),DIR)
#define LCD_CD_DIR(DIR)  SET(DDRB,_BV(PB1),DIR)
#define LCD_PWM_DIR(DIR) SET(DDRB,_BV(PB3),DIR)
#define LCD_RST_DIR(DIR) SET(DDRB,_BV(PB0),DIR)

// RAM_DIR
#define RAM_CS_DIR(DIR)	  SET(DDRD,_BV(PD0),DIR)
#define RAM_WP_DIR(DIR)   SET(DDRD,_BV(PD1),DIR)
#define RAM_HOLD_DIR(DIR) SET(DDRD,_BV(PD2),DIR)

// Pull-up resistors 
#define PULL_UP_UP_BUTTON(STATE) SET(PORTA,_BV(PA0),STATE)
#define PULL_UP_DOWN_BUTTON(STATE) SET(PORTA,_BV(PA1),STATE)
#define PULL_UP_LEFT_BUTTON(STATE) SET(PORTA,_BV(PA2),STATE)
#define PULL_UP_RIGHT_BUTTON(STATE) SET(PORTA,_BV(PA5),STATE)
#define PULL_UP_ACTION1_BUTTON(STATE) SET(PORTA,_BV(PA4),STATE)
#define PULL_UP_ACTION2_BUTTON(STATE) SET(PORTA,_BV(PA3),STATE)
#define PULL_UP_LCD_PWM(STATE) SET(PORTB,_BV(PB3),STATE)
#define SLAVE_SELECT(STATE) SET(PORTB,_BV(PB4),STATE)
#define RAM_CS(STATE) SET(DDRD,_BV(PD0),STATE);

//Devices Outputs
//#define MULTI_LINES(STATE) SET(DDR?,(_BV(P?#)|_BV(P?#)|_BV(P?#)),DIR)
#define BAT_LOW_LED(STATE) SET(PORTC,_BV(PC0),~STATE)

// Button inputs
#define UP_BUTTON GET(PINA,_BV(PA0))
#define DOWN_BUTTON GET(PINA,_BV(PA1))
#define LEFT_BUTTON GET(PINA,_BV(PA2))
#define RIGHT_BUTTON GET(PINA,_BV(PA5))
#define ACTION1_BUTTON GET(PINA,_BV(PA4))
#define ACTION2_BUTTON GET(PINA,_BV(PA3))

// LCD_STATES
#define LCD_CS0(STATE) SET(PORTB,_BV(PB2),STATE)
#define LCD_CD(STATE) SET(PORTB,_BV(PB1),STATE)
#define LCD_PWM(STATE) SET(PORTB,_BV(PB3),STATE)
#define LCD_RST(STATE) SET(PORTB,_BV(PB0),STATE)

//#define PIXEL_ON (frame_buffer[page][column])&(_BV(row%MAX_PAGE))

void initialize_IOs();

void enable_SPI();

void enable_interrupt();

void enable_ADC();

void clear_frame_buffer();

void clear_flags();

byte initialise_LCD();

void center_cursor();

void draw_pixel(byte row,byte column);

void clear_LCD();

void LCD_command_tx(byte cData);

void LCD_data_tx(byte cData);

int select_page (byte page);

int select_column (byte column);

void check_bat_lvl();

void enable_PWM();

