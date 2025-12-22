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
  graphics::Brush waitingBrush;
  graphics::Brush onTrainBrush;
  Station *destination;
  State state;

public:
  Passenger(float posX, float posY, Station *dest)
      : VisualAsset(posX, posY), radius(4.0f), destination(dest),
        state(WAITING) {
    waitingBrush.fill_color[0] = 1.0f; // R
    waitingBrush.fill_color[1] = 0.2f; // G
    waitingBrush.fill_color[2] = 0.2f; // B
    waitingBrush.outline_opacity = 0.0f;

    onTrainBrush.fill_color[0] = 1.0f; // R
    onTrainBrush.fill_color[1] = 0.7f; // G
    onTrainBrush.fill_color[2] = 0.1f; // B
    onTrainBrush.outline_opacity = 1.0f;
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
      graphics::drawDisk(x, y, radius, waitingBrush);
    } else if (state == ON_TRAIN) {
      graphics::drawDisk(x, y, radius, onTrainBrush); // Draw on train too
    }
  }

  Station *getDestination() const { return destination; }
  State getState() const { return state; }
  void setState(State s) { state = s; }
};

#endif // PASSENGER_H
