//
// Created by badru on 15.01.2020.
//

//#include "Geometry.h"

double distanceSqr(Vec2Double a, Vec2Double b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

double area (Vec2Double a, Vec2Double b, Vec2Double c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool intersect_1 (double a, double b, double c, double d) {
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