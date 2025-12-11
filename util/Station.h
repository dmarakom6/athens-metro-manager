#ifndef STATION_H
#define STATION_H

#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <string>

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
    std::vector<Station*> next;
    std::vector<Station*> prev;

    // Dragging state
    bool isDragging;
    float dragOffsetX;
    float dragOffsetY;

public:
    /**
     * @brief Constructor for Station
     * @param posX X position on canvas
     * @param posY Y position on canvas
     * @param stationName Name of the station
     * @param r Radius of the station circle
     */
    Station(float posX, float posY, const std::string& stationName, float r = 20.0f)
        : VisualAsset(posX, posY), name(stationName), radius(r), passengerCount(0), 
          isDragging(false), dragOffsetX(0.0f), dragOffsetY(0.0f) {
        // Set up the brush for drawing
        brush.fill_color[0] = 0.2f;  // R
        brush.fill_color[1] = 0.6f;  // G
        brush.fill_color[2] = 0.9f;  // B
        brush.outline_opacity = 1.0f;
        brush.outline_width = 3.0f;
    }

    /**
     * @brief Connect this station to another station (directed edge)
     * @param other The next station
     */
    void addNext(Station* other) {
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
    void update(int ms, const graphics::MouseState& mouse) override {
        (void)ms; // Suppress unused parameter warning
        
        // Convert mouse position to canvas coordinates
        float mx = graphics::windowToCanvasX((float)mouse.cur_pos_x);
        float my = graphics::windowToCanvasY((float)mouse.cur_pos_y);

        if (mouse.button_left_pressed) {
            if (!isDragging) {
                // Check if clicked on this station
                float dx = mx - x;
                float dy = my - y;
                float distSq = dx*dx + dy*dy;

                // Only start dragging if mouse is inside the station radius
                // AND we are not currently dragging another station (simple check)
                if (distSq < radius * radius) {
                    std::cout << "DEBUG: Clicked on " << name << " (DistSq: " << distSq << ", RadSq: " << radius*radius << ")" << std::endl;
                    isDragging = true;
                    dragOffsetX = mx - x;
                    dragOffsetY = my - y;
                } else if (distSq < (radius + 20) * (radius + 20)) {
                     // Debug near misses
                     std::cout << "DEBUG: Missed " << name << " (DistSq: " << distSq << ", RadSq: " << radius*radius << ")" << std::endl;
                }
            } else {
                // Continue dragging
                x = mx - dragOffsetX;
                y = my - dragOffsetY;
                std::cout << "DEBUG: Dragging " << name << " to " << x << ", " << y << std::endl;
            }
        } else {
            // Stop dragging
            if (isDragging) {
                std::cout << "DEBUG: Released " << name << std::endl;
            }
            isDragging = false;
        }
    }

    /**
     * @brief Draw the station on screen
     */
    void draw() override {
        if (!active) return;

        // Draw connections to next stations (edges)
        // We only draw 'next' connections to avoid drawing lines twice
        graphics::Brush lineBrush;
        lineBrush.fill_opacity = 0.0f;
        lineBrush.outline_opacity = 1.0f;
        lineBrush.outline_width = 4.0f;
        lineBrush.outline_color[0] = 0.5f;
        lineBrush.outline_color[1] = 0.5f;
        lineBrush.outline_color[2] = 0.5f;

        for (Station* s : next) {
            if (s && s->isActive()) {
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
        
        // Calculate distance squared
        float dx = mx - x;
        float dy = my - y;
        float distSq = dx*dx + dy*dy;
        
        // If hovered (distance < radius) or dragging, draw the name
        if (distSq < radius * radius || isDragging) {
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
            graphics::drawRect(x, y - radius - 25, textWidth + 10, 20, bgBrush);
            
            graphics::drawText(x - textWidth/2, y - radius - 20, 14, name, textBrush);
            
            // Highlight the station
            graphics::Brush highlightBrush = brush;
            highlightBrush.fill_opacity = 0.5f;
            highlightBrush.outline_color[0] = 1.0f;
            highlightBrush.outline_color[1] = 1.0f;
            highlightBrush.outline_color[2] = 1.0f;
            graphics::drawDisk(x, y, radius + 2, highlightBrush);
        }
    }

    // Getters and setters
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    
    int getPassengerCount() const { return passengerCount; }
    void addPassenger() { passengerCount++; }
    void removePassenger() { if (passengerCount > 0) passengerCount--; }

    /**
     * @brief Get adjacent stations (outgoing edges)
     * @return Vector of pointers to next stations
     */
    const std::vector<Station*>& getNext() const {
        return next;
    }
};

#endif // STATION_H
