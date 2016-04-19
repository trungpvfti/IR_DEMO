#include <Thread.h>
#include <ThreadController.h>
#include <IRLib.h>
//Create a receiver object to listen on pin 11
IRrecv My_Receiver(11);
 
//Create a decoder object
IRdecode My_Decoder;

//chân ST_CP của 74HC595
const int latchPin = 4;
//chân SH_CP của 74HC595
const int clockPin = 3;
//Chân DS của 74HC595
const int dataPin = 2;
//int data2Pin = 5;
const byte Seg[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};//,0x77,0x7C,0x39,0x5E,0x79,0x71,0x63,0x80};//0123456789abcdefo.
const byte Seg2[18] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x63,0x80};//0123456789abcdefo.
const byte Seg1[8] = {0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE};

int ledPin = 13;

// ThreadController that will controll all threads
ThreadController controll = ThreadController();

//My Thread
Thread iRThread = Thread();
//His Thread
Thread ledThread = Thread();
//Blink Led Thread
Thread blinkLedThread = Thread();
//ThreadController, that will be added to controll
ThreadController groupOfThreads = ThreadController();
void HienThiLEDDon(byte LED_No, byte value) {  
  shiftOut(dataPin, clockPin, MSBFIRST, Seg1[LED_No]);//set led nao sang
  shiftOut(dataPin, clockPin, MSBFIRST, Seg2[value]);// set gia tri hien thi
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);  
}
byte phandonvi = 0;
byte phanchu = 0;
void ledDisplayCallback(){
    HienThiLEDDon(0,12);//Hien thi chu o
    //delay(1);//Đợi 0.5 s cho mỗi lần tăng số    
    HienThiLEDDon(1,16);//hien thi chu C
    //delay(1);
    HienThiLEDDon(2,phandonvi);    
    //delay(1);//Đợi 0.5 s cho mỗi lần tăng số
    HienThiLEDDon(3,phanchu);
    //delay(1);   
}
//==========================
// callback for myThread
void iRRecieveCallback(){
  if (My_Receiver.GetResults(&My_Decoder)) {
      My_Decoder.decode();		//Decode the data
      My_Decoder.DumpResults();	//Show the results on serial monitor
      My_Receiver.resume(); 		//Restart the receiver
      Serial.print("IR_Value: ");Serial.println(My_Decoder.value,HEX);
      Serial.print("IR_Value: ");Serial.println(My_Decoder.value);
    }
    
}

// callback for blinkLedThread
void blinkLed(){
	static bool ledStatus = false;
        static unsigned char point =0;
        point++;
        phandonvi=point%10UL;
        phanchu=point/10UL;
        if(point==100)point = 0;
        //delay(1000);                                        
	ledStatus = !ledStatus;

	digitalWrite(ledPin, ledStatus);

	Serial.print("blinking: ");
	Serial.println(ledStatus);
}

void setup(){
	Serial.begin(115200);
        My_Receiver.enableIRIn(); // Start the receiver
        //Bạn BUỘC PHẢI pinMode các chân này là OUTPUT
        pinMode(latchPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(dataPin, OUTPUT);
	pinMode(ledPin, OUTPUT);

	// Configure ledThread
	ledThread.onRun(ledDisplayCallback);
	ledThread.setInterval(1);//1ms

	// Configure iRThread
	iRThread.onRun(iRRecieveCallback);
	iRThread.setInterval(1);// 1ms
	
	// Configure blinkLedThread
	blinkLedThread.onRun(blinkLed);
	blinkLedThread.setInterval(1000);//100 ms

	// Adds myThread to the controll
	controll.add(&ledThread);

	// Adds hisThread and blinkLedThread to groupOfThreads
	groupOfThreads.add(&iRThread);
	groupOfThreads.add(&blinkLedThread);

	// Add groupOfThreads to controll
	controll.add(&groupOfThreads);	
}

void loop(){
	// run ThreadController
	// this will check every thread inside ThreadController,
	// if it should run. If yes, he will run it;
	controll.run();

	// Rest of code
	float h = 3.1415;
	h/=2;
}
