#ifndef VISUAL_ASSET_H
#define VISUAL_ASSET_H

#include <sgg/graphics.h>

/**
 * @brief Base class for all visual elements in the Athens Metro Manager application.
 * 
 * This is an abstract base class that provides the interface for all objects
 * that need to be rendered on screen. It uses polymorphism to allow different
 * types of visual assets (stations, trains, passengers, UI elements) to be
 * managed uniformly through the GlobalState.
 * 
 * All derived classes must implement the pure virtual methods draw() and update().
 */
class VisualAsset {
protected:
    float x;        // X position on screen
    float y;        // Y position on screen
    bool active;    // Whether this asset is currently active/visible

public:
    /**
     * @brief Constructor for VisualAsset
     * @param posX Initial X position
     * @param posY Initial Y position
     */
    VisualAsset(float posX = 0.0f, float posY = 0.0f) 
        : x(posX), y(posY), active(true) {}

    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes
     */
    virtual ~VisualAsset() {}

    /**
     * @brief Pure virtual method to update the asset's state
     * @param ms Milliseconds elapsed since last update
     * @param mouse Current mouse state
     * 
     * This method should be overridden by derived classes to implement
     * their specific update logic (e.g., movement, animations, state changes).
     */
    virtual void update(int ms, const graphics::MouseState& mouse) = 0;

    /**
     * @brief Pure virtual method to draw the asset on screen
     * 
     * This method should be overridden by derived classes to implement
     * their specific rendering logic using the SGG library.
     */
    virtual void draw() = 0;

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    bool isActive() const { return active; }

    // Setters
    void setX(float posX) { x = posX; }
    void setY(float posY) { y = posY; }
    void setPosition(float posX, float posY) { 
        x = posX; 
        y = posY; 
    }
    void setActive(bool isActive) { active = isActive; }
};

#endif // VISUAL_ASSET_H
