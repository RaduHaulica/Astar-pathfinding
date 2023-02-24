#pragma once

#include <SFML/graphics.hpp>

#include "Utils.h"

class Tile : public IUpdate, public sf::Drawable
{
    bool isHovered(sf::Vector2f mPos)
    {
        return (mPos.x > m_rs.getPosition().x && mPos.x < m_rs.getPosition().x + Tile::tileWidth
            && mPos.y > m_rs.getPosition().y && mPos.y < m_rs.getPosition().y + Tile::tileWidth);
        //return m_rs.getGlobalBounds().contains(mPos);
    }
public:
    static const sf::Color tileDefaultColor;
    static const float tileWidth;

    sf::Vector2f m_position;
    sf::Sprite m_graphics;
    sf::RectangleShape m_rs;

    Tile* comingFrom;

    int m_f, m_g, m_h;
    int m_gridX, m_gridY;
    sf::Text m_f_text, m_g_text, m_h_text, m_x_text, m_y_text, m_comingFromText;

    bool isClicked;
    bool isPassed;
    bool isPassable;
    bool isViewed;
    bool isStart;
    bool isEnd;
    bool isFinalPath;

    float inputCooldownAccumulator{ 0.0f };
    float inputCooldownDuration{ 0.2f };

    Tile() {}

    Tile(sf::Vector2f position, sf::RectangleShape rs) :
        m_f{ 0 },
        m_g{ 0 },
        m_h{ 0 },
        m_gridX{ -1 },
        m_gridY{ -1 },
        m_position{ position },
        m_rs{ rs },
        isClicked{ false },
        isPassed{ false },
        isPassable{ true },
        isViewed{ false },
        isStart{ false },
        isEnd{ false },
        isFinalPath{ false }
    {
        AssetManager& assets = *AssetManager::getInstance();
        m_f_text.setFont(assets.font);
        m_f_text.setCharacterSize(20);
        m_f_text.setFillColor(sf::Color::Black);
        m_f_text.setPosition(rs.getPosition() + sf::Vector2f({ 15.0f, 10.0f }));

        m_g_text.setFont(assets.font);
        m_g_text.setCharacterSize(10);
        m_g_text.setFillColor(sf::Color::Black);
        m_g_text.setPosition(rs.getPosition() + sf::Vector2f({ 5.0f, 2.0f }));

        m_h_text.setFont(assets.font);
        m_h_text.setCharacterSize(10);
        m_h_text.setFillColor(sf::Color::Black);
        m_h_text.setPosition(rs.getPosition() + sf::Vector2f({ 30.0f, 2.0f }));

        m_x_text.setFont(assets.font);
        m_x_text.setCharacterSize(7);
        m_x_text.setFillColor(sf::Color::Black);
        m_x_text.setPosition(rs.getPosition() + sf::Vector2f({ 5.0f, 40.0f }));

        m_y_text.setFont(assets.font);
        m_y_text.setCharacterSize(7);
        m_y_text.setFillColor(sf::Color::Black);
        m_y_text.setPosition(rs.getPosition() + sf::Vector2f({ 15.0f, 40.0f }));

        m_comingFromText.setFont(assets.font);
        m_comingFromText.setCharacterSize(20);
        m_comingFromText.setOrigin({ 10,10 });
        m_comingFromText.setStyle(sf::Text::Bold);
        m_comingFromText.setFillColor(sf::Color::Black);
        m_comingFromText.setPosition(rs.getPosition() + sf::Vector2f({ 40.0f, 40.0f }));
        m_comingFromText.setString("^");

        setCosts(0, 0);
    }

    sf::Vector2f getPosition() const
    {
        return m_position;
    }

    void setCosts(int g, int h)
    {
        if (m_f == 1) return;
        m_g = g;
        m_g_text.setString(std::to_string(g));
        m_h = h;
        m_h_text.setString(std::to_string(h));
        m_f = g + h;
        m_f_text.setString(std::to_string(g + h));
    }

    std::array<int, 3> getCosts()
    {
        return { m_g, m_h, m_f };
    }

    void clearFlags()
    {
        isClicked = false;
        isPassed = false;
        isPassable = true;
        isViewed = false;
        isFinalPath = false;
    }

    void unmark()
    {
        m_rs.setOutlineColor(sf::Color::Transparent);
        m_rs.setFillColor(sf::Color::White);
        m_f = 0;
        m_f_text.setString("");
        m_g = 0;
        m_g_text.setString("");
        m_h = 0;
        m_h_text.setString("");
        clearFlags();
    }

    void markStart()
    {
        m_f = 1;
        m_f_text.setString("S");
        isStart = true;
    }

    void markEnd()
    {
        m_f = 1;
        m_f_text.setString("E");
        isEnd = true;
    }

    void markFinalPath()
    {
        m_f_text.setString("");
        m_comingFromText.setFillColor(sf::Color::White);
        isFinalPath = true;
    }

    void markPassed()
    {
        if (m_f == 1) return;
        isPassed = true;
    }

    void markViewed()
    {
        if (m_f == 1) return;
        isViewed = true;
    }

    virtual void update(float dt, sf::Vector2f mPos) override
    {
        if (!isClicked) m_rs.setFillColor(sf::Color::White);
        if (isViewed) m_rs.setFillColor(sf::Color::Yellow);
        if (isPassed) m_rs.setFillColor(sf::Color::Green);
        if (m_g + m_h != m_f) m_rs.setFillColor(sf::Color::Cyan);
        if (isFinalPath) m_rs.setFillColor(sf::Color::Red);

        m_rs.setOutlineColor(sf::Color::Transparent);

        if (isHovered(mPos))
        {
            m_rs.setOutlineColor(sf::Color::Magenta);
        }
        if (inputCooldownAccumulator > 0.0f)
        {
            inputCooldownAccumulator = std::fmax(0.0f, inputCooldownAccumulator - dt);
        }
        else
        {
            if (isHovered(mPos))
            {
                //std::cout << "Is hovered" << std::endl;
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    inputCooldownAccumulator = inputCooldownDuration;
                    isClicked = !isClicked;
                    isPassable = !isPassable;
                    sf::Color color;
                    color.r = 100;
                    color.g = 100;
                    color.b = 100;
                    m_rs.setFillColor(color);
                }
            }
        }
    }

    void setPreviousTile(Tile* tile, float angle)
    {
        comingFrom = tile;
        m_comingFromText.setRotation(angle);
        //std::cout << "["<<m_gridX<<", "<<m_gridY<<"] <-["<<tile->m_gridX<<", "<<tile->m_gridY<<"] rotated by " << angle << "\n";
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates rs) const
    {
        target.draw(m_rs);
        target.draw(m_x_text);
        target.draw(m_y_text);
        if (m_f != 0 && m_f == m_g + m_h)
        {
            target.draw(m_g_text);
            target.draw(m_h_text);
            target.draw(m_f_text);
        }
        if (m_f == 1)
        {
            target.draw(m_f_text);
        }
        if (m_f > 1)
            target.draw(m_comingFromText);
    }
};

const sf::Color Tile::tileDefaultColor = sf::Color(0, 0, 255);
const float Tile::tileWidth = 50.0f;