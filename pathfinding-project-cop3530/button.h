#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> action;

public:
    Button(const sf::Vector2f& size, const sf::Vector2f& position, const std::string& label, sf::Font& font, std::function<void()> function);

    void isClicked(const sf::Vector2i& mouse_pos);

    void draw(sf::RenderWindow& window);
    
};