#include <Wire.h>
#include <Adafruit_INA219.h> // You will need to download this library

Adafruit_INA219 sensor219; // Declare and instance of INA219

#define MAX7219_DIN 7
#define MAX7219_CS  8
#define MAX7219_CLK 9

void initialise()
{
  digitalWrite(MAX7219_CS, HIGH);
  pinMode(MAX7219_DIN, OUTPUT);
  pinMode(MAX7219_CS, OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
}

void output(byte address, byte data)
{
  digitalWrite(MAX7219_CS, LOW);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, address);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, data);
  digitalWrite(MAX7219_CS, HIGH);
}

void displaysetup()
{
  output(0x0f, 0x00); //display test register - test mode off
  output(0x0c, 0x01); //shutdown register - normal operation
  output(0x0b, 0x07); //scan limit register - display digits 0 thru 7
  output(0x0a, 0x0a); //intensity register - max brightness
  output(0x09, 0xff); //decode mode register - CodeB decode all digits
}

void setup() {
  // put your setup code here, to run once:
  sensor219.begin();
  initialise();
  displaysetup();
}

float voltage_avg=0, voltage_raw;
float current_avg=0, current_raw;
int loopcount;
void loop() {
  // put your main code here, to run repeatedly:
  const float update_rate = 0.1;
  int voltage, current;
  for(int i=0; i<100; i++) {
    voltage_raw = sensor219.getBusVoltage_V();
    current_raw = sensor219.getCurrent_mA();
    if (voltage_avg>0.0) {
      voltage_avg = voltage_avg*(1.0-update_rate) + voltage_raw*update_rate;
      current_avg = current_avg*(1.0-update_rate) + current_raw*update_rate;
    } else {
      voltage_avg = voltage_raw;
      current_avg = current_raw;
    }
    //delay(1);
  }  
  voltage = (int)round(voltage_avg*100); // values to display
  current = (int)round(current_avg/10);  // values to display
  //current = (int)round(voltage_raw*100); // values to display
  //power = busVoltage * (current/1000); // Calculate the Power
  if(voltage>999) voltage=999;
  if(current>999) current=999;
  
  // to screen
  char buffer[9]={'0'};
  if (loopcount>10) { // re-init once a while
    displaysetup();
    loopcount=0;
  }
  char* buf_p = buffer;
  sprintf(buf_p, "%03d %03d ", voltage, current);
  for(byte i=0; i<8; i++) {
    if(buffer[i]==0 || buffer[i]==' ') {
      output(8-i,0x0f);
    } else {
      if(i==0 || i==4)
        output(8-i,byte(buffer[i])+0x80);
      else
        output(8-i, buffer[i] );
    }
  }
  delay(100);
  loopcount++;
}
