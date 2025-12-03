#include "button.h"

Button::Button(const sf::Vector2f& size, const sf::Vector2f& position,
    const std::string& label, sf::Font& font, std::function<void()> function)
{
    // assign values and create SFML shape
    action = function;
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(sf::Color::White);
    // Create shape outline
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(3.f);

    // Create SFML text object
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);

    // Center the text
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

void Button::isClicked(const sf::Vector2i& mouse_pos) {
    if (shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mouse_pos))) {
        action();
    }
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

void Button::update_label(std::string new_label) {
    text.setString(new_label);
}
