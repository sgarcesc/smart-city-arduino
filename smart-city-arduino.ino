//Name:         smart-city-arduino
//Description:  trafic light control with integrated traficc sensors, maintenance mode, midnight mode, and C02 sensors
//Authors:
//Date:         2018-11-19

//Library definition
#include <Wire.h>              //Library required for I2C comms (LCD)
#include <LiquidCrystal_I2C.h> //Library for LCD display via I2C
#include <math.h>              //Mathematics library for pow function (CO2 computation)

#include "Configuration.h"
#include "Enumerations.h"
#include "TrafficLight.h"

//Variable definitions
char comm = '\0'; //Command to test an actuator or sensor
float volts = 0;  //Variable to store current voltage from CO2 sensor
float co2 = 0;    //Variable to store CO2 value
unsigned long previousMillis = 0;
unsigned long previousMillisBlink = 0;
unsigned long previousMillisMaintenance = 0;
unsigned int currentSequenceNumber = 0; //Current traffic light sequence
bool isInMaintenance = false;
bool changeToMaintenance = false;

//Library definitions
LiquidCrystal_I2C lcd(0x27, 16, 4); //Set the LCD address to 0x27 for a 16 chars and 4 line display

TrafficLight trafficLight1(LR1, LY1, LG1);
TrafficLight trafficLight2(LR1, LY2, LG2);

void configurePins()
{
    pinMode(P1, INPUT);   //Traffic light 1 button as Input
    pinMode(P2, INPUT);   //Traffic light 2 button as Input
    pinMode(CNY1, INPUT); //Infrared sensor 1 in traffic light 1 as Input
    pinMode(CNY2, INPUT); //Infrared sensor 2 in traffic light 1 as Input
    pinMode(CNY3, INPUT); //Infrared sensor 3 in traffic light 1 as Input
    pinMode(CNY4, INPUT); //Infrared sensor 4 in traffic light 2 as Input
    pinMode(CNY5, INPUT); //Infrared sensor 5 in traffic light 2 as Input
    pinMode(CNY6, INPUT); //Infrared sensor 6 in traffic light 2 as Input
    pinMode(LR1, OUTPUT); //Red traffic light 1 as Output
    pinMode(LY1, OUTPUT); //Yellow traffic light 1 as Output
    pinMode(LG1, OUTPUT); //Green traffic light 1 as Output
    pinMode(LR2, OUTPUT); //Red traffic light 2 as Output
    pinMode(LY2, OUTPUT); //Yellow traffic light 2 as Output
    pinMode(LG2, OUTPUT); //Green traffic light 2 as Output
}

void turnTrafficLighstOff()
{
    //Turn traffic light 1 off
    trafficLight1.TurnOff();
    //Turn traffic light 2 off
    trafficLight2.TurnOff();
}

void configureCommunications()
{
    Serial.begin(9600); //Start Serial communications with computer via Serial0 (TX0 RX0) at 9600 bauds
    lcd.begin();        //Start communications with LCD display
    lcd.backlight();    //Turn on LCD backlight
}

void setTrafficLight1BlinkToColor(unsigned long currentMillis, TrafficLightColor color)
{
    if ((unsigned long)(currentMillis - previousMillisBlink <= HALFSECOND))
    {
        trafficLight1.SetColor(color);
    }
    else if ((unsigned long)(currentMillis - previousMillisBlink <= ONESECOND))
    {
        trafficLight1.SetColor(OFF);
    }
    else
    {
        previousMillisBlink = currentMillis;
    }
}

void setTrafficLight2BlinkToColor(unsigned long currentMillis, TrafficLightColor color)
{
    if ((unsigned long)(currentMillis - previousMillisBlink <= HALFSECOND))
    {
        trafficLight2.SetColor(color);
    }
    else if ((unsigned long)(currentMillis - previousMillisBlink <= ONESECOND))
    {
        trafficLight2.SetColor(OFF);
    }
    else
    {
        previousMillisBlink = currentMillis;
    }
}

bool isInfraredSensor1On()
{
    return digitalRead(CNY1) == LOW;
}

bool isInfraredSensor2On()
{
    return digitalRead(CNY2) == LOW;
}

bool isInfraredSensor3On()
{
    return digitalRead(CNY3) == LOW;
}

bool isTrafficPresentOnLine1()
{
    return isInfraredSensor1On() || isInfraredSensor2On() || isInfraredSensor3On();
}

bool isInfraredSensor4On()
{
    return digitalRead(CNY4) == LOW;
}

bool isInfraredSensor5On()
{
    return digitalRead(CNY5) == LOW;
}

bool isInfraredSensor6On()
{
    return digitalRead(CNY6) == LOW;
}

bool isTrafficPresentOnLine2()
{
    return isInfraredSensor4On() || isInfraredSensor5On() || isInfraredSensor6On();
}

bool isTrafficLight1ButtonPressed()
{
    return digitalRead(P1) == HIGH;
}

bool isTrafficLight2ButtonPressed()
{
    return digitalRead(P2) == HIGH;
}

bool areBothTrafficLightButtonsPressed()
{
    return isTrafficLight1ButtonPressed() && isTrafficLight2ButtonPressed();
}

bool isInMaintenanceMode(unsigned long currentMillis)
{
    if (areBothTrafficLightButtonsPressed())
    {
        if (currentMillis - previousMillisMaintenance >= FIVESECONDS && changeToMaintenance == true)
        {
            changeToMaintenance = false;
            if (isInMaintenance)
            {
                isInMaintenance = false;
                return isInMaintenance;
            }
            isInMaintenance = true;
            return isInMaintenance;
        }
    }
    else
    {
        changeToMaintenance = true;
        previousMillisMaintenance = currentMillis;
    }
}

int getLightSensorFromTrafficLight1Reading()
{
    return analogRead(LDR1);
}

int getLightSensorFromTrafficLight2Reading()
{
    return analogRead(LDR2);
}

bool isLightSensorFromTrafficLight1InNightmode()
{
    return getLightSensorFromTrafficLight1Reading() < 50;
}

bool isLightSensorFromTrafficLight2InNightmode()
{
    return getLightSensorFromTrafficLight2Reading() < 50;
}

bool isIndNightMode()
{
    return isLightSensorFromTrafficLight1InNightmode() && isLightSensorFromTrafficLight2InNightmode();
}

TrafficLightsMode getCurrentTrafficLigthsMode(unsigned long currentMillis)
{
    if (isInMaintenanceMode(currentMillis))
    {
        return MAINTENANCE;
    }
    if (isIndNightMode())
    {
        return NIGHT;
    }
    return REGULAR;
}

TrafficLane getCurrentTrafficLane()
{
    if (isTrafficPresentOnLine1() && !isTrafficPresentOnLine2())
    {
        return ONE;
    }
    if (!isTrafficPresentOnLine1() && isTrafficPresentOnLine2())
    {
        return TWO;
    }
    if (isTrafficPresentOnLine1() && isTrafficPresentOnLine2())
    {
        return BOTH;
    }
    return NONE;
}

void setTrafficLightsMaintenance(unsigned long currentMillis)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Careful!!!");
    lcd.setCursor(0, 1);
    lcd.print("Traffic lights");
    lcd.setCursor(0, 2);
    lcd.print("Under maintenance");
    setTrafficLight1BlinkToColor(currentMillis, YELLOW);
    setTrafficLight2BlinkToColor(currentMillis, YELLOW);
}

void setTrafficLightsNight(unsigned long currentMillis)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Careful!!!");
    setTrafficLight1BlinkToColor(currentMillis, YELLOW);
    setTrafficLight2BlinkToColor(currentMillis, RED);
}

void setTrafficLight1Sequence(unsigned long currentMillis)
{
    if (currentMillis - previousMillis <= FIVESECONDS)
    {
        trafficLight1.SetColor(GREEN);
    }
    else if (currentMillis - previousMillis <= SIXSECONDS)
    {
        trafficLight1.SetColor(YELLOW);
    }
    else if (currentMillis - previousMillis <= 2 * SIXSECONDS)
    {
        trafficLight1.SetColor(RED);
    }
    else
    {
        previousMillis = currentMillis;
    }
}

void setTrafficLight2Sequence(unsigned long currentMillis)
{
    if (currentMillis - previousMillis <= SIXSECONDS)
    {
        trafficLight2.SetColor(RED);
    }
    else if (currentMillis - previousMillis <= 2 * FIVESECONDS)
    {
        trafficLight2.SetColor(GREEN);
    }
    else if (currentMillis - previousMillis <= 2 * SIXSECONDS)
    {
        trafficLight2.SetColor(YELLOW);
    }
    else
    {
        previousMillis = currentMillis;
    }
}

void setTrafficLightsSequence(unsigned long currentMillis)
{
    lcd.clear();
    TrafficLane currentLane = getCurrentTrafficLane();
    switch (currentLane)
    {
    case ONE:
        if (currentMillis - previousMillis >= 2 * SIXSECONDS)
        {
            trafficLight1.SetColor(GREEN);
            trafficLight2.SetColor(RED);
            previousMillis = currentMillis;
        }
        break;
    case TWO:
        if (currentMillis - previousMillis >= 2 * SIXSECONDS)
        {
            trafficLight1.SetColor(RED);
            trafficLight2.SetColor(GREEN);
            previousMillis = currentMillis;
        }
        break;
    case BOTH:
        if (currentSequenceNumber < 2)
        {
            if (currentMillis - previousMillis >= SIXSECONDS)
            {
                trafficLight1.SetColor(GREEN);
                trafficLight2.SetColor(RED);
                previousMillis = currentMillis;
                currentSequenceNumber++;
            }
        }
        else
        {
            if (currentMillis - previousMillis >= SIXSECONDS)
            {
                trafficLight1.SetColor(RED);
                trafficLight2.SetColor(GREEN);
                previousMillis = currentMillis;
                currentSequenceNumber = 0;
            }
        }

        break;
    case NONE:
        setTrafficLight1Sequence(currentMillis);
        setTrafficLight2Sequence(currentMillis);
        break;
    }
}

void setup(void)
{
    // Pin config
    configurePins();

    // Output cleaning
    turnTrafficLighstOff();

    // Communications
    configureCommunications();

    delay(ONESECOND);
}

void loop(void)
{
    // here is where you'd put code that needs to be running all the time.
    unsigned long currentMillis = millis();
    TrafficLightsMode lightsMode = getCurrentTrafficLigthsMode(currentMillis);
    lcd.setCursor(0, 0);
    lcd.print(String("Welcome!"));
    switch (lightsMode)
    {
    case REGULAR:
        setTrafficLightsSequence(currentMillis);
        break;
    case MAINTENANCE:
        setTrafficLightsMaintenance(currentMillis);
        break;
    case NIGHT:
        setTrafficLightsNight(currentMillis);
        break;
    default:
        setTrafficLightsSequence(currentMillis);
        break;
    }
}
