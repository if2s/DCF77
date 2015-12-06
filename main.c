#include <avr/io.h>
#define F_CPU 2000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

int lbit=0;
uint16_t abitdauer=0;	//dauer der aktuellen Flanke
uint16_t minpos=0;		//"Zeigerposition" im array
uint16_t minarray[60];	//array zum Ablegen des Signales
uint16_t dcf_auswerten=1;
uint16_t wochentag=0;
uint16_t kalendertag=0;
uint16_t monat=0;
uint16_t jahr=0;
uint16_t stunde=0;
uint16_t minute=0;
uint16_t dummy=0;	// dummy, da sonst ein Breakpoint zum testen nicht funktioniert

int main(void)
{
	DDRD=0x00;		//Port D als Eingang
	DDRB=0xff;		//Port B als Eingang
	OCR2A = 0x07;	//Timer Interrupt bei 8-1 (1,024 ms)
	ASSR  = (1<< AS2);			// Timer 2 asynchron
	TCCR2A |= (1 << WGM21);		// Mode 2, CTC on OCRA
	TIMSK2 |= (1 << OCIE2A);	// Set interrupt on compare match
	TCCR2B |= (1 << CS22) | (1 << CS21); // Prescaler 256; 0,128 ms; 8 Takte = 1,024 ms
	sei();
	
	for (int i=0; i<60; i++)
		minarray[i]=0;
	
    while (1) 
    {
		// wenn 1 (PD1)
		if ( PIND & (1<<PIND1) ) {
			PORTB |= (1<<PB1);		//DCF-Signal auf PB1 ausgeben (für Testzwecke)
			//PORTB |= (1<<PB5);	//DCF-Signal auf LED ausgeben (für Testzwecke)
		}
		
		 //wenn 0 (PD1)
		if ( !(PIND & (1<<PIND1)) ) {
			PORTB &= ~(1<<PB1);		//DCF-Signal auf PB1 ausgeben (für Testzwecke)
			//PORTB &= ~(1<<PB5);	//DCF-Signal auf LED ausgeben (für Testzwecke)
		}
    }
}

//wird alle 1,024 ms aufgerufen
ISR (TIMER2_COMPA_vect)
{
	PORTB ^= ( 1 << PB2 );	//Pin togglen (für Testzwecke)
	
	if (dcf_auswerten == 1)
	{
		//Wenn alter Zustand == DCF-Signal, dann aktuelle bitdauer hochzählen
		if (lbit == (PIND & (1<<PIND1)))
		{
			abitdauer++;
		}
		else
		{
			PORTB &= ~(1<<PB5);	//LED ausschalten (für Testzwecke)
		
			//Wenn mehr als 2,5s Sendepause, dann sekunde=59, warte auf Zyklusstart
			if (abitdauer >= 1220 && abitdauer <= 1950 && lbit == 0)
			{
				// genau jetzt Minutenstart
				minpos=0;
				PORTB |= (1<<PB5); //LED einschalten (für Testzwecke). Steigende Flanke genau mit Sekunde 0
			}
		
			//Kurzer Impuls, Eintrag im Array
			else if (abitdauer >= 95 && abitdauer <= 146)
			{
				minarray[minpos] = 0;
				minpos++;
			}
		
			//Langer Impuls, Eintrag im Array
			else if (abitdauer >= 185 && abitdauer <= 250)
			{
				minarray[minpos] = 1;
				minpos++;
			}
			
			abitdauer=0;
			lbit = (PIND & (1<<PIND1));
		}
		if (minpos==59)
		{
			dcf_auswerten=0;
			
			if (minarray[57]==1){
				jahr=jahr+80;}
			if (minarray[56]==1){
				jahr=jahr+40;}
			if (minarray[55]==1){
				jahr=jahr+20;}
			if (minarray[54]==1){
				jahr=jahr+10;}
			if (minarray[53]==1){
				jahr=jahr+8;}
			if (minarray[52]==1){
				jahr=jahr+4;}
			if (minarray[51]==1){
				jahr=jahr+2;}
			if (minarray[50]==1){
				jahr=jahr+1;}
				
			if (minarray[49]==1){
				monat=monat+10;}
			if (minarray[48]==1){
				monat=monat+8;}
			if (minarray[47]==1){
				monat=monat+4;}
			if (minarray[46]==1){
				monat=monat+2;}
			if (minarray[45]==1){
				monat=monat+1;}
				
			if (minarray[44]==1){
				wochentag=wochentag+4;}
			if (minarray[43]==1){
				wochentag=wochentag+2;}
			if (minarray[42]==1){
				wochentag=wochentag+1;}
			
			if (minarray[41]==1){
				kalendertag=kalendertag+20;}
			if (minarray[40]==1){
				kalendertag=kalendertag+10;}
			if (minarray[39]==1){
				kalendertag=kalendertag+8;}
			if (minarray[38]==1){
				kalendertag=kalendertag+4;}
			if (minarray[37]==1){
				kalendertag=kalendertag+2;}
			if (minarray[36]==1){
				kalendertag=kalendertag+1;}
			
			if (minarray[34]==1){
				stunde=stunde+20;}
			if (minarray[33]==1){
				stunde=stunde+10;}
			if (minarray[32]==1){
				stunde=stunde+8;}
			if (minarray[31]==1){
				stunde=stunde+4;}
			if (minarray[30]==1){
				stunde=stunde+2;}
			if (minarray[29]==1){
				stunde=stunde+1;}
				
			if (minarray[27]==1){
				minute=minute+40;}
			if (minarray[26]==1){
				minute=minute+20;}
			if (minarray[25]==1){
				minute=minute+10;}
			if (minarray[24]==1){
				minute=minute+8;}
			if (minarray[23]==1){
				minute=minute+4;}
			if (minarray[22]==1){
				minute=minute+2;}
			if (minarray[21]==1){
				minute=minute+1;}
			
			dummy=1;
				
		}
	}
}
