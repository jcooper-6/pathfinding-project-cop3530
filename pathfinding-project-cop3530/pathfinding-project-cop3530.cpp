#include <SFML/Graphics.hpp>
#include "button.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <queue>

// Constants
const int CELL_SIZE = 2;
const int COLS = 630;
const int ROWS = 310;
const int BRUSH_RADIUS = 5;

struct Cell {
    bool visited = false;
    float cost = 1.f;
};

void test_func() {
    std::cout << "button click" << std::endl;
}

void reset_map(std::vector<std::vector<Cell>>& map, sf::VertexArray& map_vertex_array, std::vector<int>& results) {
    results[0] = 0;
    results[1] = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            map[r][c].cost = 1.f;
            map[r][c].visited = false;

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

void draw_start(sf::VertexArray& map_vertex_array, sf::Vector2i start_cell, sf::Vector2i goal_cell) {
    // Start cell = Green
    int start_index = (start_cell.y * COLS + start_cell.x) * 4;
    for (int i = 0; i < 4; i++) {
        map_vertex_array[start_index + i].color = sf::Color::Green;
    }
}

void draw_goal(sf::VertexArray& map_vertex_array, sf::Vector2i start_cell, sf::Vector2i goal_cell) {
    // Goal cell = Blue
    int goal_index = (goal_cell.y * COLS + goal_cell.x) * 4;
    for (int i = 0; i < 4; i++) {
        map_vertex_array[goal_index + i].color = sf::Color::Blue;
    }
}

void run_dijkstra(std::vector<std::vector<Cell>>& map, sf::VertexArray& map_vertex_array, sf::Vector2i start_cell, sf::Vector2i goal_cell, std::vector<int>& results) {    
    sf::Clock clock;
    results[0] = 0;
    results[1] = 0;

    // Priority queue stores {total_cost_to_reach, row, col}
    std::priority_queue<std::tuple<float, int, int>, std::vector<std::tuple<float, int, int>>, std::greater<std::tuple<float, int, int>>> pq;

    std::vector<std::vector<float>> distance_map(ROWS, std::vector<float>(COLS, 1e9));
    std::vector<std::vector<std::pair<int, int>>> previous_cell_map(ROWS, std::vector<std::pair<int, int>>(COLS, { -1,-1 }));

    distance_map[start_cell.y][start_cell.x] = 0.f;
    pq.push({ 0.f, start_cell.y, start_cell.x });

    const int delta_row[4] = { -1, 1, 0, 0 };
    const int delta_col[4] = { 0, 0, -1, 1 };

    while (!pq.empty()) {
        auto [current_cost, current_row, current_col] = pq.top(); pq.pop();
        if (map[current_row][current_col].visited) continue;
        map[current_row][current_col].visited = true;
        results[0]++;

        int vertex_index = (current_row * COLS + current_col) * 4;
        for (int i = 0; i < 4; i++) {
            map_vertex_array[vertex_index + i].color = sf::Color::Yellow;
        }

        if (current_row == goal_cell.y && current_col == goal_cell.x) break;

        for (int dir = 0; dir < 4; dir++) {
            int neighbor_row = current_row + delta_row[dir];
            int neighbor_col = current_col + delta_col[dir];
            if (neighbor_row < 0 || neighbor_row >= ROWS || neighbor_col < 0 || neighbor_col >= COLS) continue;
            float new_cost = current_cost + map[neighbor_row][neighbor_col].cost;
            if (new_cost < distance_map[neighbor_row][neighbor_col]) {
                distance_map[neighbor_row][neighbor_col] = new_cost;
                previous_cell_map[neighbor_row][neighbor_col] = { current_row,current_col };
                pq.push({ new_cost, neighbor_row, neighbor_col });
            }
        }
    }

    // Reconstruct path from goal to start
    sf::Color path_color = sf::Color::Red;
    int row = goal_cell.y;
    int col = goal_cell.x;
    while (previous_cell_map[row][col].first != -1) {
        int vertex_index = (row * COLS + col) * 4;
        for (int i = 0; i < 4; i++) {
            map_vertex_array[vertex_index + i].color = path_color;
        }
        auto [prev_row, prev_col] = previous_cell_map[row][col];
        row = prev_row;
        col = prev_col;
    }
    results[1] = clock.getElapsedTime().asMilliseconds();
}

void run_a_star(std::vector<std::vector<Cell>>& map, sf::VertexArray& map_vertex_array, sf::Vector2i start_cell, sf::Vector2i goal_cell, std::vector<int>& results) {
    sf::Clock clock;
    results[0] = 0;
    results[1] = 0;

    // Priority queue stores {f_cost, g_cost, row, col}
    std::priority_queue<std::tuple<float, float, int, int>, std::vector<std::tuple<float, float, int, int>>, std::greater<std::tuple<float, float, int, int>>> pq;

    // g_cost = distance from start
    std::vector<std::vector<float>> g_cost(ROWS, std::vector<float>(COLS, 1e9));
    std::vector<std::vector<std::pair<int, int>>> previous_cell_map(ROWS, std::vector<std::pair<int, int>>(COLS, { -1,-1 }));

    g_cost[start_cell.y][start_cell.x] = 0.f;

    // f_cost = g_cost + heuristic
    auto heuristic = [&](sf::Vector2i a, sf::Vector2i b) -> float {
        return abs(a.x - b.x) + abs(a.y - b.y); // Manhattan distance
        };

    float f_start = heuristic(start_cell, goal_cell);
    pq.push({ f_start, 0.f, start_cell.y, start_cell.x });

    const int delta_row[4] = { -1, 1, 0, 0 };
    const int delta_col[4] = { 0, 0, -1, 1 };

    while (!pq.empty()) {
        auto [f_current, g_current, row, col] = pq.top(); 
        pq.pop();
        if (map[row][col].visited) continue;
        map[row][col].visited = true;
        results[0]++;

        int vertex_index = (row * COLS + col) * 4;
        for (int i = 0; i < 4; i++) {
            map_vertex_array[vertex_index + i].color = sf::Color::Yellow;
        }

        if (row == goal_cell.y && col == goal_cell.x) break;

        for (int dir = 0; dir < 4; dir++) {
            int neighbor_row = row + delta_row[dir];
            int neighbor_col = col + delta_col[dir];

            if (neighbor_row < 0 || neighbor_row >= ROWS || neighbor_col < 0 || neighbor_col >= COLS)
                continue;

            float tentative_g = g_current + map[neighbor_row][neighbor_col].cost;
            if (tentative_g < g_cost[neighbor_row][neighbor_col]) {
                g_cost[neighbor_row][neighbor_col] = tentative_g;
                previous_cell_map[neighbor_row][neighbor_col] = { row, col };
                float f_new = tentative_g + heuristic({ neighbor_col, neighbor_row }, goal_cell);
                pq.push({ f_new, tentative_g, neighbor_row, neighbor_col });
            }
        }
    }

    // Reconstruct path
    sf::Color path_color = sf::Color::Red;
    int r = goal_cell.y;
    int c = goal_cell.x;
    while (previous_cell_map[r][c].first != -1) {
        int vertex_index = (r * COLS + c) * 4;
        for (int i = 0; i < 4; i++) {
            map_vertex_array[vertex_index + i].color = path_color;
        }
        auto [prev_r, prev_c] = previous_cell_map[r][c];
        r = prev_r;
        c = prev_c;
    }
    results[1] = clock.getElapsedTime().asMilliseconds();
}


int main()
{

    // Initialize Grid/Map Information
    // Total cells/nodes in graph/grid =  195,300
    // Each cell is 2x2 pixels, and the whole map covers a 1260x620 space on the screen
    std::vector<std::vector<Cell>> map(ROWS, std::vector<Cell>(COLS));
    sf::Vector2i start_cell(0, 0);
    sf::Vector2i goal_cell(300, 300);
    std::vector<int> results(2, 0);
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
    buttons.push_back(Button(sf::Vector2f(210, 60), sf::Vector2f(535, 650), "Reset", font, [&]() {reset_map(map, map_vertex_array, results);}));
    buttons.push_back(Button(sf::Vector2f(400, 60), sf::Vector2f(10, 650), "Results Section", font, test_func));
    buttons.push_back(Button(sf::Vector2f(200, 30), sf::Vector2f(860, 640), "Dijkstra's", font, [&]() {run_dijkstra(map, map_vertex_array, start_cell, goal_cell, results); }));
    buttons.push_back(Button(sf::Vector2f(200, 30), sf::Vector2f(1070, 640), "A* Search", font, [&]() { run_a_star(map, map_vertex_array, start_cell, goal_cell, results); }));
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
            // Check for key press
            if (event.type == sf::Event::KeyPressed) {
                // Convert mouse position to grid indices
                int col = (mouse_pos.x - 10) / CELL_SIZE;
                int row = (mouse_pos.y - 10) / CELL_SIZE;

                // Boundary check
                if (col >= 0 && col < COLS && row >= 0 && row < ROWS) {
                    if (event.key.code == sf::Keyboard::S) {
                        start_cell = sf::Vector2i(col, row);
                        draw_start(map_vertex_array, start_cell, goal_cell);
                    }
                    if (event.key.code == sf::Keyboard::G) {
                        goal_cell = sf::Vector2i(col, row);
                        draw_goal(map_vertex_array, start_cell, goal_cell);
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

        if (results[1] != 0) {
            buttons[1].update_label(std::to_string(results[0]) + " cells " + std::to_string(results[1]) + " ms");
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