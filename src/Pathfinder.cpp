#include "Pathfinder.h"
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <climits> // For INT_MAX

// Internal struct for A* nodes
struct Node {
    Point point;
    int gCost; // Cost from start to current node
    int hCost; // Heuristic cost from current node to end
    int fCost() const { return gCost + hCost; }

    // For priority queue comparison
    bool operator>(const Node& other) const {
        return fCost() > other.fCost();
    }
};

Pathfinder::Pathfinder(const Grid& grid) : grid(grid) {}

int Pathfinder::calculateHeuristic(Point a, Point b) {
    // Manhattan distance is a good heuristic for grid movement (no diagonals)
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Point> Pathfinder::reconstructPath(const std::vector<Point>& cameFrom, Point current) {
    std::vector<Point> total_path;
    total_path.push_back(current);
    int currentIndex = current.y * grid.getWidth() + current.x;

    while (cameFrom[currentIndex].x != -1) { // -1 indicates no parent
        current = cameFrom[currentIndex];
        total_path.push_back(current);
        currentIndex = current.y * grid.getWidth() + current.x;
    }
    std::reverse(total_path.begin(), total_path.end());
    return total_path;
}

std::vector<Point> Pathfinder::findPath(Point start, Point end) {
    if (start == end || grid.at(end.x, end.y).obstacle == ObstacleType::Wall) {
        return {}; // No path needed or destination is a wall
    }

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::vector<Point> cameFrom(grid.getWidth() * grid.getHeight(), {-1, -1});
    std::vector<int> gCost(grid.getWidth() * grid.getHeight(), INT_MAX);

    gCost[start.y * grid.getWidth() + start.x] = 0;
    openSet.push({start, 0, calculateHeuristic(start, end)});

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.point == end) {
            return reconstructPath(cameFrom, current.point);
        }

        for (int i = 0; i < 4; ++i) {
            Point neighborPoint = {current.point.x + dx[i], current.point.y + dy[i]};

            if (neighborPoint.x < 0 || neighborPoint.x >= grid.getWidth() ||
                neighborPoint.y < 0 || neighborPoint.y >= grid.getHeight() ||
                grid.at(neighborPoint.x, neighborPoint.y).obstacle == ObstacleType::Wall) {
                continue;
            }

            int tentative_gCost = gCost[current.point.y * grid.getWidth() + current.point.x] + grid.at(neighborPoint.x, neighborPoint.y).cost;
            int neighborIndex = neighborPoint.y * grid.getWidth() + neighborPoint.x;

            if (tentative_gCost < gCost[neighborIndex]) {
                cameFrom[neighborIndex] = current.point;
                gCost[neighborIndex] = tentative_gCost;
                int hCost = calculateHeuristic(neighborPoint, end);
                openSet.push({neighborPoint, tentative_gCost, hCost});
            }
        }
    }

    return {}; // No path found
}

std::vector<Point> Pathfinder::findPathToZone(Point start, const std::string& zoneName) {
    auto it = grid.zones.find(zoneName);
    if (it == grid.zones.end()) {
        return {}; // Zone not found
    }

    const Zone& zone = it->second;
    std::vector<Point> bestPath;

    // Iterate over all cells in the zone to find the shortest path to any of them
    for (int y = zone.y; y < zone.y + zone.height; ++y) {
        for (int x = zone.x; x < zone.x + zone.width; ++x) {
            if (grid.at(x, y).obstacle != ObstacleType::Wall) {
                std::vector<Point> currentPath = findPath(start, {x, y});
                if (!currentPath.empty() && (bestPath.empty() || currentPath.size() < bestPath.size())) {
                    bestPath = currentPath;
                }
            }
        }
    }

    return bestPath;
}