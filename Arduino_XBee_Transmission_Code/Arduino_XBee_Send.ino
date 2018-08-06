#include "DHT.h"
#define DHTPIN 24
#define DHTTYPE DHT22

unsigned long previousMillis = 20000UL;        // will store last time LED was updated
long timeInterval = 20000UL;           // milliseconds of on-time

int O2 = 8;
int CO2 = 9;
int CO = 10;
int H2 = 11;
int H2S = 12;
int CH4 = 13;
int allow = 0;
float preval1 = 0;
float preval2 = 0;
float preval3 = 0;
float preval4 = 0;
float preval5 = 0;
float preval6 = 0;
float pretemp = 0;
float prehum = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  dht.begin();
}

void loop() {
  String dataOut = "";
  unsigned long currentMillis = millis();

  if((currentMillis-previousMillis)>=timeInterval){
    previousMillis = currentMillis;

    int val1 = analogRead(O2);
    float mv1 = (0.7227*val1-4.11);
    if (mv1<20.9)
    {
      dataOut = dataOut + String(mv1) + ",";
    }
    else {
      dataOut = dataOut + "20.84,";
    }

    int val2 = analogRead(CO2);
    float mv2 = (0.18013*val2-14.98-2.5+1.4);
    if (c2 > 0){
      dataOut = dataOut + String(mv2) + ",";
    }
    else{
      dataOut = dataOut + "0.00,";
    }

    int val3 = analogRead(CO);
    float mv3 = (0.675*val3-322.5+17.0);
    if (mv3 > 0){
      dataOut = dataOut + String(mv3) + ",";
    }
    else {
      dataOut = dataOut + "0.00,";
    }

    int val4 = analogRead(H2);
    float mv4 = (0.28943*val4-128.28+1.22-6);
    if (mv4 > 0){
      dataOut = dataOut + String(mv4) + ",";
    }
    else {
      dataOut = dataOut + "0.00,";
    }

    int val5 = analogRead(H2S);
    float mv5 = ( val5/1024.0)*50000;
    dataOut = dataOut + String(mv5) + ",";

    int val6 = analogRead(CH4);
    float mv6 = (0.19788*val6-16.807-2.19+3.1-3.8);
    if (mv6 > 0){
      dataOut = dataOut + String(mv6) + ",";
    }
    else {
      dataOut = dataOut + "0.00,";
    }

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!(isnan(h) || isnan(t))) {
      dataOut = dataOut + String(t) + "," + String(h) + ",@";
    }
    else{
      dataOut = dataOut + "0,0,@";
    }

    if((preval1!=val1)||(preval2!=val2)||(preval3!=val3)||(preval4!=val4)||(preval5!=val5)||(preval6!=val6)||(pretemp!=temp)||(prehum!=hum)){
      allow = 1;
      preval1 = val1;
      preval2 = val2;
      preval3 = val3;
      preval4 = val4;
      preval5 = val5;
      preval6 = val6;
      pretemp = temp;
      prehum = hum;
    }
   
    if(allow==1){
      Serial.print(dataOut);
      allow = 0;
    }
  }

  delay(10000);
}
