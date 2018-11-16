#pragma once

//I/O pin labeling
#define LDR1 0  //LDR Light sensor from traffic light 1 connected in pin A0
#define LDR2 1  //LDR Light sensor from traffic light 2 connected in pin A1
#define CO2 2   //CO2 sensor connected in pin A3
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