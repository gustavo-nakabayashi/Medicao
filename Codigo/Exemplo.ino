int numSamples=0;
long t, t0;
int dataVector[1][1000]; //Data vectors for each channel
void setup()
{
  Serial.begin(115200);

  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  //ADMUX |= (1 << REFS0);  // set reference voltage
    ADMUX |= 0x40;//REFS[1:0]=01, set voltage reference do AVcc
 // ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  ADCSRA |= 0x07;
//  ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}

ISR(ADC_vect)
{
  int sample;
 // byte x = ADCH;  // read 8 bit value from ADC
// dataVector[0][numSamples] += ADCH<<8; //Read the upper byte 
//  dataVector[0][numSamples] = ADCL;
  
//  sample = ADCL;
//  sample += ADCH<<8; //Read the upper byte 
  sample = ADCL;
  sample += ADCH<<8; //Read the upper byte 
  dataVector[0][numSamples]= sample;
  numSamples++;
}
  
void loop()
{
  if (numSamples>=1000)
  {
    noInterrupts();
    for(int i = 1; i< 1000; i++){
             Serial.print(3.3);
            Serial.print(" ");
            Serial.print(0);
            Serial.print(" ");
            Serial.print(5);
            Serial.print(" ");
//            Serial.print(0);
//            Serial.print(" ");
//            Serial.print(1000);
//            Serial.print(" ");
            Serial.println((5*float(dataVector[0][i]))/1023);
    }
    
    
     interrupts(); 

    
    // restart
 
    numSamples=0;
  }
}
