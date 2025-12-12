#ifndef TRAIN_H
#define TRAIN_H

#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <string>
#include <vector>

using namespace graphics;

class Train : public VisualAsset
{
private:
    Brush brush;
    int passengerCount;

public:
    Train(float posX, float posY, std::vector Passengers) : VisualAsset(posX, posY)
    {
        brush.fill_color[3] = {0.6f, 0.4f, 0.3f};
        passengerCount = Passengers.size();
    }
        void draw() override
        {
            drawRect(posX, posY, 70, 45, brush);
        }

        void removePassenger(int n){
              if (n > passengerCount);
        }
        void addPassenger()
        {
            return;
        }

        //Get number of Passengers
        int getPassengerCount() const { return passengerCount; }

        //TODO: REMOVE/ADD PASSENGER FROM/TO TRAIN'S VECTOR (FUNCTION)
    };

#endif // TRAIN_H
