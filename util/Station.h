#ifndef STATION_H
#define STATION_H

#include "Passenger.h"
#include "VisualAsset.h"
#include <algorithm>
#include <sgg/graphics.h>
#include <string>
#include <vector>

/**
 * @brief Represents a metro station (node in the graph)
 *
 * Inherits from VisualAsset and implements the draw() and update() methods.
 * Stations are the nodes of the metro network graph.
 */
class Station : public VisualAsset {
private:
  std::string name;
  float radius;
  graphics::Brush brush;
  int passengerCount;
  // Graph connectivity
  std::vector<Station *> next;
  std::vector<Station *> prev;

  // Waiting passengers
  std::vector<Passenger *> waitingPassengers;

  // Dragging state
  bool isDragging;
  float dragOffsetX;
  float dragOffsetY;

  // Save temporary x and y of dragging station debug
  float x2 = -1.0;
  float y2 = -1.0;

public:
  /**
   * @brief Constructor for Station
   * @param posX X position on canvas
   * @param posY Y position on canvas
   * @param stationName Name of the station
   * @param r Radius of the station circle
   */
  Station(float posX, float posY, const std::string &stationName,
          float r = 15.0f)
      : VisualAsset(posX, posY), name(stationName), radius(r),
        passengerCount(0), isDragging(false), dragOffsetX(0.0f),
        dragOffsetY(0.0f) {
    // Set up the brush for drawing
    brush.fill_color[0] = 0.2f; // R
    brush.fill_color[1] = 0.6f; // G
    brush.fill_color[2] = 0.9f; // B
    brush.outline_opacity = 1.0f;
    brush.outline_width = 3.0f;
  }

  /**
   * @brief Connect this station to another station (directed edge)
   * @param other The next station
   */
  void addNext(Station *other) {
    if (other) {
      next.push_back(other);
      other->prev.push_back(this); // Maintain the doubly-linked nature
    }
  }

  /**
   * @brief Update the station state
   * @param ms Milliseconds elapsed since last update
   * @param mouse Current mouse state
   */
  void update(int ms, const graphics::MouseState &mouse) override {
    (void)ms; // Suppress unused parameter warning

    // Convert mouse position to canvas coordinates
    float mx = graphics::windowToCanvasX((float)mouse.cur_pos_x);
    float my = graphics::windowToCanvasY((float)mouse.cur_pos_y);

    if (mouse.button_left_down) {
      if (!isDragging) {
        // Check if clicked on this station
        float dx = mx - x;
        float dy = my - y;
        float distSq = dx * dx + dy * dy;

        // Only start dragging if mouse is inside the station radius
        // AND we are not currently dragging another station (simple check)
        if (distSq < radius * radius) {
          std::cout << "DEBUG: Clicked on " << name << " (DistSq: " << distSq
                    << ", RadSq: " << radius * radius << ")" << std::endl;
          isDragging = true;
          dragOffsetX = mx - x;
          dragOffsetY = my - y;
        } else if (distSq < (radius + 20) * (radius + 20)) {
          // Debug near misses
          std::cout << "DEBUG: Missed " << name << " (DistSq: " << distSq
                    << ", RadSq: " << radius * radius << ")" << std::endl;
        }
      } else {
        // Continue dragging
        x = mx - dragOffsetX;
        y = my - dragOffsetY;
        if (x2 != x || y2 != y) {
          std::cout << "DEBUG: Dragging " << name << " to " << x << ", " << y
                    << std::endl;
          x2 = x;
          y2 = y;
          for (Passenger *p :
               waitingPassengers) { // dragging passengers with the station
            p->setPosition(x, y);
          }
        }
      }
    } else {
      // Stop dragging
      if (isDragging) {
        std::cout << "DEBUG: Released " << name << std::endl;
      }
      isDragging = false;

      // Update waiting passengers position to follow station
      // Arrange passengers in two rows, alternating positions
      float passenger_row_offset = radius * (1.0f / 6.0f);
      float passenger_spacing =
          radius / (static_cast<float>(passengerCount / 2) +
                    1.0f); // Spacing between passengers in a row

      int passenger_in_row_idx = 0; // Index for passenger within their row
      for (size_t i = 0; i < waitingPassengers.size(); ++i) {
        Passenger *p = waitingPassengers[i];
        float pasx, pasy;

        if (i % 2 == 0) // Even index: top row
        {
          pasy = y + passenger_row_offset - 5;
          pasx = x - radius / 2.0f +
                 (passenger_in_row_idx + 1) * passenger_spacing;
        } else // Odd index: bottom row
        {
          pasy = y - passenger_row_offset - 10;
          pasx = x - radius / 2.0f +
                 (passenger_in_row_idx + 1) * passenger_spacing;
          passenger_in_row_idx++; // Increment for the next pair of passengers
        }
        p->setPosition(pasx - radius - 5,
                       pasy - radius * 1.5f); // Set the new position of the
                                              // passenger (y axis is inverted)
      }
    }
  }

  /**
   * @brief Draw the station on screen
   */
  void draw() override {
    if (!active)
      return;

    // Draw connections to next stations (edges)
    // We only draw 'next' connections to avoid drawing lines twice
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

    // Draw the station as a circle
    graphics::drawDisk(x, y, radius, brush);

    // Check for hover
    graphics::MouseState ms;
    graphics::getMouseState(ms);

    // Convert mouse position to canvas coordinates
    float mx = graphics::windowToCanvasX((float)ms.cur_pos_x);
    float my = graphics::windowToCanvasY((float)ms.cur_pos_y);

    // Calculate distance
    float dx = std::abs(mx - x);
    float dy = std::abs(my - y);

    // If hovered (distance < radius), draw the name
    if (dx < radius && dy < radius) {
      graphics::Brush textBrush;
      textBrush.fill_color[0] = 1.0f;
      textBrush.fill_color[1] = 1.0f;
      textBrush.fill_color[2] = 1.0f;

      // Draw a small background for the text for better visibility
      graphics::Brush bgBrush;
      bgBrush.fill_color[0] = 0.2f;
      bgBrush.fill_color[1] = 0.2f;
      bgBrush.fill_color[2] = 0.2f;
      bgBrush.fill_opacity = 0.8f;

      // Estimate text width (rough approximation)
      float textWidth = name.length() * 8.0f;
      graphics::drawRect(x, y + radius + 25, textWidth + 10, 20, bgBrush);

      graphics::drawText(x - textWidth / 2, y + radius + 30, 14, name,
                         textBrush);

      // Highlight the station
      graphics::Brush highlightBrush = brush;
      highlightBrush.fill_opacity = 0.5f;
      highlightBrush.outline_color[0] = 1.0f;
      highlightBrush.outline_color[1] = 1.0f;
      highlightBrush.outline_color[2] = 1.0f;
      graphics::drawDisk(x, y, radius + 2, highlightBrush);
      // also check if dragging station and move accordingly
      if (ms.button_left_down && dx < radius && dy < radius) {
        setPosition(mx, my);
      }
    }
  }

  // Getters and setters
  std::string getName() const { return name; }
  void setName(const std::string &newName) { name = newName; }

  int getPassengerCount() const { return passengerCount; }
  void addPassenger() { passengerCount++; }
  void removePassenger() {
    if (passengerCount > 0)
      passengerCount--;
  }

  /**
   * @brief Get adjacent stations (outgoing edges)
   * @return Vector of pointers to next stations
   */
  const std::vector<Station *> &getNext() const { return next; }

  // Passenger management
  void addWaitingPassenger(Passenger *p) { waitingPassengers.push_back(p); }

  void removeWaitingPassenger(Passenger *p) {
    auto it = std::find(waitingPassengers.begin(), waitingPassengers.end(), p);
    if (it != waitingPassengers.end()) {
      waitingPassengers.erase(it);
    }
  }

  const std::vector<Passenger *> &getWaitingPassengers() const {
    return waitingPassengers;
  }
};

#endif // STATION_H
