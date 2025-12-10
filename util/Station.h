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

public:
    /**
     * @brief Constructor for Station
     * @param posX X position on canvas
     * @param posY Y position on canvas
     * @param stationName Name of the station
     * @param r Radius of the station circle
     */
    Station(float posX, float posY, const std::string& stationName, float r = 20.0f)
        : VisualAsset(posX, posY), name(stationName), radius(r), passengerCount(0) {
        // Set up the brush for drawing
        brush.fill_color[0] = 0.2f;  // R
        brush.fill_color[1] = 0.6f;  // G
        brush.fill_color[2] = 0.9f;  // B
        brush.outline_opacity = 1.0f;
        brush.outline_width = 3.0f;
    }

    /**
     * @brief Update the station state
     * @param ms Milliseconds elapsed since last update
     */
    void update(int ms) override {
        (void)ms; // Suppress unused parameter warning
        // Station update logic (e.g., passenger spawning, etc.)
        // For demo purposes, we'll just keep it simple
    }

    /**
     * @brief Draw the station on screen
     */
    void draw() override {
        if (!active) return;

        // Draw the station as a circle
        graphics::drawDisk(x, y, radius, brush);

        // Draw station name
        graphics::Brush textBrush;
        textBrush.fill_color[0] = 1.0f;
        textBrush.fill_color[1] = 1.0f;
        textBrush.fill_color[2] = 1.0f;
        graphics::drawText(x - 30, y + radius + 15, 14, name, textBrush);
    }

    // Getters and setters
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }
    
    int getPassengerCount() const { return passengerCount; }
    void addPassenger() { passengerCount++; }
    void removePassenger() { if (passengerCount > 0) passengerCount--; }
};

#endif // STATION_H
