/*
 * HMI.c
 *
 *  Created on: Oct 31, 2022
 *      Author: USER
 */
#include "HMI.h"
#include <util/delay.h>
#include "std_types.h"

#include "keypad.h"
#include "lcd.h"
#include "timer1.h"
#include "uart.h"


uint8 key=0;
uint8 counter = 0;

uint8 pass[5]={0};
uint8 confirm[5]={0};

uint8 pass_match=0;

uint8 option = 0;
uint8 check_pass[5]={0};

uint8 eeprom_match = 0;
volatile uint8 message_time=0;

uint8 pass_error = 0;



void main(void){
	/*********	Timer1_Configuration structure	*********/
	Timer1_ConfigType Timer1_Configuration = {0, 1000, PRESCALER_1024, CTC};

	/*********	UART_Configuration structure	*********/
	UART_ConfigType	  UART_Configuration = {EIGHT_DATA_BITS, DISABLED_PARITY, ONE_STOP_BIT, 9600};

	UART_init(&UART_Configuration);	//Initialize UART
	LCD_init();						//Initialize LCD

	Timer1_setCallBack(messageTimer);

	while(1){

		/****	Make PASS and CONFIRMATION PAASS for first time ****/
		while(pass_match == 0){				//Passwords matching result is set to zero initially to enter the while loop
			create_pass();
/*
 * Enter the create_pass function, if passwords are matched will exit the function and send both passwords to CONTROL_ECU
 */
			send_pass(pass);
			send_confirm(confirm);
			while(UART_recieveByte()!= CHECK){}	//Wait for CHECK instruction from CONTROL_ECU
			UART_sendByte(MC1_READY);			//Send Ready to CONTROL_ECU to receive the check result
			pass_match = UART_recieveByte();	//Receive pass_match result (if both passwords are matched or not)
			/***** If both passwords are matched, will exit the loop. If not, will stay in the loop *****/
		}


		/*****	Enter the option screen to receive + or - instruction ******/
		option_screen();
		/*****	If user chose + or - and entered the password correctly, it will exit the loop *****/


		if(eeprom_match == 0 && pass_error == 3){	//Check if the password is entered wrongly 3 times
			Timer1_init(&Timer1_Configuration);		//Initialize Timer1
			message_time = 0;						//Initialize counting time to zero
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Error");				//Display Error Message
			while(message_time < 60){				//Delay time using Timer1
			}
			Timer1_deInit();						//Deactivate Timer1
			pass_error = 0;							//Reset Password error variable
		}

		if(eeprom_match == 1 && option == '+'){
			Timer1_init(&Timer1_Configuration);		//Initialize Timer1
			message_time = 0;						//Reset counting time to zero
			LCD_clearScreen();
			Door_Unlocking();						//Display "Door Unocking"
			while(message_time < 18){				//Delay time using Timer1
			}
			message_time = 0;						//Reset counting time to zero
			LCD_clearScreen();
			Door_Locking();							//Display "Door Locking"
			while(message_time < 15){				//Delay time using Timer1
			}
			LCD_clearScreen();
			Timer1_deInit();						//Deactivate Timer1
		}
		else if(eeprom_match == 1 && option == '-'){
			pass_match = 0;			/***** Reset pass_match to go back to create_pass function *****/
			LCD_clearScreen();
		}
		eeprom_match = 0;			/***** Reset eeprom_match to go back to option screen *****/
	}
}
/***********************************  Function Definition  ***************************************/

void messageTimer(void){	//SET CALLBACK Function
	message_time++;			//Increment number of seconds every time interrupt occurs and set callback function is called
}

void create_pass(void){
	key = 0;
	counter = 0;
	LCD_moveCursor(0,0);
	LCD_displayString("Plz enter pass:");	//Display Enter Password to create new password
	LCD_moveCursor(1,0);
	while(key != '%'){						// '%' is used as the enter key
		key = KEYPAD_getPressedKey();		//Wait for a key to be pressed
		if(key != '%'){
			LCD_displayCharacter('*');		//Display '*' every time a key is pressed
			pass[counter] = key;			//Store the value in pass[] array
		}
		counter++;							//Increment counter to store the whole password
		_delay_ms(500);
	}
	key = 0;
	counter = 0;
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("Plz re-enter the");	//Display re enter password to take confirmation password
	LCD_moveCursor(1,0);
	LCD_displayString("same pass:");
	LCD_moveCursor(1,11);
	while(key != '%'){
		key = KEYPAD_getPressedKey();
		if(key != '%'){
			LCD_displayCharacter('*');
			confirm[counter] = key;			//Store the confirmation password in confirm[] array
		}
		counter++;
		_delay_ms(500);
	}
}

void option_screen(void){	/*** Allow user to choose operation and asks him to enter the password then it will check on it ***/
	pass_error = 0;			//Initialize pass_error to zero
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("+:Open the Door");
	LCD_moveCursor(1,0);
	LCD_displayString("-:Change Pass");
	key = KEYPAD_getPressedKey();				//Wait for key to be pressed
	option = key;								//Store the value of key in option variable to be sent later to CONTROL_ECU
	_delay_ms(500);
	UART_sendByte(OPTION);						// Prepare CONTROL_ECU for option instruction
	while(UART_recieveByte() != MC2_READY){}	// Wait for CONTROL_ECU to be ready
	UART_sendByte(option);						// Send the option

	if(key == '+' || key == '-'){
		while(eeprom_match == 0 && pass_error < 3 ){//Stay in the while loop if the password is entered wrongly 3 times
			key = 0;								//Initialize key to zero
			counter = 0;							//Initialize counter to zero
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Plz enter pass:");	//Asks user to enter password to compare it with the one stored in EEPROM
			LCD_moveCursor(1,0);
			while(key != '%'){
				key = KEYPAD_getPressedKey();		//Wait for key to be pressed
				if(key != '%'){
					LCD_displayCharacter('*');		//Display '*' instead of the key pressed

				//Store the key pressed in the check_pass[] array to be sent to CONTROL_ECU to compare it with Pass in EEPROM
					check_pass[counter] = key;
				}
				counter++;
				_delay_ms(500);
			}

			while(UART_recieveByte() != MC2_READY){}	// Wait for the CONTROL_ECU to be ready
			UART_sendByte(PASS_READY);					// Prepare CONTROL_ECU for the check_pass
			while(UART_recieveByte() != MC2_READY){}	// Wait for the CONTROL_ECU to be ready

			for(counter = 0; counter<5; counter++){		// Send the check_pass
				UART_sendByte(check_pass[counter]);
			}
			while(UART_recieveByte()!= CHECK){}			// Wait for the CHECK instruction
			UART_sendByte(MC1_READY);					// Sending ready to receive the match result from CONTROL_ECU
			eeprom_match = UART_recieveByte();
			/*
			 * Receive the result of password matching, if the password matches the PASS in EEPROM the operation will be carried
			 * if not, it will stay in the loop and asks the user to enter the password again for 3 times
			 */
			pass_error++;	//Increment the pass_error if the password is entered wrongly
		}
	}
}
void Door_Locking(void){
	LCD_moveCursor(0,0);
	LCD_displayString("Door Locking");	/***** Display "Door Locking" *****/
}
void Door_Unlocking(void){
	LCD_moveCursor(0,0);
	LCD_displayString("Door Unlocking");/***** Display "Door Unlocking" *****/
}
void send_pass(uint8 arr[]){
	UART_sendByte(PASS_READY);		/*
	 	 	 	 	 	 	 	 	 *Send PASS_READY instruction to CONTROL_ECU to prepare it to receive the PASS
	 	 	 	 	 	 	 	 	 * to be stored later in EEPROM */
	while(UART_recieveByte() != MC2_READY){}	//Wait for CONTROL_ECU to be ready
	for(counter = 0; counter<5; counter++){		//Send the Password
		UART_sendByte(pass[counter]);
	}
}
void send_confirm(uint8 arr[]){
	UART_sendByte(CONFIRM_READY);	/*
	 	 	 	 	 	 	 	 	 *Send PASS_READY instruction to CONTROL_ECU to prepare it to receive the CONFIRMATION PASS
	 	 	 	 	 	 	 	 	 * to be stored later in EEPROM */
	while(UART_recieveByte() != MC2_READY){}	//Wait for CONTROL_ECU to be ready
	for(counter = 0; counter<5; counter++){		//Send the Confirmation Password
		UART_sendByte(confirm[counter]);
	}
}
