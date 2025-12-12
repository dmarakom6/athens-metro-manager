#ifndef PASSENGER_H
#define PASSENGER_H

#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <string>
using namespace graphics;

class Passenger : public VisualAsset
{
private:
    Brush brush;
public:
    Passenger(float posX, float posY)
    : VisualAsset(posX, posY)
    {
        brush.fill_color[0] = 0.2f;
        brush.fill_color[1] = 0.6f;
        brush.fill_color[2] = 0.9f;
        brush.outline_opacity = 0.9f;
        brush.outline_width = 1.0f;
    }

    void draw() override
    {
        if (!active) return;
        drawDisk(posX, posY, 100, 200, brush);
    }

    //Get
    int getPassengerCount() const { return passengerCount; }
};

#endif // PASSENGER_H
