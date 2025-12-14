#include "util/GlobalState.h"
#include "util/Station.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sgg/graphics.h>
#include <string>
#include <vector>

// Forward declarations for callback functions
void draw();
void update(float ms);

/**
 * @brief Main draw callback function
 *
 * This function is called by SGG every frame to render the scene.
 * It delegates to GlobalState which calls draw() on all VisualAssets.
 */
void draw() {
  // Clear background
  graphics::Brush bg;
  bg.fill_color[0] = 0.1f;
  bg.fill_color[1] = 0.1f;
  bg.fill_color[2] = 0.15f;
  graphics::drawRect(400, 300, 800, 600, bg);

  // Draw title
  graphics::Brush titleBrush;
  titleBrush.fill_color[0] = 1.0f;
  titleBrush.fill_color[1] = 1.0f;
  titleBrush.fill_color[2] = 1.0f;
  graphics::drawText(250, 50, 28, "Athens Metro Manager - Demo", titleBrush);

  // Draw score
  graphics::Brush scoreBrush;
  scoreBrush.fill_color[0] = 0.8f;
  scoreBrush.fill_color[1] = 0.9f;
  scoreBrush.fill_color[2] = 1.0f;
  std::string scoreText =
      "Score: " + std::to_string(GlobalState::getInstance().getScore());
  graphics::drawText(50, 100, 18, scoreText, scoreBrush);

  // Draw level
  std::string levelText =
      "Level: " + std::to_string(GlobalState::getInstance().getLevel());
  graphics::drawText(50, 130, 18, levelText, scoreBrush);

  // Draw all visual assets (stations) through GlobalState
  GlobalState::getInstance().draw();

  // Draw instructions
  graphics::Brush instructionBrush;
  instructionBrush.fill_color[0] = 0.7f;
  instructionBrush.fill_color[1] = 0.7f;
  instructionBrush.fill_color[2] = 0.7f;
  graphics::drawText(
      200, 550, 14,
      "Demonstrating VisualAsset polymorphism with Station objects",
      instructionBrush);
}

/**
 * @brief Main update callback function
 * @param ms Milliseconds elapsed since last update
 *
 * This function is called by SGG every frame to update game logic.
 * It delegates to GlobalState which calls update() on all VisualAssets.
 */
void update(float ms) {
  // Update all visual assets through GlobalState
  GlobalState::getInstance().update(static_cast<int>(ms));

  // Simple score increment for demo purposes
  static float timeAccumulator = 0.0f;
  timeAccumulator += ms;
  if (timeAccumulator >= 1000.0f) { // Every second
    GlobalState::getInstance().addScore(10);
    timeAccumulator = 0.0f;
  }
}

/**
 * @brief Main entry point
 *
 * Sets up the SGG window, creates demo stations, and starts the message loop.
 */
int main() {
  // Create window with SGG
  graphics::createWindow(800, 600, "Athens Metro Manager");

  // Set canvas size (logical coordinates)
  graphics::setCanvasSize(800, 600);

  // Set canvas scale mode (fit was adviced)
  graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

  // Get GlobalState instance
  GlobalState &gs = GlobalState::getInstance();

  // Set window size in GlobalState
  gs.setWindowSize(800, 600);

  // Initialize GlobalState
  gs.init();

  // Get GlobalState instance

  // Seed random number generator
  std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> x_dist(50, gs.getWindowWidth() -
                                                    50); // Avoid edges
  std::uniform_int_distribution<int> y_dist(
      150, gs.getWindowHeight() - 50); // Avoid top for title/score

  std::map<std::string, Station *> stations_map;

  try {
    std::ifstream f("assets/metro3.json");
    if (!f.is_open()) {
      throw std::runtime_error("Could not open assets/metro3.json");
    }
    Json::Value data;
    f >> data;

    // First pass: Create all stations and add them to the map and GlobalState
    if (data.isMember("stations") && data["stations"].isArray()) {
      for (const auto &station_json : data["stations"]) {
        if (station_json.isMember("name") && station_json["name"].isString()) {
          std::string name = station_json["name"].asString();
          int random_x = x_dist(rng);
          int random_y = y_dist(rng);
          Station *station = new Station(random_x, random_y, name);
          stations_map[name] = station;
          gs.addVisualAsset(station);
        }
      }
    }

    // Second pass: Establish connections
    if (data.isMember("stations") && data["stations"].isArray()) {
      for (const auto &station_json : data["stations"]) {
        if (station_json.isMember("name") && station_json["name"].isString()) {
          std::string name = station_json["name"].asString();
          Station *current_station = stations_map[name];

          if (station_json.isMember("connections") &&
              station_json["connections"].isArray()) {
            for (const auto &connection_name_json :
                 station_json["connections"]) {
              if (connection_name_json.isString()) {
                std::string connection_name = connection_name_json.asString();
                if (stations_map.count(connection_name)) {
                  current_station->addNext(stations_map[connection_name]);
                } else {
                  std::cerr << "Warning: Connection to unknown station '"
                            << connection_name << "' for station '" << name
                            << "'" << std::endl;
                }
              }
            }
          }
        }
      }
    }
  } catch (const std::runtime_error &e) {
    std::cerr << "File error: " << e.what() << std::endl;
    // Fallback or exit if file is critical
  }

  std::cout << "Athens Metro Manager Demo Started!" << std::endl;
  std::cout << "Demonstrating:" << std::endl;
  std::cout << "  - VisualAsset base class with polymorphic draw() and update()"
            << std::endl;
  std::cout << "  - GlobalState singleton managing all visual assets"
            << std::endl;
  std::cout << "  - Station class inheriting from VisualAsset" << std::endl;
  std::cout << "  - SGG library integration" << std::endl;

  // Set callback functions for SGG
  graphics::setDrawFunction(draw);
  graphics::setUpdateFunction(update);

  // Start the SGG message loop (this will call draw() and update() repeatedly)
  graphics::startMessageLoop();

  // Cleanup (this will be called when window is closed)
  graphics::destroyWindow();

  std::cout << "Demo ended. Final score: " << gs.getScore() << std::endl;

  return 0;
}
