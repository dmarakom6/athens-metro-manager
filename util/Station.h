#ifndef STATION_H
#define STATION_H

#include "Passenger.h"
#include "VisualAsset.h"
#include <algorithm>
#include <iostream>
#include <sgg/graphics.h>
#include <string>
#include <vector>

class Station : public VisualAsset {
private:
  std::string name;
  float radius;
  graphics::Brush brush;
  int passengerCount;
  std::vector<Station *> next;
  std::vector<Station *> prev;
  std::vector<Passenger *> waitingPassengers;

  // Dragging state
  bool isDragging;
  float dragOffsetX;
  float dragOffsetY;

  // GLOBAL LOCK: This ensures only one station can be dragged at a time
  static Station *s_active_dragging_station;

  float x2 = -1;
  float y2 = -1;

public:
  Station(float posX, float posY, const std::string &stationName,
          float r = 15.0f)
      : VisualAsset(posX, posY), name(stationName), radius(r),
        passengerCount(0), isDragging(false), dragOffsetX(0.0f),
        dragOffsetY(0.0f) {

    brush.fill_color[0] = 0.2f;
    brush.fill_color[1] = 0.6f;
    brush.fill_color[2] = 0.9f;
    brush.outline_opacity = 1.0f;
    brush.outline_width = 3.0f;
  }

  void addNext(Station *other) {
    if (other) {
      next.push_back(other);
      other->prev.push_back(this);
    }
  }

  /**
   * @brief Update the station state
   * @param ms Milliseconds elapsed since last update
   * @param mouse Current mouse state
   */
  void update(int ms, const graphics::MouseState &mouse) override {
    (void)ms;

    // Convert mouse position to canvas coordinates
    float mx = graphics::windowToCanvasX((float)mouse.cur_pos_x);
    float my = graphics::windowToCanvasY((float)mouse.cur_pos_y);

    if (mouse.button_left_down) {
      // 1. Check if we should START dragging
      // Logic: Only start if no other station is being dragged AND mouse is
      // within radius
      if (!isDragging && s_active_dragging_station == nullptr) {
        float dx = mx - x;
        float dy = my - y;
        float distSq = dx * dx + dy * dy;

        if (distSq < radius * radius) {
          isDragging = true;
          s_active_dragging_station = this; // Claim global lock
          dragOffsetX = mx - x;
          dragOffsetY = my - y;
          std::cout << "DEBUG: Clicked on " << name << std::endl;
        }
      }

      // 2. Handle CONTINUOUS dragging
      // Logic: Only move if this specific instance owns the global lock
      if (isDragging && s_active_dragging_station == this) {
        x = mx - dragOffsetX;
        y = my - dragOffsetY;

        // Keep debug/passenger sync logic from your original code
        if (x2 != x || y2 != y) {
          x2 = x;
          y2 = y;
          for (Passenger *p : waitingPassengers) {
            p->setPosition(x, y);
          }
        }
      }
    } else {
      // 3. Handle RELEASE
      if (isDragging && s_active_dragging_station == this) {
        std::cout << "DEBUG: Released " << name << std::endl;
        s_active_dragging_station = nullptr; // Release global lock
      }
      isDragging = false;

      // RESTORED: Your original passenger alignment logic
      float passenger_row_offset = radius * (1.0f / 6.0f);
      float passenger_spacing =
          radius / (static_cast<float>(passengerCount / 2) + 1.0f);

      int passenger_in_row_idx = 0;
      for (size_t i = 0; i < waitingPassengers.size(); ++i) {
        Passenger *p = waitingPassengers[i];
        float pasx, pasy;

        if (i % 2 == 0) { // Even index: top row
          pasy = y + passenger_row_offset - 5;
          pasx = x - radius / 2.0f +
                 (passenger_in_row_idx + 1) * passenger_spacing;
        } else { // Odd index: bottom row
          pasy = y - passenger_row_offset - 10;
          pasx = x - radius / 2.0f +
                 (passenger_in_row_idx + 1) * passenger_spacing;
          passenger_in_row_idx++;
        }
        // Your specific offset: pasx - radius - 5, pasy - radius * 1.5f
        p->setPosition(pasx - radius - 5, pasy - radius * 1.5f);
      }
    }
  }

  void draw() override {
    if (!active)
      return;

    // Draw connections
    graphics::Brush lineBrush;
    lineBrush.outline_opacity = 1.0f;
    lineBrush.outline_width = 0.8f;
    lineBrush.outline_color[0] = 0.5f;
    lineBrush.outline_color[1] = 0.5f;
    lineBrush.outline_color[2] = 0.5f;

    for (Station *s : next) {
      if (s && s->getIsActive()) {
        graphics::drawLine(x, y, s->getX(), s->getY(), lineBrush);
      }
    }

    graphics::drawDisk(x, y, radius, brush);

    graphics::MouseState ms;
    graphics::getMouseState(ms);
    float mx = graphics::windowToCanvasX((float)ms.cur_pos_x);
    float my = graphics::windowToCanvasY((float)ms.cur_pos_y);

    float dx = std::abs(mx - x);
    float dy = std::abs(my - y);

    // Hover effect
    if (dx < radius && dy < radius) {
      graphics::Brush textBrush;
      textBrush.fill_color[0] = 1.0f;
      textBrush.fill_color[1] = 1.0f;
      textBrush.fill_color[2] = 1.0f;

      graphics::Brush bgBrush;
      bgBrush.fill_color[0] = 0.2f;
      bgBrush.fill_opacity = 0.8f;

      float textWidth = name.length() * 8.0f;
      graphics::drawRect(x, y + radius + 25, textWidth + 10, 20, bgBrush);
      graphics::drawText(x - textWidth / 2, y + radius + 30, 14, name,
                         textBrush);

      graphics::Brush highlightBrush = brush;
      highlightBrush.fill_opacity = 0.5f;
      highlightBrush.outline_color[0] = 1.0f;
      graphics::drawDisk(x, y, radius + 2, highlightBrush);
    }
  }

  // Getters / Setters
  std::string getName() const { return name; }
  void setName(const std::string &newName) { name = newName; }
  int getPassengerCount() const { return passengerCount; }
  void addPassenger() { passengerCount++; }
  void removePassenger() {
    if (passengerCount > 0)
      passengerCount--;
  }
  const std::vector<Station *> &getNext() const { return next; }
  void addWaitingPassenger(Passenger *p) { waitingPassengers.push_back(p); }
  void removeWaitingPassenger(Passenger *p) {
    auto it = std::find(waitingPassengers.begin(), waitingPassengers.end(), p);
    if (it != waitingPassengers.end())
      waitingPassengers.erase(it);
  }
  const std::vector<Passenger *> &getWaitingPassengers() const {
    return waitingPassengers;
  }
};

#endif