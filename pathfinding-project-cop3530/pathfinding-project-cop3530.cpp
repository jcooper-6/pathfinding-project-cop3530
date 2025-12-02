#include <SFML/Graphics.hpp>
#include "button.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

struct Cell {
    bool is_wall = false;
    bool visited = false;
    float cost = 1.f;          
    int cell_size = 2;
};

void test_func() {
    std::cout << "Do Something" << std::endl;
}

int main()
{
    // Initialize Grid/Map Information
    int cols = 630; // 1260/2
    int rows = 310; // 620/2
    // Total cells/nodes in graph/grid =  195,300
    // Each cell is 2x2 pixels, and the whole map covers a 1260x620 space on the screen
    std::vector<std::vector<Cell>> map(rows, std::vector<Cell>(cols)); // Data for pathfinding
    // SFML VertexArray to draw the entire grid with one draw() call for efficiency.
    sf::VertexArray map_vertex_array(sf::Quads, rows * cols * 4);

    // Initialize vertices in the 1D vertexarray, 4 vertices = 1 Quad/Cell
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            
            // Index is the current cell we want to create vertices for
            int index = (r * cols + c) * 4; 
            int cell_size = map[r][c].cell_size;
            // + 10 creates a nice border so the map sits in the middle of the screen.
            // Top-left
            map_vertex_array[index + 0].position = sf::Vector2f(c * cell_size + 10, r * cell_size + 10);
            // Top-right
            map_vertex_array[index + 1].position = sf::Vector2f((c + 1) * cell_size + 10, r * cell_size + 10);
            // Bottom-right
            map_vertex_array[index + 2].position = sf::Vector2f((c + 1) * cell_size + 10, (r + 1) * cell_size + 10);
            // Bottom-left
            map_vertex_array[index + 3].position = sf::Vector2f(c * cell_size + 10, (r + 1) * cell_size + 10);

            // Default color
            for (int i = 0; i < 4; i++) {
                sf::Color grey(128, 128, 128);
                map_vertex_array[index + i].color = grey;
            }
        }
    }


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
        // Clear the screen, draw buttons and map, then update the display.
        window.clear(sf::Color::White);
        for (auto& button : buttons)
            button.draw(window);
        window.draw(map_vertex_array);
        window.display();
    }

    return 0;
}