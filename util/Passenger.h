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
        x = posX;
        y = posY;
        brush.fill_color[0] = 0.2f;
        brush.fill_color[1] = 0.6f;
        brush.fill_color[2] = 0.9f;
        brush.outline_opacity = 0.9f;
        brush.outline_width = 1.0f;
    }

    void draw() override
    {
        if (!active) return;
        drawDisk(x, y, 10, brush);
    }

    void update(int ms, const graphics::MouseState& mouse) override{};

};

#endif // PASSENGER_H
