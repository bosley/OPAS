#include "Oled.h"

#define ROW_HEADER     15
#define ROW_LINE_ONE   30
#define ROW_LINE_TWO   45
#define ROW_LINE_THREE 60

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,SCL,SDA,U8X8_PIN_NONE);

Oled::Oled()
{
    pinMode(4, OUTPUT);
    digitalWrite(4, 0);	
    u8g2.begin();
}

void Oled::flip(const char *status, const char *message0, const char *message1)
{
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setFont(u8g2_font_10x20_tr);
    u8g2.drawStr(0, ROW_HEADER, OLED_TEXT_HEADER);
    u8g2.drawStr(0, ROW_LINE_ONE,   status);
    u8g2.drawStr(0, ROW_LINE_TWO,   message0);
    u8g2.drawStr(0, ROW_LINE_THREE, message1);
    u8g2.sendBuffer();
}