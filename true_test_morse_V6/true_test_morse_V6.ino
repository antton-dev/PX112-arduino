#include <Bounce2.h> 
#include <Wire.h>
#include "rgb_lcd.h"
#include <hidboot.h>
#include <usbhub.h>
// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
const String azerty="1234567890AZERTYUIOPQSDFGHJKLWXCVBNMAZERTYUIOPQSDFGHJKLWXCVBNM";
const String qwerty="1234567890qwertyuiopasdfghjkl;zxcvbnQWERTYUIOPASDFGHJKLZXCVBN:";
Bounce2::Button button = Bounce2::Button();
Bounce2::Button chg_bouton= Bounce2::Button();
rgb_lcd lcd;
volatile int mode = 0 ;
const int btn = 13;
const int buzzerPin = 2;
const int chg_mode= 3; 
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

char keyasc;
int keycode;
boolean iskeypressed;

class KeyboardInput : public KeyboardReportParser
{
 protected:
    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KeyboardInput::OnKeyDown(uint8_t mod, uint8_t key)
{
  uint8_t c = OemToAscii(mod, key);
  if (c)
    OnKeyPressed(c);
};

void KeyboardInput::OnKeyPressed(uint8_t key)
{
keyasc = (char) key;
keycode = (int)key;
iskeypressed = true; 
};

USB     Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);
KeyboardInput Prs;


String traduction_azerty(String phrases){
  String traduction="";
  for (int i = 0; i < len(phrases) ;i++){
    for (int k = 0; k <len(qwerty); k++)
    {
      if (qwerty[k]==phrases[i]){
      traduction=traduction + azerty[k];      }
    }
    }
    return(traduction);
  }
String clavier(){ //permet d'entrer des lettres au claviers
  bool enter_pressed=false;
  String sequence = "";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Trad : Morse");
  while (!enter_pressed && mode==0){
    Usb.Task();
    lcd.setCursor(0,0);
    lcd.print("Trad : Morse");
    if(iskeypressed){
      iskeypressed = false;  
      if (keycode==13){
        enter_pressed=true;
      }
      else{
      sequence=sequence+keyasc;
      }
    }      
    lcd.setCursor(0,1);
    lcd.print(return16(traduction_azerty(sequence)));
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Trad : Morse");
  return sequence;
}
void trad_h_m(){
  while (mode==0){
  trad_h_m_sonore(traduction_azerty(clavier()));}
  }
void trad_h_m_sonore(String data){ //Traduit du langage humain en morse sonore
  for (int let=0;let<len(data);let++){
    char LETTRE=data[let];
    int pos =0;
    for (int k = 0; k <len(Al); k++){
      if (Al[k]==LETTRE){
        pos = k;
     }
    }
    String morse=Mo[pos];
    for (int i=0; i<len(morse);i++){  
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
String front_dec_intervalles(String c1, String c2 ,int temps,int inter,int verif){ //permet de rentrer des caractères en morse  
  String lettre="";
  double t1;
  double cinter=0;
  double dinter=0;
  int ok=0;
  while (mode==verif){
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
          cinter=millis(); 
          valid_con=false; }
        if ((t2-t1)>=temps){
          lettre=lettre+c2;
          cinter=millis();
          valid_con=false;}
          }
          }
        }
    else{
      if (ok==1){
        dinter=millis();
        if ((dinter-cinter)>inter){
          return lettre;
        }
      }
    }
  }
  return "";
}
char lettre_m(){ //tranforme un chaine de caractère morse en lettre
  while (mode==1){
    String lettre=front_dec_intervalles(".", "-" ,300,700,1);
    for (int i=0;i<=35;i++){
      if (Mo[i]==lettre){
        return Al[i];
        }
      }  
  }
  return "";
}
String return16(String chaine){
  String nchaine="";
  int e=len(chaine);
  if (e>=16){
   for (int k = 0;k<16;k++){
    nchaine = chaine[e+k-17] +  nchaine;}
  }
  else{nchaine=chaine;}
  return nchaine ;
}

String trad_m_h(){//Traduit du langage morse à humain(ici l'on assembles les phrase en des lettres)
  String phrase="";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Trad : Humain");
  while (mode==1){
    char letre= lettre_m();
    phrase=phrase+letre;
    phrase=return16(phrase);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Trad : Humain");
    lcd.setCursor(0, 1);
    lcd.print(phrase);
        }
      return (phrase);
      }
int len(String o){
  int i=0; 
  while (true){
    if (o[i]=='\0')
      return i;
    i++;
  }
}
void changement(){
  mode=mode+1;
  if (mode==3)
    mode=0;
}

void morse(){
  lcd.setCursor(0,0);
  lcd.print("Morse");
  while(mode==2){
  String jsp= front_dec_intervalles(".", "-" ,300,700,2);
  for (int i=0; i<len(jsp);i++){  
      if (jsp[i]=='.'){
         for (int j=0; j< (tps/rateS)/2;j++){
            digitalWrite(buzzerPin, HIGH);    // turn on the buzzer
            delayMicroseconds(rateS);
            digitalWrite(buzzerPin, LOW);     // turn off the buzzer
            delayMicroseconds(rateS);
        }
      }
      if (jsp[i]=='-'){
        for (int j=0; j<(tps*3/rateL)/2;j++){
          digitalWrite(buzzerPin, HIGH);    // turn on the buzzer
          delayMicroseconds(rateL);
          digitalWrite(buzzerPin, LOW);     // turn off the buzzer
          delayMicroseconds(rateL);
        }
      } 
      delay(200);
    }
  }
}
void setup() {
      // set up the LCD's number of columns and rows:
      lcd.begin(16, 2);
      lcd.setRGB(255, 0, 0);
      pinMode(buzzerPin, OUTPUT);
      button.attach(btn, INPUT); 
      button.interval(100); 
      pinMode(chg_mode, INPUT);
      button.setPressedState(HIGH); 
      chg_bouton.attach(chg_mode, INPUT); 
      chg_bouton.interval(100); 
      chg_bouton.setPressedState(HIGH); 
      attachInterrupt(digitalPinToInterrupt(chg_mode), changement, RISING);
      Serial.begin( 115200 );
      #if  !defined(__MIPSEL__)
      while (!Serial); 
      #endif
      Serial.println("Start");
      if (Usb.Init() == -1)
        Serial.println("OSC did not start.");
      delay( 200 );
      HidKeyboard.SetReportParser(0, &Prs);



}
void loop() {
  lcd.clear();
  if (mode==0){
  trad_h_m();}
  else{
    if (mode==1){
    trad_m_h();}
    else{
      morse();
    }
  }
}