#pragma once

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
        // injecting the heap sort function that compares distances from the origin and to the destination to find the best candidate for the next move
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

    std::vector<Tile*> getTileNeighbors(int x, int y, bool justNew = true)
    {
        std::vector<Tile*> result;
        std::array<std::array<int, 3>, 3> angles{
            135,   90,   45, // -1,-1  -1,0  -1,1           //  0,0  0,1  0,2    //  135 180 225  ^ rotations corresponding to graphics
            180,    0,    0, //  0,-1   0,0   0,1  (i,j)=>  //  1,0  1,1  1,2    //   90   0 270  middle point is (x,y)
            225,  270,  315  //  1,-1   1,0   1,1           //  2,0  2,1  2,2    //   45   0 315
        };
        // ^ points up at 0 rotation and down at 180 // bad mapping but if it works it ain't entirely stupid
        // visual representation of path ancestry direction
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
                if (justNew && grid[x + i][y + j].isViewed) // already part of the possible route
                {
					// update information regarding previous tile in path for shorter way to origin => shorter way overall
					int prev_g = grid[x + i][y + j].comingFrom->getCosts()[0];
                    int this_g = grid[x][y].getCosts()[0];

                    if (this_g < prev_g)
                    {
                        grid[x + i][y + j].setPreviousTile(&grid[x][y], angles[i + 1][j + 1]); // offset by 1 to map on angles[][] indexes due to i,j negative start
                        grid[x + i][y + j].setCosts(grid[x + i][y + j].comingFrom->m_g + distanceCost(grid[x + i][y + j].comingFrom->m_gridX, grid[x + i][y + j].comingFrom->m_gridY, x + i, y + j), distanceCost(x + i, y + j, this->endX, this->endY));
                    }
                }
                else
                {
					grid[x + i][y + j].setPreviousTile(&grid[x][y], angles[i + 1][j + 1]); // offset by 1 to map on angles[][] indexes due to i,j negative start
					result.push_back(&grid[x + i][y + j]);
                }

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
            if (elem->isViewed)
            {
                
            }
            else
            {
				elem->markViewed();
				elem->setCosts(elem->comingFrom->m_g + distanceCost(sx, sy, elem->m_gridX, elem->m_gridY), distanceCost(elem->m_gridX, elem->m_gridY, ex, ey));
				tileHeap.push(elem);
            }
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

// only included in 1 file so definition here is okay for now
int Board::s_steps = 0;