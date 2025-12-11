#ifndef ATHENSMETROMANAGER_PASSENGER_H
#define ATHENSMETROMANAGER_PASSENGER_H

#endif //ATHENSMETROMANAGER_PASSENGER_H
#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <string>
using namespace graphics;

class Station : public VisualAsset
{
private:
    Brush brush;

public:
    Passenger(float posX, float posY, const std::string& stationName, float r = 20.0f)
        : VisualAsset(posX, posY),

    {
        brush.fill_color[0] = 0.2f;
        brush.fill_color[1] = 0.6f;
        brush.fill_color[2] = 0.9f;
        brush.outline_opacity = 1.0f;
        brush.outline_width = 3.0f;
    }

    void draw() override
    {
        if (!active) return;
        graphics::drawRect(x, y, 100, 200, brush);
    }

    // Getters and setters
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
};

#endif // STATION_H
