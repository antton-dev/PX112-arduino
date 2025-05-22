#include <Bounce2.h> 
#include <Wire.h>
#include "rgb_lcd.h"
Bounce2::Button button = Bounce2::Button();
bool Trad=true ;
rgb_lcd lcd;
const int btn = 4;
const int buzzerPin = 8;
const int led_td_h_m=12;
const int led_td_m_h=13;
const int chg_mode= 14;
int rateS = 625;// en microsecondes
const double tps = 300000;
int rateL = 610; // en ms
String Al= "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const String Mo[36]={
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",  // A-J
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",    // K-T
    "..-", "...-", ".--", "-..-", "-.--", "--..",                           // U-Z
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."  // 0-9
};
void trad_h_m(String data){ //Traduit du langage humain à morse
  for (int let=0;let<data.length();let++){
    char LETTRE=data[let];
    int pos = Al.indexOf(LETTRE);
    String morse=Mo[pos];
    for (int i=0; i<morse.length();i++){  
      if (morse[i]=='.'){
         for (int j=0; j< (tps/rateS)/2;j++){
            digitalWrite(buzzerPin, HIGH);    // turn on the buzzer
            delayMicroseconds(rateS);
            digitalWrite(buzzerPin, LOW);     // turn off the buzzer
            delayMicroseconds(rateS);
        }
      }
      if (morse[i]=='-'){
        for (int j=0; j<(tps*3/rateL)/2;j++){
          digitalWrite(buzzerPin, HIGH);    // turn on the buzzer
          delayMicroseconds(rateL);
          digitalWrite(buzzerPin, LOW);     // turn off the buzzer
          delayMicroseconds(rateL);
        }
      } 
      delay(200);
    }
  delay (250);
  }} 
String front_dec_intervalles(String c1, String c2 ,int temps,int inter){ //permet de rentrer des caractères en morse  
  String lettre="";
  double t1;
  double cinter=0;
  double dinter=0;
  int ok=0;
  void clear();
  while (true){
    button.update();
    if (button.isPressed()){
      t1=millis();
      button.update();
      bool valid_con= true; 
      while (valid_con){
      button.update();
      if (button.released()){
        ok=1;
        double t2=millis();
        if (t2-t1<temps){
          lettre=lettre+c1;
          Serial.println("1");
          cinter=millis();
          valid_con=false;
          lcd.setCursor(0, 0);
          lcd.print(lettre);  }
        if ((t2-t1)>=temps){
          lettre=lettre+c2;
          Serial.println("2");
          cinter=millis();
          valid_con=false;
          lcd.setCursor(0, 0);
          lcd.print(lettre);}
          }
          }
        }
    else{
      if (ok==1){
        dinter=millis();
        if ((dinter-cinter)>inter){
          Serial.println("connard");
          return lettre;
        }
      }
    }
  }
}
char lettre_m(){ //tranforme un chaine de caractère morse en lettre
  while (true){
    String lettre=front_dec_intervalles(".", "-" ,400,2000);
    Serial.println(lettre);
    for (int i=0;i<=35;i++){
      if (Mo[i]==lettre){
        Serial.println("IIIII");
        Serial.println(Al[i]);
        return Al[i];
        }
      }  
  }
  return" ";
}
String trad_m_h(){//Traduit du langage morse à humain(ici l'on assembles les phrase en des lettres)
  String phrase="";
  clear();
  while (true){
    char letre= lettre_m();
    phrase=phrase+letre;
    Serial.println(phrase);
    lcd.setCursor(0, 1);
    lcd.print(phrase);
        }
      return (phrase);
      }

void setup() {
      // set up the LCD's number of columns and rows:
      lcd.begin(16, 2);
      lcd.setRGB(255, 0, 0);
      pinMode(buzzerPin, OUTPUT);
      button.attach(btn, INPUT); 
      pinMode(2, INPUT);
      Serial.begin(9600);
      button.interval(100); 
      button.setPressedState(HIGH); 


}
void loop() {
  
  trad_m_h();
  
}