//
// Created by badru on 15.01.2020.
//


#ifndef AICUP2019_Geometry_HPP
#define AICUP2019_Geometry_HPP
#include "../model/Vec2Double.hpp"

struct Edge{
    Edge(Vec2Double p1, Vec2Double p2):start(p1), finish(p2){}
    Vec2Double start;
    Vec2Double finish;
};

double distanceSqr(Vec2Double a, Vec2Double b);

double area (Vec2Double a, Vec2Double b, Vec2Double c);

bool intersect_1 (double a, double b, double c, double d);

bool intersect (Vec2Double a, Vec2Double b, Vec2Double c, Vec2Double d);

bool isPointInTriangle(Vec2Double p, Vec2Double p1, Vec2Double p2,Vec2Double p3);

#endif