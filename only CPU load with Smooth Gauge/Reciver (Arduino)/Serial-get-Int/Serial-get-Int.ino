#include <LiquidCrystal_I2C.h>  

LiquidCrystal_I2C lcd(0x27, 16, 2);
int g;

byte gauge_empty[8] =  {B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111};    // empty middle piece
byte gauge_fill_1[8] = {B11111, B10000, B10000, B10000, B10000, B10000, B10000, B11111};    // filled gauge - 1 column
byte gauge_fill_2[8] = {B11111, B11000, B11000, B11000, B11000, B11000, B11000, B11111};    // filled gauge - 2 columns
byte gauge_fill_3[8] = {B11111, B11100, B11100, B11100, B11100, B11100, B11100, B11111};    // filled gauge - 3 columns
byte gauge_fill_4[8] = {B11111, B11110, B11110, B11110, B11110, B11110, B11110, B11111};    // filled gauge - 4 columns
byte gauge_fill_5[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};    // filled gauge - 5 columns
byte gauge_left[8] =   {B11111, B10000, B10000, B10000, B10000, B10000, B10000, B11111};    // left part of gauge - empty
byte gauge_right[8] =  {B11111, B00001, B00001, B00001, B00001, B00001, B00001, B11111};    // right part of gauge - empty

byte gauge_mask_left[8] = {B01111, B11111, B11111, B11111, B11111, B11111, B11111, B01111};  // mask for rounded corners for leftmost character
byte gauge_mask_right[8] = {B11110, B11111, B11111, B11111, B11111, B11111, B11111, B11110}; // mask for rounded corners for rightmost character

byte warning_icon[8] = {B00100, B00100, B01110, B01010, B11011, B11111, B11011, B11111};     // warning icon - just because we still have one custom character left

byte gauge_left_dynamic[8];   // left part of gauge dynamic - will be set in the loop function
byte gauge_right_dynamic[8];

int cpu_gauge = 0;
int move_offset = 0;     // used to shift bits for the custom characters

const int gauge_size_chars = 16;       // width of the gauge in number of characters
char gauge_string[gauge_size_chars+1]; // string that will include all the gauge character to be printed

void setup()
{
  lcd.begin();
  Serial.begin(115200);
  Serial.setTimeout(30);
  lcd.setCursor(0,0);
  lcd.print("      Hi!     ");
  delay(1000);
  lcd.clear();
  lcd.print("     Hazir");
  lcd.createChar(7, gauge_empty);   // middle empty gauge
  lcd.createChar(1, gauge_fill_1);  // filled gauge - 1 column
  lcd.createChar(2, gauge_fill_2);  // filled gauge - 2 columns
  lcd.createChar(3, gauge_fill_3);  // filled gauge - 3 columns
  lcd.createChar(4, gauge_fill_4);  // filled gauge - 4 columns  
  lcd.createChar(0, warning_icon); // warning icon - just because we have one more custom character that we could use  
}


void loop()
{
  if (Serial.available()) {
  g = Serial.readString().toInt();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CPU: %");
  lcd.print(g);
  cpu_gauge = int(g);

  float units_per_pixel = (gauge_size_chars*5.0)/100.0;        //  every character is 5px wide, we want to count from 0-100
  int value_in_pixels = round(cpu_gauge * units_per_pixel);    // cpu_gauge value converted to pixel width

  int tip_position = 0;      // 0= not set, 1=tip in first char, 2=tip in middle, 3=tip in last char
  
  if (value_in_pixels < 5) {tip_position = 1;}                            // tip is inside the first character
  else if (value_in_pixels > gauge_size_chars*5.0-5) {tip_position = 3;}  // tip is inside the last character
  else {tip_position = 2;}                                                // tip is somewhere in the middle

  move_offset = 4 - ((value_in_pixels-1) % 5);      // value for offseting the pixels for the smooth filling

  for (int i=0; i<8; i++) {   // dynamically create left part of the gauge
     if (tip_position == 1) {gauge_left_dynamic[i] = (gauge_fill_5[i] << move_offset) | gauge_left[i];}  // tip on the first character
     else {gauge_left_dynamic[i] = gauge_fill_5[i];}                                                     // tip not on the first character
 
     gauge_left_dynamic[i] = gauge_left_dynamic[i] & gauge_mask_left[i];                                 // apply mask for rounded corners
  }

  for (int i=0; i<8; i++) {   // dynamically create right part of the gauge
     if (tip_position == 3) {gauge_right_dynamic[i] = (gauge_fill_5[i] << move_offset) | gauge_right[i];}  // tip on the last character
     else {gauge_right_dynamic[i] = gauge_right[i];}                                                       // tip not on the last character

     gauge_right_dynamic[i] = gauge_right_dynamic[i] & gauge_mask_right[i];                                // apply mask for rounded corners
  }  

  lcd.createChar(5, gauge_left_dynamic);     // create custom character for the left part of the gauge
  lcd.createChar(6, gauge_right_dynamic);    // create custom character for the right part of the gauge

  for (int i=0; i<gauge_size_chars; i++) {  // set all the characters for the gauge
      if (i==0) {gauge_string[i] = byte(5);}                        // first character = custom left piece
      else if (i==gauge_size_chars-1) {gauge_string[i] = byte(6);}  // last character = custom right piece
      else {                                                        // character in the middle, could be empty, tip or fill
         if (value_in_pixels <= i*5) {gauge_string[i] = byte(7);}   // empty character
         else if (value_in_pixels > i*5 && value_in_pixels < (i+1)*5) {gauge_string[i] = byte(5-move_offset);} // tip
         else {gauge_string[i] = byte(255);}                        // filled character
      }
  }

    lcd.setCursor(0,1);
    lcd.print(gauge_string);
  }
}
