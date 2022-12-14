A* pathfinding algorithm
 
https://en.wikipedia.org/wiki/A*_search_algorithm
A heuristic guided graph traversal algorithm that finds the shortest path between two nodes, expanding on Djisktra's algorithm.

The heuristic is based on the fact that the distance from any node on the path is the shortest to both the start and end nodes, thus scoring each node in the current fringe.

Each step, the algorithm finds the currently best scored node in the current fringe, it adds it to the path, and then explores and scores its neighbors, adding the current node as their predecessor (essentially creating a path tree).

When the end node is found, the algorithm backtracks through each node's predecessor all the way back to the start node and creates the optimal path.
 
Tried out a few new things for this one. Euclidean, manhattan, and a hybrid chebyshev/euclidean distance.
Implemented a generic (type, predicate) heap for sorting the open node set
