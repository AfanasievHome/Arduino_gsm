#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <math.h>
#include <microDS18B20.h>

#define LED_PIN   7
#define TERMIST_B 4300
#define VIN 5.0
MicroDS18B20 <A5> ds;

volatile int alarm = LOW;
int temperature = 0; // Глобальная переменная для хранения значение температуры с датчика DS18B20
int temperature2 = 0;

String val_smsin = "";

String master_phon = "+79857049155"; 
LiquidCrystal lcd(13, 12, 11, 10,  9,  8);
String message;

 
void setup ()
{ 
 lcd.begin(16, 2);
 pinMode(LED_PIN, OUTPUT);
                    
 Serial.begin(115200);  
 // delay(300);
  //Serial.println("AT+CLIP=1");  //включить АОН
 // delay(300);
//  Serial.println("AT+GSMBUSY=1"); //запрет всех входящих звонков
//  delay(300);
//  Serial.println("AT+CMGF=1"); // текстовый формат SMS
 // delay(300);
 // Serial.println("AT+CSCS=\"GSM\"");  // кодировка текста - GSM
//  delay(300);
 // Serial.println("AT+CNMI=2,2,0,0,0"); // вывод смс в консоль
 delay(300);
Serial.println("Inicial complet"); 
 }
 
void loop ()
{
 SMS_IN ();
 //delay(300);
 IF_ALARM (); 
// delay(300);
 IF_TEMP ();
// delay(300);
 IF_TEMP2 ();

 // delay(300);
 detectTemperatureLCD();
 val_smsin = "";
}

//Функция вывода на дисплей
  void Lcd_Out (String out_up, String out_down, int str_num) {
    lcd.print(" ");
    lcd.begin(16, 2);
    lcd.setCursor(0, str_num);
    lcd.print(out_up);
    lcd.print(out_down);
  }

    void detectTemperatureLCD(){
    ds.requestTemp(); //Запросили Температуру 
    delay(1000); //Ждем ответа
    //if (ds.readTemp()){ //Читаем температуру
       ds.readTemp();
      float temperature11 = ds.getTemp(); //Берем и присваиваем температуру
      String out_up = String("T1:") +  temperature11;
     //               }
      float voltage = analogRead(A0) * VIN / 1023.0;
      float r1 = voltage / (VIN - voltage);
      float temperature22 = 1./( 1./(TERMIST_B)*log(r1)+1./(25. + 273.) ) - 213;
      String out_down = String(" T2:") + temperature22;
      int str_num = 0;
      Lcd_Out(String (out_up), String (out_down), int (str_num) );
      
    }

//Разбираем СМС + проверка входящего телефона ______________________________________________ 
void SMS_IN(){  
 if(Serial.available()) //если модуль что-то послал
  { char ch = ' ';
    String val = "";
        
    while(Serial.available()) 
     {  
       ch = Serial.read();
       val += char(ch); //собираем принятые символы в строку
       delay(3);
       Serial.print(val); 
     }
  //if(val.indexOf("+CMT") > -1) //если есть входящее sms
	//	{ 
	//		if(val.indexOf(master_phon) > -1 ) //если есть входящее sms
			{val_smsin = val;}
// }
}
}

// ОХРАНА ______________________________________________
void IF_ALARM (){
if(val_smsin.indexOf("ALARM") > -1) // смотрим, что за команда
			{alarm = !alarm ;
      digitalWrite(LED_PIN, alarm);
Serial.print(" alarm IS ");  
   Serial.println(alarm); 
    }
  
                 }


                                       
//Проверка уровня сигнала___________________________________________
//AT+CSQ — информация о качестве сигнала.
//Вернет два числа +CSQ: 21,0
//21, — уровень сигнала. Может быть в диапазоне 0..31 или 99. Чем больше тем лучше, но 99 значит его отсутствие. После прошивки значение почему то изменилось до 27.


//Проверка температуры______________________________________________
void IF_TEMP (){
  if(val_smsin.indexOf("Temp") > -1) // смотрим, что за команда
				{ 
					float voltage = analogRead(A0) * VIN / 1023.0;
          float r1 = voltage / (VIN - voltage);
          float temperature = 1./( 1./(TERMIST_B)*log(r1)+1./(25. + 273.) ) - 216;
					SMS_OUT(String(temperature), String(master_phon));  //отправляем СМС на номер 
				}
}

void IF_TEMP2 (){
  if(val_smsin.indexOf("Temp2") > -1) // смотрим, что за команда
				{ 
					  ds.requestTemp(); //Запросили Температуру 
            delay(1000); //Ждем ответа
            if (ds.readTemp()){ //Читаем температуру
            float temperature = ds.getTemp(); //Берем и присваиваем температуру
            SMS_OUT(String(temperature), String(master_phon));  //отправляем СМС на номер
								}                 
}
}

 
  
//Отправка СМС ______________________________________________ 
void SMS_OUT(String text, String phone)
{

  //Serial.println("AT+CMGS=\"" + phone + "\"");
 // delay(500);

  //Serial.print(text);
   Serial.println(text);

  delay(500);
  
  Serial.print((char)26);
  delay(500);
  Serial.println("SMS send OK");
  delay(5000);
  //Serial.println("AT+CMGD=1,4");//удаляем прочитанные смс
  //Serial.println("AT+CMGDA=\"DEL ALL\"");//еще вариант удаления всех смс 
       
}
