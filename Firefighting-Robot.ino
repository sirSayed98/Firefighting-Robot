#include <Stepper.h>
#define TRIGGER_PIN        A5            // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_Right     A3           // Arduino pin tied to echo pin on the ultrasonic sensor.
#define ECHO_PIN_Left      A2          // Arduino pin tied to echo pin on the ultrasonic sensor.
#define ECHO_PIN_Forward   A4         // Arduino pin tied to echo pin on the ultrasonic sensor.
#define Min_DISTANCE 30             // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define LedPin A1
#define step_8 500
#define CLOSE_Switch      3
#define interval 1000                      // the time we need to wait
#define fanPin 2                    // Sensor input
#define period 1000
#define stepsPerRevolution 32   // change this to fit the number of steps per revolution


 /*      START  COUNT LINES           */         
unsigned long previousMillis,start,end,acc=0; 
bool ledState,first_turn = false; // state variable for the LED
Stepper leftStepper(stepsPerRevolution,8,6,9,7);
Stepper fanStepper(stepsPerRevolution,10,11,12,13);
Stepper rightStepper(stepsPerRevolution,4,0, 5, 1);
int C_fan,C_left,C_right=0;

void setup()
{
Serial.begin(9600);
pinMode(TRIGGER_PIN,OUTPUT);
pinMode(ECHO_PIN_Right,INPUT);
pinMode(ECHO_PIN_Left,INPUT);
pinMode(ECHO_PIN_Forward,INPUT);
pinMode(LedPin,OUTPUT);
pinMode(CLOSE_Switch,INPUT);
attachInterrupt(digitalPinToInterrupt(CLOSE_Switch), close,FALLING); //use interupt
}

void loop(){
  if((unsigned long)acc>500&&!first_turn) ActivateFan(); //if acc time <500 (half second) no fire
  if(digitalRead(fanPin)==HIGH) start=millis();      // if sensor detect fire at start of loop  wait for half second  
  if(digitalRead(fanPin)==LOW) first_turn=false;    //to handle turn on and off for fan switch at HIGH only
  if(digitalRead(CLOSE_Switch)==HIGH) Led_Blank(1);  //main loop for movement
  if(digitalRead(fanPin)==HIGH) acc=acc+(millis()-start); //if sensor detect fire  at end of loop sum the time of loop to acc
  else acc=0;//reset acc if fanpin ==low
}

void Trigger(){
digitalWrite(TRIGGER_PIN,LOW);
unsigned long tmp=micros();
while(micros()-tmp<5){};
digitalWrite(TRIGGER_PIN,HIGH);
tmp=micros();
while(micros()-tmp<10){};
digitalWrite(TRIGGER_PIN,LOW);
}


void MoveMent(){ //MOVEMENT of robot
  if(CheckForward()&&C_right==0&&C_left==0) //if we didn't have obstacle in forward   //if C_right>0 that mean we have to complete 2 revolution then look forward again  
    {  Serial.println("GO Forward");
      leftStepper.step(8);    
      rightStepper.step(8); 
    }
  else if(CheckRight()&&C_left==0) //C_RIGHT=8 8*8=64 step  64/32= 2complete Revolution 90 degree
   {
Serial.println("TURN RIGHT");
	  C_right--;      
    leftStepper.step(8); 
  }
  else if(CheckLeft()&&C_right==0)  ////C_left=8 8*8=64 step  64/32= 2complete Revolution 90 degree
  {
Serial.println("TURN LEFT");
     C_left--;      
    rightStepper.step(8); 
  }
else{	
Serial.println("GO BACK");	//reverse dierction
      leftStepper.step(-8);    
      rightStepper.step(-8); 
}  
}

bool CheckForward(){
Trigger();
return (pulseIn(ECHO_PIN_Forward,HIGH)/57)>Min_DISTANCE ? true:false ;}


bool CheckRight()
{
if(C_right >=1)
return true;
else{
Trigger();
 if((pulseIn(ECHO_PIN_Right,HIGH)/57)>Min_DISTANCE )
 {
  C_right=8;return true;
 }
 else return false;
}
}

bool CheckLeft()
{
if(C_left >=1)
return true ;
Trigger();
 if((pulseIn(ECHO_PIN_Left,HIGH)/57)>Min_DISTANCE )
 {
  C_left=8;return true;
 }
 else return false;
}

void ActivateFan() {
while(C_fan<2)
{
Led_Blank(0);
 
fanStepper.step(4); //c_fan=0
if(digitalRead(fanPin)==LOW&&C_fan==0) C_fan++;//C_fan=1
if(C_fan==1&&digitalRead(fanPin)==HIGH) C_fan++;//C_fan=2  
}  
C_fan=0;acc=0;first_turn=true;
}

void close(){digitalWrite(LedPin, LOW);}

void Led_Blank(int a){
unsigned long currentMillis = millis(); // grab current time  
if((unsigned long)(currentMillis - previousMillis) >= interval) 
{  ledState = !ledState;             
  digitalWrite(LedPin, ledState);  
  previousMillis = millis();
  if(a==1)MoveMent();
}
}
