#include "MyStrategy.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <list>

MyStrategy::MyStrategy() {
    PotentialField = {};
    //SavedPath = {};
}

void MyStrategy::SetPotentialField(const Game &game){
    int width = game.level.tiles.size();
    int height = game.level.tiles[0].size();

    for (int i=0;i<width;i++) {
        PotentialField.push_back(std::vector<double>(height, 0));

    }
    bool assert1 = game.level.tiles.size() == PotentialField.size();
    bool assert2 = game.level.tiles[0].size() == PotentialField[0].size();
};

double distanceSqr(Vec2Double a, Vec2Double b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

inline double area (Vec2Double a, Vec2Double b, Vec2Double c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

inline bool intersect_1 (double a, double b, double c, double d) {
    if (a > b) std::swap(a, b);
    if (c > d) std::swap(c, d);
    return std::max(a, c) <= std::min(b, d);
}

bool intersect (Vec2Double a, Vec2Double b, Vec2Double c, Vec2Double d) {
    return intersect_1 (a.x, b.x, c.x, d.x)
           && intersect_1 (a.y, b.y, c.y, d.y)
           && area(a,b,c) * area(a,b,d) <= 0
           && area(c,d,a) * area(c,d,b) <= 0;
}

bool isPointInUnit(const Vec2Double point, const Unit &unit)
{
    return point.x >= unit.position.x - unit.size.x / 2
           &&
           point.x <= unit.position.x + unit.size.x / 2
           &&
           point.y >= unit.position.y
           &&
           point.y <= unit.position.y + unit.size.y;
}

bool isPointInUnitWithRadius(const Vec2Double point, const Unit &unit, double radius)
{
    return point.x >= unit.position.x - unit.size.x / 2 - radius
           &&
           point.x <= unit.position.x + unit.size.x / 2 + radius
           &&
           point.y >= unit.position.y - radius
           &&
           point.y <= unit.position.y + unit.size.y - radius;
}


struct Edge{
    Edge(Vec2Double p1, Vec2Double p2):start(p1), finish(p2){}
    Vec2Double start;
    Vec2Double finish;
};

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

double GetHeuristicPathLength(Vec2Double from, Vec2Double to)
{
    return fabs(from.x - to.x) + fabs(from.y - to.y);
}

PathNode GetMinF(std::list<PathNode> list)
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


std::vector<Vec2Double> GetPathForNode(PathNode pathNode)
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

std::vector<PathNode> GetNeighbours(PathNode pathNode, Vec2Double goal,int sizeX, int sizeY, const Game &game, const Unit &currentUnit)
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
                if(isPointInUnit(point, currentUnit))
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

bool isPointInArena(Vec2Double pos, const Game &game){
    return (pos.x>=0 && pos.x<=game.level.tiles.size() &&
    pos.y>=0 && pos.y<=game.level.tiles[0].size());
}

std::vector<Vec2Double> FindPath(Vec2Double from, Vec2Double to, const Game &game, const Unit &currentUnit)
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

bool isPointInTriangle(Vec2Double p, Vec2Double p1, Vec2Double p2,Vec2Double p3){
    std::vector<Edge> stackEnges = {};
    int countIntersect = 0;
    stackEnges.push_back(Edge(p1,p2));
    stackEnges.push_back(Edge(p2,p3));
    stackEnges.push_back(Edge(p3,p1));

    for(Edge item : stackEnges)
    {
        if(intersect (p, Vec2Double(10000,1), item.start, item.finish))
            countIntersect++;
    }
    if(countIntersect%2==0)
        return false;
    else {
        return true;
    }
}

Vec2Double TurnAndResize(Vec2Double from, Vec2Double to, double lenght, double angle)
{


    double l = sqrt(
            (to.x - from.x) * (to.x - from.x) +
            (to.y - from.y) * (to.y - from.y));

    double Vx = to.x - from.x;
    double Vy = to.y - from.y;
    double x = Vx * cos(angle) - Vy * sin(angle);
    double y = Vy * cos(angle) + Vx * sin(angle);
    to.x = from.x + x;
    to.y = from.y + y;


    Vx = to.x - from.x;
    Vy = to.y - from.y;
    to.x = from.x + Vx * (lenght/l);
    to.y = from.y + Vy * (lenght/l);


    return to;
}



bool isIntersectUnit(const Vec2Double unitPos, const Vec2Double aim, const Unit unit, double r)
{

    if(intersect(unitPos,aim,Vec2Double(unit.position.x-unit.size.x/2-r, unit.position.y-r), Vec2Double(unit.position.x+unit.size.x/2+r, unit.position.y-r))){
        return true;
    }
    else if(intersect(unitPos,aim,Vec2Double(unit.position.x-unit.size.x/2-r, unit.position.y+ unit.size.y+r), Vec2Double(unit.position.x+unit.size.x/2+r, unit.position.y+ unit.size.y+r))){
        return true;
    }
    else  if(intersect(unitPos,aim,Vec2Double(unit.position.x-unit.size.x/2-r, unit.position.y-r), Vec2Double(unit.position.x-unit.size.x/2-r, unit.position.y+ unit.size.y+r))){
        return true;
    }
    else  if(intersect(unitPos,aim,Vec2Double(unit.position.x+unit.size.x/2+r, unit.position.y-r), Vec2Double(unit.position.x+unit.size.x/2+r, unit.position.y+ unit.size.y+r))){
        return true;
    }
    return false;
}

bool isObstacleForAim(const Vec2Double unitPos, const Vec2Double aim, const Game &game)
{
    if(distanceSqr(unitPos, aim)<5) {
        return false;
    }
    int sqareStartX =  fmin(floor(unitPos.x), floor(aim.x));
    int sqareFinishX = fmax(ceil(unitPos.x),ceil(aim.x));
    int sqareStartY = fmin(floor(unitPos.y),floor(aim.y));
    int sqareFinishY = fmax(ceil(unitPos.y),ceil(aim.y));

    for(int i = sqareStartX; i<=sqareFinishX;i++)
    {
        for(int j = sqareStartY; j<=sqareFinishY;j++)
        {
            if(game.level.tiles[size_t(i)][size_t(j)] ==Tile::WALL)
            {
                if(intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i), size_t(j)+1))){
                    return true;
                }
                else if (intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i)+1, size_t(j)))){
                    return true;
                }
                else  if (intersect(unitPos,aim,Vec2Double(size_t(i)+1, size_t(j)), Vec2Double(size_t(i)+1, size_t(j)+1))){
                    return true;
                }
                else  if (intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)+1), Vec2Double(size_t(i)+1, size_t(j)+1))){
                    return true;
                }
            }
        }
    }
    return false;
}

double distToObstacle(const Vec2Double unitPos, const Vec2Double aim, const Game &game)
{
    if(distanceSqr(unitPos, aim)<5) {
        return false;
    }
    double min = 10000;
    int sqareStartX =  fmin(floor(unitPos.x), floor(aim.x));
    int sqareFinishX = fmax(ceil(unitPos.x),ceil(aim.x));
    int sqareStartY = fmin(floor(unitPos.y),floor(aim.y));
    int sqareFinishY = fmax(ceil(unitPos.y),ceil(aim.y));

    for(int i = sqareStartX; i<=sqareFinishX;i++)
    {
        for(int j = sqareStartY; j<=sqareFinishY;j++)
        {
            if(game.level.tiles[size_t(i)][size_t(j)] != Tile::EMPTY)
            {
                double dist = distanceSqr(unitPos, Vec2Double(size_t(i), size_t(j)));
                if(min<dist &&  intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i), size_t(j)+1))){
                    min = dist;
                }
                else if (min<dist &&  intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i)+1, size_t(j)))){
                    min = dist;
                }
                else  if (min<dist &&  intersect(unitPos,aim,Vec2Double(size_t(i)+1, size_t(j)), Vec2Double(size_t(i)+1, size_t(j)+1))){
                    min = dist;
                }
                else  if (min<dist &&  intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)+1), Vec2Double(size_t(i)+1, size_t(j)+1))){
                    min = dist;
                }
            }
        }
    }
    return sqrt(min);
}

int signedMax(int a, int b)
{
    if(abs(a)>=abs(b))
        return a;
    else return b;

}
int getSign(int x)
{
    if (x>0)
        return 1;
    else return -1;
}

void PutPotential(double power, double step, double** matr, int sizeX, int sizeY, Vec2Double p)
{
    int s = 0;
    int x = int(abs(floor(p.x)));
    int y = int(abs(floor(p.y)));
    for(int l = 0;l<fabs(power);l=l+step, s++)
    {
        for(int temp = y-s;temp<=s+y;temp++)
        {
            int tempArrMinX = int(floor(x-s));
            int tempArrMaxX = int(floor(x+s));
            if(tempArrMinX>=0 && tempArrMinX<sizeX && temp>=0 && temp<sizeY)
                matr[tempArrMinX][temp]=signedMax(matr[tempArrMinX][temp],getSign(power)*(abs(power)-l));
            if(tempArrMaxX<sizeX && tempArrMaxX>=0 && temp>=0 && temp<sizeY)
                matr[tempArrMaxX][temp]=signedMax(matr[tempArrMaxX][temp],getSign(power)*(abs(power)-l));
        }
        for(int temp = x-s+1;temp<=s+x-1;temp++)
        {
            int tempArrMinY = int(floor(y-s));
            int tempArrMaxY = int(floor(y+s));
            if(tempArrMinY>=0 && tempArrMinY<sizeY && temp>=0 && temp<sizeX)
                matr[temp][tempArrMinY]=signedMax(matr[temp][tempArrMinY],getSign(power)*(abs(power)-l));
            if(tempArrMaxY>=0 && tempArrMaxY<sizeY  && temp>=0 && temp<sizeX)
                matr[temp][tempArrMaxY]=signedMax(matr[temp][tempArrMaxY],getSign(power)*(abs(power)-l));
        }
    }
}

double putp(double oldV, double newV)
{
    if(oldV==80)
        return 80;
    else if(oldV == 0)
        return newV;
    else
        return (oldV+newV)/2;

}

void PutAvgPotential(double power, double step, double** matr, int sizeX, int sizeY, Vec2Double p)
{
    int s = 0;
    int x = int(abs(floor(p.x)));
    int y = int(abs(floor(p.y)));
    for(int l = 0;l<fabs(power);l=l+step, s++)
    {
        for(int temp = y-s;temp<=s+y;temp++)
        {
            int tempArrMinX = int(floor(x-s));
            int tempArrMaxX = int(floor(x+s));
            if(tempArrMinX>=0 && tempArrMinX<sizeX && temp>=0 && temp<sizeY && matr[tempArrMinX][temp]<80)
                matr[tempArrMinX][temp]=putp(matr[tempArrMinX][temp],getSign(power)*(abs(power)-l));
            if(tempArrMaxX<sizeX && tempArrMaxX>=0 && temp>=0 && temp<sizeY && matr[tempArrMaxX][temp]<80)
                matr[tempArrMaxX][temp]=putp(matr[tempArrMaxX][temp],getSign(power)*(abs(power)-l));
        }
        for(int temp = x-s+1;temp<=s+x-1;temp++)
        {
            int tempArrMinY = int(floor(y-s));
            int tempArrMaxY = int(floor(y+s));
            if(tempArrMinY>=0 && tempArrMinY<sizeY && temp>=0 && temp<sizeX && matr[temp][tempArrMinY]<80)
                matr[temp][tempArrMinY]=putp(matr[temp][tempArrMinY],getSign(power)*(abs(power)-l));
            if(tempArrMaxY>=0 && tempArrMaxY<sizeY  && temp>=0 && temp<sizeX && matr[temp][tempArrMaxY]<80)
                matr[temp][tempArrMaxY]=putp(matr[temp][tempArrMaxY],getSign(power)*(abs(power)-l));
        }
    }
}

bool isCorrectCoordinate(int x, int y, int sizeX, int sizeY)
{
    if(x>=0 && x<sizeX && y>=0 && y<sizeY)
        return true;
    return false;
}


void PutRayPotential(const Vec2Double unitPos, const Vec2Double aim, const Game &game, double damage, double** matr, int sizeX, int sizeY)
{
    if(distanceSqr(unitPos, aim)<5) {
        PutPotential(damage, 1, matr, sizeX, sizeY,aim);
    }
    int sqareStartX =  fmin(floor(unitPos.x), floor(aim.x));
    int sqareFinishX = fmax(ceil(unitPos.x),ceil(aim.x));
    int sqareStartY = fmin(floor(unitPos.y),floor(aim.y));
    int sqareFinishY = fmax(ceil(unitPos.y),ceil(aim.y));

    for(int i = sqareStartX; i<=sqareFinishX;i++)
    {
        for(int j = sqareStartY; j<=sqareFinishY;j++)
        {
            if(game.level.tiles[size_t(i)][size_t(j)] == Tile::EMPTY)
            {
                if(isCorrectCoordinate(i, j, sizeX, sizeY) && (
                        intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i), size_t(j)+1))
                        ||
                        intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i)+1, size_t(j)))
                        ||
                        intersect(unitPos,aim,Vec2Double(size_t(i)+1, size_t(j)), Vec2Double(size_t(i)+1, size_t(j)+1))
                        ||
                        intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)+1), Vec2Double(size_t(i)+1, size_t(j)+1))
                )){
                    PutPotential(damage, 1, matr, sizeX, sizeY,Vec2Double(i, j));
                }

            }
        }
    }
}


double getSumOfVectorOnInfluenseMap(Vec2Double fromV, Vec2Double toV, double** matr, int sizeX, int sizeY){
    double sum = 0;
    int sqareStartX =  fmin(floor(fromV.x), floor(toV.x));
    int sqareFinishX = fmax(ceil(fromV.x),ceil(toV.x));
    int sqareStartY = fmin(floor(fromV.y),floor(toV.y));
    int sqareFinishY = fmax(ceil(fromV.y),ceil(toV.y));

    for(int i = sqareStartX; i<=sqareFinishX;i++)
    {
        for(int j = sqareStartY; j<=sqareFinishY;j++) {
            if (isCorrectCoordinate(i, j, sizeX, sizeY) && (
                    intersect(fromV, toV, Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i), size_t(j) + 1))
                    ||
                    intersect(fromV, toV, Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i) + 1, size_t(j)))
                    ||
                    intersect(fromV, toV, Vec2Double(size_t(i) + 1, size_t(j)),
                              Vec2Double(size_t(i) + 1, size_t(j) + 1))
                    ||
                    intersect(fromV, toV, Vec2Double(size_t(i), size_t(j) + 1),
                              Vec2Double(size_t(i) + 1, size_t(j) + 1))
            )) {
                sum += matr[i][j];
            }
            else if(!isCorrectCoordinate(i, j, sizeX, sizeY))
            {sum +=80;}
        }
    }
    return sum;
}

Vec2Double GetMinPotentialByRadius(int radius, double** matr, int sizeX, int sizeY, Vec2Double source) {

    double min = 10000;
    Vec2Double minPos = source;
    int x = int(abs(floor(source.x)));
    int y = int(abs(floor(source.y)));

    for (int temp = y-radius+1; temp < y+radius; temp++) {
        int minX = x-radius;
        int maxX = x+radius;
        double minSum = getSumOfVectorOnInfluenseMap(source, Vec2Double (minX,temp), matr, sizeX, sizeY);
        double maxSum = getSumOfVectorOnInfluenseMap(source, Vec2Double (maxX,temp), matr, sizeX, sizeY);
        if(minSum<min)
        {
            min = minSum;
            minPos.x = minX;
            minPos.y = temp;
        }
        if(maxSum<min)
        {
            min = maxSum;
            minPos.x = maxX;
            minPos.y = temp;
        }
    }
    for (int temp = x-radius+1; temp < x+radius; temp++) {
        int minY = y-radius;
        int maxY = y+radius;
        double minSum = getSumOfVectorOnInfluenseMap(source, Vec2Double (temp,minY), matr, sizeX, sizeY);
        double maxSum = getSumOfVectorOnInfluenseMap(source, Vec2Double (temp,maxY), matr, sizeX, sizeY);
        if(minSum<min)
        {
            min = minSum;
            minPos.x = temp;
            minPos.y = minY;
        }
        if(maxSum<min)
        {
            min = maxSum;
            minPos.x = temp;
            minPos.y = maxY;
        }
    }
//    std::cerr<<'\n';
//    for (int tempY = y+radius-1; tempY >= y-radius; tempY--) {
//        for (int tempX = x-radius+1; tempX <= x+radius; tempX++) {
//
//            if(isCorrectCoordinate(tempX,tempY, sizeX, sizeY ))
//            {
//                if(tempX==minPos.x && tempY==minPos.y )
//                    std::cerr<<'!'<< matr[tempX][tempY]<< '!';
//                else if(tempX==x && tempY==y)
//                    std::cerr<<'('<< matr[tempX][tempY]<< ')';
//                else std::cerr<<' '<< matr[tempX][tempY]<< ' ';
//            }
//
//        }
//        std::cerr<<'\n';
//    }
//    std::cerr<<min<<'\n';

    return minPos;

}

double **array_generator(unsigned int dim1, unsigned int dim2) {
    double **ptrary = new double * [dim1];
    for (int i = 0; i < dim1; i++) {
        ptrary[i] = new double [dim2];
    }
    return ptrary;
}

void array_destroyer(double **ary, unsigned int dim1) {
    for (int i = 0; i < dim1; i++) {
        delete [] ary[i];
    }
    delete [] ary;
}


UnitAction MyStrategy::getAction(const Unit &unit, const Game &game,
                                 Debug &debug) {


    const Unit *nearestEnemy = nullptr;
    const LootBox *nearestWeapon = nullptr;
    bool swapWeapon = false;

    bool isRocketInMyHand = unit.weapon.get()!=nullptr && unit.weapon.get()->typ==WeaponType::ROCKET_LAUNCHER;


    for (const Unit &other : game.units) {
        if (other.playerId != unit.playerId) {
            if (nearestEnemy == nullptr ||
                distanceSqr(unit.position, other.position) <
                distanceSqr(unit.position, nearestEnemy->position)) {
                nearestEnemy = &other;
            }
        }
    }

    for (const LootBox &lootBox : game.lootBoxes) {
        if (std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)) {
            if (nearestWeapon == nullptr ||
                distanceSqr(unit.position, lootBox.position) <
                distanceSqr(unit.position, nearestWeapon->position)) {
                nearestWeapon = &lootBox;
            }
        }
    }
//    if(game.currentTick==80)
//    {
//        int y = 0;
//    }
//    std::cerr<<game.currentTick<<'\n';

    if(game.currentTick%(10*unit.id)==0)
    {
        for(auto unitItem : game.units)
        {
            if(unitItem.playerId == unit.playerId)
            {
                SavedPath[unitItem.id]=FindPath(unitItem.position, (unit.weapon==nullptr)? nearestWeapon->position: nearestEnemy->position, game, unit);

            }
        }
    }

    //SetPotentialField(game);
    int width = game.level.tiles.size();
    int height = game.level.tiles[0].size();
    // std::vector<std::vector<double>> PotentialFields = {};
    double **a = array_generator(width+1,height+1);
    for (int i=0;i<width;i++) {
        for (int j=0;j<height;j++) {
            a[i][j] = 0;
        }
    }


    for (const Mine &mine : game.mines) {
        PutPotential(60, 10, a, width, height, nearestEnemy->position);
    }

//    for (int i=0;i<width;i++) {
//        PotentialFields.push_back(std::vector<double>(height, 0));
//    }


    for (const Unit &other : game.units) {
        if (other.playerId != unit.playerId) {

            PutPotential(30, 10, a, width, height, other.position);
            PutAvgPotential(-30, 10, a, width, height, Vec2Double(other.position.x, other.position.y+4) );
            PutAvgPotential(60, 20, a, width, height, Vec2Double(other.position.x, other.position.y-4) );
        }
        else{
            PutPotential(isRocketInMyHand?50:30, 10, a, width, height, other.position);
        }
    }

    bool isMeOnFire = false;
    for(auto bullet : game.bullets)
    {
        double raduis = bullet.size;
        if (bullet.explosionParams.get() != nullptr && bullet.explosionParams.get()->radius != 0)
            raduis+=bullet.explosionParams.get()->radius;
        if(!isMeOnFire){
            Vec2Double nextPosOfBullet = bullet.position;
            nextPosOfBullet.x+=bullet.velocity.x;
            nextPosOfBullet.y+=bullet.velocity.y;
            nextPosOfBullet = TurnAndResize(bullet.position,nextPosOfBullet, 30,0);
            debug.draw(CustomData::Line(Vec2Float(bullet.position.x ,bullet.position.y),
                                        Vec2Float(nextPosOfBullet.x, nextPosOfBullet.y),
                                        0.1,ColorFloat(0,0,100,50)));
            if(isIntersectUnit(bullet.position,nextPosOfBullet, unit, raduis))
                isMeOnFire = true;
        }
        int damage = 80;
        int step = 20;
//
        PutPotential(damage, step, a, width, height, bullet.position);

        Vec2Double negatPotentialVecP1 = bullet.position;
        Vec2Double newPosForBullet2 = bullet.position;
        newPosForBullet2.x+=bullet.velocity.x/30;
        newPosForBullet2.y+=bullet.velocity.y/30;
        Vec2Double negatPotentialVecP2 = unit.position;
        double coeff =negatPotentialVecP1.x< newPosForBullet2.x;
        negatPotentialVecP2  = TurnAndResize(bullet.position, newPosForBullet2, 4+unit.size.y*(coeff? 2:1), 3.14/2);
        Vec2Double negatPotentialVecP3  = TurnAndResize(bullet.position, newPosForBullet2, 4+unit.size.y*(coeff? 1:2), -3.14/2);

        PutPotential(-damage, step, a, width, height, negatPotentialVecP2);
        PutPotential(-damage, step, a, width, height, negatPotentialVecP3);

        Vec2Double newPosForBullet = bullet.position;
        for(int sptps = 0; sptps<100;sptps++)
        {


            damage=damage*(sptps/10);
            newPosForBullet.x+=bullet.velocity.x/60;
            newPosForBullet.y+=bullet.velocity.y/60;
            PutPotential(damage, step, a, width, height, newPosForBullet);

            Vec2Double p1 = newPosForBullet;
            Vec2Double p2 = p1;
            p2.x+=p1.x/30;
            p2.y+=p1.y/30;

            Vec2Double newP2  = TurnAndResize(p1, p2, 3+unit.size.y*(coeff? 2:1), 3.14/2);
            Vec2Double newP3  = TurnAndResize(p1, p2, 3+unit.size.y*(coeff? 1:2), -3.14/2);

            PutPotential(-damage/2, step, a, width, height, newP2);
            PutPotential(-damage/2, step, a, width, height, newP3);


        }
    }
    bool isHealthPackFounded = false;
    for (const LootBox &lootBox : game.lootBoxes) {

        if (std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {

            if (unit.health < game.properties.unitMaxHealth) {
                isHealthPackFounded = true;
                PutAvgPotential(-20, 1, a, width, height, lootBox.position);
            }
        }
    }
    for(int i = 0; i<width;i++)
    {
        bool isWallDetected = true;
        for(int j = 0; j<height;j++)
        {
            if(game.level.tiles[size_t(i)][size_t(j)] == Tile::WALL)
            {
//                if(isRocketInMyHand)
//                    PutPotential(80, 60, a, width,height, Vec2Double(i,j));
//                else
                PutPotential(80, 80, a, width,height, Vec2Double(i,j));
                isWallDetected = false;
            }
            if(game.level.tiles[size_t(i)][size_t(j)] == Tile::LADDER)
            {
                PutPotential(10, 5, a, width,height, Vec2Double(i,j));
            }
            if(isWallDetected)
            {
                a[i][j] = 80;
            }
        }
    }
//    if(game.bullets.size()>0) {
//        for (int j = height; j >= 0; j--) {
//            for (int i = 0; i < width; i++) {
//                //std::cerr << a[i][j] << ' ';
//                if (a[i][j] == 0)
//                    std::cerr << "  ";
//                else if (a[i][j] == 80)
//                    std::cerr << "__";
//                else{
//
////                    else if(a[i][j]>0)
////                        std::cerr << "+";
////                    else
//                       // sss =a[i][j]<10? " "+std::to_string(int(a[i][j])): std::to_string(int(a[i][j]));
//                    std::string sss = "";
//                    if(a[i][j]<0)
//                        sss = std::to_string(int(a[i][j]));
//                    else sss =fabs(a[i][j]<10)? " "+std::to_string(int(a[i][j])): std::to_string(int(a[i][j]));
//                    std::cerr << sss;
//                }
//
//            }
//            std::cerr << '\n';
//        }
//    std::cerr <<"______________________________________"<< '\n';
//    }
    // targetPos =  GetMinPotentialByRadius(int radius, const double** matr, int sizeX, int sizeY, Vec2Double source)

    Vec2Double targetPos = unit.position;




    Vec2Double aim = Vec2Double(0, 0);
    if (nearestEnemy != nullptr) {
        aim = Vec2Double(nearestEnemy->position.x - unit.position.x,
                         nearestEnemy->position.y - unit.position.y);
    }




    bool isObstacleDetected =  isObstacleForAim(unit.position, Vec2Double(nearestEnemy->position.x, nearestEnemy->position.y), game);


    bool isMyUnitOnAimLine = false;
    if(unit.weapon!= nullptr) {
        for (const Unit &other : game.units) {
            if (other.playerId == unit.playerId && unit.id != other.id) { //if союзник
                Vec2Double pc = Vec2Double(nearestEnemy->position.x,
                                           nearestEnemy->position.y);
                Vec2Double p1 = pc;
                Vec2Double p2 = pc;
                double l = sqrt(distanceSqr(unit.position, pc));

                p1 = TurnAndResize(unit.position, pc, 30, unit.weapon.get()->spread);
                p2 = TurnAndResize(unit.position, pc, 30, -unit.weapon.get()->spread);


                if(isPointInTriangle(Vec2Double(other.position.x, other.position.y), unit.position, p1,p2)
                   ||
                   isPointInTriangle(Vec2Double(other.position.x, other.position.y+other.size.y / 2), unit.position, p1,p2)
                   ||
                   isPointInTriangle(Vec2Double(other.position.x, other.position.y+other.size.y), unit.position, p1,p2)
                        )
                    isMyUnitOnAimLine = true;
            }
        }
    }

    bool isShoot= !isMyUnitOnAimLine;
    isShoot = isShoot && (!isObstacleDetected || (!isRocketInMyHand && sqrt(distanceSqr(unit.position, nearestEnemy->position)) < 4));
//    if(!isShoot)
//    {
//        std::cerr << "dist to enemy : " << sqrt(distanceSqr(unit.position, nearestEnemy->position))<<'\n';
//    }
    if (unit.weapon == nullptr && nearestWeapon != nullptr) {
        targetPos = nearestWeapon->position;
    } else if (nearestEnemy != nullptr) {
        targetPos = nearestEnemy->position;
        if(isMeOnFire) // или если на меня летит пуля
        {
            targetPos =   GetMinPotentialByRadius(3, a, width,height, unit.position);
        }else {
            //if (sqrt(distanceSqr(unit.position, nearestEnemy->position)) > 7) {
            while (SavedPath.count(unit.id) != 0 && SavedPath[unit.id].size() > 0 && isPointInUnitWithRadius(SavedPath[unit.id][0], unit, 2) &&
                   SavedPath[unit.id].size() > 0) {
                SavedPath[unit.id].erase(SavedPath[unit.id].begin());
            }
            if (SavedPath.count(unit.id) != 0 && SavedPath[unit.id].size() > 1) {
                targetPos = SavedPath[unit.id][0];
            }

            else {
                double l = sqrt(
                        (nearestEnemy->position.x - unit.position.x) *
                        (nearestEnemy->position.x - unit.position.x) +
                        (nearestEnemy->position.y - unit.position.y) *
                        (nearestEnemy->position.y - unit.position.y));

                targetPos = nearestEnemy->position;
                double Vx = targetPos.x - unit.position.x;
                double Vy = targetPos.y - unit.position.y;
                targetPos.x = unit.position.x + Vx * ((l - 7) / l);
                targetPos.y = unit.position.y + Vy * ((l - 7) / l);
            }
        }

    }
    array_destroyer(a, width+1);

    Vec2Double nearestHealthPack = Vec2Double(0, 0);
    if(unit.weapon != nullptr || unit.health<game.properties.unitMaxHealth ){
        for (const LootBox &lootBox : game.lootBoxes) {
            if (std::dynamic_pointer_cast<Item::Weapon>(lootBox.item) && unit.weapon != nullptr) {
                if (distanceSqr(unit.position, lootBox.position) < 5 ) {
                    if(game.properties.weaponParams.at(unit.weapon.get()->typ).bullet.damage< game.properties.weaponParams.at(std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)->weaponType).bullet.damage
                       && (WeaponType::ROCKET_LAUNCHER!=std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)->weaponType)){
                        targetPos = lootBox.position;
                        swapWeapon = true;
                    }
                }

            }
            if (unit.health<game.properties.unitMaxHealth && !isMeOnFire && std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {
                if ((nearestHealthPack.x==0 && nearestHealthPack.y==0) || distanceSqr(unit.position, lootBox.position)< distanceSqr(unit.position, nearestHealthPack)) {
                    nearestHealthPack = lootBox.position;
                    targetPos = lootBox.position;
                }
            }
        }
    }
////    game.properties.weaponParams.first;


    bool jump = targetPos.y > unit.position.y;
    if (targetPos.x > unit.position.x && (
            game.level.tiles[size_t(unit.position.x + 1)][size_t(unit.position.y)] ==
            Tile::WALL || (abs(nearestEnemy->position.x - unit.position.x + 1)<3 &&  abs(nearestEnemy->position.y - unit.position.y)<3)
    )) {
        jump = true;
    }
    if (targetPos.x < unit.position.x && ((game.level.tiles[size_t(unit.position.x - 1)][size_t(unit.position.y)] ==
                                           Tile::WALL) || (fabs(nearestEnemy->position.x - unit.position.x - 1)<3 &&  fabs(nearestEnemy->position.y - unit.position.y)<3))) {
        jump = true;
    }


    UnitAction action;


    Vec2Double prev = unit.position;
    for(auto unitItem : game.units) {
        if(unitItem.playerId == unit.playerId)
        for (auto item :SavedPath[unitItem.id]) {
            debug.draw(CustomData::Line(Vec2Float(prev.x, prev.y),
                                        Vec2Float(item.x, item.y),
                                        0.1, ColorFloat(50, 50, 50, 50)));
            prev = item;
        }
    }

    //if(unit.weapon.get() == nullptr || isObstacleDetected || unit.health<game.properties.unitMaxHealth)
    action.velocity = (targetPos.x - unit.position.x)*game.properties.unitMaxHorizontalSpeed;
    // else
    // action.velocity = 0;
    bool isReloadMyGun = false;
    isReloadMyGun = (unit.weapon != nullptr && unit.weapon.get()->fireTimer!= nullptr && isObstacleDetected);
    if(unit.weapon != nullptr && unit.weapon.get()->lastAngle.get()!=nullptr && nearestEnemy!=nullptr)
    {
//        atan2(action.aim)
        double angle = *unit.weapon.get()->lastAngle.get() ;
        Vec2Double aimOld = Vec2Double(unit.position.x+1 ,unit.position.y);
        double leng = sqrt(distanceSqr(unit.position, aim));
        aimOld = TurnAndResize(unit.position, aimOld, leng+2 ,angle);

        debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
                                    Vec2Float(aimOld.x, aimOld.y),
                                    0.1,ColorFloat(100,0,0,50)));
        debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
                                    Vec2Float(aim.x+unit.position.x, aim.y+unit.position.y),
                                    0.1,ColorFloat(0,100,0,50)));
        if(isIntersectUnit(unit.position, aimOld, *nearestEnemy, 0)){
            aim.x = aimOld.x - unit.position.x;
            aim.y = aimOld.y - unit.position.y;
        }

    }


    action.jump = jump;
    action.jumpDown = !action.jump;

    action.aim = aim;
    action.reload = false;//isReloadMyGun;


    action.shoot = isShoot;
    action.swapWeapon = swapWeapon;
    action.plantMine = false;




//    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
//                                Vec2Float(aim.x+unit.position.x, aim.y+unit.position.y),
//                                0.1,ColorFloat(50,50,50,50)));
//
//
//
//
//    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
//                                Vec2Float(targetPos.x, targetPos.y + (game.properties.unitSize.y/2)),
//                                0.1,ColorFloat(100,0,0,50)));
    std::string tsxt = isMyUnitOnAimLine? "true":"false";
    std::string isMeOnFireS = isMeOnFire? "true":"false";
    debug.draw(CustomData::Log(
            std::string("isMyUnitOnAimLine: ")+tsxt+" isMeOnFire:" + isMeOnFireS));
    //array_destroyer(a, width+1);
//    if(game.players.size()<=1) {
//        for (auto const &player: game.players) {
//            std::cerr << player.score<< '\n';
////        for(auto const & unit : game.units)
////        {
////            if()
////            std::cerr<<player.score;
////        }
//        }
//    }
    return action;

}