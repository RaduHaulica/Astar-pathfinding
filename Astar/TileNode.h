#pragma once

#include "Tile.h"

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