/* 4us precision chronometer
 * -------------------------
 *
 * Chronometer with a four microsecond of precision based on
 * 16bits internal timer-1 of ATmega168.
 * Only one interrupt by 100ms.
 *
 *
 * Created by Vincent Le Gouic in 27th January 2009
 * Copyleft 2009
 * France
 *
 */

#include <LiquidCrystal.h>
//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10;

// DECLARO LAS BARRERAS
const int barrera_3 = 3;
const int barrera_2 = 2;
int barrera_i;
int barrera_f;


// DECLARO LOS TIEMPOS
unsigned long tiempo_i = 0;
unsigned long tiempo_f = 0;

// DECLARO UNA BANDERA PARA LA SELECCIÓN DE PASOS
int k;

// DECLARO LAS VARIABLES DE TIEMPOS
unsigned int horas = 0;
unsigned int minutos = 0;
unsigned int segundos = 0;
unsigned int centesimos =0;
unsigned int milisegundos;

// INICIALIZO LA PANTALLA
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);


// Variables definition
uint16_t microseconds = 0, milliseconds = 0;
uint8_t seconds = 0, minutes = 0, hours = 0;


// Interrupt service routines
ISR(TIMER1_COMPA_vect)  // Timer-1 OC1A Match interrupt handler
{
  milliseconds += 100;
  if (milliseconds == 1000)
  {
    milliseconds = 0;
    seconds++;
  }
  if (seconds == 60)
  {
    seconds = 0;
    minutes++;
  }
  if (minutes == 60)
  {
    minutes = 0;
    hours++;
  }
}


// Reset chronometer
void reset_chrono(void)
{
  unsigned char sreg;
  sreg = SREG;  // Save global interrupts flag
  cli();  // Disable interrupts
  TCNT1 = 0;  // Reset timer-1 counter
  SREG = sreg;  // Restore global interrupts flag
  milliseconds = 0;
  seconds = 0;
  minutes = 0;
  hours = 0;
}


// Read microseconds counter
uint32_t read_microseconds(void)
{
  uint32_t microseconds;
  unsigned char sreg;
  sreg = SREG;  // Save global interrupts flag
  cli();  // Disable interrupts
  microseconds = TCNT1*4;  // one timer-1 count for 4us
  SREG = sreg;  // Restore global interrupts flag
  return microseconds;
}


// Setup
void setup(void)
{
  Serial.begin(9600);  // Open serial port with 9600bps

   // DECLARO LAS BARRERAS 
 pinMode(barrera_3,INPUT);
 pinMode(barrera_2,INPUT);

 // IMPRIMO EL MENSAJE EN PANTALLA
 lcd.begin(16, 2);
 lcd.setCursor(0,0);
 lcd.print("Cronometro");
 delay(1000);

 // MUESTRO EL TIEMPO
 lcd.setCursor(0,1);
 //lcd.print("Tiempo:");
  
  // Set timer-1 parameters
  TCCR1A = 1<<WGM10 | 1<<WGM11; // Fast PWM mode enabled with 16bits registers
  TCCR1B = 1<<CS10 | 1<<CS11 | 1<<WGM12 | 1<<WGM13;  // Fixe clock for timer1 F_CPU=16MHz/64 (prescaler)
  OCR1A = 24999;  // Set the top limit counter to 24999
  TIMSK1 = 1<<OCIE1A;  // Enable interrupt on 0C1A match
  sei();  // Enable interrupts
}


// Main loop
void loop(void)
{
  char char_readed = 0; 
  char_readed = Serial.read();  // Read serial port character
  
  // LEO LAS BARRERAS
  barrera_i = digitalRead(barrera_3); 
  barrera_f = digitalRead(barrera_2);

  // DEFINO QUE HAGO CON LAS BARRERAS

  //if (digitalRead(reset) == LOW){
  //k=0;
  //}

if (barrera_i == LOW){
k=1;
}
if (barrera_f == LOW){
k=2;
}
  
  if (char_readed == 0x31 || k == 1)  // Type "1" to reset chronometer
  {
    reset_chrono();
    Serial.println("Reset chronometer");
  }
  
  if (char_readed == 0x32 || k == 2)  // Type "2" to print chronometer time
  {
    lcd.setCursor(0,1);
    uint32_t microseconds_count = 0;
    uint16_t milliseconds_display = 0, microseconds_display = 0, milliseconds_rest = 0;
    microseconds_count = read_microseconds();
    milliseconds_rest = (uint16_t)(microseconds_count/1000);  // Take the two most important digits (decimal value) of microseconds counter
    milliseconds_display = milliseconds + milliseconds_rest;  // Add rest to milliseconds hundred
    microseconds_display = (uint16_t)(microseconds_count-(uint32_t)(milliseconds_rest*1000));  // Conserve microseconds
    Serial.print(hours, DEC);
    Serial.print("h ");
    Serial.print(minutes, DEC);
    Serial.print("m ");
    Serial.print(seconds, DEC);
    Serial.print("s ");
    lcd.print(seconds, DEC);
    lcd.print("s ");
    Serial.print(milliseconds_display, DEC);
    Serial.print("ms ");
    lcd.print(milliseconds_display, DEC);
    lcd.print("ms ");
    Serial.print(microseconds_display, DEC);
    Serial.print("us ");
    lcd.print(microseconds_display, DEC);
    lcd.print("us ");
    Serial.println();
  }
}
