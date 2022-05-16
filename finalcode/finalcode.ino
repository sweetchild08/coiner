#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#define BOUNCE_DURATION 20    //20 ms
#define BOUNCE_DURATIONhop 80 //20 ms

char *host = "192.168.141.234";
char *url = "/";

#define BILL D3
#define COIN D5
#define HOPPER D6
#define RELAY D7
#define BUTTON D4
#define buzzer D8

#define RESET D0

const char *ssid = "money-changer";
const char *password = "c0in1234";
unsigned int addr = 0;
unsigned int coinstat;

int coinvaladd = 0;
int opervaladd = 10;
int coinval;
int coinAdd = 90;
int ou;
int peronecount = 0;
int newcoin;
int go = 0;
int counterstate = 0;
long count10;
long totaldisp = 0;
int state = 0;
int count1;
int countone;
int countfive;
int countten;
int timebounce;
int timebounceadd = 100;
int i;
int countedcoin;
int ii;
int iii;
unsigned long flashcount = 0;
unsigned long bouncetimeone = 0;
int nval;
int newvalue[4];
int tenvalue[3];
int first1;
int second1;
int third1;
int first2;
int second2;
int third2;
int pertencount;
int fourth2;
long number;
long pass = 1141;
int checked = true;
volatile unsigned long bouncetime = 0;
volatile unsigned long bouncetime2 = 0;
volatile unsigned long bouncetimehop = 0;
volatile int credit = 20;
volatile int dispensed = 0;
int stopcount = 0;
unsigned long prevmillis = 0;
volatile int yy;
int coinvalue;
String ope = "";
int coinset = 1;
int operation = 1;
// 1 is changer
// 2 is dispenser
int coincount;
long endcount;
unsigned int average;
void setup()
{
  Serial.begin(115200);
  ESP.wdtFeed();
  //  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  EEPROM.begin(sizeof(int));
  EEPROM.get(addr, coinstat);

  state = 1;
  buzz();
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  lcd.begin();
  Wire.begin(D2, D1);
  lcd.clear();
  lcd.backlight();
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(RESET, INPUT_PULLUP);
  coinvalue = 5;
  coinval = 5;
  operation = 1;
  timebounce = 40;
  lcd.begin();
  state = 0;
  lcd.clear();
  lcd.backlight();
  lcd.print("initializing..");

  //EICRA &= ~3;  // clear existing flags
  //EICRA |= 2;   // set wanted flags (falling level interrupt)
  //EIMSK |= 1;   // enable it
  //EIFR = bit (INTF0);  // clear flag for interrupt 0 (PIN D2)
  //EIFR = bit (INTF1);  // clear flag for interrupt 1 (PIN D3)
  delay(1000);
  ESP.wdtFeed();
  lcd.clear();
  lcd.print("Coin Value: ");
  lcd.print(coinval);
  delay(1000);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  pinMode(BILL, INPUT_PULLUP);
  pinMode(COIN, INPUT_PULLUP);
  pinMode(HOPPER, INPUT);
  ESP.wdtFeed();
  attachInterrupt(digitalPinToInterrupt(BILL), ISR_count, CHANGE);
  attachInterrupt(digitalPinToInterrupt(COIN), ISR_count3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HOPPER), ISR_count2, FALLING);
  delay(1000);
  ESP.wdtFeed();
}

void sendsms()
{
  String a="/"+(String)coinstat+"/"+(String)credit;
  String endpoint="http://192.168.141.234"+a;

  // // Use WiFiClient class to create TCP connections
  WiFiClient client;

  // if (!client.connect(host, 80))
  // {
  //   Serial.println("connection failed");
  //   Serial.println("wait 5 sec...");
  //   delay(5000);
  //   return;
  // }
  // String req="GET " + a + " HTTP/1.1\r\n" +
  //              "Host: " + host + "\r\n \r\n" ;
  // Serial.println(req);
  // client.print(req);

  // //read back one line from server
  // Serial.println("receiving from remote server");
  // String line = client.readStringUntil('\r');
  // Serial.println(line);

  // Serial.println("closing connection");
  // client.stop();
  HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, endpoint)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
}
ICACHE_RAM_ATTR void ISR_count()
{

  if (millis() > bouncetime)
  {

    if (!digitalRead(BILL))
    {

      credit += 10;
      //Serial.print("credit: ");
      //Serial.println(credit);
    }

    bouncetime = millis() + BOUNCE_DURATION;
  }
}
ICACHE_RAM_ATTR void ISR_count3()
{

  if (millis() > bouncetime2)
  {

    if (!digitalRead(COIN))
    {

      credit += 1;
      coinstat++;
      //Serial.print("credit: ");
      //Serial.println(credit);
    }

    bouncetime2 = millis() + BOUNCE_DURATION;
  }
}

ICACHE_RAM_ATTR void ISR_count2()
{
  if (millis() > bouncetimehop)
  {
    delayMicroseconds(10000);
    if (digitalRead(HOPPER) == LOW)
    {
      dispensed++;

      bouncetimehop = millis() + BOUNCE_DURATIONhop;
    }
  }
}

void loop()
{
  // sei();
  coinchangerset();
  ESP.wdtFeed();
}

void change1(unsigned long zzz)
{
  cli();
  delay(800);
  sei();
  flashcount = millis();
  lcd.begin();
  lcd.print("Operating");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait...");
  lcd.setCursor(0, 2);
  lcd.print("Dispense: ");
  lcd.print(count1 / coinval);
  lcd.print(" coins");
  lcd.setCursor(0, 3);
  lcd.print("Amount: ");
  lcd.print(count1);
  lcd.print(" Pesos");
  Serial.print("Zzz: ");
  Serial.println(zzz);
  long qqq = zzz / coinval;
  yy = 1;
  unsigned long delays;
  if (qqq < 10)
  {
    delays = 7000;
  }
  else if ((qqq > 10) || (qqq < 50))
  {
    delays = qqq * 400;
  }
  else
  {
    delays = qqq * 300;
  }
  //credit = credit - (zzz);
  digitalWrite(RELAY, LOW);
  while (dispensed < qqq)
  {
    if ((millis() - flashcount) > delays)
    {
      digitalWrite(RELAY, HIGH);
      yy = 0;
      break;
    }
  }
  digitalWrite(RELAY, HIGH);
  cli();
  //EIFR = bit (INTF0);  // clear flag for interrupt 0 (PIN D2)
  //EIFR = bit (INTF1);  // clear flag for interrupt 1 (PIN D3)
  //sei();
  if (yy == 0)
  {
    delay(100);
    int output = qqq - dispensed;
    Serial.print("OUT");
    Serial.println(output);
    credit = output * coinval;
    state = 11;
    dispensed = 0;
    coinstat -= output;
    EEPROM.put(addr, coinstat);
    EEPROM.commit();

    sendsms();
  }
  else if (yy == 1)
  {
    state = 0;
    delay(100);
    delay(100);
    credit = 0;
    dispensed = 0;
  }
  sei();

  dispensed = 0;
  go = 0;
  count1 = 0;
}
void coinchangerset()
{
  if (state == 1)
  {
    lcddisplay2();
  }
  else if (state == 0)
  {
    lcddisplay();
  }
  int xxx = !digitalRead(BUTTON);
  int res = !digitalRead(RESET);
  if (res)
  {
    coinstat = 500;
    EEPROM.put(addr, coinstat);
    Serial.println("reseted");
    EEPROM.commit();
  }
  if (xxx)
  {
    lcd.clear();
    lcd.print("dispensing ");
    delay(1000);

    if (credit >= coinvalue)
    {
      coincount = credit / coinvalue;
      yy = 1;
      cli();
      totaldisp = 0;
      dispensed = 0;
      endcount = coincount;
      pertencount = endcount / 10;
      peronecount = endcount - (pertencount * 10);
      flashcount = millis();
      //sei();
      if (pertencount > 0)
      {
        for (ou = 0; ou < pertencount; ou++)
        {
          sei();
          delay(1000);
          digitalWrite(RELAY, LOW);
          while (dispensed < 10)
          {
            delay(0);
            if ((millis() - flashcount) > 8000)
            {
              digitalWrite(RELAY, HIGH);
              yy = 0;
              break;
            }
          }
          digitalWrite(RELAY, HIGH);
          // EIFR = bit (INTF1);
          delay(2000);
          cli();
          //  // clear flag for interrupt 1 (PIN D3)
          totaldisp = totaldisp + dispensed;
          dispensed = 0;
          flashcount = millis();

          if (yy == 0)
          {
            break;
          }
        }
      }
      if (peronecount > 0)
      {
        sei();
        delay(1000);
        digitalWrite(RELAY, LOW);
        while (dispensed < peronecount)
        {
          delay(0);
          if ((millis() - flashcount) > 8000)
          {
            digitalWrite(RELAY, HIGH);
            yy = 0;
            break;
          }
        }
        digitalWrite(RELAY, HIGH);
        //EIFR = bit (INTF1);
        delay(2000);
        cli();
        // EIFR = bit (INTF1);  // clear flag for interrupt 1 (PIN D3)
        totaldisp = totaldisp + dispensed;
      }
      digitalWrite(RELAY, HIGH);
      delay(1000);
      cli();
      //EIFR = bit (INTF0);  // clear flag for interrupt 0 (PIN D2)
      //EIFR = bit (INTF1);  // clear flag for interrupt 1 (PIN D3)
      if (yy == 0)
      {
        delay(100);
        int output = totaldisp * coinvalue;
        Serial.print("OUT");
        Serial.println(output);
        coinstat -= output;
        EEPROM.put(addr, coinstat);
        EEPROM.commit();
    sendsms();
        credit = credit - output;
        state = 1;
        dispensed = 0;
        ou = 0;
        totaldisp = 0;
      }
      else
      {
        int output = totaldisp * coinvalue;
        Serial.print("OUT");
        Serial.println(output);
        coinstat -= output;
    sendsms();
        EEPROM.put(addr, coinstat);
        EEPROM.commit();
        credit = credit - output;
        state = 0;
        delay(100);
        delay(100);
        //credit = credit;
        dispensed = 0;
        ou = 0;
        totaldisp = 0;
      }
      sei();
    }
  }
}

void lcddisplay()
{
  digitalWrite(buzzer, LOW);
  lcd.clear();
  lcd.print("Available Coins: ");
  lcd.print(coinstat);
  lcd.setCursor(0, 1);
  lcd.print("Bill/Coin to 5 Peso");
  lcd.setCursor(0, 2);
  lcd.print("Insert Money");
  lcd.setCursor(0, 3);
  lcd.print("Credit: ");
  lcd.print(credit);
  delay(200);
}
void lcddisplay2()
{
  digitalWrite(buzzer, HIGH);
  lcd.clear();
  lcd.print("NO MORE COINS");
  lcd.setCursor(0, 1);
  lcd.print("Credit: ");
  lcd.print(credit);
  delay(200);
}

void buzz()
{
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(buzzer, LOW);
}
