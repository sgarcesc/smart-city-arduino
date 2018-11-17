//Name:         Ciudad inteligente
//Description:  
//Author:       Roberto Alejandro
//              Sebastian Garces
//              Giancarlo Corredor
//              Harol Perez Giraldo
//Date:         17/11/2018

//Library definition
#include <Wire.h> //Library required for I2C comms (LCD)
#include <LiquidCrystal_I2C.h>  //Library for LCD display via I2C
#include <math.h> //Mathematics library for pow function (CO2 computation)

//I/O pin labeling
#define LDR1 0 //LDR Light sensor from traffic light 1 connected in pin A0
#define LDR2 1  //LDR Light sensor from traffic light 2 connected in pin A1
#define CO2 2  //CO2 sensor connected in pin A3
#define P1 37  //Traffic light 1 button connected in pin 37
#define P2 36  //Traffic light 2 button connected in pin 36
#define CNY1 35 //Infrared sensor 1 in traffic light 1 connected in pin 35
#define CNY2 34 //Infrared sensor 2 in traffic light 1 connected in pin 34
#define CNY3 33 //Infrared sensor 3 in traffic light 1 connected in pin 33
#define CNY4 32 //Infrared sensor 4 in traffic light 2 connected in pin 32
#define CNY5 31 //Infrared sensor 5 in traffic light 2 connected in pin 31
#define CNY6 30 //Infrared sensor 6 in traffic light 2 connected in pin 30
#define LR1 22 //Red traffic light 1 connected in pin 22
#define LY1 23 //Yellow traffic light 1 connected in pin 23
#define LG1 24 //Green traffic light 1 connected in pin 24
#define LR2 25 //Red traffic light 2 connected in pin 25
#define LG2 27 //Green traffic light 2 connected in pin 27
#define LY2 26 //Yellow traffic light 2 connected in pin 26


//Constant definitions
//->CO2
const float DC_GAIN = 8.5;  //define the DC gain of amplifier CO2 sensor
//const float ZERO_POINT_VOLTAGE = 0.4329; //define the output of the sensor in volts when the concentration of CO2 is 400PPM
const float ZERO_POINT_VOLTAGE = 0.265; //define the output of the sensor in volts when the concentration of CO2 is 400PPM
const float REACTION_VOLTAGE = 0.059;   //define the “voltage drop” of the sensor when move the sensor from air into 1000ppm CO2
const float CO2Curve[3] = {2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTAGE / (2.602 - 3))}; //Line curve with 2 points

//Variable definitions
float volts = 0;  //Variable to store current voltage from CO2 sensor
float co2 = 0;  //Variable to store CO2 value

//Library definitions
LiquidCrystal_I2C lcd(0x27, 16, 4); //Set the LCD address to 0x27 for a 16 chars and 4 line display

const unsigned long TSem = 5000;
unsigned long tini = 0;
unsigned long tiniMantenimiento = 0;
unsigned long tiniCO2 = 0;
int ciclo = 0;
bool mantenimiento = false;
bool cambioMantenimiento = false;
bool activoCO2 = false;
bool activoSensorLuz = false;

//Configuration
void setup() {
  //Pin config
  pinMode(P1, INPUT); //Traffic light 1 button as Input
  pinMode(P2, INPUT); //Traffic light 2 button as Input
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

  //Output cleaning
  digitalWrite(LR1, LOW); //Turn Off Red traffic light 1
  digitalWrite(LY1, LOW); //Turn Off Yellow traffic light 1
  digitalWrite(LG1, LOW); //Turn Off Green traffic light 1
  digitalWrite(LR2, LOW); //Turn Off Red traffic light 2
  digitalWrite(LY2, LOW); //Turn Off Yellow traffic light 2
  digitalWrite(LG2, LOW); //Turn Off Green traffic light 2

  //Communications
  Serial.begin(9600); //Start Serial communications with computer via Serial0 (TX0 RX0) at 9600 bauds
  lcd.begin();  //Start communications with LCD display
  lcd.backlight();  //Turn on LCD backlight

  tini = millis();
  semaforo_1(1);
  semaforo_2(1);
}

  void Semaforos(){
    if (millis() - tini <= TSem){
      semaforo_2(1);
      semaforo_1(3);      
    }
    else if (millis() - tini <= 1.1*TSem){
      semaforo_2(1);
      semaforo_1(4); 
    } else if (millis() - tini <= 2*TSem){    
      semaforo_1(1); 
      semaforo_2(3);
    } else if (millis() - tini <= 2.1*TSem){    
      semaforo_1(1); 
      semaforo_2(4);
    }
    else {
      tini = millis();
    }
  }

  // 1: Rojo ON
  // 2: Amarrillo ON
  // 3: Verde ON
  // 4: Amarrillo ON, Verde ON
  void semaforo_1(int opcion){
    if (opcion == 1){
      digitalWrite(LR1, HIGH);
      digitalWrite(LY1, LOW);
      digitalWrite(LG1, LOW);
    } else if (opcion == 2){
      digitalWrite(LR1, LOW);
      digitalWrite(LY1, HIGH);
      digitalWrite(LG1, LOW);
    } else if (opcion == 3){
      digitalWrite(LR1, LOW);
      digitalWrite(LY1, LOW);
      digitalWrite(LG1, HIGH);
    } else if (opcion == 4){
      digitalWrite(LR1, LOW);
      digitalWrite(LY1, HIGH);
      digitalWrite(LG1, HIGH);
    }
  }

  // 1: Rojo ON
  // 2: Amarrillo ON
  // 3: Verde ON
  // 4: Amarrillo ON, Verde ON
  void semaforo_2(int opcion){
    if (opcion == 1){
      digitalWrite(LR2, HIGH);
      digitalWrite(LY2, LOW);
      digitalWrite(LG2, LOW);
    } else if (opcion == 2){
      digitalWrite(LR2, LOW);
      digitalWrite(LY2, HIGH);
      digitalWrite(LG2, LOW);
    } else if (opcion == 3){
      digitalWrite(LR2, LOW);
      digitalWrite(LY2, LOW);
      digitalWrite(LG2, HIGH);
    }else if (opcion == 4){
      digitalWrite(LR2, LOW);
      digitalWrite(LY2, HIGH);
      digitalWrite(LG2, HIGH);
    }
  }

  void MantenimientoSemaforo()
  { 
    if (millis() - tini <= 500){
      semaforo_1(2);
      semaforo_2(2);
    }else if (millis() - tini <= 1000) {
      digitalWrite(LY1, LOW);
      digitalWrite(LY2, LOW);
    } else{
      tini = millis();
    }
  }

  void IntermitenciaSemaforo()
  { 
    if (millis() - tini <= 500){
      semaforo_1(2);
      semaforo_2(1);
    }else if (millis() - tini <= 1000) {
      digitalWrite(LY1, LOW);
      digitalWrite(LR2, LOW);
    } else{
      tini = millis();
    }

    if (activoSensorLuz == false) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Transite Con");
      lcd.setCursor(0, 1);
      lcd.print("Precaucion");
    }
  }

  bool ViaPrincipal() {
    return digitalRead(CNY1) == LOW || digitalRead(CNY2) == LOW || digitalRead(CNY3) == LOW;
  }

  bool ViaTunel(){
    return digitalRead(CNY4) == LOW || digitalRead(CNY5) == LOW || digitalRead(CNY6) == LOW;
  }

  void GetCO2(){
      volts = analogRead(CO2) * 5.0 / 1023.0;  //Convert CO2 ADC to volts
        
      if (volts / DC_GAIN >= ZERO_POINT_VOLTAGE) {   
          co2 = 0;
      }
      else {
          co2 = pow(10, ((volts / DC_GAIN) - CO2Curve[1]) / CO2Curve[2] + CO2Curve[0]);
      }
  }

  void loop() {

  // Semaforos en mantenimiento
    if (digitalRead(P1) == HIGH && digitalRead(P2) == HIGH)
    {
      if (millis() - tiniMantenimiento >= 5000 && cambioMantenimiento == true)
      {
        if (mantenimiento == false)
        {
          mantenimiento = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Atencion!!!");
          lcd.setCursor(0, 1);
          lcd.print("Mantenimiento De");
          lcd.setCursor(0, 2);
          lcd.print("Semaforos");
          cambioMantenimiento = false;       
        }
        else {
          mantenimiento = false;        
          cambioMantenimiento = false;
          lcd.clear();
        }
      }
    }
    else
    {
      cambioMantenimiento = true;
      tiniMantenimiento = millis();    
    }
    // Fin Semaforos en mantenimiento

    if (mantenimiento){
    MantenimientoSemaforo(); 
    } else if (analogRead(LDR1) < 100 && analogRead(LDR2) < 100) {  // Intermitencia nocturna
      IntermitenciaSemaforo();    
      activoSensorLuz = true;
    }  // Fin Intermitencia nocturna
    else {    
      activoSensorLuz = false;
      // CO2
      //Serial.println(String(GetCO2()));
      if (millis() - tiniCO2 >= 5000)
      {
          GetCO2();
          tiniCO2 = millis();
      }

      if (co2 > 10000) {
        if (activoCO2 == false)
        {
          activoCO2 = true;
          lcd.setCursor(0, 0);
          lcd.print("Nivel alto de CO2");
          lcd.setCursor(0, 1);
          lcd.print("Reduzca su velocidad");
          semaforo_1(1);
          semaforo_2(3);
        }
      }
      else {        
        lcd.clear();    
        activoCO2 = false;
      
        if (ViaPrincipal() == false && ViaTunel() == false){
          Semaforos();
        } else if (ViaTunel() && ViaPrincipal() == false) {
          if (millis() - tini >= 2.1*TSem){
              semaforo_1(1);
              semaforo_2(4);
              semaforo_2(3);
              tini = millis();
          }
        } else if (ViaTunel() && ViaPrincipal()) {
          if (ciclo <= 1){
            if (millis() - tini >= TSem){
              semaforo_1(4);
              semaforo_1(3);
              semaforo_2(1);
              ciclo++;
              tini = millis();
            }
          }else{
            if (millis() - tini >= TSem){
              ciclo = 0;
              semaforo_1(1);
              semaforo_2(4);
              semaforo_2(3);
              tini = millis();
            }
          }  
        } else if (ViaTunel() == false && ViaPrincipal()) {
          if (millis() - tini >= 2.1*TSem){
            semaforo_1(3);
            semaforo_2(1);
            tini = millis();
          }    
        } 
      }  
    }
  }