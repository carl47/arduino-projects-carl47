#define IRpin_PIN      PIND
#define IRpin          2

unsigned int time[66];  //counter for pulse length
boolean valid = true; //valid ir code ?
byte IRaddress = 0;  //the ir address byte
byte IRcommand = 0;  //the ir command byte
byte data[8] = {11,22,33,44,55,66,77,88};

void setup() {
Serial.begin(9600);  //only for display of operation
TCCR1A = 0x00;  //set up timer 1 as 16 bit counter 
TCCR1B = 0x02;  // normal mode counts to 65535 and resets
//speed = 2 Mhz
}//end of setup

void loop() {
while (IRpin_PIN & (1 << IRpin)) {
  TCNT1 = 0x00;  //set start value of counter  
}//end of wait out low
while (! (IRpin_PIN & _BV(IRpin))) {
    time[0] = TCNT1;
    if(TCNT1 > 64000){
    time[0] = 0;
    break; //prevent overflow
    }//end of prevent overflow
}//end of find length of high

if((time[0] < 19000) & (time[0] > 17000)){
    TCNT1 = 0x00;  //reset the counter //look for low pulse
    while (IRpin_PIN & (1 << IRpin)) {
     time[1] = TCNT1;
     if(TCNT1 > 64000){
     time[1] = 0;
     break; //prevent overflow
     }//end of prevent overflow
    }//end of start low pulse 
    
     if((time[1] < 9500) && (time[1] > 8500)){       
       //we have a valid start sequence 9000ms followed by 4500ms
       
       valid = true; //start with high hopes
       for(int i = 0; i < 64; i +=2){ //find the 64 data bits
       TCNT1 = 0x00;  //reset the counter //look for high pulse 
       while (! (IRpin_PIN & _BV(IRpin))) {
        time[2+i] = TCNT1;
        if(TCNT1 > 64000){
        time[2+i] = 0;
        break; //prevent overflow
        }//end of prevent overflow
       }//end of find length of high
       if(time[2+i] < 900 || time[2+i] > 4000){
        valid = false;
        break; //end this pair search
       }//end of valid length ?
       TCNT1 = 0x00;  //reset the counter //look for low pulse
       while (IRpin_PIN & (1 << IRpin)) {
       time[3+i] = TCNT1;
        if(TCNT1 > 64000){
        time[3+i] = 0;
        break; //prevent overflow
        }//end of prevent overflow
       }//end of find length of low
       if(time[2+i] < 900 || time[2+i] > 4000){
        valid = false;
        break; //end this pair search
       }//end of valid length ?
       }//end of find pairs
    }//end of we have a start sequence
    else{
       valid = false;
    }//end of not start sequence
}//end of we have a start pulse
else {
  valid = false;
}//not valid start pulse

if(valid){
  for(byte i = 0; i < 16; i += 2){
    IRaddress = IRaddress << 1; //move bit along 
    byte bit = 0; //is the pair a 1 or 0
    if((time[2+i]< 2000) && (time[3+i]<2000)){bit = 1;}
    IRaddress = IRaddress | bit;  //add the bit value
  }//end of get address byte 

 for(byte i = 0; i < 16; i += 2){
    IRcommand = IRcommand << 1; //move bit along 
    byte bit = 0; //is the pair a 1 or 0
    if((time[34+i]< 2000) && (time[35+i]<2000)){bit = 1;}
    IRcommand = IRcommand | bit;  //add the bit value
  }//end of get address byte 
 //for display only
 unsigned int disp = IRaddress;
 data[0] = IRaddress;//MSB of IR
 disp = disp << 8; //into MSB
 data[1] = IRcommand;//LSB of IR
 disp = disp | IRcommand;

 Serial.println( disp, HEX); //print display for example only
   
}//end of if valid
}//end of loop
