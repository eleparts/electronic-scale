/*
* 엘레파츠 전자저울 예제 프로그램 - 종합
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
* 
* SW AutoOffset      : D7
* SW OffsetIncrease  : D6
* SW OffsetReduction : D5
* SW UnitChange      : D4
* 
* RGB-R : D10
* RGB-G : D9
* RGB-B : D8
*/ 

#include "HX711.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// circuit wiring
#define LOADCELL_DOUT_PIN         2
#define LOADCELL_SCK_PIN          3
#define SW_AUTO_OFFSET_PIN        7
#define SW_OffSET_INCREASE_PIN    6
#define SW_OffSET_REDUCTION_PIN   5
#define SW_UNIT_CHANGE_PIN        4
#define RGB_R_PIN                 10
#define RGB_G_PIN                 9
#define RGB_B_PIN                 8

// 오프셋 수동 조절 시 변경할 단위(g)
#define CHANGE_OFFSET_VALUE       0.1

// 최대 측정 범위(g) - 로드셀에 맞춰 설정
#define LOADCELL_MAX_RANGE        100000

// 평균을 구하기 위한 무게 측정 횟수
#define NUMBER_OF_MEASURMENTS     5

// 안정화 오차값(g) MAX 수치 - 무게 측정 시 흔들림 등으로 값이 출렁이는 경우에 대한 오차 경고(LED) 범위 지정
#define STABILIZATION_ERROR_VALUE 5 

// scale - 10Kg loadcell : 226 / 5kg loadcell : 372
// ADC 모듈에서 측정된 결과값을 (loadcellValue)값 당 1g으로 변환해 줌
float loadcellValue = 226.0;

// SW status - PULLUP / avtive : low
int sw_AutoOffset        = HIGH;
int sw_OffsetIncrease    = HIGH;
int sw_OffsetReduction   = HIGH;
int sw_UnitChange        = HIGH; 

// 무게 단위 표시
enum SELECT_UNIT_LIST{
  AutoUnit,
  GramUnit,
  KiloGramUnit
};

enum SELECT_UNIT_LIST selectUnit = AutoUnit;

HX711 scale;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  
  // 로드셀 HX711 보드 pin 설정
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // 스위치 초기화
  pinMode(SW_AUTO_OFFSET_PIN, INPUT_PULLUP);
  pinMode(SW_OffSET_INCREASE_PIN, INPUT_PULLUP);
  pinMode(SW_OffSET_REDUCTION_PIN, INPUT_PULLUP);
  pinMode(SW_UNIT_CHANGE_PIN, INPUT_PULLUP);

  // LED 초기화
  pinMode(RGB_R_PIN, OUTPUT);
  pinMode(RGB_G_PIN, OUTPUT);
  pinMode(RGB_B_PIN, OUTPUT);

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

  // 오프셋 자동 설정
  AutoOffsetSetting();

  // 스케일 설정
  scale.set_scale(loadcellValue); 

  //스케일 값 확인
  Serial.print("Scale value :\t");
  Serial.println(scale.get_scale());
  

  // 3초 대기후 LCD 클리어
  delay(3000);
  lcd.clear();

  // 무게 안내문자 표시
  lcd.print("Weight : ");
}

// loop
void loop() {
  // put your main code here, to run repeatedly:
  float value;

  // 스위치 확인/제어
  SwitchCheck();

  // 값 측정, 신뢰성 확인 및 LED제어
  value = CheckValue();
 
  // 화면 출력
  printValue(value);
   
  delay(500);
}




// 스위치 동작 확인&처리 함수
void SwitchCheck(){
  
  // 스위치가 눌리지 않으면 딜레이 없이 바로 종료
  if(!GetSwitchValue()){
    return;
  }

  // 스위치가 눌리면 반복 진입/값 읽기
  do
  {
    // 스위치 상태에 따라 각 동작 진행
    if (sw_AutoOffset == LOW){

      lcd.clear();
      lcd.print("== Auto Offset ==");
      Serial.println("== Auto Offset ==");
      delay(1000);

      AutoOffsetSetting();
      break;

    }else if (sw_OffsetIncrease == LOW){

      OffsetSetting(CHANGE_OFFSET_VALUE);
      delay(800);

    }else if (sw_OffsetReduction == LOW){

      OffsetSetting(-1 * CHANGE_OFFSET_VALUE);
      delay(800);
      
    }else if (sw_UnitChange == LOW){

      UnitChange();
      break;
      
    }else{
      // err
      return;
    }
  } while (GetSwitchValue());

  // 화면 복구 후 반복 종료
  delay(1500);

  lcd.clear();
  lcd.print("Weight : ");
  lcd.setCursor(6,1); 

  return;
}

// 스위치 값 읽어오기 / 스위치 입력 시 HIGH 반환
int GetSwitchValue(void){

  sw_AutoOffset      = digitalRead(SW_AUTO_OFFSET_PIN);
  sw_OffsetIncrease  = digitalRead(SW_OffSET_INCREASE_PIN);
  sw_OffsetReduction = digitalRead(SW_OffSET_REDUCTION_PIN);
  sw_UnitChange      = digitalRead(SW_UNIT_CHANGE_PIN);

  return !(sw_AutoOffset && sw_OffsetIncrease && sw_OffsetReduction && sw_UnitChange);
}


// 오프셋 자동 설정 함수
void AutoOffsetSetting(){

  // 오프셋 설정
  scale.tare(10);   

  // 설정된 오프셋 시리얼로 확인
  Serial.print("Offset value :\t");
  Serial.println(scale.get_offset());

  // 자동 설정된 오프셋 값 LCD에 출력
  lcd.clear();
  lcd.print("Offset : ");
  lcd.setCursor(5,1);
  lcd.print(scale.get_offset());

  return;
}


// 오프셋 수동 설정 함수
void OffsetSetting(float value){

  long old_offset = 0, new_offset = 0;

  // 변경할 오프셋 값 계산 (로드셀 scale 값 x 변경할 무게 value )
  old_offset = scale.get_offset();
  new_offset = old_offset - (loadcellValue * value);

  scale.set_offset(new_offset);

  // 변경 오프셋 시리얼 출력
  Serial.print("change Offset :\t");
  if(value > 0){ 
    Serial.print("+"); 
  }
  Serial.println(value);
  Serial.print("old Offset value :\t");
  Serial.println(old_offset);
  Serial.print("new Offset value :\t");
  Serial.println(new_offset);

  // 변경 오프셋 LCD 출력
  lcd.clear();
  lcd.print("Offset : ");
  if(value > 0){ 
    lcd.print("+"); 
  }
  lcd.print(value);
  lcd.print("g ");

  lcd.setCursor(0,1);
  lcd.print(old_offset);
  lcd.print(" >");
  lcd.print(new_offset);

  return;
}

// LED 제어
void ControllLed(float value){

  if(value > LOADCELL_MAX_RANGE){
    // 최대 측정범위 초과시 적색 LED 점등
    RedGreenBuleLED(HIGH,LOW,LOW);

  }else{
    // 정상 측정시 녹색 LED 점등
    RedGreenBuleLED(LOW,HIGH,LOW);
  }
  
  return;
}

// LED R/G/B 제어 함수
void RedGreenBuleLED(bool red, bool green, bool bule){

  digitalWrite(RGB_R_PIN, red);
  digitalWrite(RGB_G_PIN, green);
  digitalWrite(RGB_B_PIN, bule);

} 

// 단위 기호 변경 함수
void UnitChange(){

  Serial.print("Change unit :\t");

  lcd.clear();
  lcd.print("Change unit:");

  if(selectUnit == AutoUnit){
    // Auto Unit -> gram Unit

    selectUnit = GramUnit;
    Serial.println("Gram");
    lcd.setCursor(8,1);
    lcd.print("Gram");

  }else if (selectUnit == GramUnit){
    // gram Unit -> kg Unit
    
    selectUnit = KiloGramUnit;
    Serial.println("Kilogram");
    lcd.setCursor(6,1);
    lcd.print("Kilogram");

  }else{
    // kg Unit -> Auto Unit
    
    selectUnit = AutoUnit;
    Serial.println("Auto");
    lcd.setCursor(8,1);
    lcd.print("Auto");
  }

  return;
}


// 측정 값 출력
void printValue(float value){

  if(selectUnit == AutoUnit){

    if(value > 1000){       // kg
      
      float kiloGramValue = value / 1000;
    
      // 측정값 시리얼 출력
      Serial.print("kg value :\t");
      Serial.print(kiloGramValue, 2);
      Serial.println(" kg");

      // 커서 위치 설정 및 측정값 LCD 출력 
      lcd.setCursor(6,1);
      lcd.print(kiloGramValue);
      lcd.print(" kg    ");

    }else{                    // g
      
      // 측정값 시리얼 출력
      Serial.print("value :\t");
      Serial.print(value, 2);
      Serial.println(" g");

      // 커서 위치 설정 및 측정값 LCD 출력 
      lcd.setCursor(6,1);
      lcd.print(value);
      lcd.print(" g    ");

    }

  }else if (selectUnit == GramUnit){

    // 측정값 시리얼 출력
    Serial.print("value :\t");
    Serial.print(value, 2);
    Serial.println(" g");

    // 커서 위치 설정 및 측정값 LCD 출력 
    lcd.setCursor(6,1);
    lcd.print(value);
    lcd.print(" g    ");

  }else if (selectUnit == KiloGramUnit){

    float kiloGramValue = value / 1000;
    
    // 측정값 시리얼 출력
    Serial.print("kg value :\t");
    Serial.print(kiloGramValue, 2);
    Serial.println(" kg");

    // 커서 위치 설정 및 측정값 LCD 출력 
    lcd.setCursor(6,1);
    lcd.print(kiloGramValue);
    lcd.print(" kg    ");

  }else{
    Serial.print("Unit err");
    selectUnit = AutoUnit;
  }

  return;
}


// 값 (NUMBER_OF_MEASURMENTS)개를 받아 배열에 저장
float* GetValue(){

  static float valueList[NUMBER_OF_MEASURMENTS] = {0,};
  
  for(int i=0;i<NUMBER_OF_MEASURMENTS;i++){
    valueList[i] = scale.get_units();
  }
  
  return valueList;
}


// 측정된 값들의 안정화 여부 체크(측정 값 신뢰성 체크), 측정값이 안정화되지 않은 상태이면 1 반환
int AccuracyCheck(float valueList[], int size){

  float maxValue, minValue;

  // 측정 값 안정화여부 판단 시 오차%가 Xg 이상인 경우 MAX +-Xg으로 고정 / X = STABILIZATION_ERROR_VALUE
  if((valueList[0] * 0.2) > STABILIZATION_ERROR_VALUE){

    maxValue = valueList[0] + STABILIZATION_ERROR_VALUE;    // + Xg
    minValue = valueList[0] - STABILIZATION_ERROR_VALUE;    // - Xg

  }else{
    maxValue = valueList[0] * 1.2;    // +20%
    minValue = valueList[0] * 0.8;    // -20%
  }

  for(int i=1; i<size; i++){

    if((valueList[i] < minValue) || (maxValue < valueList[i])){

      return 1;
    }
  }

  return 0;
}



// 측정 평균값 구하기
float AverageValue(float valueList[], int size){

  float value = 0.0;

  for(int i=0; i<size; i++){
    value = value + valueList[i];
  }

  value = value / size;

  return value;
}


// 측정 값 신뢰성 체크 및 LED제어
float CheckValue(){

  float* valueList = GetValue();

  float value = AverageValue(valueList, NUMBER_OF_MEASURMENTS);

  // 측정 값 신뢰성검사 & LED 제어/ 측정 값이 1g 이하인 경우 기본 오차를 감안하여 신뢰성 검사 하지 않음 
  if(AccuracyCheck(valueList, NUMBER_OF_MEASURMENTS) && (value > 1)){
 
    RedGreenBuleLED(LOW,LOW,HIGH);
  
  }else{

    ControllLed(value);
  }

  return value;
}
