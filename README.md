# electronic-scale

아두이노 전자저울의 예제 코드입니다.  
전자저울 모듈은 아래 엘레파츠 링크에서 만나보실 수 있습니다.  

저울(로드셀, A/D보드 포함) 모듈 (납땜 작업 필요) 
- [로드셀 5kg 모듈](https://www.eleparts.co.kr/EPXNX99X)  
- [로드셀 10kg 모듈](https://www.eleparts.co.kr/EPXNXBP3)  
  
  
##  필요 구성 상품 리스트

아래 리스트는 본 예제를 실행하기 위한 구성품 목록입니다.

|예제 번호	|     구성품     	|  	  링크                   	    |
|-----------|-------------------|----------------------------------|
|1|아두이노			|https://www.eleparts.co.kr/EPX4P4P3|
|1|USB 케이블|https://www.eleparts.co.kr/EPXCTKGJ|
|1-A|저울 모듈  5kg  |https://www.eleparts.co.kr/EPXNX99X|
|1-B|저울 모듈  10kg  |https://www.eleparts.co.kr/EPXNXBP3|
|1|M/F 점퍼 케이블		|https://www.eleparts.co.kr/EPXCTKH9|
|2|1602 IIC LCD 		|https://www.eleparts.co.kr/EPXHVBKK|
|2|브레드보드			|https://www.eleparts.co.kr/EPX34N7P|
|2|M/M 점퍼			|https://www.eleparts.co.kr/EPXCTKH8|
|3|스위치 4개			|https://www.eleparts.co.kr/goods/catalog?&code=00620002&search_text=TS-1212T|
|3|저항 /10k			|https://www.eleparts.co.kr/EPX3333P|  
|3|RGB LED 모듈		|https://www.eleparts.co.kr/EPXHLMFP|  
  
 ※ 1번 예제는 1번 구성품, 2번 예제는 1번+2번 구성품, 3번 예제는 1번+2번+3번 구성품이 필요합니다.  

## 필수 라이브러리  
  
아래 두 라이브러리가 필요하며, 아두이노 IDE 내에서 라이브러리 관리자를 통해 다운로드 할 수 있습니다.  
  
**GitHub 링크**  
- [전자저울 / HX711 ADC 라이브러리 (HX711)](https://github.com/bogde/HX711)  
- [I2C LCD 라이브러리 (LiquidCrystal_I2C)](https://github.com/johnrickman/LiquidCrystal_I2C)  

**아두이노 라이브러리 매니저**  
  
- **HX711**  
![HX711](./library%20image/01.HX711.png)  
  
- **LiquidCrystal_I2C**  
![LiquidCrystal_I2C](./library%20image/02.LiquidCrystal_I2C.png)  
  
## H/W 연결  
   
[회로 연결 이미지](https://github.com/eleparts/electronic-scale/tree/master/circuit%20image)  


## 참고자료(사용법) - 블로그 포스팅
  
엘레파츠 블로그  
- [01. 조립 및 동작 테스트하기](https://blog.naver.com/elepartsblog/221651788693)   
- [02. I2C LCD로 측정 값 출력하기](https://blog.naver.com/elepartsblog/221651804182)  
- [03. 스위치로 저울 조작하기](https://blog.naver.com/elepartsblog/221651833358)  
  
