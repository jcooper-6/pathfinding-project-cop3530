#include <SFML/Graphics.hpp>
#include "button.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

// Constants
const int CELL_SIZE = 2;
const int COLS = 630;
const int ROWS = 310;
const int BRUSH_RADIUS = 2;

struct Cell {
    bool is_wall = false;
    bool visited = false;
    float cost = 1.f;
};

void test_func() {
    std::cout << "Do Something" << std::endl;
}

void reset_map(std::vector<std::vector<Cell>>& map, sf::VertexArray& map_vertex_array) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            map[r][c].cost = 1.f;

            int index = (r * COLS + c) * 4;
            for (int i = 0; i < 4; i++) {
                sf::Color grey(128, 128, 128);
                map_vertex_array[index + i].color = grey;
            }

        }
    }
}

void brush(std::vector<std::vector<Cell>>& map, sf::VertexArray& map_vertex_array, std::string& brush_type, sf::Vector2i mouse_pos) {


    int col = (mouse_pos.x - 10) / CELL_SIZE;
    int row = (mouse_pos.y - 10) / CELL_SIZE;
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS)
        return;

    float cost = 1.f;
    int b_radius = BRUSH_RADIUS;
    sf::Color color = sf::Color(128, 128, 128);

    if (brush_type == "forest") {
        color = sf::Color::Green;
        cost = 50.f;
    }
    else if (brush_type == "river") {
        color = sf::Color::Blue;
        cost = 200.f;
    }
    else if (brush_type == "mountain") {
        color = sf::Color::Black;
        cost = 400.f;
    }
    else if (brush_type == "erase") {
        color = sf::Color(128, 128, 128);
        cost = 1.f;
        b_radius = 5;
    }


    for (int dr = -b_radius; dr <= b_radius; dr++) {
        for (int dc = -b_radius; dc <= b_radius; dc++) {

            int rr = row + dr;
            int cc = col + dc;

            // Boundary check
            if (rr < 0 || rr >= ROWS || cc < 0 || cc >= COLS)
                continue;

            int index = (rr * COLS + cc) * 4;
            // Update terrain cost in map
            map[rr][cc].cost = cost;

            // Update vertex array color
            for (int i = 0; i < 4; i++) {
                map_vertex_array[index + i].color = color;
            }

        }
    }
}

int main()
{
    // Initialize Grid/Map Information
    // Total cells/nodes in graph/grid =  195,300
    // Each cell is 2x2 pixels, and the whole map covers a 1260x620 space on the screen
    std::vector<std::vector<Cell>> map(ROWS, std::vector<Cell>(COLS)); // Data for pathfinding
    // SFML VertexArray to draw the entire grid with one draw() call for efficiency.
    sf::VertexArray map_vertex_array(sf::Quads, ROWS * COLS * 4);

    // Initialize vertices in the 1D vertexarray, 4 vertices = 1 Quad/Cell
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            
            // Index is the current cell we want to create vertices for
            int index = (r * COLS + c) * 4;
            // + 10 creates a nice border so the map sits in the middle of the screen.
            // Top-left
            map_vertex_array[index + 0].position = sf::Vector2f(c * CELL_SIZE + 10, r * CELL_SIZE + 10);
            // Top-right
            map_vertex_array[index + 1].position = sf::Vector2f((c + 1) * CELL_SIZE + 10, r * CELL_SIZE + 10);
            // Bottom-right
            map_vertex_array[index + 2].position = sf::Vector2f((c + 1) * CELL_SIZE + 10, (r + 1) * CELL_SIZE + 10);
            // Bottom-left
            map_vertex_array[index + 3].position = sf::Vector2f(c * CELL_SIZE + 10, (r + 1) * CELL_SIZE + 10);

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

    // Brush Functions and Variables
    std::string current_brush = "forest";

    // Create buttons
    buttons.push_back(Button(sf::Vector2f(100, 60), sf::Vector2f(535, 650), "Start", font, test_func));
    buttons.push_back(Button(sf::Vector2f(100, 60), sf::Vector2f(645, 650), "Reset", font, [&]() {reset_map(map, map_vertex_array);}));
    buttons.push_back(Button(sf::Vector2f(400, 60), sf::Vector2f(10, 650), "Results Section", font, test_func));
    buttons.push_back(Button(sf::Vector2f(200, 30), sf::Vector2f(860, 640), "Dijkstra's", font, test_func));
    buttons.push_back(Button(sf::Vector2f(200, 30), sf::Vector2f(1070, 640), "A* Search", font, test_func));
    buttons.push_back(Button(sf::Vector2f(136, 30), sf::Vector2f(860, 680), "Forest", font, [&]() {current_brush = "forest"; }));
    buttons.push_back(Button(sf::Vector2f(136, 30), sf::Vector2f(996, 680), "River", font, [&]() {current_brush = "river"; }));
    buttons.push_back(Button(sf::Vector2f(136, 30), sf::Vector2f(1132, 680), "Mountain", font, [&]() {current_brush = "mountain"; }));

   

    while (window.isOpen())
    {
        mouse_pos = sf::Mouse::getPosition(window);
        // Manage window events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Check for mouse click
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Check if mouse clicked a button
                    for (auto& button : buttons) {
                        button.isClicked(mouse_pos);
                    }
                }
            }
        }
        // Check for drawing and erasing
        bool mouse_leftclick_hold = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        if (mouse_leftclick_hold) {
            brush(map, map_vertex_array, current_brush, mouse_pos);
        }
        bool mouse_rightclick_hold = sf::Mouse::isButtonPressed(sf::Mouse::Right);
        if (mouse_rightclick_hold) {
            std::string erase = "erase";
            brush(map, map_vertex_array, erase, mouse_pos);
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