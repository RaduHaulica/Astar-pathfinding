## A* pathfinding algorithm

The [A* pathfinding algorithm] is heuristic guided graph traversal algorithm that finds the shortest path between two nodes, expanding on Djisktra's shortest path algorithm.

The heuristic is based on the distance from the current node to both the start and end points, and each point is scored when it's added to the current fringe. This means that any node added to the path is the closest to both the start and end nodes from the available options at the time of choosing.

Each step, the algorithm finds the currently best scored node in the current fringe, it adds it to the path, and then explores and scores its neighbors, adding the current node as their predecessor (essentially creating a path tree).

The admissibility criterion for the path is relaxed, opting for speed of search instead of examining all equally ranked next steps, which results in suboptimal paths.

When the end node is found, the algorithm backtracks through each node's predecessor all the way back to the start node and creates a solution path.
 
Tried out a few new things for this one. Euclidean, manhattan, and settled on a hybrid chebyshev/euclidean distance (octile).

Implemented a generic (type, predicate) heap as a utility for sorting the open node set.

## How to use



## More detail

[A* pathfinding algorithm]: <https://en.wikipedia.org/wiki/A*_search_algorithm>