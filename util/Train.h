#ifndef TRAIN_H
#define TRAIN_H

#include "Passenger.h"
#include "Station.h"
#include "VisualAsset.h"
#include <cmath>
#include <iostream>
#include <sgg/graphics.h>
#include <string>
#include <vector>

using namespace graphics;

class Train : public VisualAsset {
private:
  Brush brush;
  std::vector<Passenger *> passengers;
  int capacity = 6;
  float speed;
  float height = 38.0;
  float width = 22.0;

  Station *currentStation;
  Station *nextStation;
  Station *previousStation;

  // For smooth movement
  float t; // interpolation factor 0..1

public:
  Train(float posX, float posY, Station *startStation)
      : VisualAsset(posX, posY), capacity(6), speed(0.0005f),
        currentStation(startStation), nextStation(nullptr),
        previousStation(nullptr), t(0.0f) {
    x = posX;
    y = posY;
    // Brush so Train's colour is gray
    brush.fill_color[0] = 0.65f;
    brush.fill_color[1] = 0.65f;
    brush.fill_color[2] = 0.65f;
    brush.outline_opacity = 0.0f;

    // Pick initial random next station if available
    pickNextStation();
  }

  void pickNextStation() {
    if (!currentStation)
      return;
    const auto &connections = currentStation->getNext();
    if (connections.empty()) {
      nextStation = nullptr;
      return;
    }

    // Randomly pick next station
    std::vector<Station *> validConnections;
    for (Station *s : connections) {
      if (s != previousStation) {
        validConnections.push_back(s);
      }
    }

    // If dead end (only connection is previous), go back
    if (validConnections.empty()) {
      validConnections = connections;
    }

    // Randomly pick next station from valid connections
    int idx = std::rand() % validConnections.size();
    nextStation = validConnections[idx];
    t = 0.0f;
  }

  void arriveAtStation() {
    previousStation = currentStation;
    currentStation = nextStation;
    nextStation = nullptr;
    t = 0.0f;

    // 1. Disembark passengers
    for (auto it = passengers.begin(); it != passengers.end();) {
      Passenger *p = *it;
      if (p->getDestination() == currentStation) {
        p->setState(Passenger::COMPLETED);
        // p->setActive(false); // Maybe hide them or keep them visible at
        // station? For now, let's just remove them from train
        it = passengers.erase(it);
        std::cout << "Passenger disembarked at " << currentStation->getName()
                  << std::endl;
      } else {
        ++it;
      }
    }

    // Iterate over a copy of waiting passengers to avoid iterator
    // invalidation specific issues or just loop and remove.
    std::vector<Passenger *> boarding;
    const auto &waiting = currentStation->getWaitingPassengers();

    for (Passenger *p : waiting) {
      if ((int)passengers.size() < capacity) {
        boarding.push_back(p);
      }
    }

    for (Passenger *p : boarding) {
      currentStation->removeWaitingPassenger(p);
      p->setState(Passenger::ON_TRAIN);
      passengers.push_back(p);
      std::cout << "Passenger embarked at " << currentStation->getName()
                << std::endl;
    }

    // 3. Move to next
    pickNextStation();
  }

  // Draw Train as a Rectangle
  void draw() override {
    if (!active)
      return;

    // Rotate towards next station
    if (currentStation && nextStation) {
      // Get cords of each station
      float cx = currentStation->getX();
      float cy = currentStation->getY();
      float nx = nextStation->getX();
      float ny = nextStation->getY();
      // Calculate slope with them and use arctan to convert to degrees
      float slope = (ny - cy) / (nx - cx);
      float aa = atan(slope);
      float angle = aa * 180 / M_PI;
      float orient;
      if (cx == nx) {
        orient = 90;
      } else if (cy == ny) {
        orient = 0;
      } else {
        orient = angle;
      }
      graphics::setOrientation(angle);
    }

    drawRect(x, y, width, height, brush);
    resetPose(); // Reset rotation for other objects
  }

  void update(int ms, const MouseState &) override {
    if (!GlobalState::getInstance().isSimulating())
      return;
    if (!currentStation || !nextStation)
      return;

    // "Lerp" towards next station
    float dt = (float)ms * speed;
    t += dt;

    if (t >= 1.0f) {
      arriveAtStation();
    } else {
      // Linear interpolation
      float startX = currentStation->getX();
      float startY = currentStation->getY();
      float endX = nextStation->getX();
      float endY = nextStation->getY();

      x = startX + (endX - startX) * t;
      y = startY + (endY - startY) * t;
    }

    // Update passengers position to follow train
    // Arrange passengers in two rows, alternating positions
    float passenger_row_offset =
        height * (1.0f / 6.0f); // Offset from train center for rows
    float passenger_spacing =
        width / (static_cast<float>(capacity / 2) +
                 1.0f); // Spacing between passengers in a row

    int passenger_in_row_idx = 0; // Index for passenger within their row
    for (size_t i = 0; i < passengers.size(); ++i) {
      Passenger *p = passengers[i];
      float pasx, pasy;

      if (i % 2 == 0) // Even index: top row
      {
        pasy = y + passenger_row_offset;
        pasx =
            x - width / 2.0f + (passenger_in_row_idx + 1) * passenger_spacing;
      } else // Odd index: bottom row
      {
        pasy = y - passenger_row_offset;
        pasx =
            x - width / 2.0f + (passenger_in_row_idx + 1) * passenger_spacing;
        passenger_in_row_idx++; // Increment for the next pair of passengers
      }
      p->setPosition(pasx, pasy); // Set the new position of the passenger
    }
  };

  // Get number of Passengers
  int getPassengerCount() const { return (int)passengers.size(); }
};

#endif // TRAIN_H
