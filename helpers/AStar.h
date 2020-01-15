//
// Created by badru on 15.01.2020.
//

#ifndef AICUP2019_ASTAR_H
#define AICUP2019_ASTAR_H

#include "vector"
#include <cmath>
#include <list>
#include <algorithm>
#include "../model/Game.hpp"

// A*
struct PathNode
{
    // Координаты точки на карте.
    Vec2Double Position;
    // Длина пути от старта (G).
    double PathLengthFromStart=10000;
    // Точка, из которой пришли в эту точку.
    PathNode* CameFrom;
    // Примерное расстояние до цели (H).
    double HeuristicEstimatePathLength;

    std::vector<Vec2Double> path = {};
    // Ожидаемое полное расстояние до цели (F).
    double EstimateFullPathLength() {
        return this->PathLengthFromStart + this->HeuristicEstimatePathLength;
    }

    const double EstimateFullPathLengthConst() const {
        return this->PathLengthFromStart + this->HeuristicEstimatePathLength;
    }

    bool operator < (const PathNode &node) const
    {
        return EstimateFullPathLengthConst()<node.EstimateFullPathLengthConst();
    }

    bool operator == (const PathNode &node)
    {
        return floor(Position.x) == floor(node.Position.x) && floor(Position.y) == floor(node.Position.y);
    }
};

class AStar {
public :
    AStar(){};

    bool isPointInUnit(const Vec2Double point, const Vec2Double unitPosition, const Vec2Double unitSize);

    double GetHeuristicPathLength(Vec2Double from, Vec2Double to);

    PathNode GetMinF(std::list<PathNode> list);

    std::vector<Vec2Double> GetPathForNode(PathNode pathNode);

    std::vector<PathNode> GetNeighbours(PathNode pathNode, Vec2Double goal,int sizeX, int sizeY, const Game &game, const Unit &currentUnit);

    std::vector<Vec2Double> FindPath(Vec2Double from, Vec2Double to, const Game &game, const Unit &currentUnit);
};

#endif //AICUP2019_ASTAR_H
