//
// Created by badru on 15.01.2020.
//

#include "../model/Vec2Double.hpp"
// TODO: поечму без inline повторяется где то?


// TODO: переделать на вектор с поворотами и прочим
// структура "Ребра"
struct Edge{
    Edge(Vec2Double p1, Vec2Double p2):start(p1), finish(p2){}
    Vec2Double start;
    Vec2Double finish;
};

// расстояние между 2-мя точками
inline double distanceSqr(Vec2Double a, Vec2Double b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

// ориентированная площадь треугольника
// Исп-я понятие косого (псевдополяр) произвед векторов
// a ^ b = |a||b|sin(<(a,b))=2S
// угол вращения между векторами против часовой стрелки
// 2s=|x1 y1 1|
//    |x2 y2 1|=(x2-x1)(y3-y1)-(y2-y1)(x3-x1)
//    |x3 y3 1|
inline double OrientedArea (Vec2Double a, Vec2Double b, Vec2Double c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// проверка, что точки не лежат на одной плоскости
inline bool isPointNotOnSamePlane (double a, double b, double c, double d) {
    if (a > b) std::swap(a, b);
    if (c > d) std::swap(c, d);
    return std::max(a, c) <= std::min(b, d);
}

// Чтобы отрезки AB и CD пересекались, нид чтобы A и B находились
// по разные стороны от прямой CD и аналогично C и D по разные стороны от AB
// Нужно вычислить ориентированные площади треугольников и сравнить знаки
inline bool intersect (Vec2Double a, Vec2Double b, Vec2Double c, Vec2Double d) {
    return isPointNotOnSamePlane (a.x, b.x, c.x, d.x)
           && isPointNotOnSamePlane (a.y, b.y, c.y, d.y)
           && OrientedArea(a,b,c) * OrientedArea(a,b,d) <= 0
           && OrientedArea(c,d,a) * OrientedArea(c,d,b) <= 0;
}

// проверка на нахождение точки внутри треугольника (подходит для выпуклых многоугольников)
// Проверяем каждую грань на пересечение с лучом из искомой точки (луч в рандомное место)
// Если кол-во перемещений кратно 2-м - точка не в многоугольнике
// инеаче - внутри
// Примеч: можно случайно попасть на стык ребер - решается выбором луча поизощреннее
inline bool isPointInTriangle(Vec2Double p, Vec2Double p1, Vec2Double p2,Vec2Double p3){
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

// вариативный шаблон для построения вектора из точек из списка аргументов
template <class Arg>
std::vector<Arg> GetArgsToArray(Arg first)
{
    return {first};
}

template <class Arg, class... Other>
std::vector<Arg> GetArgsToArray(Arg first, Other... other)
{
    std::vector<Arg> res = {first};
    std::vector<Arg> sec = GetArgsToArray(other...);
    res.insert(res.end(), sec.begin(), sec.end());
    return res;
}

template <class... Args>
std::vector<Vec2Double> GetToArray(Args... args)
{
    std::vector<Vec2Double> stackEnges = {};
    stackEnges = GetArgsToArray(args...);
    return stackEnges;
}

// проверка на нахождение точки внутри выпуклого многоугольника
// Проверяем каждую грань на пересечение с лучом из искомой точки (луч в рандомное место)
// Если кол-во перемещений кратно 2-м - точка не в многоугольнике
// инеаче - внутри
// Примеч: можно случайно попасть на стык ребер - решается выбором луча поизощреннее
template <class... Args>
inline bool isPointInPolygon(Vec2Double p, Args ...args){
    std::vector<Vec2Double> points = {};
    std::vector<Edge> stackEnges = {};
    int countIntersect = 0;

    points = GetToArray(args...);

    if(points.size()==0) return false;

    Vec2Double first = points[0];
    for(int i=1; i<points.size();i++){
            stackEnges.push_back(Edge(first,points[i]));
            first = points[i];
    }

    stackEnges.push_back(Edge(first,stackEnges[0].start));
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