#include "MyStrategy.hpp"
#include <cmath>
#include <iostream>

MyStrategy::MyStrategy() {
    PotentialField = {};
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

    if(game.currentTick==141)
    {
        int stop = 1;
    }
    const Unit *nearestEnemy = nullptr;
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
    const LootBox *nearestWeapon = nullptr;
    for (const LootBox &lootBox : game.lootBoxes) {
        if (std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)) {
            if (nearestWeapon == nullptr ||
                distanceSqr(unit.position, lootBox.position) <
                distanceSqr(unit.position, nearestWeapon->position)) {
                nearestWeapon = &lootBox;
            }
        }
    }

    const Unit *nearestHealthPack = nullptr;

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
    //std::cerr<<game.currentTick<<'\n';

    PutPotential(60, 10, a, width, height, nearestEnemy->position);
    PutPotential(isRocketInMyHand?5:3, 1, a, width, height, unit.position);
    for(auto bullet : game.bullets)
    {
        if(bullet.unitId!=unit.id || isRocketInMyHand) {
            int damage = 80;
            int step = damage / 3;
//            if (bullet.explosionParams.get() != nullptr && bullet.explosionParams.get()->radius != 0)
//                step = damage / ((bullet.explosionParams.get()->radius) * 2);
            PutPotential(damage, step, a, width, height, bullet.position);

            Vec2Double negatPotentialVecP1 = bullet.position;
            Vec2Double newPosForBullet2 = bullet.position;
            newPosForBullet2.x+=bullet.velocity.x/30;
            newPosForBullet2.y+=bullet.velocity.y/30;
            Vec2Double negatPotentialVecP2 = unit.position;
            double coeff =negatPotentialVecP1.x< newPosForBullet2.x;
            negatPotentialVecP2  = TurnAndResize(bullet.position, newPosForBullet2, 3+unit.size.y*(coeff? 2:1), 3.14/2);
            Vec2Double negatPotentialVecP3  = TurnAndResize(bullet.position, newPosForBullet2, 3+unit.size.y*(coeff? 1:2), -3.14/2);

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
    }
    bool isHealthPackFounded = false;
    for (const LootBox &lootBox : game.lootBoxes) {

        if (std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {

            if (unit.health < game.properties.unitMaxHealth) {
                isHealthPackFounded = true;
                PutPotential(-20, 2, a, width, height, lootBox.position);
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
                PutPotential(10, 3, a, width,height, Vec2Double(i,j));
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
//                else{
//                    std::string sss = "";
//                    if(a[i][j]<0)
//                        sss = std::to_string(int(a[i][j]));
//                    else sss =a[i][j]<10? " "+std::to_string(int(a[i][j])): std::to_string(int(a[i][j]));
//                    std::cerr << sss;
//                }
//
//            }
//            std::cerr << '\n';
//        }
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

    for (const Unit &other : game.units) {
        if (other.playerId == unit.playerId && unit.id!=other.id) { //if союзник
            if (intersect(unit.position, aim, Vec2Double(other.position.x, other.position.y-other.size.y/2), Vec2Double(other.position.x, other.position.y-other.size.y*1.5)))
                    isMyUnitOnAimLine = true;
            }
    }


    bool isShoot= !isMyUnitOnAimLine;
    isShoot = isShoot && !isObstacleDetected || (!isRocketInMyHand) || sqrt(distanceSqr(unit.position, nearestEnemy->position)) < 4;
//    if(!isShoot)
//    {
//        std::cerr << "dist to enemy : " << sqrt(distanceSqr(unit.position, nearestEnemy->position))<<'\n';
//    }
    if (unit.weapon == nullptr && nearestWeapon != nullptr) {
        targetPos = nearestWeapon->position;
    } else if (nearestEnemy != nullptr) {
        targetPos = nearestEnemy->position;
        if(isObstacleDetected) {
            if (sqrt(distanceSqr(unit.position, nearestEnemy->position)) > 10) {
                double l = sqrt(
                        (nearestEnemy->position.x - unit.position.x) * (nearestEnemy->position.x - unit.position.x) +
                        (nearestEnemy->position.y - unit.position.y) * (nearestEnemy->position.y - unit.position.y));

                targetPos = nearestEnemy->position;
                double Vx = targetPos.x - unit.position.x;
                double Vy = targetPos.y - unit.position.y;
                targetPos.x = unit.position.x + Vx * ((l - 10) / l);
                targetPos.y = unit.position.y + Vy * ((l - 10) / l);
            } else {
                targetPos = nearestEnemy->position;
                double angle = 3.14;
                double Vx = targetPos.x - unit.position.x;
                double Vy = targetPos.y - unit.position.y;
                double x = Vx * cos(angle) - Vy * sin(angle);
                double y = Vy * cos(angle) + Vx * sin(angle);
                targetPos.x = unit.position.x + x;
                targetPos.y = unit.position.y + y;

            }
        }
        if(!isObstacleDetected)
            targetPos =   GetMinPotentialByRadius(3, a, width,height, unit.position);
    }
    array_destroyer(a, width+1);

    Vec2Double nearestHealthPack = Vec2Double(0, 0);
    if(unit.weapon != nullptr || unit.health<game.properties.unitMaxHealth ){
        for (const LootBox &lootBox : game.lootBoxes) {
            if (std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)) {
                if (distanceSqr(unit.position, lootBox.position) < 5) {
                    if(game.properties.weaponParams.at(unit.weapon.get()->typ).bullet.damage< game.properties.weaponParams.at(std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)->weaponType).bullet.damage
                    && (WeaponType::ROCKET_LAUNCHER!=std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)->weaponType)){
                        targetPos = lootBox.position;
                        swapWeapon = true;
                    }
                }

            }
            if (unit.health<game.properties.unitMaxHealth &&  std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {
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



    //if(unit.weapon.get() == nullptr || isObstacleDetected || unit.health<game.properties.unitMaxHealth)
        action.velocity = (targetPos.x - unit.position.x)*game.properties.unitMaxHorizontalSpeed;
   // else
    // action.velocity = 0;



    action.jump = jump;
    action.jumpDown = !action.jump;
    action.aim = aim;
    action.reload = false;
    action.shoot = isShoot;
    action.swapWeapon = swapWeapon;
    action.reload = false;
    action.plantMine = false;

    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
                                Vec2Float(nearestEnemy->position.x, nearestEnemy->position.y + (game.properties.unitSize.y/2)),
                                0.1,ColorFloat(50,50,50,50)));

    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
                                Vec2Float(targetPos.x, targetPos.y + (game.properties.unitSize.y/2)),
                                0.1,ColorFloat(100,0,0,50)));
    std::string tsxt = isShoot? "true":"false";
    debug.draw(CustomData::Log(
            std::string("isShoot: ")+tsxt));
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