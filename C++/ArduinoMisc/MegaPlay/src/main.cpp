#include <Arduino.h>


// include the library code:
#include <LiquidCrystal.h>

// Bosley's LED
#include <BLed.h>
#include <BButtons.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int pinButtonZero    = 2, 
    pinButtonOne     = 3, 
    pinButtonTwo     = 4, 
    pinButtonThree   = 5, 
    pinButtonFour    = 6;

// 4 Pin RGB Control
RGBLed rgbLed(22, 24, 26);

// Control the buttons!
BTNS::BButtons * buttonManager;

void button0()
{
    lcd.setCursor(0, 0);
    lcd.print("Push: 0");
    rgbLed.enableRed();
}
void button1()
{
    lcd.setCursor(0, 0);
    lcd.print("Push: 1");
    rgbLed.enableBlue();
}
void button2()
{
    lcd.setCursor(0, 0);
    lcd.print("Push: 2");
    rgbLed.enableGreen();
}
void button3()
{
    lcd.setCursor(0, 0);
    lcd.print("Push: 3");
    rgbLed.off();
}
void button4()
{
    lcd.setCursor(0, 0);
    lcd.print("Push: 4");
}


void setup() {
    lcd.begin(16, 2);

    pinMode(pinButtonZero,    INPUT);
    pinMode(pinButtonOne,     INPUT);
    pinMode(pinButtonTwo,     INPUT);
    pinMode(pinButtonThree,   INPUT);
    pinMode(pinButtonFour,    INPUT);

    buttonManager = new BTNS::BButtons();
    buttonManager->addButton(pinButtonZero,     BTNS::buttonCallback (button0) );
    buttonManager->addButton(pinButtonOne,      BTNS::buttonCallback (button1) );
    buttonManager->addButton(pinButtonTwo,      BTNS::buttonCallback (button2) );
    buttonManager->addButton(pinButtonThree,    BTNS::buttonCallback (button3) );
    buttonManager->addButton(pinButtonFour,     BTNS::buttonCallback (button4) );
}

void loop() {
    lcd.setCursor(0, 1);
    lcd.print(millis() / 1000);

    lcd.setCursor(0, 0);
    lcd.print("Waiting");

    buttonManager->tick();
    delay(50);
}
