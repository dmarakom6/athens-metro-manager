#ifndef GLOBAL_STATE_H
#define GLOBAL_STATE_H

#include "VisualAsset.h"
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <memory>
#include <thread>
#include <vector>

/**
 * @brief Singleton class that manages the global state of the Athens Metro
 * Manager application.
 *
 * This class stores the game state (level, score, simulation data) and manages
 * all VisualAsset objects in the game. It provides the central init(),
 * update(), and draw() methods that coordinate all game objects.
 */
class GlobalState {

private:
  int level;
  std::atomic<int> score;
  int windowWidth;
  int windowHeight;
  std::atomic<bool> simulating;

  std::thread score_thread;
  std::atomic<bool> keep_thread_alive;

  // STL container to manage all visual assets (stations, trains, passengers, UI
  // elements)
  std::vector<VisualAsset *> visualAssets;

public:
  /**
   * @brief Get the singleton instance of GlobalState
   * @return Reference to the single GlobalState instance
   */
  static GlobalState &getInstance() {
    static GlobalState instance;
    return instance;
  }

  // Delete copy constructor and assignment operator to prevent copying
  // (singleton class)
  GlobalState(const GlobalState &) = delete;
  GlobalState &operator=(const GlobalState &) = delete;

  /**
   * @brief Initialize the game state and all visual assets
   *
   * This method should be called once at the start of the application.
   * It initializes all game systems and creates initial visual assets.
   */
  void init() {
    // Initialize game state
    level = 1;
    score = 0;

    // Set the font for text rendering
    graphics::setFont("assets/fonts/Roboto-Regular.ttf");

    // Initialize stations, create initial UI elements, etc.

    std::ifstream people_file("assets/metro3.json", std::ifstream::binary);
    Json::Value metro;
    people_file >> metro;

    if (debugMode) {
      std::cout << metro << std::endl;
    }

    // Start score thread
    keep_thread_alive = true;
    score_thread = std::thread([this]() {
      while (keep_thread_alive) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        if (!keep_thread_alive)
          break;

        // Only subtract score if simulation is running
        if (simulating) {
          score -= 2;
          // ensure score doesn't go below 0
          if (score < 0)
            score = 0;
        }
      }
    });
  }

  /**
   * @brief Update all game objects
   * @param ms Milliseconds elapsed since last update
   *
   * This method calls update() on all active VisualAsset objects,
   * allowing them to update their state (movement, animations, etc.)
   */
  void update(int ms) {
    // Get mouse state once per frame
    graphics::MouseState mouse;
    graphics::getMouseState(mouse);

    // Update all visual assets
    for (auto *asset : visualAssets) {
      if (asset && asset->getIsActive()) {
        asset->update(ms, mouse);
      }
    }

    // TODO: Update game logic (spawn passengers, move trains, etc.)
  }

  /**
   * @brief Draw all game objects
   *
   * This method calls draw() on all active VisualAsset objects,
   * rendering them to the screen using the SGG library.
   */
  void draw() {
    // Draw all visual assets
    for (auto *asset : visualAssets) {
      if (asset && asset->getIsActive()) {
        asset->draw();
      }
    }
  }

  // Asset management methods
  /**
   * @brief Add a visual asset to be managed by GlobalState
   * @param asset Pointer to the VisualAsset to add
   */
  void addVisualAsset(VisualAsset *asset) {
    if (asset) {
      visualAssets.push_back(asset);
    }
  }

  /**
   * @brief Remove a visual asset from management
   * @param asset Pointer to the VisualAsset to remove
   * @note This does not delete the asset, only removes it from the container
   */
  void removeVisualAsset(VisualAsset *asset) {
    for (auto it = visualAssets.begin(); it != visualAssets.end(); ++it) {
      if (*it == asset) {
        visualAssets.erase(it);
        break;
      }
    }
  }

  /**
   * @brief Get all visual assets
   * @return Const reference to the vector of visual assets
   */
  const std::vector<VisualAsset *> &getVisualAssets() const {
    return visualAssets;
  }

  // Level management
  int getLevel() const { return level; }
  void setLevel(int newLevel) { level = newLevel; }

  // Score management
  int getScore() const { return score; }
  void setScore(int newScore) { score = newScore; }
  void addScore(int points) { score += points; }

  // Window size management (as mentioned in README)
  int getWindowWidth() const { return windowWidth; }
  int getWindowHeight() const { return windowHeight; }
  void setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
  }

  // Simulation state
  bool isSimulating() const { return simulating; }
  void setSimulating(bool sim) { simulating = sim; }

private:
  /**
   * @brief Private constructor for singleton pattern
   */
  GlobalState()
      : level(0), score(0), windowWidth(800), windowHeight(600),
        simulating(false), keep_thread_alive(true), debugMode(false) {}

public:
  bool isDebugMode() const { return debugMode; }
  void setDebugMode(bool debug) { debugMode = debug; }

private:
  bool debugMode;

  /**
   * @brief Private destructor - cleans up all visual assets
   */
  ~GlobalState() {
    // Stop thread
    keep_thread_alive = false;
    if (score_thread.joinable()) {
      score_thread.join();
    }

    // Clean up all visual assets
    for (auto *asset : visualAssets) {
      delete asset;
    }
    visualAssets.clear();
  }
};

#endif // GLOBAL_STATE_H