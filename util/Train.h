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
  int capacity;
  float speed;

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
    // Note: In a real app we would inject an RNG or use a static one.
    // For simplicity here using std::rand (assuming seeded in main)
    // Filter out previous station to avoid immediate U-turn
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

    // 2. Embark waiting passengers
    // We need non-const access to station to modify its waiting list
    // But Station::getWaitingPassengers returns const&.
    // We should add a method in Station to pop passengers or make it friend.
    // For this iteration, let's assume we can modify it or we rely on Station
    // methods. Wait, I didn't add pop method to Station. I added
    // removeWaitingPassenger.

    // Let's iterate over a copy of waiting passengers to avoid iterator
    // invalidation specific issues or just loop and remove.
    std::vector<Passenger *> boarding;
    const auto &waiting = currentStation->getWaitingPassengers();

    for (Passenger *p : waiting) {
      if ((int)passengers.size() >= capacity)
        break;

      // Logic check: is this train useful for the passenger?
      // For random walk train, maybe passenger just hops on?
      // Requirement: "passengers... embark... randomly (based on the
      // destination)" Let's assume they take any train for now as they don't
      // know the path.

      boarding.push_back(p);
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
      float dx = nextStation->getX() - currentStation->getX();
      float dy = nextStation->getY() - currentStation->getY();
      float angle = std::atan2(dy, dx) * 180.0f / 3.14159f;
      graphics::setOrientation(angle);
    } else {
      graphics::setOrientation(0.0f);
    }

    drawRect(x, y, 38, 22, brush);
    graphics::resetPose(); // Reset rotation for other objects
  }

  // TODO: Trains will be displayed below the Station object, doesn't completely
  // disappear tho
  void update(int ms, const graphics::MouseState &) override {
    if (!GlobalState::getInstance().isSimulating())
      return;
    if (!currentStation || !nextStation)
      return;

    // Move logic using lerp
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
    for (Passenger *p : passengers) {
      p->setPosition(x, y);
    }
  };

  // Removed old unused methods
  // TODO: REMOVE/ADD PASSENGER FROM/TO TRAIN'S VECTOR FUNCTIONS
  // Remove or Add Passenger to Train

  // Get number of Passengers
  int getPassengerCount() const { return (int)passengers.size(); }
};

#endif // TRAIN_H
