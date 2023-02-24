#pragma once

#include <SFML/Graphics.hpp>


// calculates heuristic distance between two points on the game board
int distanceCost(float x1, float y1, float x2, float y2);

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


/**
/ singleton used for loading app-wide resources
*/
class AssetManager
{
private:
    // static pointer that points to the only instance of this class
    static AssetManager* instance;

    AssetManager() {}; // private constructor (defined)

    void initialize();

public:
    static sf::Font font;

    // deleting copy constructor
    AssetManager(const AssetManager& other) = delete;

    ~AssetManager();

    static AssetManager* getInstance();

};