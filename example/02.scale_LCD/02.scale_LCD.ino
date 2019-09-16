/*
* 엘레파츠 전자저울 예제 프로그램 - I2C LCD  
* 사용 라이브러리
* HX711 : https://github.com/bogde/HX711
* I2C LCD : https://github.com/johnrickman/LiquidCrystal_I2C
* 
* 아두이노 GPIO 연결
* loadcell / HX711 board
* Dout PIN : D2
* SCK PIN  : D3
* 
* I2C LCD (1602)
* SDA : A4
* SCL : A5
*/

#include "HX711.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// HX711 circuit wiring
#define LOADCELL_DOUT_PIN         2
#define LOADCELL_SCK_PIN          3

// scale - 10Kg loadcell : 226 / 5kg loadcell : 372
// ADC 모듈에서 측정된 결과값을 (loadcellValue)값 당 1g으로 변환해 줌
float loadcellValue = 226.0;

HX711 scale;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  
  // 로드셀 HX711 보드 pin 설정
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // LCD 초기화 및 electronic scale 표시
  lcd.init();
  lcd.backlight();
  lcd.print("electronic");
  lcd.setCursor(8,1);
  lcd.print("scale");
  

  // 부팅 후 잠시 대기 (2초)
  delay(2000);

  // 측정값 1회 읽어오기
  Serial.print("read: \t\t");
  Serial.println(scale.read());
  delay(1000);

  // 스케일 설정, 오프셋 설정(10회 측정 후 평균값 적용)
  scale.set_scale(loadcellValue); 
  scale.tare(10);   

  // 설정된 오프셋 및 스케일 값 확인
  Serial.print("Offset value :\t");
  Serial.println(scale.get_offset());
  Serial.print("Scale value :\t");
  Serial.println(scale.get_scale());

  // 자동 설정된 오프셋 값 LCD에 출력
  lcd.clear();
  lcd.print("Offset : ");
  lcd.setCursor(5,1);
  lcd.print(scale.get_offset());

  // 3초 대기후 LCD 클리어
  delay(3000);
  lcd.clear();

  // 무게 안내문자 표시
  lcd.print("Weight : ");
}

void loop() {
  // put your main code here, to run repeatedly:

  float value;

  // 오프셋 및 스케일이 적용된 측정값 (5회 측정 평균값) 
  value = scale.get_units(5);

  // 측정값 시리얼 출력
  Serial.print("value :\t");
  Serial.print(value, 2);
  Serial.println(" g");

  // 커서 위치 설정 및 측정값 LCD 출력 
  lcd.setCursor(6,1);
  lcd.print(value);
  lcd.print(" g    ");
   
  delay(1000);
}
