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
    Train(float posX, float posY, std::vector<Passenger> Passengers) : VisualAsset(posX, posY)
    {
        x = posX;
        y = posY;
        // brush.fill_color = {0.6f, 0.4f, 0.3f};
        passengerCount = Passengers.size();
    }
        void draw() override
        {
            drawRect(x, y, 70, 45, brush);
        }

        void removePassenger(int n) const {
              if (n > passengerCount) return;
        }
        void addPassenger() const
        {
            return;
        }

        //Get number of Passengers
        int getPassengerCount() const { return passengerCount; }

        //TODO: REMOVE/ADD PASSENGER FROM/TO TRAIN'S VECTOR (FUNCTION)
    };

#endif // TRAIN_H
