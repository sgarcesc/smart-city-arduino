#include "TrafficLight.h"

TrafficLight::TrafficLight(unsigned int redPin, unsigned int yelloPin, unsigned int greenPin)
    : _redPin(redPin), _yellowPin(yelloPin), _greenPin(greenPin)
{
}

TrafficLight::~TrafficLight()
{
}

void TrafficLight::TurnOff()
{
    TrafficLight::TurnRedLightOff();
    TrafficLight::TurnYellowLightOff();
    TrafficLight::TurnGreenLightOff();
}

void TrafficLight::SetColor(TrafficLightColor color)
{
    switch (color)
    {
    case RED:
        TrafficLight::TurnRedLightOn();
        TrafficLight::TurnYellowLightOff();
        TrafficLight::TurnGreenLightOff();
        break;
    case YELLOW:
        TrafficLight::TurnRedLightOff();
        TrafficLight::TurnYellowLightOn();
        TrafficLight::TurnGreenLightOff();
        break;
    case GREEN:
        TrafficLight::TurnRedLightOff();
        TrafficLight::TurnYellowLightOff();
        TrafficLight::TurnGreenLightOn();
        break;
    case OFF:
        TrafficLight::TurnOff();
        break;
    }
}

void TrafficLight::TurnRedLightOff()
{
    digitalWrite(_redPin, LOW);
}

void TrafficLight::TurnYellowLightOff()
{
    digitalWrite(_yellowPin, LOW);
}

void TrafficLight::TurnGreenLightOff()
{
    digitalWrite(_greenPin, LOW);
}

void TrafficLight::TurnRedLightOn()
{
    digitalWrite(_redPin, HIGH);
}

void TrafficLight::TurnYellowLightOn()
{
    digitalWrite(_yellowPin, HIGH);
}

void TrafficLight::TurnGreenLightOn()
{
    digitalWrite(_greenPin, HIGH);
}