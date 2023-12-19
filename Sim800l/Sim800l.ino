#include <SoftwareSerial.h>
SoftwareSerial SI800L(10, 11);//Rx-11, Tx-10
int gas = A0;
String number1 = "0819990184";// nhap sdt vào đây
int value = 0;
int lua =12;
int nt = 1; //Cho nhắn cảnh báo 1 nhắn tin 1 lần

//Servo
#include <Servo.h>     
Servo myservo; 
int pos = 0;

//gas
int phantram;
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2); //SCL A5 SDA A4


// //Điều khiển
char incomingByte; 
String inputString;

//Ma trận phím 3x4
char password[5];
char open[]="1234";
int i = 0;
int on = 0;
int dem =0;
int demsolannhap = 0;

#include "Keypad.h"
const byte ROWS = 4; // số hàng bàn phím
const byte COLS = 3; // số cột bàn phím
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5}; // thứ tự các chân "hàng" được nối vào arduino R1 = D8, R2 = D7, R3 = D6, R4 = D5
byte colPins[COLS] = {4, 3, 2}; // thứ tự các chân "cột" được nối vào arduino C1 = D4, C2 = D3, C3 = D2
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{

Serial.begin(9600);
SI800L.begin(9600);//19200
delay(2000); 

//LCD
 lcd.init();                    
 lcd.backlight(); 

pinMode(lua, INPUT); 
pinMode(gas, INPUT);

      SI800L.print("AT+CLIP=1\r"); // test
      delay(1000); 
      SI800L.println("AT");

//Servo
myservo.attach(9);

//Điều khiển
SI800L.println("AT+CMGF=1");  //Lệnh đưa SMS về chế độ Text
delay(1000);  
SI800L.println("AT+CNMI=1,2,0,0,0");  // Chỉ định cách xử lý tin nhắn SMS mới đến 
delay(1000);

}

//Điều khiển mở cửa
void conServo(){
for(pos = 0; pos < 180; pos += 1){ 
        myservo.write(pos);
        delay(15);
    }
    
}

//Điều khiển khóa cửa
void conServoClose(){
for(pos = 180; pos > 0; pos --){ 
        myservo.write(pos);
        delay(15);
    }
    
}

//Hàm ma kiểm tra mật khẩu
void pass()
{
while(demsolannhap < 3 ){

 char key = keypad.getKey();

  lcd .setCursor(1,0);
  lcd.print("pass: ");
  lcd.setCursor(9, 0);
  lcd.print(password);

  //Nếu nhận tín hiệu từ bàn phím thì thực hiện
  if(key)
  {
    lcd.setCursor(9, 0);
    password[i] = key;
    i++; 
    on++;
    lcd.print(password);
  }
  //Nếu đã nhập đủ 4 ký tự thì so sánh
  if(on == 4)
  {
    if(!strcmp(password, open))
    {
      lcd.setCursor(3,1);
      lcd.print("thanh cong!");
      delay(1500);
      demsolannhap = 3;
    }
    if(strcmp(password, open))
    {
      lcd.setCursor(3,1);
      lcd.print("nhap lai");
      delay(1500);
      on = 0;
      i=0;
      demsolannhap ++;
      lcd.clear();
      char password1 = new char[5];
      password[0]= '_';
      password[1]= '_';
      password[2]= '_';
      password[3]= '_';
      if(demsolannhap == 3)
      {
        call();
        for (int i = 1;i<=15;i = i + 1) 
        {  //đếm 25s, trong khi 25s này nó vừa đủ gọi, hết 25s nó sẽ thực hiện bước tt là cúp máy
          Serial.println(i);
          delay(1000);
        }
        SI800L.println("ATH");  // thực hiện lệnh cúp máy 
        Serial.println("cup may chuyen sang gui tin nhan"); 
        message("Nhap ma pin sai!");
        delay(2000);
        Serial.println("da gui tin nhan");
      }
    }
      
  }
  }  
}

//Hàm gọi điện

void call()
{
  //cú pháp gọi: ATD 0819990184 ;\r\n
  SI800L.print (F("ATD"));
  SI800L.print (number1);
  SI800L.print (F(";\r\n"));
  
}

//Hàm gửi tin nhắn
void message(String m)
{
    // cách gửi tin nhắn B1: AT+CMGF=1   >>> OK
    /////////////////////B2: AT+CMGS="0819990184"  >>> TRA VE >   >>> NHAP TIN NHAN >>> CTRL + Z de gui

  SI800L.println("AT+CMGS=\"" + number1 + "\"\r"); 
  delay(1000);
  String SMS = m;
  SI800L.println(SMS);
  delay(100);
  SI800L.println((char)26);// ASCII code of CTRL+Z
  delay(1000);

  }

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

void loop() 
{
  //ma trận 3x4, mã pin
  password[0]= '_';
  password[1]= '_';
  password[2]= '_';
  password[3]= '_';

  pass();

//cảm biến lửa-> nếu đầu vào digital bằng 1 thì thực thi
  
  Serial.read();
  if((digitalRead(lua))==0)
    {
      Serial.println("Dang goi");
      lcd.setCursor(2, 0);
      lcd.print("co lua");
      lcd.setCursor(0, 1);
      delay(100);
      call();
      for (int i = 1;i<=15;i = i + 1) 
      {  //đếm 15s, trong khi 15s này nó vừa đủ gọi, hết 15s nó sẽ thực hiện bước tt là cúp máy
        Serial.println(i);
        delay(1000);
      }

      SI800L.println("ATH");  // thực hiện lệnh cúp máy 
      Serial.println("cup may chuyen sang gui tin nhan"); 
      message("co lua");
      delay(2000);
      Serial.println("da gui tin nhan");
   } 

   
int dk = 1;
while(dk == 1){
//Điều khiển Servo
  if(SI800L.available()){
      inputString = "";

      // Serial Buffer
      while(SI800L.available()){
        incomingByte = SI800L.read();
        inputString += incomingByte; 
        }
      
      delay(10);      

      Serial.println(inputString);
      inputString.toUpperCase(); // Uppercase the Received Message

      if (inputString.indexOf("MOCUA") > -1){
          conServo();
          }else if (inputString.indexOf("KHOACUA") > -1){
          conServoClose();
          }


        delay(1000);

        inputString = "";
  }
  dk=0;
}
//Cảm biến gas
  value = analogRead(gas);

  //Đo nồng độ và chuyển về phân trăm
  float nongdo = analogRead(A0);

  // Serial.println(nongdo);
  phantram = map(nongdo,0,1023,0,100);


    lcd.setCursor(2, 0);
    lcd.print("An toan");
    lcd.setCursor(3,1);
    lcd.print(phantram);
    lcd.setCursor(5,1);
    lcd.print("% ");
    delay(1);

    if(phantram>30)
    {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("khong an toan");
      nongdo = analogRead(A0);
      phantram = map(nongdo,0,1023,0,100); 
      lcd.setCursor(3,1);
      lcd.print(phantram);
      lcd.setCursor(5,1);
      lcd.print("% ");
      delay(1);

      while(phantram > 30 && dem < 3){
      if(dem<1){
        conServo();
      }

      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("khong an toan");
      lcd.setCursor(3,1);
      lcd.print(phantram);
      lcd.setCursor(5,1);
      lcd.print("% ");
      call();
      for (int i = 1;i<=15;i = i + 1) 
      {  //đếm 15s, trong khi 15s này nó vừa đủ gọi, hết 15s nó sẽ thực hiện bước tt là cúp máy
        Serial.println(i);
        delay(1000);
      }
    
      SI800L.println("ATH");  // thực hiện lệnh cúp máy 
      if(dem <1){
        Serial.println("cup may chuyen sang gui tin nhan"); 

         message("Khi gas o muc khong an toan");
      delay(2000);
      Serial.println("da gui tin nhan");
      }
    

      lcd.clear();

      nongdo = analogRead(A0);
      phantram = map(nongdo,0,1023,0,100);
      dem +=1;

  }
}else if(phantram < 15){
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("An toan");
    lcd.setCursor(3,1);
    lcd.print(phantram);
    lcd.setCursor(5,1);
    lcd.print("% ");
    delay(2000);
  }else if(phantram >= 15 && phantram <30){
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Canh bao 1!");
    lcd.setCursor(3,1);
    lcd.print(phantram);
    lcd.setCursor(5,1);
    lcd.print("% ");
    delay(2000);
    if(nt==1){
    message("Khi gas o muc canh bao 1!");
    nt = 0;
    }

  }

  lcd.clear();
}