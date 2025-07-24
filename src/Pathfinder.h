#pragma once
#include "Grid.h"
#include <vector>
#include <string>

// A simple struct for a point/coordinate
struct Point {
    int x, y;

    // Equality operator for comparisons
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class Pathfinder {
public:
    /**
     * @brief Constructs a Pathfinder that operates on a given grid.
     * @param grid A constant reference to the grid to navigate.
     */
    explicit Pathfinder(const Grid& grid);

    /**
     * @brief Finds the shortest path from a start point to an end point using A*.
     * @param start The starting grid coordinates.
     * @param end The destination grid coordinates.
     * @return A vector of Points representing the path. Empty if no path is found.
     */
    std::vector<Point> findPath(Point start, Point end);

    /**
     * @brief Finds the shortest path from a start point to the nearest valid cell of a named zone.
     * @param start The starting grid coordinates.
     * @param zoneName The name of the destination zone (e.g., "Cafe", "Forest").
     * @return A vector of Points representing the path. Empty if no path or zone is found.
     */
    std::vector<Point> findPathToZone(Point start, const std::string& zoneName);

private:
    const Grid& grid;

    // Helper function to reconstruct the path once the destination is reached.
    std::vector<Point> reconstructPath(const std::vector<Point>& cameFrom, Point current);
    
    // Heuristic function for A* (Manhattan distance).
    int calculateHeuristic(Point a, Point b);
};