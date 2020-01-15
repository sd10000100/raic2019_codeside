//
// Created by badru on 15.01.2020.
//

#ifndef AICUP2019_INFLUENCEMAPBUILDER_H
#define AICUP2019_INFLUENCEMAPBUILDER_H

#include <cmath>
#include <vector>
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

double **array_generator(unsigned int dim1, unsigned int dim2);

void array_destroyer(double **ary, unsigned int dim1);

int signedMax(int a, int b);

int getSign(int x);

void PutPotential(double power, double step, double** matr, int sizeX, int sizeY, Vec2Double p);

double putp(double oldV, double newV);

void PutAvgPotential(double power, double step, double** matr, int sizeX, int sizeY, Vec2Double p);

bool isCorrectCoordinate(int x, int y, int sizeX, int sizeY);

double getSumOfVectorOnInfluenseMap(Vec2Double fromV, Vec2Double toV, double** matr, int sizeX, int sizeY);

Vec2Double GetMinPotentialByRadius(int radius, double** matr, int sizeX, int sizeY, Vec2Double source);

#endif //AICUP2019_INFLUENCEMAPBUILDER_H
