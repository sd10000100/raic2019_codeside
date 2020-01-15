//
// Created by badru on 15.01.2020.
//

#include "AStar.h"

bool AStar::isPointInUnit(const Vec2Double point, const Vec2Double unitPosition, const Vec2Double unitSize)
{
    return point.x >= unitPosition.x - unitSize.x / 2
           &&
           point.x <= unitPosition.x + unitSize.x / 2
           &&
           point.y >= unitPosition.y
           &&
           point.y <= unitPosition.y + unitSize.y;
}

double AStar::GetHeuristicPathLength(Vec2Double from, Vec2Double to)
{
    return fabs(from.x - to.x) + fabs(from.y - to.y);
}

PathNode AStar::GetMinF(std::list<PathNode> list)
{
    PathNode minElem;
    double minVal = 10000;

    for(PathNode &item : list)
    {
        double temp = item.EstimateFullPathLength();
        if(temp<minVal)
        {
            minElem = item;
            minVal = temp;
        }
    }
    return minElem;
}

std::vector<Vec2Double> AStar::GetPathForNode(PathNode pathNode)
{
    return pathNode.path;
//    std::vector<Vec2Double> result = {};
//    PathNode* currentNode = &pathNode;
//    while (currentNode != nullptr)
//    {
//        result.push_back(currentNode->Position);
//        currentNode = currentNode->CameFrom;
//    }
//    std::reverse(std::begin(result), std::end(result));
//    return result;
}

std::vector<PathNode> AStar::GetNeighbours(PathNode pathNode, Vec2Double goal,int sizeX, int sizeY, const Game &game, const Unit &currentUnit)
{
    std::vector<PathNode> result ={};

// Соседними точками являются соседние по стороне клетки.
    std::vector<Vec2Double> neighbourPoints = {};

    int x = floor(pathNode.Position.x);
    int y = floor(pathNode.Position.y);
    neighbourPoints.push_back(Vec2Double(x + 1, y));
    neighbourPoints.push_back(Vec2Double(x - 1, y));
    neighbourPoints.push_back(Vec2Double(x, y + 1));
    neighbourPoints.push_back(Vec2Double(x, y - 1));

    neighbourPoints.push_back(Vec2Double(x + 1, y-1));
    neighbourPoints.push_back(Vec2Double(x - 1, y-1));
    neighbourPoints.push_back(Vec2Double(x+1, y + 1));
    neighbourPoints.push_back(Vec2Double(x-1, y + 1));

    for (Vec2Double point : neighbourPoints)
    {
        // Проверяем, что не вышли за границы карты.
        if (point.x < 0 || point.x >= sizeX)
            continue;
        if (point.y < 0 || point.y >= sizeY)
            continue;
        // Проверяем, что по клетке можно ходить.
        auto temp = game.level.tiles[point.x][point.y];
        if ((temp == Tile::WALL))
            continue;
        bool isSomeUnitNear = false;
        for(auto unit : game.units)
        {
            if(unit.id!=currentUnit.id)
            {
                if(isPointInUnit(point, currentUnit.position, currentUnit.size))
                    isSomeUnitNear = true;
            }
        }
        if(isSomeUnitNear)
            continue;
        // Заполняем данные для точки маршрута.
        PathNode neighbourNode;
        neighbourNode.Position.x = point.x;
        neighbourNode.Position.y = point.y;
        neighbourNode.path = pathNode.path;
        //neighbourNode.CameFrom = &pathNode;
//        for(auto item : pathNode.path)
//        {
//            neighbourNode.path.push_back(Vec2Double(item.x, item.y));
//        }
        neighbourNode.path.push_back(Vec2Double(pathNode.Position.x, pathNode.Position.y));
        neighbourNode.PathLengthFromStart = pathNode.PathLengthFromStart +1,
                neighbourNode.HeuristicEstimatePathLength = GetHeuristicPathLength(point, goal);
        result.push_back(neighbourNode);
    }
    return result;
}


std::vector<Vec2Double> AStar::FindPath(Vec2Double from, Vec2Double to, const Game &game, const Unit &currentUnit)
{
    std::list<PathNode> Idle = {};
    std::list<PathNode> visited = {};

    from.x = floor(from.x);
    from.y = floor(from.y);

    to.x = floor(to.x);
    to.y = floor(to.y);


    int width = game.level.tiles.size();
    int height = game.level.tiles[0].size();

    // Шаг 2.
    PathNode startNode = PathNode();
    startNode.CameFrom = nullptr;
    startNode.path = {};
    startNode.Position = from;
    startNode.PathLengthFromStart = 0,
            startNode.HeuristicEstimatePathLength = GetHeuristicPathLength(from, to);

    Idle.push_back(startNode);

    while (Idle.size()> 0) {

//        std::cerr<<"Idle size: "<< Idle.size()<<'\n';
//        std::cerr<<"visited size: "<< visited.size()<<'\n';

        PathNode currentNode = GetMinF(Idle);
        if (floor(currentNode.Position.x) == floor(to.x) && floor(currentNode.Position.y) == floor(to.y)) {
            return GetPathForNode(currentNode);
        }
        // Шаг 5.

        //Idle = removeItemFromList(currentNode, Idle);
        Idle.pop_front();
        visited.push_back(currentNode);

        // Шаг 6.
        auto neighs = GetNeighbours(currentNode, to, width, height, game, currentUnit);
        for (auto neighbourNode : neighs) {
            // Шаг 7.
//            if (GetCountByPosition(neighbourNode.Position, visited, game) > 0)
//                continue;
            auto visitedNodeIter = std::find(visited.begin(), visited.end(), neighbourNode);
            if(visitedNodeIter!=visited.end())
                continue;
            auto idleNodeIter = std::find(Idle.begin(), Idle.end(), neighbourNode);
            // Шаг 8.
            if (idleNodeIter == Idle.end())
                Idle.push_back(neighbourNode);
            else if (idleNodeIter->PathLengthFromStart > neighbourNode.PathLengthFromStart) {
                // Шаг 9.

                idleNodeIter->CameFrom = &neighbourNode;
                idleNodeIter->path = {};
                //neighbourNode.CameFrom = &pathNode;
                for(auto item : neighbourNode.path)
                {
                    idleNodeIter->path.push_back(Vec2Double(item.x, item.y));
                }
                idleNodeIter->path.push_back(Vec2Double(neighbourNode.Position.x, neighbourNode.Position.y));
                //neighbourNode.CameFrom = &pathNode;

                idleNodeIter->PathLengthFromStart = neighbourNode.PathLengthFromStart;
                //Idle.push_back(*openNode);
            }
        }

    }
    // Шаг 10.
    return {};
}
// A*