#include <SFML/Graphics.hpp>

#include <array>
#include <vector>
#include <concepts>
#include <functional>
#include <iostream>
#include <ostream>

#include "heap.h"
#include "Tile.h"
#include "Utils.h"
#include "Board.h"

// ==================================================
//             utility
// ==================================================



std::ostream& operator<<(std::ostream& os, const Tile& tile)
{
    os << "[" << tile.m_gridX << ", " << tile.m_gridY << "] F:" << tile.m_f << " G:" << tile.m_g << " H:" << tile.m_h;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Tile* tile)
{
    os << "[" << tile->m_gridX << ", " << tile->m_gridY << "] F:" << tile->m_f << " G:" << tile->m_g << " H:" << tile->m_h;
    return os;
}

// ==================================================
//             main
// ==================================================

int main()
{
    sf::Text helpText;
    helpText.setFont(AssetManager::getInstance()->font);
    helpText.setString("Click & drag to draw walls\nPress SPACE to start A* algorithm");
    helpText.setCharacterSize(20);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(100, 5);

    sf::RenderWindow window(sf::VideoMode(1200, 1200), "SFML works!");

    Board* board = new Board();
    sf::RectangleShape rs;
    rs.setFillColor(Tile::tileDefaultColor);
    rs.setSize({ Tile::tileWidth, Tile::tileWidth });
    rs.setOutlineColor(sf::Color::Transparent);
    rs.setOutlineThickness(5.0f);
    board->initializeGrid({ 50,50 }, rs);

    rs.setPosition({ 0,0 });
    rs.setOutlineThickness(5);
    rs.setFillColor(sf::Color::Green);
    rs.setOutlineColor(sf::Color::Transparent);

    sf::Clock frameClock;
    float dt;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //system("CLS");

        // meat and potatoes
        dt = frameClock.restart().asSeconds();
        sf::Vector2f mPos = sf::Vector2f(sf::Mouse::getPosition(window));
        //std::cout << mPos.x <<", "<< mPos.y << std::endl;
        board->update(dt, mPos);

        //display
        window.clear();
        window.draw(*board);
        window.draw(helpText);
        //window.draw(rs);
        window.display();
    }

    return 0;
}