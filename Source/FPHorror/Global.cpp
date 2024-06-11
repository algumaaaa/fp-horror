#define _USE_MATH_DEFINES

#include "Global.h"
#include <math.h>
#include "Engine/GameEngine.h"

double Global::EaseInOutElastic(double x)
{
    double c5 = (2 * M_PI) / 4.5;
    if (x == 0) {
        return 0;
    } else if (x == 1) {
        return 1;
    } else if (x < 0.5) {
        return -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2;
    } else {
        return (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
    }
}

double Global::EaseInOutQuart(double x)
{
    return x < 0.5 ? 8 * pow(x, 4) : 1 - pow(-2 * x + 2, 4) / 2;
}

double Global::EaseOutExpo(double x) 
{
    return (x == 1) ? 1 : 1 - pow(2, -10 * x);
}

void Global::Print(FString s)
{
    if (GEngine)
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, s,false);
}