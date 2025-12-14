#ifndef PASSENGER_H
#define PASSENGER_H

#include "VisualAsset.h"
#include <sgg/graphics.h>

class Passenger : public VisualAsset {
private:
  float radius;
  graphics::Brush brush;

public:
  Passenger(float posX, float posY) : VisualAsset(posX, posY), radius(5.0f) {
    brush.fill_color[0] = 1.0f; // R
    brush.fill_color[1] = 0.2f; // G
    brush.fill_color[2] = 0.2f; // B
    brush.outline_opacity = 0.0f;
  }

  void update(int ms, const graphics::MouseState &mouse) override {
    // No update logic for now
    (void)ms;
    (void)mouse;
  }

  void draw() override {
    if (!active)
      return;
    graphics::drawDisk(x, y, radius, brush);
  }
};

#endif // PASSENGER_H
