
// --------------- 
//Initialization 

void setup() { 
    //Configure the ADC pins as inputs 
    Serial.begin(9600);
    pinMode(A0, INPUT); 
    pinMode(A1, INPUT); 
    //Configure the ADC mux
    ADMUX = 0x00; //MUX[3:0]=0000 -> select analog channel 0, 
                  //ADLAR=0 -> AD samples are right adjusted
    ADMUX |= 0x40;//REFS[1:0]=01, set voltage reference do AVcc 
    
    //Configure the ADC prescaler and enable interrupts 
    ADCSRA = 0x00; 
    ADCSRA |= 0x07; //ADPS[2:0]=111, set prescaler to 128 -> fs = 4807.69Hz  2 channels 
    ADCSRA |= 0x08; //ADIE=1, enable ADC interrupts 
    ADCSRA |= 0x20; //ADATE=1, enable auto-trigger 
    
    //Configure the ADC trigger source 
    ADCSRB = 0x03; //ACME=0, ADTS[2:0]=011 -> trigger source = timer/counter0
    
    //Disable the ADC digital input buffers 
    DIDR0 = 0xFF; 
    
    //Enable microprocessor global interrupts 
    SREG |= 0x80;
    
    //Enable the ADC and start the acquisition 
    ADCSRA |= 0x80; //ADEN=1, enable AD converter 
//    ADCSRA |= 0x40; //ADSC=1, start AD conversion 
    TCCR0A = 0x82;
    TCCR0B = 0x03;//Pre scale = 64
    TCNT0 = 0;
    OCR0A = 39;
    TIMSK0 = 0x02;
} 

// --------------- 
// Global variables

const int N = 2; //Number of channels
const int tam = 100;  //Size of the data buffers 

bool procStatus = false; //Flag to start data processing  
int dataVector[N][tam]; //Data vectors for each channel
int counter = 0; //Controls the number of samples 

//Loop to process the data 
void loop(){
    int i; 
    char cmd; 

    //Verify if it is time to process data 
    if (procStatus == true){ 
      noInterrupts(); 
        for(int i = 1; i< tam; i++){
            Serial.print(3.3);
            Serial.print(" ");
            Serial.print(0);
            Serial.print(" ");
            Serial.print(5);
            Serial.print(" ");
            Serial.print((5*dataVector[0][i])/1023);
            Serial.print(" ");
            Serial.print((5*dataVector[1][i])/1023);
            Serial.println(" ");
            Serial.flush();
        }
        
        procStatus = false; 
        interrupts(); 
    }
}

// --------------- 
//ADC interrupt service routine 

ISR(ADC_vect){
int sample, CH; 
//Serial.print(3);
//Read the latest sample 
    sample = ADCH; //Read the lower byte 
    sample += ADCL<<8; //Read the upper byte 
    //Store data from a specific channel.Halt acquisition 
    //after 'tam' samples and start processing. 
    //Serial.print(sample);
    
    if (procStatus == false){
     // Serial.print(procStatus);
      // Serial.print(ADMUX,HEX);
        CH = ADMUX & 0x0F; //AD channel 
        dataVector[CH][counter] = sample; //Store the data 
        if (++CH < N){ //Verify if all channels were acquired 
            ADMUX += 1; //If not, go to the next channel 
        } 
        else{ 
            ADMUX &= 0xF0; //If so, turn to channel 0 and 
            counter++;      //update the number of samples 
        } 
        //Verify if it is time to process the data 
        if (counter == tam){ 
            counter = 0; 
            procStatus = true; //Set the flag to start processing 
        }
    } 
}

ISR(TIMER0_COMPA_vect){
 // Serial.print(2);
}


