#include <LiquidCrystal_I2C.h>  
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte degreeChar[8] = {
  0b01110,
  0b01010,
  0b01110,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
boolean conL = false;
int l, t, ind1;
unsigned long lastT;
char c;
String s;

void setup()
{
  lcd.begin();
  Serial.begin(115200);
  Serial.setTimeout(30);
  lcd.setCursor(0,0);
  lcd.print("      Hi!     ");
  lcd.createChar(0, degreeChar);
  delay(500);
  lcd.clear();
  lcd.print("     Ready");
}


void loop()
{
  if (Serial.available()) {
    c = Serial.read();  //gets one byte from serial buffers
    if (c == '*') {
      s = Serial.readString();
      ind1 = s.indexOf(',');
      l = s.substring(0, ind1).toInt();
      t = s.substring(ind1+1).toInt();

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CPU Load    Temp");
    
      lcd.setCursor(4,1);
      lcd.print("%");
      lcd.print(l);
      lcd.setCursor(12,1);
      lcd.print(t);
      lcd.print("C");
      lcd.write((byte)0);
      
      lastT = millis();
      conL = true;
    }
  }
  else if (millis()- lastT > 3000 && conL) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connection Lost");
    conL = false;
  }
}
