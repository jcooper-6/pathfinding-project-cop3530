#include <SFML/Graphics.hpp>
#include "button.h"
#include <unordered_map>
#include <string>
#include <iostream>

void test_func() {
    std::cout << "Do Something" << std::endl;
}

int main()
{
    // Initialize Button Vector
    std::vector<Button> buttons;

    // Mouse Position Variable
    sf::Vector2i mouse_pos;

    // Load font
    sf::Font font;
    if (!font.loadFromFile("fonts/DejaVuSans.ttf"))
        return -1;  // Could not load font

    // Create window
    sf::RenderWindow window(sf::VideoMode(1280, 720), "pathfinding-sfml-project");

    // Create buttons
    buttons.push_back(Button(sf::Vector2f(100, 60), sf::Vector2f(535, 650), "Start", font, test_func));
    buttons.push_back(Button(sf::Vector2f(100, 60), sf::Vector2f(645, 650), "Reset", font, test_func));
    buttons.push_back(Button(sf::Vector2f(400, 60), sf::Vector2f(10, 650), "Results Section", font, test_func));
    buttons.push_back(Button(sf::Vector2f(200, 30), sf::Vector2f(860, 640), "Dijkstra's", font, test_func));
    buttons.push_back(Button(sf::Vector2f(200, 30), sf::Vector2f(1070, 640), "A* Search", font, test_func));
    buttons.push_back(Button(sf::Vector2f(410, 30), sf::Vector2f(860, 680), "Generate New Map", font, test_func));


    while (window.isOpen())
    {
        // Manage window events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Check for mouse click
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Get mouse position
                    mouse_pos = sf::Mouse::getPosition(window);
                    // Check if mouse clicked a button
                    for (auto& button : buttons) {
                        button.isClicked(mouse_pos);
                    }
                }
            }
        }
        // Clear the screen and draw buttons, then update the display.
        window.clear(sf::Color::White);
        for (auto& button : buttons)
            button.draw(window);
        window.display();
    }

    return 0;
}