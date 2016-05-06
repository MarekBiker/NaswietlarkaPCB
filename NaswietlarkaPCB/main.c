/*
* NaswietlarkaPCB.c
*
* Created: 11.04.2016 17:53:16
* Author : Marek
*/

/////////////  HELP  //////////////

//WARTOSC<<BIT
//PORTB |= 1<<0;
//PORTB &= 0<<0;

///////////////////////////////////

#include <avr/io.h>
#include "HD44780.h"
#include <stdio.h>
#include <util/delay.h>

#define DefaultTime 600
#define ButtonDelay 20


int main(void)
{
	// DEKLARACJE ZMIENNYCH
	short FunctionMode = 0;
	short Time_s;
	short Time_m;
	int TimeSelect = DefaultTime;
	short Start = 0;
	short Start_old = 0;
	char bufor[16];
	short EndFlag = 0;
	int TimeRemaining = 0;
	short EndSwitch = 0;
	short EndSwitch_old = 0;
	
	// INICJALIZACJA WYŒWIETLACZA
	LCD_Initalize();
	
	// USTAWIENIE WYJŒÆ//
	DDRB |= 0b00000111; // PB0, PB1, PB2 jako wyjscie
	DDRC |= 0b0000001;  // PC0 jako wyjscie
	
	// USTAWIENIE WEJŒÆ//
	// porty sa domyslnie jako wejscia
	PORTC |= 0b0111110; // ustawienie podciagania do VCC
	
	
	TCCR1A |=(1<<WGM10);
	TCCR1B |=(1<<WGM12);
	TCCR1A |=(1<<COM1A1);
	TCCR1B |=(1<<CS10);
	TCCR1B |=(1<<CS11);
	
	OCR1A = 120;
	
	PORTB |= 1<<0;
	
	while (1)
	{
		_delay_ms(20);
		Start_old = Start;
		EndSwitch_old = EndSwitch;
		
		LCD_Clear();
		
		if ( !(PINC & (1 << 3)) )
		{
			Start = 1;
		}
		else
		{
			Start = 0;
		}
		
		if ( !(PINC & (1 << 4)) | !(PINC & (1 << 5)) )
		{
			EndSwitch = 1;
		}
		else
		{
			EndSwitch = 0;
		}
		
		// TRYB WPROWADZANIA CZASU NAŒWIETLANIA
		if (FunctionMode == 0)
		{
			
			// zwiêkszanie czasu naœwietlania
			if ( !(PINC & (1 << 1)) )
			{
				TimeSelect = TimeSelect + 1;
				if (TimeSelect > 3600)
				{
					TimeSelect = 3600;
				}
				_delay_ms(ButtonDelay);
			}
			
			// zmniejszanie czasu naœwietlania
			if ( !(PINC & (1 << 2)) )
			{
				TimeSelect = TimeSelect - 1;
				if (TimeSelect < 1)
				{
					TimeSelect = 1;
				}
				_delay_ms(ButtonDelay);
			}
			
			// obs³uga wyœwietlacza
			Time_m = TimeSelect / 60;
			Time_s = TimeSelect - (Time_m * 60);
			
			sprintf(bufor,"**** %d:%d ****",Time_m,Time_s);
			
			LCD_WriteText(" Wprowadz czas: ");
			LCD_GoTo(0,1);
			LCD_WriteText(bufor);
			
			// jeœli START to zmienia tryb i rozpoczyna naœwietlanie
			if ( (Start == 1) & (Start_old == 0) )
			{
				// zalaczanie silnika
				PORTB &= ~(1<<0);
				// zalaczanie lampy
				PORTC |= 1<<0;
				
				TimeRemaining = TimeSelect;
				
				EndFlag = 0;
				
				FunctionMode = 1;
			}
		}
		
		// TRYB NASWIETLANIA
		else
		{
			// zmiana kierunku obrotów silnika jeœli pojawi siê sygna³ z krañcówki
			if ( (EndSwitch == 1) & (EndSwitch_old == 0) )
			{
				PORTB = PORTB ^ 0b00000100;
			}
			
			// obs³uga wyœwietlacza
			
			Time_m = TimeRemaining / 60;
			Time_s = TimeRemaining - (Time_m * 60);
			
			sprintf(bufor,"**** %d:%d ****",Time_m,Time_s);
			
			LCD_WriteText("Pozostaly czas: ");
			LCD_GoTo(0,1);
			LCD_WriteText(bufor);
			
			if (TimeRemaining == 0)
			{
				EndFlag = 1;
			}
						
			
			// jeœli ponownie START to wy³¹cza naœwietlanie i przechodzi do poprzedniego trybu
			if ( ((Start == 1) & (Start_old == 0)) | (EndFlag == 1) )
			{
				// wylaczanie silnika
				PORTB |= 1<<0;
				// wylaczanie lampy
				PORTC &= ~(1<<0);
				
				FunctionMode = 0;
			}
		}
	}
}

