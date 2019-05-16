#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SET_BIT(PORT,PIN)     PORT |= (1<<PIN)
#define CLR_BIT(PORT,PIN)     PORT &= ~(1<<PIN)
#define SEAT_OCCUPIED_LED_ON      SET_BIT(PORTD,PD7)
#define SEAT_OCCUPIED_LED_OFF     CLR_BIT(PORTD,PD7)
#define SEAT_BELT__LED_ON          SET_BIT(PORTD,PD3)
#define SEAT_BELT_LED_OFF          CLR_BIT(PORTD,PD3)
#define ENGINE_LED_ON          SET_BIT(PORTD,PD5)
#define ENGINE_LED_OFF          CLR_BIT(PORTD,PD5)



volatile unsigned int SEAT_OCCUPIED_FLAG=0; 
volatile unsigned int SEAT_BELT_FLAG=0; 
volatile unsigned int ENGINE_ON_FLAG=0;
int ADC_VALUE;


void INIT_T0_ON()
{
  TCCR0A|=(1<<WGM01);
  TCCR0B&=~((1<<CS00)|(1<<CS01));
  TCCR0B|=(1<<CS02);
  OCR0A=255;
  OCR0B=255;
  TIMSK0|= (1<<OCIE0A)|(1<<OCIE0B);
  
}
void INIT_ADC5()
{
  ADMUX=0X05;// PIN AD5 CHANNEL
  ADMUX|=(1<<REFS0);// REFRENCE TO ADC
  ADCSRA|=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);//ENABLING
}
uint16_t adc5_read()
{ INIT_ADC5();
  SET_BIT(ADCSRA,ADSC);//SOC
  while(ADCSRA&(1<<ADSC));
  return(ADC);
}



void init()
{
  CLR_BIT(DDRB,PB0); //seat occupied  Switch
    SET_BIT(PORTB,PB0); //PULLUP Enabled
   CLR_BIT(DDRC,PC0); //seat belt    Switch
    SET_BIT(PORTC,PC0); //PULLUP Enabled
    
   CLR_BIT(DDRD,PD2); //Engine   Switch
    SET_BIT(PORTD,PD2); //PULLUP Enabled

  SET_BIT(DDRD,PD7);  //LED_seat occupied as output
  SET_BIT(DDRD,PD3);  //LED  seat belt  as output
    SET_BIT(DDRD,PD5);  //LED  Engine as output
   PCICR|=(1<<PCIE0);
    PCMSK0|=(1<<PCINT0);
  PCICR |=(1<<PCIE1);
    PCMSK1 |=(1<<PCINT8);
  EICRA |=  (1 << ISC00);
    EIMSK |= (1 << INT0);
INIT_T0_ON();
 INIT_ADC5();
  sei();
}

int main()
{
  init();
   while(1)
    {
     if(SEAT_OCCUPIED_FLAG==1)
     {
        SEAT_OCCUPIED_LED_ON ;
       _delay_ms(1000);
        if(SEAT_BELT_FLAG==1)
  {
    SEAT_BELT__LED_ON;
     _delay_ms(1000);
          if(ENGINE_ON_FLAG==1)
          { ENGINE_LED_ON ;
            ADC_VALUE=adc5_read();
           if(left>=0 && left<=200)
           {
             OCR0B=50;
           }
      else  if(ADC_VALUE>200 && ADC_VALUE<=400)
           {
             OCR0B=100;
           }
            
           else  if(ADC_VALUE>400 && ADC_VALUE<=700)
           {
             OCR0B=150;
           }
            
           else  if(ADC_VALUE>700 && ADC_VALUE<=1023)
           {
             OCR0B=200;
           }
            
            _delay_ms(1000);
          }
          else
          {OCR0B=0;
            ENGINE_LED_OFF ;
          }
  }
  else
  {
     ENGINE_LED_OFF ; 
   SEAT_BELT_LED_OFF ; 
  }
   
     }
     else
     {ENGINE_LED_OFF ;
     SEAT_BELT_LED_OFF ; 
     SEAT_OCCUPIED_LED_OFF;
     }
   
 
}
    
    
  return 0;
}

ISR(PCINT0_vect)                   //seat occupied
{
    SEAT_OCCUPIED_FLAG = ! SEAT_OCCUPIED_FLAG ;
}

ISR(PCINT1_vect)                   //seatbelt
{
    if( SEAT_OCCUPIED_FLAG==1)
        SEAT_BELT_FLAG = !SEAT_BELT_FLAG;
}
ISR (INT0_vect)                     //engine
{
    if(SEAT_BELT_FLAG==1 ||  SEAT_OCCUPIED_FLAG==1)
    {
        ENGINE_ON_FLAG=!ENGINE_ON_FLAG;
    }
}
ISR(TIMER0_COMPA_vect)
{
  SET_BIT(PORTD,6);
}
ISR(TIMER0_COMPB_vect)
{
  CLR_BIT(PORTD,6);
}


