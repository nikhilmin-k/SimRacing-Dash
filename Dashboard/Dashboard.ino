#include "SevSeg.h"
#include <ESP32Servo.h>

typedef enum {
  RPM,
  RPM_TICK,
  SPEED,
  GEAR,
  TYRE_STATUS,
  TYRE_TEMP,
  TIME,
  INIT,
  FUEL,
  LAP,
  CURRENT_TIME,
  FUEL_TICK,
  CURRENT_POSITION,
  PERFORMANCE_METER
} MessageType;

typedef struct {
  String Command;
  String Value;
} Command;

typedef struct{
  int Percent;
  String Gear;
  String Rpm;
} LEDDisplay;

LEDDisplay ldisplay = {0,"N",""};

//Bar LEDs
int barPins[] = {15,21,22,23,18,25,26,27};
int barpinslock = 0;
int barnum = 0;

//Seven Segment Display 
SevSeg sevseg;
int sevseglock = 0;
int num = 0;

//Servo connection
Servo myServo;
int servoPin = 19;
float servoPos;
const int servoRes = 16;
int servolock = 0;
bool servodir = false;

//PWM for Servo
const int servoChannel = 1;

void setup() {
  Serial.begin(115200);
  
  //Bar LEDs setup
  for(int i=0; i<sizeof(barPins); i++){
    pinMode(barPins[i], OUTPUT);
  }
  
  //Servo PWM setup
  ledcSetup(servoChannel, 50, servoRes);
  ledcAttachPin(servoPin, servoChannel); 
  pinMode(servoPin, OUTPUT);
  servoPos = 0; 

  //Seven Segment Display setup
  byte numDigits = 1;
  byte digitPins[] = {};
  byte segmentPins[] = {32,2,4,5,12,13,14,33};
  bool resistorsOnSegments = true;

  byte hardwareConfig = COMMON_ANODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}

void loop() {

  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('|');
    //prevent bad message
    int iPosDollar = command.indexOf("$");
    if(command != "" && (iPosDollar==1 || iPosDollar==2))
    {
      ParseCommand(command);
    }
  }
  RefreshDisplay();
  
}

void RefreshDisplay()
{
    //set num to the in-game gear
  if(sevseglock > 1){ //if statements exist for spin locking if needed
    sevseg.setNumber(gearToInt(ldisplay.Gear));
    sevseg.refreshDisplay();
    sevseglock = 0;
  }

  if(barpinslock > 1){
    setBarPinsPercent(ldisplay.Percent);
    barpinslock = 0;
  }

  if(servolock > 1){
    setServoPercent(ldisplay.Percent);
    
    servolock = 0;
  }
  
  //delay(1);
  servolock++;
  sevseglock++;
  barpinslock++;
}

Command ExtractCommand(String pStr)
{
  Command cCmd;

  char *array[5] = {"","","","",""};
  char *sep = "$";

  char buf[64];
  pStr.toCharArray(buf,sizeof(buf));
  split(array,buf,sep); 
  
  cCmd.Command = String(array[0]);
  cCmd.Value = String(array[1]);    

  return cCmd;
}

void ParseCommand(String pStr){
  Command cmd = ExtractCommand(pStr);
  String iValue = cmd.Value;  
  switch (cmd.Command.toInt()){
    case RPM:
      ldisplay.Rpm = iValue;
      break;
    case RPM_TICK:
      ldisplay.Percent = iValue.toInt();
      break;
    case GEAR:
      ldisplay.Gear = iValue;
      break;
    default:
      break;
  }
}

int gearToInt(String gear){
  if(gear == "N")
    return 0;
  else
    return gear.toInt();
}

void setServoPercent(int percent){
  setServo(180.0-(((float)percent/100.0)*180));
}

void setBarPinsPercent(int percent){
  if(percent>74){
    setBarPins((percent-74)/3);
  }
  else{
    setBarPins(0);
  }
}

void setBarPins(int barnum){
  for(int i=0; i<barnum; i++){
      digitalWrite(barPins[i], HIGH);}
    for(int i=barnum; i<sizeof(barPins); i++){
      digitalWrite(barPins[i], LOW);}
}

void setServo(float pos){//16 bit servo requires 16 bit input
  uint32_t duty = (((pos/180.0)*2000)/20000.0*65536.0) + 1634;
     // convert 0-180 degrees to 0-65536

  ledcWrite(servoChannel,duty);
    // set channel to pos
}

void split(char *array[],char *pStr,char *pSep)
{
    int i = 0;
    char *p = strtok (pStr, pSep);
    while (p != NULL)
    {
        array[i++] = p;
        p = strtok (NULL, pSep);
    }
}
