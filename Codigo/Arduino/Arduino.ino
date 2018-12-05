// --------------- 
//Initialization 

void setup() { 
    //Configure the ADC pins as inputs 
    Serial.begin(115200);
    pinMode(A0, INPUT); 
    pinMode(A1, INPUT); 
    pinMode(A2, INPUT); 
    pinMode(A3, INPUT); 
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
const int tamVec = 100;  //Size of the data buffers 
int var = 1;
bool procStatus = false; //Flag to start data processing  
int dataVector[N][tamVec]; //Data vectors for each channel
int counter = 0; //Controls the number of samples 
bool calcEnergia = false;
int luz;
int temperatura;  
float energia=0;
//Loop to process the data 
void loop(){
    int i; 
    char cmd; 
     //analogWrite(5, 100);
    //Verify if it is time to process data 
    if (procStatus == true){ 
//      Serial.print(float(dataVector[0][0])*5/1023);//Corrente
//      Serial.print(" ");
//      Serial.print(float(dataVector[1][0])*5/1023);//Tensao
//      Serial.print(" ");
      Serial.print(float(temperatura)*5/1023);//temperatura
      Serial.print(" ");
      Serial.print(float(luz)*5/1023);//luz
      Serial.print(" ");
//      Serial.print(energia);//energia
//      Serial.print(" ");
        for(int i = 1; i< tamVec; i++){
            Serial.print(3.3);
            Serial.print(" ");
            Serial.print(0);
            Serial.print(" ");
            Serial.print(5);
            Serial.print(" ");
            Serial.print(float(dataVector[0][i])*5/1023);
            Serial.print(" ");
            Serial.print(float(dataVector[1][i])*5/1023);
            Serial.println(" ");
          //  Serial.flush();
        }
        noInterrupts(); 
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
      sample = ADCL;
  sample += ADCH<<8; //Read the upper byte 
    //Store data from a specific channel.Halt acquisition 
    //after 'tamVec' samples and start processing. 
    //Serial.print(sample);
    
    if (procStatus == false){
     // Serial.print(procStatus);
      // Serial.print(ADMUX,HEX);
        CH = ADMUX & 0x0F; //AD channel 
        switch (var){

      case 1://Corrente
        ADMUX = 0X41;//Proxima leitura de corrente
        dataVector[CH][counter] = sample; //Store the data 
        var = 2;
        break;

      case 2://Tensao
        ADMUX = 0X42;//Proxima leitura de temperatura
        dataVector[CH][counter] = sample; //Store the data 
        var = 3;
        calcEnergia = true;
        break;

      case 3://Luz
        ADMUX = 0X40;//Proxima leitura de tensao
        var = 4;
        counter++;
        if ((tamVec-counter)<=1){
          luz = sample;
        }
        break;

      case 4://Corrente
        ADMUX = 0X41;//Proxima leitura de corrente
        dataVector[CH][counter] = sample; //Store the data 
        var = 5;
        break;

      case 5://Tensao
        ADMUX = 0X43;//Proxima leitura de luminancia
        dataVector[CH][counter] = sample; //Store the data 
        var = 6;
        calcEnergia = true;
        break;

      case 6://Temperatura
        ADMUX = 0X40;//Proxima leitura de tensao
        var = 1;
        counter++;      //update the number of samples 
        if ((tamVec-counter)<=1){
          temperatura = sample;
        }
        break;   

      default:
        var = 1;//Iniciar a variavel para 1 em caso de bugs
        break;
    }
    
        //Verify if it is time to process the data 
        if (counter == tamVec){ 
            counter = 0; 
            procStatus = true; //Set the flag to start processing 
        }
    } 
}

ISR(TIMER0_COMPA_vect){
 // Serial.print(2);
}

