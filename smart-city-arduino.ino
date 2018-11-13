//Name:         smart-city-arduino
//Description:  trafic light control with integrated traficc sensors, maintenance mode, midnight mode, and C02 sensors
//Authors:
//Date:         2018-11-19

//Library definition
#include <Wire.h>              //Library required for I2C comms (LCD)
#include <LiquidCrystal_I2C.h> //Library for LCD display via I2C
#include <math.h>              //Mathematics library for pow function (CO2 computation)

//I/O pin labeling
#define LDR1 0  //LDR Light sensor from traffic light 1 connected in pin A0
#define LDR2 1  //LDR Light sensor from traffic light 2 connected in pin A1
#define CO2 3   //CO2 sensor connected in pin A3
#define P1 37   //Traffic light 1 button connected in pin 37
#define P2 36   //Traffic light 2 button connected in pin 36
#define CNY1 35 //Infrared sensor 1 in traffic light 1 connected in pin 35
#define CNY2 34 //Infrared sensor 2 in traffic light 1 connected in pin 34
#define CNY3 33 //Infrared sensor 3 in traffic light 1 connected in pin 33
#define CNY4 32 //Infrared sensor 4 in traffic light 2 connected in pin 32
#define CNY5 31 //Infrared sensor 5 in traffic light 2 connected in pin 31
#define CNY6 30 //Infrared sensor 6 in traffic light 2 connected in pin 30
#define LR1 22  //Red traffic light 1 connected in pin 22
#define LY1 23  //Yellow traffic light 1 connected in pin 23
#define LG1 24  //Green traffic light 1 connected in pin 24
#define LR2 25  //Red traffic light 2 connected in pin 25
#define LG2 27  //Green traffic light 2 connected in pin 27
#define LY2 26  //Yellow traffic light 2 connected in pin 26

//Constant definitions
//->CO2
const float DC_GAIN = 8.5;                                                               //define the DC gain of amplifier CO2 sensor
const float ZERO_POINT_VOLTAGE = 0.265;                                                  //define the output of the sensor in volts when the concentration of CO2 is 400PPM
const float REACTION_VOLTAGE = 0.059;                                                    //define the “voltage drop” of the sensor when move the sensor from air into 1000ppm CO2
const float CO2Curve[3] = {2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTAGE / (2.602 - 3))}; //Line curve with 2 points
const unsigned long HALFSECOND = 500;                                                    //half second
const unsigned long ONESECOND = 1000;                                                    //one second
const unsigned long TWOSECONDS = 2000;                                                   //two seconds
const unsigned long THREESECONDS = 3000;                                                 //three seconds
const unsigned long FOURSECONDS = 4000;                                                  //four seconds
const unsigned long FIVESECONDS = 5000;                                                  //five seconds
const unsigned long SIXSECONDS = 6000;                                                   //six seconds
const unsigned long SEVENSECONDS = 7000;                                                 //seven seconds
const unsigned long EIGHTSECONDS = 8000;                                                 //eight seconds
const unsigned long NINESECONDS = 9000;                                                  //nine seconds
const unsigned long TENSECONDS = 10000;                                                  //ten seconds
const unsigned long SEQUENCETIME = FIVESECONDS;                                          //Traffic lights sequence time

//Variable definitions
char comm = '\0'; //Command to test an actuator or sensor
float volts = 0;  //Variable to store current voltage from CO2 sensor
float co2 = 0;    //Variable to store CO2 value
unsigned long previousMillis = 0;
enum trafficLightColor
{
    RED = 100,
    YELLOW = 101,
    GREEN = 102,
    OFF = 103
};                                              //Traffic light color
trafficLightColor nextTrafficLight1State = OFF; //Traffic light 1 state
trafficLightColor nextTrafficLight2State = OFF; //Traffic light 2 state
enum trafficLane
{
    ONE = 200,
    TWO = 201,
    BOTH = 203,
    NONE = 204
};
trafficLane currentTrafficLane = NONE;
unsigned int currentSequenceNumber = 0; //Current traffic light sequence

//Library definitions
LiquidCrystal_I2C lcd(0x27, 16, 4); //Set the LCD address to 0x27 for a 16 chars and 4 line display

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
    turnTrafficLights1Off();
    //Turn traffic light 2 off
    turnTrafficLights2Off();
}

void configureCommunications()
{
    Serial.begin(9600); //Start Serial communications with computer via Serial0 (TX0 RX0) at 9600 bauds
    lcd.begin();        //Start communications with LCD display
    lcd.backlight();    //Turn on LCD backlight
}

//Off
void turnTrafficLights1Off()
{
    turnRedTrafficLight1Off();
    turnYellowTrafficLight1Off();
    turnGreenTrafficLight1Off();
}

void turnTrafficLights2Off()
{
    turnRedTrafficLight2Off();
    turnYellowTrafficLight2Off();
    turnGreenTrafficLight2Off();
}

void turnRedTrafficLight1Off()
{
    digitalWrite(LR1, LOW); //Turn Off Red traffic light 1
}

void turnYellowTrafficLight1Off()
{
    digitalWrite(LY1, LOW); //Turn Off Yellow traffic light 1
}

void turnGreenTrafficLight1Off()
{
    digitalWrite(LG1, LOW); //Turn Off Green traffic light 1
}

void turnRedTrafficLight2Off()
{
    digitalWrite(LR2, LOW); //Turn Off Red traffic light 2
}

void turnYellowTrafficLight2Off()
{
    digitalWrite(LY2, LOW); //Turn Off Yellow traffic light 2
}

void turnGreenTrafficLight2Off()
{
    digitalWrite(LG2, LOW); //Turn Off Green traffic light 2
}

//On
void turnRedTrafficLight1On()
{
    digitalWrite(LR1, HIGH); //Turn On Red traffic light 1
}

void turnYellowTrafficLight1On()
{
    digitalWrite(LY1, HIGH); //Turn On Yellow traffic light 1
}

void turnGreenTrafficLight1On()
{
    digitalWrite(LG1, HIGH); //Turn On Green traffic light 1
}

void turnRedTrafficLight2On()
{
    digitalWrite(LR2, HIGH); //Turn On Red traffic light 2
}

void turnYellowTrafficLight2On()
{
    digitalWrite(LY2, HIGH); //Turn On Yellow traffic light 2
}

void turnGreenTrafficLight2On()
{
    digitalWrite(LG2, HIGH); //Turn On Green traffic light 2
}

void setTrafficLight1ToColor(trafficLightColor color)
{
    switch (color)
    {
    case RED:
        turnRedTrafficLight1On();
        turnYellowTrafficLight1Off();
        turnGreenTrafficLight1Off();
        break;
    case YELLOW:
        turnRedTrafficLight1Off();
        turnYellowTrafficLight1On();
        turnGreenTrafficLight1Off();
        break;
    case GREEN:
        turnRedTrafficLight1Off();
        turnYellowTrafficLight1Off();
        turnGreenTrafficLight1On();
        break;
    case OFF:
        turnTrafficLights1Off();
        break;
    }
}

void setTrafficLight2ToColor(trafficLightColor color)
{
    switch (color)
    {
    case RED:
        turnRedTrafficLight2On();
        turnYellowTrafficLight2Off();
        turnGreenTrafficLight2Off();
        break;
    case YELLOW:
        turnRedTrafficLight2Off();
        turnYellowTrafficLight2On();
        turnGreenTrafficLight2Off();
        break;
    case GREEN:
        turnRedTrafficLight2Off();
        turnYellowTrafficLight2Off();
        turnGreenTrafficLight2On();
        break;
    case OFF:
        turnTrafficLights2Off();
        break;
    }
}

void setTrafficLight1BlinkToColor(unsigned long currentMillis, trafficLightColor color)
{
    if ((unsigned long)(currentMillis - previousMillis <= HALFSECOND))
    {
        setTrafficLight1ToColor(color);
    }
    else if ((unsigned long)(currentMillis - previousMillis <= ONESECOND))
    {
        setTrafficLight1ToColor(OFF);
    }
    else
    {
        previousMillis = currentMillis;
    }
}

void setTrafficLight2BlinkToColor(unsigned long currentMillis, trafficLightColor color)
{
    if ((unsigned long)(currentMillis - previousMillis <= HALFSECOND))
    {
        setTrafficLight2ToColor(color);
    }
    else if ((unsigned long)(currentMillis - previousMillis <= ONESECOND))
    {
        setTrafficLight2ToColor(OFF);
    }
    else
    {
        previousMillis = currentMillis;
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

trafficLane getCurrentTrafficLane()
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

void setTrafficLight1Sequence(unsigned long currentMillis)
{
    if (currentMillis - previousMillis <= FIVESECONDS)
    {
        setTrafficLight1ToColor(GREEN);
    }
    else if (currentMillis - previousMillis <= SIXSECONDS)
    {
        setTrafficLight1ToColor(YELLOW);
    }
    else if (currentMillis - previousMillis <= 2 * SIXSECONDS)
    {
        setTrafficLight1ToColor(RED);
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
        setTrafficLight2ToColor(RED);
    }
    else if (currentMillis - previousMillis <= 2 * FIVESECONDS)
    {
        setTrafficLight2ToColor(GREEN);
    }
    else if (currentMillis - previousMillis <= 2 * SIXSECONDS)
    {
        setTrafficLight2ToColor(YELLOW);
    }
    else
    {
        previousMillis = currentMillis;
    }
}

void setTrafficLightsSequence(unsigned long currentMillis)
{
    setTrafficLight1Sequence(currentMillis);
    setTrafficLight2Sequence(currentMillis);
}

void setup(void)
{
    // Pin config
    configurePins();

    // Output cleaning
    turnTrafficLighstOff();

    // Communications
    configureCommunications();
}

void loop(void)
{
    // here is where you'd put code that needs to be running all the time.
    unsigned long currentMillis = millis();
    setTrafficLightsSequence(currentMillis);
}
