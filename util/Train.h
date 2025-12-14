#ifndef TRAIN_H
#define TRAIN_H

#include "Passenger.h"
#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <string>
#include <vector>

using namespace graphics;

class Train : public VisualAsset {
private:
  Brush brush;
  int passengerCount;

public:
  Train(float posX, float posY, std::vector<Passenger *> Passengers)
      : VisualAsset(posX, posY) {
    x = posX;
    y = posY;
    // Brush so Train's colour is gray
    brush.fill_color[0] = 0.65f;
    brush.fill_color[1] = 0.65f;
    brush.fill_color[2] = 0.65f;
    brush.outline_opacity = 0.0f;
    passengerCount = Passengers.size();
  }

  // Draw Train as a Rectangle
  void draw() override {
    if (!active)
      return;
    drawRect(x, y, 38, 22, brush);
  }

  // TODO: Trains will be displayed below the Station object, doesn't completely
  // disappear tho
  void update(int, const graphics::MouseState &) override {};

  // TODO: REMOVE/ADD PASSENGER FROM/TO TRAIN'S VECTOR FUNCTIONS
  // Remove or Add Passenger to Train
  void removePassenger(int n) {
    if (n > passengerCount)
      return;
  }

  void addPassenger(int) { return; }

  // Get number of Passengers
  int getPassengerCount() const { return passengerCount; }
};

#endif // TRAIN_H
