## A* pathfinding algorithm

![A* cover image](https://github.com/RaduHaulica/Astar-pathfinding/blob/43e1e362dae158c8f2e4d3219702f7c091d1b142/Astar/media/astar%20cover.png)

The [A* pathfinding algorithm] is heuristic guided graph traversal algorithm that finds the shortest path between two nodes, expanding on Djisktra's shortest path algorithm.

The heuristic is based on the distance from the current node to both the start and end points, and each point is scored when it's added to the current fringe. This means that any node added to the path is the closest to both the start and end nodes from the available options at the time of choosing.

Each step, the algorithm finds the currently best scored node in the current fringe, it adds it to the path, and then explores and scores its neighbors, adding the current node as their predecessor (essentially creating a path tree).

![A* pathfinding complete](https://github.com/RaduHaulica/Astar-pathfinding/blob/43e1e362dae158c8f2e4d3219702f7c091d1b142/Astar/media/astar%20full.gif)

The admissibility criterion for the path is relaxed, opting for speed of search instead of examining all equally ranked next steps, which sometimes results in suboptimal paths (but very close).

When the end node is found, the algorithm backtracks through each node's predecessor all the way back to the start node and creates a solution path.
 
Tried out a few new things for this one. Euclidean, manhattan, and settled on a hybrid chebyshev/euclidean distance (octile).

Implemented a generic (type, predicate) heap as a utility for sorting the open node set.

## How to use

Click and drag to create walls.

Press Space to start algorithm visualization.

## More details

[A* pathfinding algorithm]: <https://en.wikipedia.org/wiki/A*_search_algorithm>
