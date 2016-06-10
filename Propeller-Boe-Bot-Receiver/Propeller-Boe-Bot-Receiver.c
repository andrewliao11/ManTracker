#include "simpletools.h"
#include "servo.h"
#include "fdserial.h"
#include "wavplayer.h"
#include "servodiffdrive.h"
#include "ping.h"                             // Include ping header

#define PIN_SOUND       26 // pin for the speaker, default=26
#define PIN_XBEE_RX      0
#define PIN_XBEE_TX      1
#define PIN_SERVO_LEFT  14
#define PIN_SERVO_RIGHT 15
#define PIN_SERVO_MID 16

volatile int ch;
volatile int XeeChange=0;
int * cog_ptr[8];
volatile fdserial *xbee;

void XBee();
void forward();
void left();
void right();
void stop();
void backward();

int main (void) {
  xbee = fdserial_open(PIN_XBEE_RX, PIN_XBEE_TX, 0, 9600);
  sd_mount(DO, CLK, DI, CS);                      // Mount SD card
  drive_pins(PIN_SERVO_LEFT, PIN_SERVO_RIGHT);    // Set the Left and Right servos
  print("=================================\n");
  print("  Propeller Boe-Bot ManTracker \n");
  print("=================================\n\n");
  print("[1] Move forward\n");
  print("[2] Turn Left\n");
  print("[3] Turn Right\n");
  print("[4] Stop\n");
  print("[5] Move backward\n");
  
  cog_ptr[1] = cog_run(&XBee,32);
  int cmDist=10;

  while(1)
  { 
    // use PING to detect obstacle
    cmDist = ping_cm(19);                 
    print("cmDist = %d\n", cmDist);
    // Beep when the obstacle is less than 20 cm away
    if (cmDist<20){
      stop();
      freqout(4, 500, 6000);
      }
      else
      {
    // check if the Xbee signal change
    if(XeeChange)
    {
       
      ch = char2lowercase(ch);
      switch ((char) ch)
      {
        case '1':
          forward();
          break;
        case '2':
          left();
          break;
        case '3':
          right();
          break;
        case '4':
          stop();
          break;
        case '5':
          backward();
          break;
        default:
           print("%c", (char) ch);
      }     
      XeeChange = 0;
      print("%c", (char) ch);
      }   
       }   
    pause(1);
    
  }    

  return 0;
}


void forward(){
  
  // with Boe-Bot car pin 14,15 connect to servo 
  servo_speed(14,30);
  servo_speed(15,-30);
  }
  
  
void left(){
  
  servo_speed(14,0);
  servo_speed(15,-30);
  }
  
void right(){
  servo_speed(14,30);
  servo_speed(15,0);
  }
  
void stop(){
  servo_speed(14,0);
  servo_speed(15,0);
  }
  
void backward(){
  
  servo_speed(14,-10);
  servo_speed(15,10);
  
  }

void XBee(){
   
   putchar(CLS);
   fdserial_rxFlush(xbee);
   while(1){
     ch = fdserial_rxChar(xbee);
     XeeChange = 1;
     fdserial_txChar(xbee, ch);
     fdserial_txFlush(xbee);
   }
}  
