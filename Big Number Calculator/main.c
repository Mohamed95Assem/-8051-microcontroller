/*
 * final
 *
 * Created: 6/10/2021 
 * Author : M.Assem
 */ 

#include <reg51.h>																												// Include x51 header file

unsigned char value_to_compare  , counter1 = 0 , counter2 = 0 , operation , result = 0 , carry = 0 , remainder ;
signed char general_counter = 0 , counter_first = 0 , counter_second = 0 , counter_result = 0 , counter_add = 0 , counter_send = 0 , length_result = 0 ;


unsigned char first_arr [17] ;
unsigned char second_arr [17] ;
unsigned char result_arr [32] ;

unsigned char welcom_arr[15] = "Mohamed Assem" ;																//Welcome message


									/* UART initialization */
void UART_Init()																													//uart initialization
{
	TMOD = 0x20;																														//Timer 1, 8-bit auto reload mode 
	TH1 = 0xFD;																															// Load value for 9600 baud rate 
	SCON = 0x50;																														// Mode 1, reception enable 
	TR1 = 1;																																// Start timer 1 
}
								/*Send data(ascii) to UART*/
void Transmit_data(unsigned char tx_data)
{
	SBUF = tx_data;																													// Load unsigned char in SBUF register
	while (TI==0);																													// Wait until stop bit transmit
	TI = 0;																																	// Clear TI flag 
}

				/*Read first Numbers from UART and save it in array*/
void Read_First_Number(){
	while(counter1 < 17){
		while(RI == 0);																												//Wait until stop bit REVIEVE
		value_to_compare = SBUF ;																							//add value in array
		RI = 0 ;
		if(counter1 == 16){
			while(1){
				while(RI == 0);
				operation = SBUF ;
				RI = 0 ;
				if(operation == '+' || operation == '*'){
					break;
				}
			}
			Transmit_data(operation);
			break ;
		}
		if(value_to_compare == '0' && counter1 > 0)
		{
			first_arr[counter1] = (value_to_compare - 48) ;											//convert form hexadecimal to decimal
			Transmit_data(value_to_compare);
			counter1 ++ ;
		}
		else if(value_to_compare >= '1' && value_to_compare <= '9')						//true --> if the char between 0 to 9
		{
			first_arr[counter1] = (value_to_compare - 48) ;											//convert form hexadecimal to decimal
			Transmit_data(value_to_compare);
			counter1 ++ ;
		}
		else if(value_to_compare >= 'A' && value_to_compare <= 'F')						//true --> if the char between A to F
		{
			first_arr[counter1] = (value_to_compare - 55) ;											//convert form hexadecimal to decimal
			Transmit_data(value_to_compare);
			counter1 ++ ;
		}	
		else if(value_to_compare >= 'a' && value_to_compare <= 'f')						//true --> if the char between a to f
		{
			first_arr[counter1] = (value_to_compare - 87) ;											//convert form hexadecimal to decimal
			Transmit_data(value_to_compare);
			counter1 ++ ;
		}
		else if((value_to_compare == '*' || value_to_compare == '+') && counter1 > 0 )
		{
			operation = value_to_compare ;
			Transmit_data(value_to_compare);
			break ;
		}
	}
}

					/*Read second Numbers from UART and save it in array*/
void Read_second_Number(){
	while(counter2 < 17){
		while(RI == 0);																												//Wait until stop bit REVIEVE
		value_to_compare = SBUF ;																							//add value in array
		RI = 0 ;
		if(counter2 == 16){
			Transmit_data('=');
			break ;
		}
		if(value_to_compare >= '0' && value_to_compare <= '9')							//true --> if the char between 0 to 9
		{
			second_arr[counter2] = (value_to_compare - 48) ;											//convert form hexadecimal to decimal
			Transmit_data(value_to_compare);
			counter2 ++ ;
		}
		else if(value_to_compare >= 'A' && value_to_compare <= 'F')							//true --> if the char between A to F
		{
			second_arr[counter2] = (value_to_compare - 55) ;											//convert form hexadecimal to decimal
			Transmit_data(value_to_compare);
			counter2 ++ ;
		}	
		else if(value_to_compare >= 'a' && value_to_compare <= 'f')							//true --> if the char between a to f
		{
			second_arr[counter2] = (value_to_compare - 87) ;											//convert form hexadecimal to decimal
			Transmit_data(value_to_compare);
			counter2 ++ ;
		}
		else if(value_to_compare == '=' && counter2 > 0 )
		{
			Transmit_data(value_to_compare);
			break ;
		}
	}
}

										/*	MUL FUNCTION		*/
void Mul(){
	for( counter_second = (counter2 - 1 ) ; counter_second >= 0 ; counter_second-- ){
		counter_result = 0 ;
		for( counter_first = (counter1 - 1) ; counter_first >= 0 ; counter_first-- ){
			result = (second_arr[counter_second] * first_arr[counter_first]) + carry ;
			carry = (result_arr[counter_result + counter_add] + result) / 16 ;
			result_arr[counter_result + counter_add] = ((result_arr[counter_result + counter_add] + result) % 16 ) ;
			counter_result ++ ;
		}
		if(carry > 0 ){
			result_arr[counter_result + counter_add] = carry ;
			carry = 0 ;
			if(counter_second == 0){
				counter_send ++ ;
			}
		}
		counter_add++ ;
	}
}

										/*	ADD FUNCTION		*/
void Add(){
	if(counter1 > counter2){
		general_counter = counter1 ;
	}
	else{
		general_counter = counter2 ;
	}
	for( general_counter = (general_counter - 1) ; general_counter >= 0 ; general_counter-- ){
		result = second_arr[counter2 - 1] + first_arr[counter1 - 1] + carry ;
		carry = result / 16 ;
		result_arr[counter_result] = result % 16  ;
		counter_result ++ ;
		counter1-- ;
		counter2-- ;
	}
	if(carry > 0 ){
		result_arr[counter_result] = carry ;
		counter_result ++ ;
		carry = 0 ;
	}
}

										/*	MAIN FUNCTION		*/
void main(){
	UART_Init();																							// UART initialize function 
	for(general_counter = 0 ; welcom_arr[general_counter]!=0 ; general_counter++)
	{
		Transmit_data(welcom_arr[general_counter]);
	}
	general_counter = 0;
	Transmit_data('\r');
	while(1){
		Read_First_Number();
		Read_second_Number();
		
		if(operation == '*'){
			Mul();
			for(length_result = counter1 + counter2 + counter_send - 2 ; length_result >= 0 ; length_result--){
			remainder = result_arr[length_result] ;											
			if (remainder < 10){																	//if remainder from 0 to 9
				remainder = 48 + remainder;													//add 48 to send ascii of ( 0 : 9 )
				Transmit_data(remainder);
			}
			else{
				remainder = 55 + remainder;													//add 55 to send ascii of ( A : F )
				Transmit_data(remainder);
			}
			}
		}
		else if(operation == '+'){
			Add();
			for(length_result = counter_result - 1 ; length_result >= 0 ; length_result--){
			remainder = result_arr[length_result] ;												
			if (remainder < 10){																	//if remainder from 0 to 9
				remainder = 48 + remainder;													//add 48 to send ascii of ( 0 : 9 )
				Transmit_data(remainder);
			}
			else{
				remainder = 55 + remainder;													//add 55 to send ascii of ( A : F )
				Transmit_data(remainder);
			}
			}
		}	
		Transmit_data('\r');
		
											/*set all variables to default*/
		result = 0 ;
		counter_add = 0 ;
		counter_send = 0 ;
		counter_result = 0 ;
		length_result = 0 ;
		counter1 = 0 ;
		counter2 = 0 ;
		general_counter = 0 ;
		
		for(general_counter = 17; general_counter >= 0; general_counter--)			//clear the first number's array array
		first_arr[general_counter] = 0 ;
		for(general_counter = 17; general_counter >= 0; general_counter--)			//clear the second number's array array
		second_arr[general_counter] = 0 ;
		for(general_counter = 32; general_counter >= 0; general_counter--)			//clear the result number's array array
		result_arr[general_counter] = 0 ;
	}
}