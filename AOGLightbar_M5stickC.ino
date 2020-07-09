
////////////////// User Settings /////////////////////////
  #define USE_BLUETOOTH_SERIAL 1         // use USB Serial -> 0  use Bluetooth Serial -> 1
  #define NUMPIXELS   30                 //How many pixels
  #define Neopixel_Pin 32                //GPIP32:M5stickC  GPIO26:M5Lite
  #define mmPerLightbarPixel  40         // 40 = 4cm
  #define NIGHT_BRIGHTNESS 50            //night mode brightness
  #define USE_LCD 1                      //1:LCD display(onlyM5stickC)
  #include <M5StickC.h>
  #include <FastLED.h>
/////////////////////////////////////////////
  #if (USE_BLUETOOTH_SERIAL)
    #include "BluetoothSerial.h"
    BluetoothSerial SerialAOG;
  #else
    HardwareSerial SerialAOG(0);
  #endif
  
  CRGBArray<NUMPIXELS > leds;
  const byte centerpixel = (NUMPIXELS-1) /2;
  bool nightmode =0;
  //loop time variables in microseconds
  const unsigned int LOOP_TIME = 40;
  unsigned long lastTime = LOOP_TIME;
  unsigned long currentTime = LOOP_TIME;
  byte watchdogTimer = 20;
  byte serialResetTimer = 100; //if serial buffer is getting full, empty it
  float XeRoll = 0;

  //Program flow
  bool isDataFound = false, isSettingFound = false, isMachineFound=false, isAogSettingsFound = false; 
  bool MMAinitialized = false, isRelayActiveHigh = true;
  int header = 0, tempHeader = 0, temp, EEread = 0;
  byte relay = 0, relayHi = 0, uTurn = 0;
  byte remoteSwitch = 0, workSwitch = 0, steerSwitch = 1, switchByte = 0;
  int16_t distanceFromLine = 0, gpsSpeed = 0;
  
  //steering variables
  float steerAngleActual = 0;
  float steerAngleSetPoint = 0; //the desired angle from AgOpen
  int steeringPosition = 0; //from steering sensor
  float steerAngleError = 0; //setpoint - actual

void setup()
{ 
  //set up communication
  #if (USE_BLUETOOTH_SERIAL)
    SerialAOG.begin("M5stickC"); //Bluetooth Serial
  #else
    SerialAOG.begin(38400);  //USB Serial
  #endif
  M5.begin();
  M5.Lcd.setRotation(1);//1:PWR botton down 3:PWR button up
  FastLED.addLeds<WS2811,Neopixel_Pin,GRB>(leds, NUMPIXELS ).setCorrection(TypicalLEDStrip);
  M5.Lcd.fillScreen(TFT_BLACK);
}// End of Setup

void loop()
{
	// Loop triggers every 100 msec and sends back gyro heading, and roll, steer angle etc	 
	currentTime = millis();
  M5.update();
	if (currentTime - lastTime >= LOOP_TIME)
	{
		lastTime = currentTime;

   
    //If connection lost to AgOpenGPS, the watchdog will count up and turn off steering
    if (watchdogTimer++ > 250) watchdogTimer = 12;

    //clean out serial buffer to prevent buffer overflow
    if (serialResetTimer++ > 20)
    {
      while (SerialAOG.available() > 0) char t = SerialAOG.read();
      serialResetTimer = 0;
    }

  delay (5);

  
  if (SerialAOG.available() > 0 && !isDataFound && !isSettingFound && !isMachineFound && !isAogSettingsFound) 
  {
    int temp = SerialAOG.read();
    header = tempHeader << 8 | temp;               //high,low bytes to make int
    tempHeader = temp;                             //save for next time
    if (header == 32766) isDataFound = true;        //Do we have a match?
    else if (header == 32764) isSettingFound = true;     //Do we have a match?
    else if (header == 32762) isMachineFound = true;     //Do we have a match?    
    else if (header == 32763) isAogSettingsFound = true;     //Do we have a match?    
  }

  //Data Header has been found, so the next 8 bytes are the usbData
  if (SerialAOG.available() > 7 && isDataFound)
  {
    isDataFound = false;
    
    //was section control lo byte
    SerialAOG.read();
    gpsSpeed = SerialAOG.read() * 0.25;  //actual speed times 4, single byte

    //distance from the guidance line in mm
    distanceFromLine = (int16_t)(SerialAOG.read() << 8 | SerialAOG.read());   //high,low bytes

    //set point steer angle * 100 is sent
    steerAngleSetPoint = ((float)(SerialAOG.read() << 8 | SerialAOG.read()))*0.01; //high low bytes

    //auto Steer is off if 32020,Speed is too slow, motor pos or footswitch open
    if (distanceFromLine == 32020 | distanceFromLine == 32000 
         | steerSwitch == 1 )
      {
       watchdogTimer = 12; //turn off steering motor
       serialResetTimer = 0; //if serial buffer is getting full, empty it
      }
    else          //valid conditions to turn on autosteer
      {
       watchdogTimer = 0;  //reset watchdog
       serialResetTimer = 0; //if serial buffer is getting full, empty it
      }      
    SerialAOG.read();
    SerialAOG.read();

    //////////////////////////////////////////////////////////////////////////////////////

          //SerialAOG Send to agopenGPS **** you must send 10 numbers ****
      SerialAOG.print("127,253,");
      SerialAOG.print((int)(steerAngleActual * 100)); //The actual steering angle in degrees
      SerialAOG.print(",");
      SerialAOG.print((int)(steerAngleSetPoint * 100));   //the setpoint originally sent
      SerialAOG.print(",");
      SerialAOG.print("0,");                 //No IMU installed      
      SerialAOG.print("0,"); //roll in degrees * 16
      SerialAOG.println("0,0,0,0");//steering switch status ,pwmDisplay,0,0
      SerialAOG.flush();   // flush out buffer
  }
  //night mode Button
  if(M5.BtnA.pressedFor(100)){
    nightmode=!nightmode;
  }
  if (nightmode){
    FastLED.setBrightness(NIGHT_BRIGHTNESS);
  }else{
    FastLED.setBrightness(  255 );
  }
  lightbar(distanceFromLine);
  if(distanceFromLine != 32020){
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setCursor(0, 15);
    M5.Lcd.setTextSize(5);
    int16_t cmdistance=distanceFromLine*0.1;
    if(cmdistance > 0){
      M5.Lcd.setTextColor(RED);
      M5.Lcd.print("<");
    }else if(cmdistance < 0){
      M5.Lcd.setTextColor(GREEN);
      M5.Lcd.print(">");
      cmdistance*=-1;
    }else{
      M5.Lcd.setTextColor(WHITE);
    }
    M5.Lcd.setCursor(16, 15);
    M5.Lcd.printf("%4d",cmdistance);
  }

  } // end of main loop
}


void lightbar(float distanceFromLine ){
  int level = constrain (distanceFromLine / mmPerLightbarPixel , -centerpixel ,centerpixel);
  byte n = level + centerpixel;
    for (int i = 0 ;i < NUMPIXELS; i++){
      if (i == centerpixel && i == n){//Center
          leds[i] = CRGB::Yellow;
      }else if(level < 0 && i >= n && i < centerpixel && distanceFromLine != 32020){ //Right Bar
          leds[i] = CRGB::Green;
      }else if (level > 0 && i <= n && i > centerpixel && distanceFromLine != 32020){//Left Bar
          leds[i] = CRGB::Red;
      }else{
        leds[i] = CRGB::Black;//Clear
      }
    }
FastLED.show();
}
