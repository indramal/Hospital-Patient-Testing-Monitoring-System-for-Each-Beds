//////////////////////////////////////

#include <SoftwareSerial.h> // Software Serial
#include <LiquidCrystal_I2C.h> // For LCD

/////// For Communicate with Arduino //////////

SoftwareSerial mySerial(10, 11);

String sdata;
char* chardata;
int len;
int pos;
char First[10];
char Second[10];

///////////////////////////////////////

#define tempsensorPin A1 // For LM35 temperature sensor

LiquidCrystal_I2C lcd(0x27,16,2);  // LCD address to 0x27 for a 16 chars and 2 line display

//////// Parametes /////////
int32_t spoval = 0;
int32_t bmpval = 0;
unsigned long temval = 0;
float  temvalavg;
float voltage;
float temperatureC;
int32_t minspo = 95; // Set minimum SPO2 value
int32_t maxspo = 100; // Set maximum SPO2 value
int32_t minbmp = 60; // Set minimum BMP value
int32_t maxbmp = 100; // Set maximum BMP value
int mintemp = 36; // Set minimum temperature value
int maxtemp = 37.1; // Set maximum temperature value
//////////////////////////

int y = 0; // For LCD refresh

const int buzzer = 5; // For Buzzer
int almst = 0;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  //Serial.println("Project Start!");
    
  ////////////// LCD //////////////////
  lcd.init(); // Display Initialising
  lcd.clear(); // Clean LCD Display     
  lcd.backlight(); // Backlight is on
  ////////////////////////////////////

  ///////////// ECG ////////////////
  pinMode(9, INPUT); // Setup for leads off detection LO +
  pinMode(8, INPUT); // Setup for leads off detection LO -
  /////////////////////////////////

  pinMode(buzzer, OUTPUT); // For Buzzer
  
}

void loop() {

  ////// Serial Values Read /////////

  if (mySerial.available()) {
    sdata = mySerial.readStringUntil('\n');
    
    //Serial.print("BMP:");
    //Serial.print(bmpval);
    //Serial.print(" - SPO: ");
    //Serial.println(spoval);
  }

  /////////////////////////////////////

  ///// Temperature read //////////////
  int x;
  for(x=0; x<100 ; x++){
    temval += analogRead(tempsensorPin);
  }
  temvalavg = temval/100;
  // Convert that reading into voltage
  voltage = temvalavg * (5.0 / 1024.0);
  // Convert the voltage into the temperature in Celsius
  temperatureC = voltage * 100;
  temval = 0;
  /////////////////////////////////////
  
  if(y >= 100){

    chardata = sdata.c_str();
    len = strlen(chardata);
    pos = strcspn(chardata,"/");
    
    strncpy(Second, chardata+pos+1, len);
    strncpy(First, chardata, pos);

    bmpval = atol( First );
    spoval = atol( Second );
    
    memset(First, 0, sizeof First);
    memset(Second, 0, sizeof Second);
    memset(chardata, 0, sizeof chardata);
    sdata = "";
    
    lcd.clear();
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("BPM:");
    lcd.print(bmpval);  
    lcd.setCursor(8,0);   //Set cursor to character 2 on line 0
    lcd.print("T:");
    lcd.print(temperatureC);
    lcd.setCursor(0,1);   //Set cursor to character 2 on line 0
    lcd.print("SpO2:");
    lcd.print(spoval);
    lcd.print("%");

    if(almst == 0){
      lcd.setCursor(14,1);
      lcd.print("");
    }else if(almst == 1){
      lcd.setCursor(14,1);
      lcd.print("OX");
    }else if(almst == 2){
      lcd.setCursor(14,1);
      lcd.print("HR");
    }else if(almst == 3){
      lcd.setCursor(14,1);
      lcd.print("TP");
    }

    y = 0;
    bmpval = 0;
    spoval = 0;
    delay(1);
  }
  y = y + 1;
  
  /////////// ECG Plot //////////////
  if((digitalRead(8) == 1)||(digitalRead(9) == 1)){
    //Serial.println('!');
  } else{
    Serial.println(analogRead(A0));
  }
  //////////////////////////////////

  ////////////// Alarm ///////////////

  // For SPO2 value
  if(spoval < minspo || spoval > maxspo){
     //tone(buzzer, 1000);
     almst = 1;
  }else{
    noTone(buzzer);
    almst = 0;
  }
  // For BMP value
  if(bmpval < minbmp || bmpval > maxbmp){
     //tone(buzzer, 1000);
     almst = 2;
  }else{
    noTone(buzzer);
    almst = 0;
  }
  // For Temp value
  if(temperatureC < mintemp || temperatureC > maxtemp){
     //tone(buzzer, 10);
     almst = 3;     
  }else{
    noTone(buzzer);
    almst = 0;
  }

  ///////////////////////////
  
}
