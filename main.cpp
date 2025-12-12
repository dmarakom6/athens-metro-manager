#include <iostream>
#include <sgg/graphics.h>
#include "util/GlobalState.h"
#include "util/Station.h"
#include "util/Passenger.h"
#include "util/Train.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <queue>
#include <map>
#include <cmath>
#include "util/SimulateButton.h"

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
    std::string scoreText = "Score: " + std::to_string(GlobalState::getInstance().getScore());
    graphics::drawText(50, 100, 18, scoreText, scoreBrush);

    // Draw level
    std::string levelText = "Level: " + std::to_string(GlobalState::getInstance().getLevel());
    graphics::drawText(50, 130, 18, levelText, scoreBrush);

    // Draw all visual assets (stations) through GlobalState
    GlobalState::getInstance().draw();

    // Draw instructions
    graphics::Brush instructionBrush;
    instructionBrush.fill_color[0] = 0.7f;
    instructionBrush.fill_color[1] = 0.7f;
    instructionBrush.fill_color[2] = 0.7f;
    graphics::drawText(200, 550, 14, "Demonstrating VisualAsset polymorphism with Station objects", instructionBrush);
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
    if (timeAccumulator >= 1000.0f) {  // Every second
        GlobalState::getInstance().addScore(10);
        timeAccumulator = 0.0f;
    }
}

// Forward declaration
void runSimulation();

void runSimulation() {
    GlobalState& gs = GlobalState::getInstance();
    const auto& assets = gs.getVisualAssets();
    std::vector<Station*> stations;
    
    // Filter stations
    for (auto* asset : assets) {
        Station* s = dynamic_cast<Station*>(asset);
        if (s) {
            stations.push_back(s);
        }
    }

    if (stations.size() < 2) {
        std::cout << "Not enough stations to simulate." << std::endl;
        return;
    }

    std::cout << "\n--- Starting Simulation (8 Passengers) ---\n" << std::endl;

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, stations.size() - 1);

    for (int i = 0; i < 8; ++i) {
        Station* start = stations[dist(rng)];
        Station* end = stations[dist(rng)];

        //TODO: CREATE PASSENGER OBJECT 

        // Ensure start != end
        int attempts = 0;
        while (start == end && attempts < 10) {
            end = stations[dist(rng)];
            attempts++;
        }

        if (start == end) {
             std::cout << "Passenger " << i + 1 << ": Stayed at " << start->getName() << " (Time: 0)" << std::endl;
             continue;
        }

        // BFS for pathfinding (poor source management but easy to implement)
        std::queue<std::pair<Station*, std::vector<Station*>>> q; // we need fifo for this: (Station, Path to this Station)
        std::map<Station*, bool> visited;
        
        q.push({start, {start}});
        visited[start] = true;

        bool found = false;
        std::vector<Station*> path;

        while (!q.empty()) {
            auto current_pair = q.front();
            q.pop();
            Station* curr = current_pair.first;
            std::vector<Station*> curr_path = current_pair.second;

            if (curr == end) {
                found = true;
                path = curr_path;
                break;
            }

            for (Station* neighbor : curr->getNext()) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    std::vector<Station*> new_path = curr_path;
                    new_path.push_back(neighbor);
                    q.push({neighbor, new_path});
                }
            }
        }

        if (found) {
            // Calculate Path Distance (Sum of edge segments)
            float total_distance = 0.0f;
            for (size_t j = 0; j < path.size() - 1; ++j) {
                float dx = path[j]->getX() - path[j+1]->getX();
                float dy = path[j]->getY() - path[j+1]->getY();
                total_distance += std::sqrt(dx*dx + dy*dy);
            }

            // Calculate Direct Distance (Best case scenario)
            float start_x = start->getX();
            float start_y = start->getY();
            float end_x = end->getX();
            float end_y = end->getY();
            float direct_distance_sq = (end_x - start_x)*(end_x - start_x) + (end_y - start_y)*(end_y - start_y);
            float direct_distance = std::sqrt(direct_distance_sq);

            std::cout << "Passenger " << i + 1 << ": " << start->getName() << " -> " << end->getName() 
                      << " | Metro Time: " << (int)total_distance 
                      << " | Direct Distance (Best): " << (int)direct_distance 
                      << " | Deviation: " << (int)(total_distance - direct_distance) << std::endl;
        } else {
             std::cout << "Passenger " << i + 1 << ": " << start->getName() << " -> " << end->getName() 
                      << " | Path not found!" << std::endl;
        }

        //TODO: DELETE PASSENGER OBJECT


    }
    std::cout << "\n--- Simulation Ended ---\n" << std::endl;
}

// Helper to add button
void setupSimulationButton() {
     GlobalState& gs = GlobalState::getInstance();
     // Place button at bottom right
     float btnW = 120.0f;
     float btnH = 40.0f;
     float btnX = gs.getWindowWidth() - btnW/2 - 20; 
     float btnY = gs.getWindowHeight() - btnH/2 - 20;
     
     SimulateButton* btn = new SimulateButton(btnX, btnY, btnW, btnH, "Simulate", runSimulation);
     gs.addVisualAsset(btn);
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
    GlobalState& gs = GlobalState::getInstance();
    
    // Set window size in GlobalState
    gs.setWindowSize(800, 600);
    
    // Initialize GlobalState
    gs.init();
    
    setupSimulationButton();


    // Get GlobalState instance


    // Seed random number generator
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> x_dist(50, gs.getWindowWidth() - 50); // Avoid edges
    std::uniform_int_distribution<int> y_dist(150, gs.getWindowHeight() - 50); // Avoid top for title/score

    std::map<std::string, Station*> stations_map;

    try {
        std::ifstream f("assets/metro3.json");
        if (!f.is_open()) {
            throw std::runtime_error("Could not open assets/metro3.json");
        }
        Json::Value data;
        f >> data;

        // First pass: Create all stations and add them to the map and GlobalState
        if (data.isMember("stations") && data["stations"].isArray()) {
            for (const auto& station_json : data["stations"]) {
                if (station_json.isMember("name") && station_json["name"].isString()) {
                    std::string name = station_json["name"].asString();
                    int random_x, random_y;
                    bool position_found = false;
                    const int MIN_SPACING_SQUARED = 100 * 100; // Minimum distance squared

                    // Try to find a non-overlapping position
                    int attempts = 0;
                    const int MAX_ATTEMPTS = 1000; // Prevent infinite loops in dense scenarios
                    while (!position_found && attempts < MAX_ATTEMPTS) {
                        random_x = x_dist(rng);
                        random_y = y_dist(rng);
                        position_found = true; // Assume position is good until collision is found

                        for (const auto& pair : stations_map) {
                            Station* existing_station = pair.second;
                            float dx = random_x - existing_station->getX();
                            float dy = random_y - existing_station->getY();
                            if ((dx * dx + dy * dy) < MIN_SPACING_SQUARED) {
                                position_found = false; // Collision detected, try again
                                break;
                            }
                        }
                        attempts++;
                    }

                    if (!position_found) {
                        std::cerr << "Warning: Could not find a non-overlapping position for station '" << name << "' after " << MAX_ATTEMPTS << " attempts. Placing it anyway." << std::endl;
                        // Fallback: place it even if it overlaps, or handle error
                        random_x = x_dist(rng); // Place randomly one last time
                        random_y = y_dist(rng);
                    }

                    Station* station = new Station(random_x, random_y, name);
                    stations_map[name] = station;
                    gs.addVisualAsset(station);
                    Passenger* pas = new Passenger(100, 50);
                    gs.addVisualAsset(pas);
                    std::vector<Passenger*> passengersss= {pas};
                    Τrain* tr = new Train(200, 80, passengersss);


                }
            }
        }

        // Second pass: Establish connections
        if (data.isMember("stations") && data["stations"].isArray()) {
            for (const auto& station_json : data["stations"]) {
                if (station_json.isMember("name") && station_json["name"].isString()) {
                    std::string name = station_json["name"].asString();
                    Station* current_station = stations_map[name];

                    if (station_json.isMember("connections") && station_json["connections"].isArray()) {
                        for (const auto& connection_name_json : station_json["connections"]) {
                            if (connection_name_json.isString()) {
                                std::string connection_name = connection_name_json.asString();
                                if (stations_map.count(connection_name)) {
                                    current_station->addNext(stations_map[connection_name]);
                                } else {
                                    std::cerr << "Warning: Connection to unknown station '" << connection_name << "' for station '" << name << "'" << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "File error: " << e.what() << std::endl;
        // Fallback or exit if file is critical
    }

    std::cout << "Athens Metro Manager Demo Started!" << std::endl;
    std::cout << "Demonstrating:" << std::endl;
    std::cout << "  - VisualAsset base class with polymorphic draw() and update()" << std::endl;
    std::cout << "  - GlobalState singleton managing all visual assets" << std::endl;
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

