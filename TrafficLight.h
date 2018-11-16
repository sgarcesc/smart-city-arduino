#include "Arduino.h"
#include "Enumerations.h"

class TrafficLight
{
  private:
    unsigned int _redPin;
    unsigned int _yellowPin;
    unsigned int _greenPin;

    void TurnRedLightOff();
    void TurnYellowLightOff();
    void TurnGreenLightOff();
    void TurnRedLightOn();
    void TurnYellowLightOn();
    void TurnGreenLightOn();
  public:
    TrafficLight(unsigned int redPin, unsigned int yelloPin, unsigned int greenPin);
    ~TrafficLight();

    void TurnOff();
    void SetColor(TrafficLightColor color);
};