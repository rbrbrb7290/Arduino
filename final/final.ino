#include <SoftwareSerial.h> //블루투스
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //LCD
#include <Adafruit_Sensor.h>
#include <DHT.h> //센서
#include <DHT_U.h>
#include <string.h>

//블루투스
int T= 2;
int R = 3;
SoftwareSerial my_blue(T,R);
//온습도
int Sensor_pin = 8;
DHT my_sensor(Sensor_pin, DHT11);

//초음파&릴레이
int echoPin = 6; 
int trigPin = 7;
int relay = 13;
int bufferPosition;

LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  my_blue.begin(38400);
  bufferPosition = 0; 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.begin(16,2); //LCD크기 16*2로 초기화
}

//온습도 센서
void dht11(int temp, int humi) {
Serial.print("Temperature: ");
Serial.print(temp);
Serial.println("C");
Serial.print("Humidity : ");
Serial.print(humi);
Serial.println("%");
}

//초음파센서
void sensor(float duration ,float distance) {
  pinMode(relay, OUTPUT);
  digitalWrite(trigPin, HIGH); // 초음파를 보낸다. 다 보내면 echo가 HIGH 상태로 대기하게 된다.
  delay(10);
  digitalWrite(trigPin, LOW);
    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  duration = pulseIn(echoPin, HIGH); 
  // HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
  distance = ((float)(340 * duration) / 10000) / 2;  
  Serial.print(distance);
  Serial.println("cm");
  // 수정한 값을 출력
  delay(100);
}

//LCD출력함수
void humi_LCD(int temp, int humi) {
   lcd.clear(); 
    // I2C_LCD를 초기화 해줍니다.
  lcd.init();
  // I2C_LCD의 백라이트를 켜줍니다.
  lcd.backlight();
  // I2C_LCD에 "Hello, world!"라는 메세지를 첫번째줄 첫번째칸에 출력합니다.(0, 0)
  lcd.setCursor(0, 0);
  lcd.print("  humi : ");
  lcd.print(humi);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("  temp : ");
  lcd.print(temp);
  lcd.print("C");
}

String test;
char testNumber = '0';
void loop() 
{ 
   float duration, distance;
   int temp = my_sensor.readTemperature(); //온도
   int humi = my_sensor.readHumidity(); //습도
   
    dht11(temp, humi); //온습도센서가동
    humi_LCD(temp, humi); //LCD모니터 가동
    
    //블루투스로 받아온 데이터가 있을때
    if(my_blue.available()) { 
       char data = my_blue.read(); //loop문안에서 계속해서 데이터를 받아옴
       test+= (String) data; //받은 데이터를 문자열로 형변환
       Serial.println("받아오는 문자열 : " + test);
    }
     if(test == "on") {
        Serial.println("BT통신을 통한 가습기 on");
        testNumber = '1'; //받아온 문자열이 on일때 testNumber에 1을 넘겨줌
        test = "";
     } else if(test == "off") {
        testNumber = '2';
        test = "";
        Serial.println("BT통신을 통한 가습기 off");
        digitalWrite(relay, LOW);
        testNumber = '0';
     }
     
     //'on'을 입력했을 때 실행
    if( testNumber == '1') {
       float duration, distance;
       temp = my_sensor.readTemperature();
       humi = my_sensor.readHumidity();
      
        sensor(duration, distance); //초음파센서 가동
        dht11(temp, humi);
        humi_LCD(temp, humi);
         
        if(humi < 60) { 
          digitalWrite(relay, HIGH); //릴레이 on
        } else if(humi > 75) { 
          digitalWrite(relay, LOW); //릴레이 off
        }
    }
}
