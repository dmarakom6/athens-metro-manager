#ifndef SIMULATE_BUTTON_H
#define SIMULATE_BUTTON_H

#include "VisualAsset.h"
#include <sgg/graphics.h>
#include <functional>
#include <string>

/**
 * @brief A clickable button to trigger the simulation
 */
class SimulateButton : public VisualAsset {
private:
    float width;
    float height;
    std::string text;
    std::function<void()> onClickCallback;
    bool isHovered;
    
    // Simple state to prevent multiple clicks per press
    bool wasPressed;

public:
    SimulateButton(float posX, float posY, float w, float h, const std::string& label, std::function<void()> callback)
        : VisualAsset(posX, posY), width(w), height(h), text(label), onClickCallback(callback), 
          isHovered(false), wasPressed(false) 
    {}
    void update(int ms, const graphics::MouseState& mouse) override {
        (void)ms;
        
        float mx = graphics::windowToCanvasX((float)mouse.cur_pos_x);
        float my = graphics::windowToCanvasY((float)mouse.cur_pos_y);

        // Calculate button bounds based on center (x, y) and dimensions (width, height)
        float left = x - width / 2.0f;
        float right = x + width / 2.0f;
        float top = y - height / 2.0f;
        float bottom = y + height / 2.0f;

        if (mx >= left && mx <= right && my >= top && my <= bottom) {
            isHovered = true;
            if (mouse.button_left_pressed && !wasPressed) {
                if (onClickCallback) {
                    onClickCallback();
                }
                wasPressed = true;
            }
        } else {
            isHovered = false;
        }

        if (!mouse.button_left_pressed) {
            wasPressed = false;
        }
    }

    void draw() override {
        if (!active) return;

        graphics::Brush br;
        if (isHovered) {
            br.fill_color[0] = 0.3f; 
            br.fill_color[1] = 0.8f; 
            br.fill_color[2] = 0.3f;
        } else {
            br.fill_color[0] = 0.2f; 
            br.fill_color[1] = 0.7f; 
            br.fill_color[2] = 0.2f;
        }
        br.outline_opacity = 1.0f;
        
        graphics::drawRect(x, y, width, height, br);

        graphics::Brush textBr;
        textBr.fill_color[0] = 1.0f;
        textBr.fill_color[1] = 1.0f;
        textBr.fill_color[2] = 1.0f;
        
        // Rough centering of text (approx 8px per char width for size 14?)
        graphics::drawText(x - (text.length() * 4), y + 5, 16, text, textBr);
    }
};

#endif // SIMULATE_BUTTON_H
