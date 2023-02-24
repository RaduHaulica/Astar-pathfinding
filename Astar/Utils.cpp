#include "Utils.h"

int distanceCost(float x1, float y1, float x2, float y2)
{
    int deltaX = std::abs(x1 - x2);
    int deltaY = std::abs(y1 - y2);
    //return fmax(deltaX, deltaY) * 10 + fmin(deltaX, deltaY) * 4; // 10 lateral 14 diagonal hybrid chebyshev
    //return std::abs(x1 - x2) + std::abs(y1 - y2); // manhattan distance
    return 10 * std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)); // euclidian distance
}

sf::Font AssetManager::font;
AssetManager* AssetManager::instance = nullptr;

void AssetManager::initialize()
{
	font.loadFromFile("Roboto-Bold.ttf");
}

AssetManager* AssetManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new AssetManager();
        instance->initialize();
    }
    return instance;
}

AssetManager::~AssetManager()
{
    delete instance;
}