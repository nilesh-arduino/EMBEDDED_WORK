/*
########################################################################################################################
#                                <<   SMART TWO TAP 20 LITER PUSH BUTTON BASED DISPENSER  >>                           #
# Developer:Nilesh Vyas                                                                                                #
#                                                                                                                      #
#                                                                                                                      #
########################################################################################################################
*/
//----------------------------------header File declaration------------------------------------------------//
#include<LiquidCrystal.h>
#include<SPI.h>
#include<EEPROM.h>
#include<DS3231.h>
#include<SimpleModbusSlave.h>
DS3231 rtc(20,21);
//--------------------------------------enum declaration---------------------------------------------------//

//enum{UNLOCK,LOCK};


//-----------------------------------PIN DECLARATION--------------------------------------------------------//
#define SETSWITCH_ADC_PIN               A0
#define DISPENSE_BUTTON_PIN             A14
#define SV1_PIN                         41
#define SV2_PIN                         39
#define SV3_PIN                         33
#define FLOWSENSOR_1_PIN                23
#define FLOWSENSOR_2_PIN                25
#define BOOSTER_PUMP_PIN                35
#define OZONATOR_PIN                    37
#define RST_PIN                         49
#define SS_PIN                          53
#define TX_ENABLE_PIN                   29

//RTC is on 20 21

//--------------------------------holding Register Map--------------------------------------------------//
enum 
{
  FLAG1,
  
  TSDD_U,//timstamp dispensing data higher 2 bytes
  TSDD_L,//time stamp dispensing data lower 2 bytes
  TVD_U,//total volume dispensed upper 2 bytes
  TVD_L,//total volume dispensed lower 2 bytes
  TRCH_U,//total recharge done upper 2 bytes
  TRCH_L,//total recharge done lower 2 bytes
  TCR_U,//total collection RFID upper 2 bytes
  TCR_L,//total collection RFID lower 2 bytes
  TCC_U,//total collection coin upper 2 bytes
  TCC_L,//total collection coin lower 2 bytes
  TCS_U,//total collection sale upper 2 bytes
  TCS_L,//total collection sale lower 2 bytes
  TCV1_U,//total count vol1 upper 2 bytes
  TCV1_L,//total count vol1 lower 2 bytes
  TCV2_U,//total count vol2 upper two bytes
  TCV2_L,//total count vol2 lower two bytes
  TCV3_U,//total count vol3 upper two bytes
  TCV3_L,//total count vol3 lower two bytes
  TCV4_U,//total count vol4 upper two bytes
  TCV4_L,//total count vol4 lower two bytes
  TCV5_U,//total count vol5 upper two bytes
  TCV5_L,//total count vol5 lower two bytes
  TCCV1_U,//total coin count value1 upper two bytes
  TCCV1_L,//total coin count value1 lower two bytes
  TCCV2_U,//total coin count value2 upper two bytes
  TCCV2_L,//total coin count value2 lower two bytes
  PHOW,//pH of water
  TLV,//Tank level
  TOW,//temperature of water
  LAT_U,//lattitude
  LAT_L,//
  LONG_U,//longitude
  LONG_L,  
  ALT,//altitude  
  //--------------registers for transaction log-------------------------------//
  FLAG2, //transaction log status  
  TSTL_U,//timestamp higher 2 bytes
  TSTL_L,//timestamp lower 2 bytes
  TN,//tap no
  TOR,//type of request
  TID_U,//transaction Id upper 2 bytes
  TID_L,//transaction Id lower 2 bytes
  QT,//quantity requested
  AM,//amount for the quantity
  DT,//dispense time in seconds
  CB_U,//card balance upper 2 bytes
  CB_L,//card balance lower 2 bytes
  TR_U,//total rechage upper 2 bytes
  TR_L,//total recharge lower 2 bytes
  ED_U,//expiry date upper 2 bytes
  ED_L,//expiry date upper two bytes  

  //-------------------registers for operator login/logout-----------------------//
  FLAG3,  //operator punch status
  OPID_U, //operatoe punch id higher 2 bytes
  OPID_L,//operator punch id lower 2 bytes
  PUNT_U,//punch time higher 2 bytes
  PUNT_L,//punch time lower 2 bytes

  //-----------------registers for configure parameters--------------------------//
  SFLG,//registers for configuration parameters
  RFLG,//reset flag to set parameters if changed remotrly
  TV1,//tariff volume1
  TV2,//tariff volume2
  TV3,//tariff volume3
  TV4,//tariff volume4
  TV5,//tariff volume5
  ACV1,//allowed coin value1
  ACV2,//allowed coin value2
  WV1,//water volume1
  WV2,//water volume2
  WV3,//water volume3
  WV4,//water volume4
  WV5,//water volume5
  VCV1,//volume coin value1
  VCV2,//volume coin value2
  PV1,//pulse coin value1
  PV2,//pulse coin value2
  PV3,//pulse coin value3
  PV4,//pulse coin value4
  PV5,//pulse coin value5
  PCV1,//pulse coin value1
  PCV2,//pulse coin value2
  CPV1,//coin pulse value1
  CPV2,//coin pulse value2
  SNUB_U,//serial no upper bound higher 2 bytes
  SNUB_L,//serial no upper bound lower 2 bytes
  SNLB_U,//serial no lower bound higher 2 bytes
  SNLB_L,//serial no lower bound lower 2 bytes
  OSN_U,//operator serial no higher 2 bytes
  OSN_L,//operator serial no lower 2 bytes
  SSN_U,//supervisor serial no higher 2 bytes
  SSN_L,//supervisor serial no lower 2 bytes
  ASN_U,//admin serial no higher 2 bytes
  ASN_L,//admin serial no lower 2 bytes
  MSC,//machine state control

  //-----------------registers for supervisor set/reset------------------------------//
  FLAG4,//flag for supervisor set/reset status
  TSTC_U,//time stamp higher 2 bytes
  TSTC_L,//time stamp lower 2 bytes
  TCID_U,//trip collection id higher 2 bytes
  TCID_L,//trip collection id lower 2 bytes
  TTRC,//trip total reset count
  TTVD,//trip total volume dispensed
  TTSC,//trip total smart card collection
  TTCC,//trip total coin collection
  TTCR,//trip total card recharge
  TTCS,//trip total collection sale
  TAAC,//trip actual amount collection
  //-------------------------Audit Trail---------------------------------------------//
  //Total transaction
  //Total failed transaction
  //total successful transaction
  //total transaction using card
  //total transaction using coin
  //total no of trip reset
  //total no of configuration changes
  //Total count operator login
  TIME_U,
  TIME_L, 
  HOLDING_REGS_SIZE 
};
unsigned int holdingRegs[HOLDING_REGS_SIZE];  
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

LiquidCrystal lcd1(13,12,11,10,9,8);
char VER_INFO[8]="12345678";
unsigned long currentMillis=0;
bool BYPASS_OPERATOR_LOGIN;
bool BYPASS_RTC;
bool                  TAP1_DISABLE;
bool                  TAP2_DISABLE;


byte                  SLAVE_ID;
byte                  ERROR_CODE;

int                   OZONATION_COUNT;
//calibration constants
float                 PUSHBUTTON_CALIBRATION_VOLTAGE[7]={1.58, 2.49, 3.41, 3.80, 4.09,4.32, 4.98}; 
float                 SWITCH_CALIBRATION_VOLTAGE[16]={1.87, 2.03, 2.25, 2.49, 3.00, 3.44, 4.11, 4.99};  
float                 PUSHBUTTON_OFFSET=0.15;
float                 SWITCH_OFFSET=0.08;

unsigned int          DISPENSE_LOOP_RATE=50; 
unsigned int          TIMEOUT_CONSTANT;
unsigned int          DISPENSE_UPDATE_DISPLAY;
unsigned int          WATER_VOL1;
unsigned int          WATER_VOL2;
unsigned int          PULSE_VOL1;
unsigned int          PULSE_VOL2;
unsigned int          OFFSET_LEVEL;
unsigned int          THRESOLD_LEVEL;
unsigned int          BAUD_RATE;
unsigned int          ALTITUDE;
unsigned int          FLOW_CONSTANT_1;
unsigned int          FLOW_CONSTANT_2;
unsigned long         TOTAL_VOLUME_DISPENSED;    
unsigned long         CURRENT_TIME_STAMP;
unsigned long         LATTITUDE;
unsigned long         LONGITUDE;

/* LCD VARIABLE*/
int screenWidth = 16;
int screenLength = 2;
int stringStart,stringStop=0;
int scrollCursor = screenWidth;
String lcdInfo = "Press button 2 to 3 sec for start dispensing      ";

unsigned long TOTAL_RECHARGE;
unsigned long TOTAL_COLL_CARD;
unsigned long TOTAL_COLL_COIN;
unsigned long TOTAL_COLL_SALE;
unsigned long TOTAL_COUNT_VOL1;
unsigned long TOTAL_COUNT_VOL2;
unsigned long TOTAL_COUNT_VOL3;
unsigned long TOTAL_COUNT_VOL4;
unsigned long TOTAL_COUNT_VOL5;
unsigned long TOTAL_COIN_COUNT_VALUE1;
unsigned long TOTAL_COIN_COUNT_VALUE2;
unsigned int PH_WATER;
int tempOfWater;
int TANK_LEVEL;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
/**************************************************************************************************/

  class settingParameter 
   {  
      //-------------------------------------configuration parameter---------------------------------------------------------------//    
      unsigned int panelNo;
      //-------------------------------------Dispenser configuration----------------------------------------------------------------//    
    
      unsigned int volume1,volume2,volume3,volume4,volume5,volume6,volume7;//maximum allowed value is 60000 for each volume
      unsigned int pulseVolume1,pulseVolume2,pulseVolume3,pulseVolume4,pulseVolume5,pulseVolume6,pulseVolume7;     
      unsigned int offsetLevel,thresoldLevel;  
      //---------------------------------Remote Control Parameter-----------------------------------------------------------------//
      bool machineStateControl;
    
      unsigned long totalVolumeDispensed,totalCollectionSale;
     
     
      //--------------------------Flag variable-------------------------------------------------------------------------------------//
      bool byPassOperatorLogin,bypassRtc;
      unsigned int baudRate;
      byte slaveId;
      unsigned long longitude,lattitude;
      unsigned int alltitude;
    public:
        settingParameter()
        {  
                     //--------Initialize Dispenser Parameters-----// 
                      panelNo              =12345;                    
                      volume1              = 20000;
                      volume2              = 20000;
                      pulseVolume1         = 1000;
                      pulseVolume2         = 1000;
                      offsetLevel          = 0;
                      thresoldLevel        = 150;         
                     //---Initialize Remote Control Parameters------//
                      machineStateControl  = LOW;
                      totalVolumeDispensed = 0;    
                                  
                   

                     //----------------------Bypass Flag-------------------------//
                      byPassOperatorLogin       = LOW; 
                      bypassRtc                 = HIGH;
                      baudRate                  = 4800;
                      slaveId                   = 2;
                      lattitude                 = 28632986;
                      longitude                 = 77219374;
                      alltitude                 = 216;
             } 
     void getData();
     void setData();
 };
void settingParameter::getData()
 {
               
        WATER_VOL1                =   volume1;
        WATER_VOL2                =   volume2;
        FLOW_CONSTANT_1           =   pulseVolume1;
        FLOW_CONSTANT_2           =   pulseVolume2;
        TOTAL_VOLUME_DISPENSED    =   totalVolumeDispensed;
        OFFSET_LEVEL              =   offsetLevel;
        THRESOLD_LEVEL            =   thresoldLevel;  
        SLAVE_ID                  =   slaveId;
        BAUD_RATE                 =   baudRate;
        LATTITUDE                 =   lattitude;
        LONGITUDE                 =   longitude;
        ALTITUDE                  =   alltitude;
 }
void settingParameter::setData()
 {    
      
       volume1              =   WATER_VOL1;
       volume2              =   WATER_VOL2;
       pulseVolume1         =   FLOW_CONSTANT_1;
       pulseVolume2         =   FLOW_CONSTANT_2;
       totalVolumeDispensed =   TOTAL_VOLUME_DISPENSED;
       offsetLevel          =   OFFSET_LEVEL;
       thresoldLevel        =   THRESOLD_LEVEL;
       slaveId              =   SLAVE_ID;
       baudRate             =   BAUD_RATE;
       lattitude            =   LATTITUDE;
       longitude            =   LONGITUDE;   
       alltitude            =   ALTITUDE; 
 }
           
/************************************************************************************************/
class switchPress
{    
      byte pin;
      int readValue;
      unsigned long lastDebounceTime,DebounceDelay;
      byte lastButtonState;
    public:   
      bool s1, s2, s3;
      float voltage;    
      switchPress(byte analogPin)
      {
          pin = analogPin;
          pinMode(pin, INPUT);
          s1 = s2 = s3 = 0;
          voltage=0;
          readValue=0;
          lastDebounceTime=0;
          DebounceDelay=50;
          lastButtonState=0;
      }
      void setState(bool a, bool b, bool c);
      void switchDetect(float *, float);
};

void switchPress::setState(bool a, bool b, bool c)
{
    //Serial.print("Button=");
    s1 = a;
    //Serial.print(s1);
    s2 = b;
    //Serial.print(s2);
    s3 = c;
    //Serial.println(s3);  
}
void switchPress::switchDetect(float *a, float offset)
{  
    setState(0,0,0);
    readValue= analogRead(pin);
    voltage=readValue*5.0/1023.0; 
    // Serial.print("voltage : ");
    // Serial.println(voltage); 
    byte buttonState=8; 
    for (int i = 0; i < 8; i++)
    {
        if (voltage > (a[i] - offset) && voltage < (a[i] + offset))
        {
            buttonState=i;
            break;
        }
        else
        {
            buttonState=8;
        }
    }   
    if(buttonState!=lastButtonState)
    {
        lastDebounceTime=currentMillis;
    } 
    if(currentMillis-lastDebounceTime>DebounceDelay)
    {
        switch (buttonState)
        {
            case 0:
                setState(1, 1, 1);
                break;
            case 1:
                setState(1, 1, 0);
                break;
            case 2:
                setState(1, 0, 1);
                break;
            case 3:
                setState(1, 0, 0);
                break;
            case 4:
                setState(0, 1, 1);
                break;
            case 5:
                setState(0, 1, 0);
                break;
            case 6:
                setState(0, 0, 1);
                break;             
            default:
                setState(0, 0, 0);
                break;
        }
   }
   lastButtonState=buttonState; 
}

/***********************************************************************************/

//------------------------------Code to detect Five Buttons for Dispensing-----------------------------------------//
class ButtonDetect
{
    byte pin;
    int readValue;
    unsigned long lastDebounceTime,debounceDelay;
    byte lastButtonState;
  public:
      bool s1,s2,s3,s4,s5,s6;
      float voltage;
      ButtonDetect(byte analogPin)
      {
          pin = analogPin;
          pinMode(pin,INPUT);
          s1=s2=s3=s4=s5=s6=0;
          voltage=0;
          readValue=0;
          lastDebounceTime=0;
          debounceDelay=50;
          lastButtonState=0;
      }
      void setState(bool a,bool b,bool c,bool d, bool e,bool f);
      void switchDetect(float *, float);
 };
void ButtonDetect::setState(bool a, bool b, bool c, bool d,bool e,bool f)
{
    s1 = a;
    s2 = b;
    s3 = c;
    s4 = d;
    s5 = e;
    s6 = f; 
}
void ButtonDetect::switchDetect(float *a, float offset)
{  
     voltage=0;
     for(byte i=0;i<50;i++)
    {
        readValue= analogRead(pin);
        voltage+=readValue*5.0/1023.0;   
    }
    voltage=voltage/50.0;
    //Serial.print("Voltage");
    //Serial.println(voltage);
    byte buttonState=7;
    for (int i = 0; i < 7; i++)
    {
        if (voltage > (a[i] - offset) && voltage < (a[i] + offset))
        {
            buttonState=i; 
            break;
        }
        else
        {
            buttonState=7;
        }
    }
    if(buttonState!=lastButtonState)
    {
        lastDebounceTime=currentMillis;
    }
    if(currentMillis-lastDebounceTime>debounceDelay)
    {
        switch (buttonState)
        {
            case 0:
              setState(0, 0, 0, 0, 0, 1);//switch s6 is pressed
              break;
            case 1:
              setState(0, 0, 0, 0, 1, 0);//switch s5 is pressed
              break;
            case 2:
              setState(0, 0, 0, 1, 0, 0);//switch s4 is pressed
              break;
            case 3:
              setState(0, 0, 1, 0, 0, 0);//switch s3 is pressed
              break;
            case 4:
              setState(0, 1, 0, 0, 0, 0);//switch s2 is pressed
              break;
            case 5:
              setState(1, 0, 0, 0, 0, 0);//switch s1 is pressed
              break; 
            default:
              setState(0, 0, 0, 0, 0, 0);//no switch is pressed
              break;  
        }
   }
   lastButtonState=buttonState;
}  
/***********************************************************************************/

class LcdRead:public switchPress
{
    byte init_curPos,curPos;
    byte final_curPos ;   
    char arr1[16];
    unsigned long buttondetectMillis;  
   public:
      bool shiftButton, selectButton, enterButton, resetButton;
      bool status_of_setting;  
        
      LcdRead(byte analogPin):switchPress(analogPin)
      {      
          pinMode(analogPin,INPUT);
          status_of_setting = LOW;
          curPos=0;
          init_curPos = 0;
          final_curPos=0;      
          shiftButton=selectButton=enterButton=resetButton=LOW;
          buttondetectMillis=0;
          for (byte i = 0; i < 16; i++)
          {
              arr1[i] = '0';         
          }
      }
      unsigned long readValue(char variable[16],unsigned long num);
      unsigned int readValue(char variable[16], unsigned int num);
      byte readValue(char variable[16], byte num);
      byte calculateDigits(unsigned long num, char *arr);
      void setDigits(byte digit, byte len, char *arr);
      void setDisplay(String parameter, byte a, byte b, char *arr);
      byte shiftCursor(byte curPos,byte initPos, byte finalPos);
      void incrCursor(byte curPos, char *arr);
      unsigned long charTodec(char arr[16],byte a,byte b);
      void button();
};
unsigned long LcdRead:: readValue(char parameter[16], unsigned long num)
{
      bool lastshiftButton=HIGH,lastselectButton=HIGH,lastenterButton=HIGH;
      unsigned long timeoutMillis=0;
      init_curPos=4;
      final_curPos=12;  
      unsigned long number = num;    
      byte digits = calculateDigits(number, arr1);  
      setDigits(8, digits, arr1);  
      setDisplay(parameter, init_curPos, final_curPos, arr1);  
      timeoutMillis=currentMillis;  
      //button();
      while (1)
      {    
           button(); 
       
          if (shiftButton == HIGH && lastshiftButton==LOW) //This block of code will shift the cursor position
          {     
              curPos = shiftCursor(curPos,init_curPos, final_curPos);
              timeoutMillis=currentMillis;      
          }
          if (selectButton == HIGH && lastselectButton==LOW) //This block of code will increment the value at cursor position
          {            
              incrCursor(curPos, arr1)  ;  
              timeoutMillis=currentMillis;      
          }      
          if(enterButton==HIGH && lastenterButton==LOW)
          {
               num = charTodec(arr1,4,12);
               status_of_setting=HIGH;
               lcd1.noCursor();         
               return num; 
          }
          else if (currentMillis-timeoutMillis>=20000)
          {
                lcd1.clear();
                lcd1.setCursor(0, 0);
                lcd1.print(F("TimeOut Error!"));                 
                status_of_setting = LOW;
                lcd1.noCursor();
                return num;
          }        
        lastshiftButton=shiftButton;
        lastselectButton=selectButton; 
        lastenterButton=enterButton;  
    }
}
unsigned int LcdRead:: readValue(char parameter[16], unsigned int num)
{
      bool lastshiftButton=HIGH,lastselectButton=HIGH,lastenterButton=HIGH;
      unsigned long timeoutMillis=0;
      init_curPos=5;
      final_curPos=10;  
      unsigned int number = num;    
      byte digits = calculateDigits(number, arr1);  
      setDigits(5, digits, arr1);  
      setDisplay(parameter, init_curPos, final_curPos, arr1);  
      timeoutMillis=currentMillis;  
      //button();
      while (1)
      {    
          button(); 
       
          if (shiftButton == HIGH && lastshiftButton==LOW) //This block of code will shift the cursor position
          {     
                curPos = shiftCursor(curPos,init_curPos, final_curPos);
                timeoutMillis=currentMillis;      
          }
          if (selectButton == HIGH && lastselectButton==LOW) //This block of code will increment the value at cursor position
          {            
                incrCursor(curPos, arr1)  ;  
                timeoutMillis=currentMillis;      
          }      
          if(enterButton==HIGH && lastenterButton==LOW)
          {
               num = charTodec(arr1,5,10);
               status_of_setting=HIGH;
               lcd1.noCursor();         
               return num; 
          }
          else if (currentMillis-timeoutMillis>=20000)
          {
                lcd1.clear();
                lcd1.setCursor(0, 0);
                lcd1.print(F("TimeOut Error!"));                 
                status_of_setting = LOW;
                lcd1.noCursor();
                return num;
          }        
          lastshiftButton=shiftButton;
          lastselectButton=selectButton; 
          lastenterButton=enterButton;  
    }
}

byte LcdRead::readValue(char parameter[16],byte num)
{
      bool lastshiftButton=HIGH,lastselectButton=HIGH,lastenterButton=HIGH;
      unsigned long timeoutMillis=0,updateMillis=0;
      init_curPos=6;
      final_curPos=9;
      byte number = num;    
      byte digits = calculateDigits(number, arr1);  
      setDigits(3, digits, arr1);  
      setDisplay(parameter, init_curPos,final_curPos, arr1); 
      switchDetect(SWITCH_CALIBRATION_VOLTAGE,SWITCH_OFFSET);  
      timeoutMillis=currentMillis; 
      //button();
      while (1)
      {    
           button();           
          if (shiftButton == HIGH && lastshiftButton==LOW) //This block of code will shift the cursor position
          {      
              curPos = shiftCursor(curPos, init_curPos,final_curPos);
              timeoutMillis=currentMillis;
              //status_of_setting=HIGH;
          }
          if (selectButton == HIGH && lastselectButton==LOW) //This block of code will increment the value at cursor position
          {            
              incrCursor(curPos, arr1)  ;
              timeoutMillis=currentMillis;  
              //status_of_setting=HIGH;         
          }
          if(enterButton==HIGH && lastenterButton==LOW)
          {
               num = charTodec(arr1,6,9);  
               status_of_setting = HIGH;
               lcd1.noCursor();
               return num;  
          }
          else if (currentMillis-timeoutMillis>=20000)
          {
                lcd1.clear();
                lcd1.setCursor(0, 0);
                lcd1.print(F("TimeOut Error!"));                
                status_of_setting = LOW;
                lcd1.noCursor();
                return num;
          }   
          lastshiftButton=shiftButton;
          lastselectButton=selectButton;  
          lastenterButton=enterButton;  
      } 
}

byte LcdRead::calculateDigits(unsigned long number, char *arr)
{
      byte k = 0;
      while (number != 0)
      {
          arr[k] = number % 10 + '0';
          number = number / 10;
          k++;
      }
      return k;
}
void LcdRead::setDigits(byte digit, byte len, char *arr)
{
      byte l = len;
      char a[16];
      for (byte i = 0; i < 16; i++)
      a[i] = arr[i];
      for (byte i = 0; i < digit; i++)
      {
           if (i < (digit - len))
          {
              arr[i] = '0';
          }
          else
          {
              arr[i] = a[l - 1];
              l--;
          }
      }
}

void LcdRead::setDisplay(String parameter, byte a, byte b, char *arr)
{
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print(parameter);
      init_curPos = a;
      final_curPos = b;
      char buff[16];
      for (byte i = 0; i < 16; i++)
      {
          buff[i] = arr[i];
          arr[i] = '0';
      }
      byte i = 0;
      for (byte j = init_curPos; j < final_curPos; j++)
      {
          arr[j] = buff[i];
          i++;
      }
      for (byte j = init_curPos; j < final_curPos; j++)
      {
          lcd1.setCursor(j, 1);
          lcd1.print(arr[j]);
      }
      lcd1.setCursor(init_curPos, 1);
      curPos=init_curPos;
      lcd1.cursor();
}
byte LcdRead::shiftCursor(byte curPos,byte initPos, byte finalPos)
{    
      curPos = curPos + 1;
      if (curPos == finalPos)
      {
           curPos = initPos;
      }
      lcd1.setCursor(curPos, 1);    
      return curPos;
}
void LcdRead::incrCursor(byte curPos, char *arr)
{  
      arr[curPos] = arr[curPos] + 1;
      if (arr[curPos] > '9')
      arr[curPos] = '0';
      lcd1.setCursor(curPos, 1);
      lcd1.print(arr[curPos]);
      lcd1.setCursor(curPos, 1);  
}
unsigned long LcdRead::charTodec(char *arr,byte iniPos,byte finalPos)
{  
      byte b;
      unsigned long c = 0;
      for (byte i = iniPos; i < finalPos ; i++)
      {
           b = arr[i]- 48;
           c = c * 10 + b;
      }  
      return c;
}
void LcdRead::button()
{ 
      switchDetect(SWITCH_CALIBRATION_VOLTAGE,SWITCH_OFFSET);
      //Serial.print("setting Button:");
      shiftButton = s1;
      //Serial.print(s1);  
      selectButton = s2;
      //Serial.print(s2);
      enterButton = s3;
      //Serial.println(s3);   
}
//--------------------------------------------------------------------------------//
// This class dispense the volume of water required                               //
//--------------------------------------------------------------------------------//

/*******************************************************************************************************************/
//--------------------------------------------Code for water dsipensing--------------------------------------------//
class WATER_ATM
{
    byte SV_PIN, FLOW_PIN, OZ_PIN, BOOST_PIN;
    bool flowPulse, lastflowPulse, fallingEdge;
    unsigned long pulseCount, ozMillis, dispenseMillis, flowMillis;
    bool yes, no;
    bool svState, ozState, boostState, on, off, enable, disable;
    unsigned int ozonationTime, updateDisplay,dispenseTimeout;
    float flowscaleFactor;
  public:
    unsigned long pulseRequired,Dispensed_pulseCount,Dispensed_Time;
    float Dispensed_Volume,flowVolume;
    bool IsDispensing, scan,dispensingComplete,Dispensing, dispense,lastBreakState,BREAK_DISPENSE;
   
    WATER_ATM(byte a, byte b, byte c, byte d)
    {
        SV_PIN = a;
        FLOW_PIN = b;
        OZ_PIN = c;
        BOOST_PIN = d;      
        pinMode(SV_PIN, OUTPUT);
        digitalWrite(SV_PIN,off);
        pinMode(OZ_PIN, OUTPUT);
        digitalWrite(OZ_PIN,off);
        pinMode(BOOST_PIN, OUTPUT);
        digitalWrite(BOOST_PIN,off);  
        pinMode(FLOW_PIN, INPUT_PULLUP);
      
        on = yes = enable = HIGH;
        off = no = disable = LOW;
        scan = enable;
        pulseCount = 0;
        pulseRequired = 0;
        svState = off; ozState = off; boostState = off;
        ozMillis = dispenseMillis=0;
        flowscaleFactor = 0;
        flowVolume = 0;
        Dispensed_Volume=0;
        dispenseTimeout=30000;//The dispensing process will terminate if no pulse is detected for 10sec      
        Dispensing=no;
    }
    
    void startSV()//function to start solenoid valve
    {
        digitalWrite(SV_PIN, on);
        svState = on;
        //Serial.println("SV started");
    }
    void stopSV()                     //function to stop solenoid valve
    {
        digitalWrite(SV_PIN, off);
        svState = off;
        //Serial.println("SV stopped");
    }
    void startOzonator()             //function to start ozonator
    {
        digitalWrite(OZ_PIN, on);
        ozState = on;
        //Serial.println("Ozonator Started");
    }
    void stopOzonator()              //function to stop ozonator
    {
        digitalWrite(OZ_PIN, off);
        ozState = off;
        //Serial.println("Ozonator Stopped");
    }
    void startBooster()              //function to start booster pump
    {
        digitalWrite(BOOST_PIN, on);
        boostState = on;
        //Serial.println("Booster Started");
    }
    void stopBooster()               //function to stop booster
    {
        digitalWrite(BOOST_PIN, off);
        boostState = off;
        //Serial.println("Booster Stopped");
    }   
    
    void startDispensing(unsigned int ozTime,float amountOfwater, unsigned int flowConstant)
    {
        scan = disable;
        ozonationTime = ozTime;
        IsDispensing = true; 
        dispensingComplete=false; 
        //flow constant is in litre per min and amount of water is in Litre     
        pulseRequired = flowConstant;//(flowConstant * amountOfwater*60.0)/10.0;
        //Serial.print("Pulse required");
        //Serial.println(pulseRequired);
        flowscaleFactor = (amountOfwater*1000) / pulseRequired;      
    }   
    void Update()
    {      
     
      if (IsDispensing == yes && dispense == no)
      {
          if (ozState != on)
          {     
                      
              lcd1.clear();
              lcd1.setCursor(0,0);
              lcd1.print("Ozonation Start");
              startOzonator();
              Serial.println(F("Ozonation Started"));
              ozMillis = currentMillis;
              
          }
          
          else if (currentMillis - ozMillis >= ozonationTime)//The value of ozonation time can be configured
          {
              ozMillis = currentMillis;
              OZONATION_COUNT=0;
              stopOzonator();
              dispense = yes;
              Dispensing=yes;
              Serial.print(F("Dispensing started"));
          }
          if(ozState==on)
          {
            OZONATION_COUNT = (currentMillis - ozMillis)/1000;
            lcd1.setCursor(7,1);
            lcd1.print(OZONATION_COUNT);
            //OZONATION_COUNT--;
          }

      }
      if (IsDispensing == yes && dispense == yes)
      {
          if (svState != on && boostState != on)
          {          
              startBooster();
              startSV();
              dispenseMillis = currentMillis;
          }
          else if (pulseCount >= pulseRequired || BREAK_DISPENSE == HIGH && lastBreakState == LOW)
          {
              lastBreakState = BREAK_DISPENSE;
              BREAK_DISPENSE = LOW;
              IsDispensing = no;
              Serial.print("BREAK_DISPENSE = ");
              Serial.println(BREAK_DISPENSE);
              Serial.println("STOP DISPENSING");
         
              stopSV();
             // stopBooster();
              Dispensed_Volume=flowVolume;
              Dispensed_Time=(currentMillis-ozMillis);
              Dispensed_pulseCount=pulseCount;
              dispense = no;
              scan = enable;          
              flowVolume = 0;
              pulseCount = 0;
              dispensingComplete=yes;
              Dispensing=no;
              Serial.println(F("Dispensing Complete"));          
          }
          else
          {
              flowPulse = digitalRead(FLOW_PIN);
              if (flowPulse == LOW && lastflowPulse == HIGH)
              {
                  fallingEdge = 1;
              }
              if (fallingEdge == 1)
              {
                  if (flowPulse == HIGH && lastflowPulse == LOW)
                  {
                      pulseCount += 1;
                      flowVolume = flowscaleFactor * pulseCount;
                      fallingEdge = 0;
                  }
              }
              if(lastflowPulse!=flowPulse)
              {
                    dispenseMillis=currentMillis;
              }
              lastflowPulse = flowPulse;
              //if no water is comming for around 10min then dispense timeout          
          }

          if (currentMillis- dispenseMillis >= dispenseTimeout)//The value of dispenseTimeout can be configured.
          {
              dispenseMillis = currentMillis;         
              stopSV();
              stopBooster();
              pulseCount = 0;
              IsDispensing = no;
              dispense = no;
              scan = enable;
              Dispensing=no;
              dispensingComplete=no;
              TOTAL_VOLUME_DISPENSED += flowVolume;
              flowVolume = 0;
              lcd1.clear();
              lcd1.print("Dispense Timeout");
              delay(2000);
              lcd1.clear();
              ERROR_CODE = 15;
              Serial.print("Dispense Timeout : ");
              Serial.println(TOTAL_VOLUME_DISPENSED);
          }
      }
    }
};



settingParameter parameters;
Time  t;


WATER_ATM Tap1(SV2_PIN, FLOWSENSOR_1_PIN, OZONATOR_PIN, BOOSTER_PUMP_PIN);
WATER_ATM Tap2(SV1_PIN, FLOWSENSOR_2_PIN, OZONATOR_PIN, BOOSTER_PUMP_PIN);
ButtonDetect pushButton(DISPENSE_BUTTON_PIN);
LcdRead setting(SETSWITCH_ADC_PIN);

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

unsigned long samplingMillis;
bool Button1,Button2,Button3,Button4,Button5,Button6;
byte IsSettingsChanged=0;

unsigned long savedataMillis,timecheckMillis,settingCheckMillis,dispenseloopMillis,dispensedisplayMillis,updateDataModbusMillis;
bool par_changed,lockDisplay;
bool lastState_IsDispensing_tap1,lastState_IsDispensing_tap2;
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void setup()
{
    OCR0A = 0xAF;  
    lcd1.begin(16,2);
    Serial.begin(9600);  
    SPI.begin();
    rtc.begin();  
    VersionInformation();
    delay(2000);
    displayTimestamp();
    delay(2000);
    //message.initializeModem(9600); 
    dispenseDisplay(0);  
    pinMode(SV1_PIN,OUTPUT);
    if(0)
    {
       byte checkByte=0;
       EEPROM.write(0,checkByte);
    }
    Serial.print("reading eeprom");
    checkEEPROM();
    //readParameter();
    //parameters.getData();  
    TAP1_DISABLE=LOW;
    TAP2_DISABLE=LOW;
    //--------------Define modbus communication configuration setting here----------------------//
    //pinMode(22,OUTPUT);
    //modbus_configure(&Serial1, BAUD_RATE, SERIAL_8N2, SLAVE_ID,29 , HOLDING_REGS_SIZE, holdingRegs);
    //modbus_update_comms(BAUD_RATE, SERIAL_8N2, SLAVE_ID);
    // updateConfigurationDataOnModbus();
    modbus_configure(&Serial1, BAUD_RATE, SERIAL_8N2, SLAVE_ID,29 , HOLDING_REGS_SIZE, holdingRegs);
    modbus_update_comms(BAUD_RATE, SERIAL_8N2, SLAVE_ID);
    updateDispensingDataOnModbus();  
    //digitalWrite(20,HIGH);
    //checkRTC();
    //------------------------------------------------------------------------------------------//
    TIMSK0 |= _BV(OCIE0A);//enable interrupt  
  
}
//-----------------------------------------------------------------------------------//
//   This is the handler for TIMER1 and it contains code for GSM communication       //
//-----------------------------------------------------------------------------------//
SIGNAL(TIMER0_COMPA_vect) 
{
      currentMillis=millis();  
      if(currentMillis-samplingMillis>=20)
      {
            samplingMillis=currentMillis; 
            
            //call tapUpdate here
            if((Tap1.IsDispensing==HIGH)&&(Tap2.IsDispensing==HIGH))
            {
               lastState_IsDispensing_tap1=Tap1.IsDispensing;
               lastState_IsDispensing_tap2=Tap2.IsDispensing;
        
                pushbuttonDetect();
                if(Button3==LOW)
                {
                      Tap1.lastBreakState = Button3;
                      Tap1.BREAK_DISPENSE = LOW;
                }
                else if(Button3 == HIGH)
                {
                      Tap1.BREAK_DISPENSE = Button3;
                     // Tap1.stopBooster();
                }
                if(Button4==LOW)
                {
                      Tap2.lastBreakState = Button4;
                      Tap2.BREAK_DISPENSE = LOW;
                }
                else if(Button4 == HIGH)
                {
                      Tap2.BREAK_DISPENSE = Button4;
                      //Tap2.stopBooster();
                }
                Tap1.Update();
                Tap2.Update();
            }
            else if(Tap1.IsDispensing==HIGH)
            {
               
                lastState_IsDispensing_tap1=Tap1.IsDispensing;
                
                 pushbuttonDetect();
                if(Button3==LOW)
                {
                      Tap1.lastBreakState = Button3;
                      Tap1.BREAK_DISPENSE = LOW;
                }
                else if(Button3 == HIGH)
                {
                      Tap1.BREAK_DISPENSE = HIGH;
                }
                Tap1.Update();
             
            }

            else if(Tap2.IsDispensing==HIGH)
            {
               
                lastState_IsDispensing_tap2=Tap2.IsDispensing;
                
                 pushbuttonDetect();
                if(Button4==LOW)
                {
                      Tap2.lastBreakState = Button4;
                      Tap2.BREAK_DISPENSE = LOW;
                }
                else if(Button4 == HIGH)
                {
                      Tap2.BREAK_DISPENSE = Button4;
                }
                Tap2.Update();
             
            }
            
         
      }  
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//*************************************************************************This is the main loop****************************************************************************************//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void loop()
{
       if(Tap1.IsDispensing!=HIGH && Tap2.IsDispensing!=HIGH)
      {
          if(currentMillis-timecheckMillis>=1000)
          {
                timecheckMillis=currentMillis;
                modbus_update();
          }
          if((currentMillis-settingCheckMillis)>=3000)
          {
              settingCheckMillis=currentMillis;
              controllerSettings();
          }
         if((currentMillis-updateDataModbusMillis)>=5000)
          {
            updateDataModbusMillis=currentMillis;
            //rtcTime();
            if(BYPASS_RTC==HIGH)
            {
                CURRENT_TIME_STAMP = getUnix_Time();
            }
            updateDispensingDataOnModbus();
            Serial.println("DATA UPDATE");
          }
          dispenseDisplay(2);
          
      }
  
      if(lockDisplay!=LOW)
      {
            dispenseDisplay(0);
            lockDisplay=LOW;
      }

      if(currentMillis-dispenseloopMillis>=DISPENSE_LOOP_RATE)
      {
            dispenseloopMillis=currentMillis;
            if(Tap2.scan==HIGH||Tap1.scan==HIGH)
            {   
                if(ERROR_CODE>0)
                {
                    ERROR_CODE=0;
                    delay(1000);
                    lockDisplay=HIGH;
                }
                if(Tap1.IsDispensing!=HIGH || Tap2.IsDispensing!= HIGH)
                {
                    pushbuttonDetect();
                    if(Button3==LOW)
                    {
                        Tap1.lastBreakState = Button3;
                        Tap1.BREAK_DISPENSE = LOW;
                    }
                    else if(Button3 == HIGH)
                    {
                        Tap1.BREAK_DISPENSE = Button3;
                    }
                    if(Button4==LOW)
                    {
                      Tap2.lastBreakState = Button4;
                      Tap2.BREAK_DISPENSE = LOW;
                    }
                    
                    else if(Button4 == HIGH)
                    {
                        Tap2.BREAK_DISPENSE = Button4;
                        Serial.println("Tap2 Detect");
                    }
                    if(Tap1.BREAK_DISPENSE == HIGH && Tap1.lastBreakState == LOW)
                    {
                        Tap1.lastBreakState = Tap1.BREAK_DISPENSE;
                        Serial.print("Tap1.BREAK_DISPENSE");
                        Serial.println(Tap1.BREAK_DISPENSE);
                        
                        Tap1.BREAK_DISPENSE = LOW;
                        Tap1.startDispensing(2000,get_waterVolume(1),getpulseRequired(1));
                       // Tap1.startDispensing(2000,300,30);
                    }
                    if(Tap2.BREAK_DISPENSE == HIGH && Tap2.lastBreakState == LOW)
                    {
                        Serial.println("TAP2");
                        Tap2.lastBreakState = Tap2.BREAK_DISPENSE;
                        Tap2.BREAK_DISPENSE = LOW;
                        Tap2.startDispensing(2000,get_waterVolume(2),getpulseRequired(2));
                       
                    }
                    //delay(1000);
                }
                
          /*   else if((Tap1.dispensingTimeoutMillis>=60000 )== HIGH && TAP2_DISABLE==HIGH)
            {
                   
                    Tap1.dispensingTimeoutMillis=0;
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("*** NO WATER ***");
                    delay(2000);
                    TAP2_DISABLE=LOW;
                    lockDisplay=HIGH;
             }*/
          
            
            
        }
    }
    if(Tap1.dispense==HIGH || Tap2.dispense==HIGH)
    { 
        if(currentMillis-dispensedisplayMillis>=500)
        {
            dispensedisplayMillis=currentMillis;
            lcd1.clear();
            Serial.print("Tap1 : ");
            Serial.println(Tap1.dispense);
            Serial.print("Tap2 : ");
            Serial.println(Tap2.dispense);
            if(Tap1.dispense==HIGH && Tap2.dispense==HIGH)
            {
                  lcd1.setCursor(0,0);
                  lcd1.print("Tap1: ");
                  lcd1.print(Tap1.flowVolume);
                  lcd1.setCursor(0,1);
                  lcd1.print("Tap2: ");
                  lcd1.print(Tap2.flowVolume);
            }
            else if(Tap2.dispense==HIGH)
            {
                  lcd1.setCursor(0,0);
                  lcd1.print("Tap2:Dispensing");
                  lcd1.setCursor(0,1);
                  lcd1.print("Vol: ");
                  lcd1.print(Tap2.flowVolume);  
            }
            else if(Tap1.dispense==HIGH)
            {
                lcd1.setCursor(0,0);
                lcd1.print("Tap1:Dispensing");
                lcd1.setCursor(0,1);
                lcd1.print("Vol: ");
                lcd1.print(Tap1.flowVolume);  
         
            }
     
        }
    }
    if((Tap1.IsDispensing!=HIGH) && (Tap2.IsDispensing!=HIGH))
    {
      Tap1.stopBooster();
      Tap2.stopBooster();
    }

    if((Tap1.IsDispensing==LOW && lastState_IsDispensing_tap1==HIGH) || (Tap2.IsDispensing==LOW && lastState_IsDispensing_tap2==HIGH)  )
    {
          if(Tap1.IsDispensing==LOW && lastState_IsDispensing_tap1==HIGH)
          {
                lastState_IsDispensing_tap1=Tap1.IsDispensing;
                if(Tap1.dispensingComplete==HIGH)
                {
                    
                    lcd1.clear();
                    lcd1.setCursor(0,0);
                    lcd1.print("VOLUME DISPENSED");
                    lcd1.setCursor(2,1);
                    TOTAL_VOLUME_DISPENSED += Tap1. Dispensed_Volume;
                    lcd1.print(Tap1.Dispensed_Volume);
                    lcd1.setCursor(13,1);
                    lcd1.print("ML");
                    Serial.print("TOTAL VOLUME DISPENSED");
                    Serial.println(TOTAL_VOLUME_DISPENSED);
                    delay(500);
                    ERROR_CODE=20;
                    dispenseDisplay(0);
                   
                }
          }
          else if(Tap2.IsDispensing==LOW && lastState_IsDispensing_tap2==HIGH)
          {
                lastState_IsDispensing_tap2=Tap2.IsDispensing;
                if(Tap2.dispensingComplete==HIGH)
                {
                    lcd1.clear();
                    lcd1.setCursor(0,0);
                    lcd1.print("VOLUME DISPENSED");
                    lcd1.setCursor(2,1);
                    TOTAL_VOLUME_DISPENSED += Tap2. Dispensed_Volume;
                    lcd1.print(Tap2.Dispensed_Volume);
                    lcd1.setCursor(13,1);
                    lcd1.print("ML");
                    Serial.print("TOTAL VOLUME DISPENSED");
                    Serial.println(TOTAL_VOLUME_DISPENSED);
                    delay(500);
                    ERROR_CODE=20;
                    dispenseDisplay(0);
                   
                }
          }
    }
}

void displayTimestamp()
{
      if(BYPASS_RTC == HIGH)
      {
        lcd1.clear();
        lcd1.setCursor(0,0);
        lcd1.print(F("Date:"));  
        lcd1.print(rtc.getDateStr());
        lcd1.setCursor(0,1);
        lcd1.print(F("Time:"));
        lcd1.print(rtc.getTimeStr());  
      }
      else
      {
        dispenseDisplay(7);
      }
 }

void pushbuttonDetect()
{
    PUSHBUTTON_OFFSET=0.15;
    pushButton.switchDetect(PUSHBUTTON_CALIBRATION_VOLTAGE, PUSHBUTTON_OFFSET);
    //Serial.println("pushButton:");
    Button1=pushButton.s1;
    //Serial.print(Button1);
    Button2=pushButton.s2;
    //Serial.print(Button2);
    Button3=pushButton.s3;
    //Serial.println(Button3);
    Button4=pushButton.s4;
    //Serial.print(Button4);
    Button5=pushButton.s5;
    //Serial.print(Button5);
    Button6=pushButton.s6;
    //Serial.println(Button6);
}
//function to select 

float get_waterVolume(byte selectedVolume)
{
    float volume=0.0;//This is in litre
    switch(selectedVolume)
    {
        case 1:
            volume=WATER_VOL1/1000.0;
            break;

        case 2:
            volume=WATER_VOL2/1000.0;
            break;

        default:
           volume=0;
           break;
    }
    return volume;   
}

void checkEEPROM()
{
    int checkByte_addr=0;
    byte checkByte;
    int eeAddress=0;
    checkByte=EEPROM.read(checkByte_addr);
    Serial.println(checkByte);
    if(checkByte!=1)
    {
        checkByte=1;
        EEPROM.write(checkByte_addr,checkByte);    
        eeAddress=checkByte_addr+sizeof(checkByte);
        EEPROM.put(eeAddress,parameters);
        parameters.getData();
    }
    else
    {
        readParameter();
    }    
} 

void dispenseDisplay(byte state)
{
    switch(state)
    {
        case 0:
            lcd1.clear();
            lcd1.setCursor(0,0);      
            lcd1.print(F("*<PUSH BUTTON>*"));
            break;
                
        case 1:
            lcd1.clear();
            lcd1.setCursor(0,1);
            lcd1.print(F("Ozonation Start"));
            break;

        case 2:
            delay(250);
            lcd1.setCursor(scrollCursor,1);
            lcd1.print(lcdInfo.substring(stringStart,stringStop));
            if(stringStart==0 && scrollCursor>0)
            {
                scrollCursor--;
                stringStop++;
            }
            else if(stringStart==stringStop)
            {
                stringStart=stringStop=0;
                scrollCursor = screenWidth;
            }
            else if(stringStop==lcdInfo.length() && scrollCursor == 0)
            {
                stringStart++;
                lcd1.print(lcdInfo.substring(stringStart,stringStop));
                stringStop=0;
                stringStart=0;
                lcd1.clear();
                dispenseDisplay(0);
            }
            else 
            {
                stringStart++;
                stringStop++;
            }
            break;
          
         case 3:
            lcd1.clear();
            lcd1.setCursor(0,0);
            lcd1.print(F("!...NO WATER....!"));
            lcd1.setCursor(0,1);
            lcd1.print(F("Please Fill Tank"));
            break;
          
        case 4:
            lcd1.clear();
            lcd1.setCursor(0,0);
            lcd1.print("Date:");
            lcd1.print(rtc.getDateStr());
            lcd1.setCursor(0,1);
            lcd1.print(F("Time:"));
            lcd1.print(rtc.getTimeStr());
            break;
          
       case 5:
            lcd1.clear();
            lcd1.setCursor(0,0);
            lcd1.print(F("**Panel Locked**"));
            break;
          
       case 6:
            lcd1.clear();
            lcd1.setCursor(0,0);
            lcd1.print(F("RTC DISABLE:OFF "));
            break;
            
      case 7:
            lcd1.clear();
            lcd1.setCursor(0,0);
            lcd1.print(F("RTC DISABLE:ON  "));
            break;


       case 8:
          displayTimestamp();
          delay(1000);
          break;
            
      default:
          break;       
     }
 }
  
void readParameter()
{
      int eeAddress=sizeof(byte);
      EEPROM.get(eeAddress, parameters);
      parameters.getData();     
}

void writeParameter()
{
      int eeAddress=sizeof(byte);
      parameters.setData();
      EEPROM.put(eeAddress, parameters);
}    

void VersionInformation()
{
    
      lcd1.clear();  
      lcd1.setCursor(0,0);
      lcd1.print(F("  !! SWAJAL !!     ")); 
      lcd1.setCursor(0,1);
      lcd1.print(F("Water Pvt. Ltd. "));   
      delay(2000);
     
}




/******************************Function to perform the control setting**************************************************/
//This function is called for setting up the various constant for the program run
void controllerSettings()
{
    bool yes = HIGH, no = LOW, recharge = 0;
    unsigned int rechargeAmount=0;
    unsigned int Timeup = 0;
    unsigned int collection=0;
    unsigned long timeoutMillis=0;
    unsigned long pass1=0;
    byte SW1=0,SW2=0;
    setting.button();
    if (setting.shiftButton == HIGH && setting.selectButton == HIGH)
    {
        Serial.println(F("Inside Setting Control"));
        IsSettingsChanged = 1;
        int wait = 2;
        for (int i = 0; i < 100; i++)//The wait for two button press to enter in setting is set to 2s
        {
            setting.button();
            SW1 = setting.shiftButton;
            SW2 = setting.selectButton;
            wait = wait + SW1 * SW2;
            delay(1);
            Serial.print(F("wait count="));
            Serial.println(wait);      
        }
        //check for wait count
        if (wait > 100)
        {      
            Serial.println(F("setting start"));
            wait = 0;    
            unsigned int password=0;
            password = setting.readValue("Enter Password", password);
            switch (password)
            { 
     
                case 111:
                    dispenseDisplay(8);
                    ERROR_CODE=20;
                    break;             
               
                case 503:
                    FLOW_CONSTANT_1=setting.readValue("Flow Constant 1",FLOW_CONSTANT_1);
                    FLOW_CONSTANT_2=setting.readValue("Flow Constant 2",FLOW_CONSTANT_2);
                    break;

                case 502:
                    WATER_VOL1 = setting.readValue("WATER VOL1",WATER_VOL1);
                    WATER_VOL2 = setting.readValue("WATER VOL2",WATER_VOL2);
                    break;

                case 12121:
                    if(BYPASS_RTC==HIGH)
                    {
                      BYPASS_RTC = LOW;
                      dispenseDisplay(7);
                    }
                    else
                    {
                      BYPASS_RTC = HIGH;
                      dispenseDisplay(6);
                    }
                    break;

                case 919:
                    byte Dow,Dom,Month,Hour,Minute;
                    unsigned int Year;
                    Dow=setting.readValue("Enter DOW",Dow);
                    Dom=setting.readValue("Enter DOM",Dom);
                    Month=setting.readValue("Enter Month",Month);
                    Year=setting.readValue("Enter Year",Year);
                    Hour=setting.readValue("Enter Hour",Hour);
                    Minute=setting.readValue("Enter Minute",Minute);
                    setTime_rtc(Dow,Hour,Minute,0,Dom,Month,Year);
                    ERROR_CODE=20;
                    break;   
      
                default:
                    lcd1.clear();
                    lcd1.print(F("Incorrect Key"));
                    lcd1.noCursor();
                    ERROR_CODE=20;
                    break;
          }    
          if(setting.status_of_setting==LOW)
          {
              ERROR_CODE=1;
              return;
          }
      }
    }
    if(currentMillis-savedataMillis>=10000)//this the time will decide the write operation over eeprom
    {
          savedataMillis=currentMillis;
          par_changed=HIGH;
    }
    if(par_changed==HIGH||setting.status_of_setting==HIGH)
    {
        par_changed=LOW;
        setting.status_of_setting=LOW; 
        writeParameter();//the the parameter which have been changed  
        ERROR_CODE=50;
    }    
}

//this function is used for calculation of pulse                            
unsigned int getpulseRequired(byte volCount)
{
    unsigned long pulseRequired=0;
    switch(volCount)
    {
        case 1:
            pulseRequired=FLOW_CONSTANT_1;
            break;

        case 2:
            pulseRequired=FLOW_CONSTANT_2;
            break;
            
        default:
            break;
    }
    return pulseRequired;
} 


void displayParametersOnLcd(unsigned long parameter,String label)
{
    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print(label);
    lcd1.setCursor(0,1);
    lcd1.print(parameter);
}  



  
 
void updateDispensingDataOnModbus()
{
  holdingRegs[FLAG1]  = HIGH;
  holdingRegs[TSDD_U] = ((unsigned long)CURRENT_TIME_STAMP>>16) & 0xffff;
  holdingRegs[TSDD_L] = (unsigned long)CURRENT_TIME_STAMP & 0xffff;
  
  holdingRegs[TVD_U]  = ((unsigned long)TOTAL_VOLUME_DISPENSED>>16) & 0xffff;
  holdingRegs[TVD_L]  = (unsigned long)TOTAL_VOLUME_DISPENSED & 0xffff;
  
  holdingRegs[TRCH_U] = (TOTAL_RECHARGE>>16) & 0xffff;
  holdingRegs[TRCH_L] = TOTAL_RECHARGE & 0xffff;
  
  holdingRegs[TCR_U]  = (TOTAL_COLL_CARD>>16) & 0xffff;
  holdingRegs[TCR_L]  = TOTAL_COLL_CARD & 0xffff;
  
  holdingRegs[TCC_U]  = (TOTAL_COLL_COIN>>16) & 0xffff;
  holdingRegs[TCC_L]  = TOTAL_COLL_COIN & 0xffff;

  holdingRegs[TCS_U]  = (TOTAL_COLL_SALE>>16) & 0xffff;
  holdingRegs[TCS_L]  = TOTAL_COLL_SALE & 0xffff;
  
  holdingRegs[TCV1_U] = (TOTAL_COUNT_VOL1>>16) & 0xffff;
  holdingRegs[TCV1_L] = TOTAL_COUNT_VOL1 & 0xffff;
  
  holdingRegs[TCV2_U] = (TOTAL_COUNT_VOL2>>16) & 0xffff;
  holdingRegs[TCV2_L] = TOTAL_COUNT_VOL2 & 0xffff;
  
  holdingRegs[TCV3_U] = (TOTAL_COUNT_VOL3>>16) & 0xffff;
  holdingRegs[TCV3_L] = TOTAL_COUNT_VOL3 & 0xffff;
  
  holdingRegs[TCV4_U] = (TOTAL_COUNT_VOL4>>16) & 0xffff;
  holdingRegs[TCV4_L] = TOTAL_COUNT_VOL4 & 0xffff;
  
  holdingRegs[TCV5_U] = (TOTAL_COUNT_VOL5>>16) & 0xffff;
  holdingRegs[TCV5_L] = TOTAL_COUNT_VOL5 & 0xffff; 

  holdingRegs[TCCV1_U]= (TOTAL_COIN_COUNT_VALUE1>>16) & 0xffff;
  holdingRegs[TCCV1_L]= TOTAL_COIN_COUNT_VALUE1 & 0xffff;

  holdingRegs[TCCV2_U]= (TOTAL_COIN_COUNT_VALUE2>>16) & 0xffff;
  holdingRegs[TCCV2_L]= TOTAL_COIN_COUNT_VALUE2 & 0xffff;
  
  holdingRegs[PHOW]   = ((unsigned int)(PH_WATER*10)) & 0xffff;
  holdingRegs[TOW]    = tempOfWater;
  holdingRegs[TLV]    = TANK_LEVEL;
  holdingRegs[LAT_U]  = (LATTITUDE>>16) & 0xffff;
  holdingRegs[LAT_L]  = LATTITUDE & 0xffff;
  holdingRegs[LONG_U] = (LONGITUDE>>16) & 0xffff;
  holdingRegs[LONG_L] = LONGITUDE & 0xffff;
  holdingRegs[ALT]    = ALTITUDE;  
  } 
     
 
//-----------------------------------------------------------------------------//                               
void setTime_rtc(byte dw,byte hh,byte mm,byte ss,byte d,byte m,unsigned int y)
{
    rtc.setDOW(dw);              // Set Day-of-Week to SUNDAY  
    rtc.setTime(hh, mm, ss);     // Set the time to 12:00:00 (24hr format)
    rtc.setDate(d, m, y);        // Set the date to January 1st, 2014
}

unsigned long getUnix_Time()
{
    return rtc.getUnixTime(rtc.getTime());
}




