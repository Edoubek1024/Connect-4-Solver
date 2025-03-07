#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "../include/connect-4.h"
#include "../include/json.hpp"
#include "../include/openings.h"
#include <iostream>
#include <windows.h>
#include <fstream>

// This project uses json to store optimal openings placements
// There are over 4 trillion possible board states of a typical Connect 4 game
// So any pre-registered openings is exponentially more efficient
using json = nlohmann::json;

sf::RenderWindow window(sf::VideoMode({(State::COLS*150), (State::ROWS*150)}), "Connect 4", sf::Style::Titlebar | sf::Style::Close);
bool game_over;
State state;

// Initializes the window, progress variables, and collects openings from the openings.json file
void init() {
    sf::Image icon;
    if (icon.loadFromFile("sprites/red_to_move.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
    game_over = false;

    std::ifstream inputFile("openings.json");
    json j;
    if (inputFile.is_open()) {
        inputFile >> j;
    }

    openings = j.get<std::map<std::string, int>>();

    inputFile.close();
}

// Carries out instances in which the game ends
void gameOver() {
    int winner = state.isAWin();
    if (winner == 0) {
        std::cout << "Cat's game! Hope you had fun anyway!" << std::endl;
    } else {
        std::string player = (winner == 1) ? "Player 1" : "Player 2";
        std::cout << player << " just won!" << std::endl;
    }
    Sleep(2000);
    window.close();
}

// Renders the grid for the players to see
// Red pieces correspond to Player 1, or the person who places their initial piece first
// Yellow pieces correspond to Player 2, or the person who places their initial piece second
void renderGrid() {
    sf::RectangleShape rectangle({static_cast<float>(State::COLS*150), static_cast<float>(State::ROWS*150)});
    rectangle.setFillColor(sf::Color::Blue);
    window.draw(rectangle);

    sf::CircleShape circle(60);
    for (int x = 0; x < State::COLS; x++) {
        for (int y = 0; y < State::ROWS; y++) {
            circle.setPosition(sf::Vector2f(x * 150 + 15, y * 150 + 15));
            if (state.board[x][y] == 0) {
                circle.setFillColor(sf::Color::Black);
            } else if (state.board[x][y] == 1) {
                circle.setFillColor(sf::Color::Red);
            } else if (state.board[x][y] == -1) {
                circle.setFillColor(sf::Color::Yellow);
            }
            window.draw(circle);
        }
    }
}

// The icon of the window changes to display whoever's turn it is
// If the red piece is on top, it is Player 1's turn
// If the yellow piece is on top, it is Player 2's turn
void changeIcon() {
    sf::Image icon;
    if (state.current_player == 1 && icon.loadFromFile("sprites/red_to_move.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    } else if (state.current_player == -1 && icon.loadFromFile("sprites/yellow_to_move.png")) {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
}

// Handles all events, including placement by the player, or AI usage by the player
void eventHandler() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            } else if (event.key.code == sf::Keyboard::P) {
                std::cout << "Calculating..." << std::endl;
                int best_move = state.bestMove(); // Calculates the best move using a negamax algorithm
                state.place(best_move); // Places the most optimal move
                std::cout << "Placed!" << std::endl;
                // If someone wins, the game over function is carried out
                if (state.isAWin() != 0) {
                    game_over = true;
                }
                changeIcon();
            }
        }
        // Places a piece in the clicked column if allowed
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            int x = event.mouseButton.x;
            int y = event.mouseButton.y;
            // This allows for only clicks that directly apply to the desired column
            // A narrower hitbox allows for no misclicks
            if (x % 150 > 15 && x % 150 < 135) {
                state.place(x / 150);
                // If someone wins, the game over function is carried out
                if (state.isAWin() != 0) {
                    game_over = true;
                }
                changeIcon();
            }
        }
    }
}

int main() {

    init();

    while (window.isOpen()) {
        eventHandler();
        window.clear();
        renderGrid();
        window.display();
        if (game_over || state.turn == State::COLS*State::ROWS) {
            gameOver();
        }
    }

    // Dump any new opening pairs into the openings file
    std::ofstream outputFile("openings.json");
    json j = openings;
    if (outputFile.is_open()) {
        outputFile << j.dump(4);
        outputFile.close();
    }

    return 0;
}
