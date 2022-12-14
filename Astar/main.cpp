#include <SFML/Graphics.hpp>

#include <array>
#include <vector>
#include <concepts>
#include <functional>
#include <iostream>
#include <ostream>

#include "heap.h"

class AssetManager
{
public:
    sf::Font font;

    void initialize()
    {
        font.loadFromFile("Roboto-Bold.ttf");

    }
};

AssetManager assets;

// ==================================================
//             concepts & interfaces
// ==================================================

class IUpdate
{
public:
    virtual void update(float dt, sf::Vector2f mPos) = 0;
};

template <class GraphicsComponent>
concept isDrawable = std::derived_from<GraphicsComponent, sf::Drawable>;

// ==================================================
//             classes
// ==================================================

int distanceCost(float x1, float y1, float x2, float y2)
{
    int deltaX = std::abs(x1 - x2);
    int deltaY = std::abs(y1 - y2);
    //return fmax(deltaX, deltaY) * 10 + fmin(deltaX, deltaY) * 4; // 10 lateral 14 diagonal hybrid chebyshev
    //return std::abs(x1 - x2) + std::abs(y1 - y2); // manhattan distance
    return 10 * std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)); // euclidian distance
}

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

class TileNode
{
public:
    Tile* m_tile;
    TileNode* m_tileParent;

    TileNode& operator=(const TileNode& other)
    {
        m_tile = other.m_tile;
        m_tileParent = other.m_tileParent;
        return *this;
    }

    bool operator==(const TileNode& other)
    {
        return m_tile == m_tile;
    }

};

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

class Board : public IUpdate, public sf::Drawable
{
public:
    static const int size = 15;
    static int s_steps;

    bool started{ false };

    std::array<std::array<Tile, size>, size> grid;
    Heap<Tile*, std::function<bool(Tile*, Tile*)>> tileHeap;

    int startX, startY, endX, endY, currentX, currentY;

    float inputCooldownDuration{ 0.1f };
    float inputCooldownAccumulator{ 0.0f };
    float pathStepAccumulator{ 0.0f };

    Board()
    {
        tileHeap.setPredicate([](Tile* a, Tile* b) {
            //std::cout << "A: " << * a << " < B: " << * b << " ? " << ((a->m_f == b->m_f) ? (a->m_h < b->m_h) : (a->m_f < b->m_f)) << std::endl;
            return (a->m_f == b->m_f) ? (a->m_h < b->m_h) : (a->m_f < b->m_f);
            });
    }

    virtual void update(float dt, sf::Vector2f mPos) override
    {
        if (inputCooldownAccumulator > 0.0f)
        {
            inputCooldownAccumulator = std::fmax(0.0f, inputCooldownAccumulator - dt);
        }
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                inputCooldownAccumulator = inputCooldownDuration;
                started = true;
                findPath(currentX, currentY, endX, endY);
            }
        }

        if (started)
        {
            if (pathStepAccumulator > 0.01f)
            {
                findPath(currentX, currentY, endX, endY);
                pathStepAccumulator = 0.0f;
            }
            else
            {
                pathStepAccumulator += dt;
            }
        }

        for (auto& arr : grid)
            for (auto& elem : arr)
                elem.update(dt, mPos);
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates rs) const
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                //std::cout << "elem[" <<i<< "][" << j << "]= "<< grid[i][j].getPosition().x <<", "<< grid[i][j].getPosition().y << std::endl;
                target.draw(grid[i][j]);
            }
        }
    }

    std::vector<Tile*> getTileNeighbors(int x, int y)
    {
        std::vector<Tile*> result;
        std::array<std::array<int, 3>, 3> angles{
            135,   90,   45, // -1,-1  -1,0  -1,1           //  0,0  0,1  0,2    //  135 180 225  ^ rotations corresponding to graphics
            180,    0,    0, //  0,-1   0,0   0,1  (i,j)=>  //  1,0  1,1  1,2    //   90   0 270  middle point is (x,y)
            225,  270,  315  //  1,-1   1,0   1,1           //  2,0  2,1  2,2    //   45   0 315
        };
        // ^ points up at 0 rotation and down at 180 // bad mapping but if it works it ain't entirely stupid
        for (int j = -1; j <= 1; j++)
        {
            for (int i = -1; i <= 1; i++)
            {
                //if ((i + j) % 2 == 0) continue; // removes diagonal movement
                if (x + i < 0 || x + i >= size || y + j < 0 || y + j >= size) continue; // out of bounds
                if (i == 0 && j == 0) continue; // identity
                if (!grid[x + i][y + j].isPassable) continue; // obstacle
                if (grid[x + i][y + j].isStart == true) continue; // start tile
                if (grid[x + i][y + j].isPassed) continue; // already part of the tested route
                if (grid[x + i][y + j].isViewed) continue; // already part of the possible route

                grid[x + i][y + j].setPreviousTile(&grid[x][y], angles[i + 1][j + 1]); // offset by 1 to map on angles[][] indexes
                result.push_back(&grid[x + i][y + j]);
            }
        }
        //std::cout << "Neighbors of ("<<x<<", "<<y<<"): " << result.size() << "\n";
        for (auto elem : result)
        {
            //std::cout << " [" << elem->m_gridX << ", " << elem->m_gridY << "]";
        }
        //std::cout << "\n";
        return result;
    }

    void findPath(int sx, int sy, int ex, int ey)
    {
        if (sx == ex && sy == ey)
        {
            //std::cout << "Requested path already reached\n";
            return;
        }
        //std::cout << "findPath called for " << "[" << sx << ", " << sy << "] -> [" << ex << ", " << ey << "]\n";
        std::vector<Tile*> neighbors = getTileNeighbors(sx, sy);
        for (auto& elem : neighbors)
        {
            elem->markViewed();
            elem->setCosts(elem->comingFrom->m_g + distanceCost(sx, sy, elem->m_gridX, elem->m_gridY), distanceCost(elem->m_gridX, elem->m_gridY, ex, ey));
            tileHeap.push(elem);
        }
        //tileHeap.print();

        Tile* bestPath = tileHeap.pop();
        //std::cout << "Popped: [" << bestPath->m_gridX << ", " << bestPath->m_gridY << "]\n";

        currentX = bestPath->m_gridX;
        currentY = bestPath->m_gridY;
        bestPath->markPassed();

        if (bestPath->m_gridX == ex && bestPath->m_gridY == ey)
        {
            bestPath = bestPath->comingFrom;
            std::cout << "destination reached\n";
            while (bestPath->comingFrom != nullptr)
            {
                std::cout << "marking [" << bestPath->m_gridX << ", " << bestPath->m_gridY << "]\n";
                bestPath->markFinalPath();
                bestPath = bestPath->comingFrom;
            }
        }

        //findPath(currentX, currentY, ex, ey); // goes through the whole algorithm
    }

    void initializeGrid(sf::Vector2f initialPosition, sf::RectangleShape modelTile)
    {
        float tileWidth = Tile::tileWidth;
        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < size; i++)
            {
                sf::Vector2f newPos = initialPosition + sf::Vector2f(i * (Tile::tileWidth + 5), j * (Tile::tileWidth + 5));
                modelTile.setPosition(newPos);
                grid[i][j] = Tile(newPos, modelTile);
                grid[i][j].m_gridX = i;
                grid[i][j].m_gridY = j;
                grid[i][j].comingFrom = nullptr;
                grid[i][j].m_x_text.setString(std::to_string(grid[i][j].m_gridX));
                grid[i][j].m_y_text.setString(std::to_string(grid[i][j].m_gridY));
            }
        }


        startX = currentX = 2;
        startY = currentY = 1;
        grid[startX][startY].markStart();

        endX = 12;
        endY = 10;
        grid[endX][endY].markEnd();

        //tileHeap.push(&grid[startX][startY]);

        //std::cout << "[2,3] -> [3,6] = "<< distanceCost(startX, startY, endX, endY) << std::endl;
    }
};
int Board::s_steps = 0;

int main()
{
    assets.initialize();

    sf::Text helpText;
    helpText.setFont(assets.font);
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