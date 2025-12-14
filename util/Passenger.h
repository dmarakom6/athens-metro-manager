#ifndef PASSENGER_H
#define PASSENGER_H

#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <string>

// Forward declaration to avoid circular dependencies
class Station;

class Passenger : public VisualAsset {
public:
  enum State { WAITING, ON_TRAIN, COMPLETED };

private:
  float radius;
  graphics::Brush brush;
  Station *destination;
  State state;

public:
  Passenger(float posX, float posY, Station *dest)
      : VisualAsset(posX, posY), radius(4.0f), destination(dest),
        state(WAITING) {
    brush.fill_color[0] = 1.0f; // R
    brush.fill_color[1] = 0.2f; // G
    brush.fill_color[2] = 0.2f; // B
    brush.outline_opacity = 0.0f;
  }

  void update(int ms, const graphics::MouseState &mouse) override {
    (void)ms;
    (void)mouse;
    // Logic handled by Train/Station mostly
  }

  void draw() override {
    if (!active || state == COMPLETED)
      return;

    // Draw slightly different if waiting
    if (state == WAITING) {
      graphics::drawDisk(x, y, radius, brush);
    } else if (state == ON_TRAIN) {
      graphics::drawDisk(x, y, radius, brush); // Draw on train too
    }
  }

  Station *getDestination() const { return destination; }
  State getState() const { return state; }
  void setState(State s) { state = s; }
};

#endif // PASSENGER_H
