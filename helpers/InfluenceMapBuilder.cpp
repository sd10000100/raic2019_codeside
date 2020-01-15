#include "InfluenceMapBuilder.h"

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
    return minPos;

}