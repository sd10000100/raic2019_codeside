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
            if(game.level.tiles[size_t(i)][size_t(j)] != Tile::EMPTY)
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
                                                            //  width,     height
//void PutPotential(double power, double step, double** matr, int sizeX, int sizeY, Vec2Double p)
//{
//    int s = 0;
//    int x = int(abs(floor(p.x)));
//    int y = int(abs(floor(p.y)));
//    for(int l = 0;l<fabs(power);l=l+step, s++)
//    {
//        for(int temp = y-s;temp<=s+y;temp++)
//        {
//            int tempArrMinX = int(floor(x-s));
//            int tempArrMaxX = int(floor(x+s));
//            if(tempArrMinX>=0 && tempArrMinX<sizeX && temp>=0 && temp<sizeY)
//                matr[tempArrMinX][temp]=signedMax(matr[tempArrMinX][temp],getSign(power)*(abs(power)-l));
//            if(tempArrMaxX<sizeX && tempArrMaxX>=0 && temp>=0 && temp<sizeY)
//                matr[tempArrMaxX][temp]=signedMax(matr[tempArrMaxX][temp],getSign(power)*(abs(power)-l));
//        }
//        for(int temp = x-s+1;temp<=s+x-1;temp++)
//        {
//            int tempArrMinY = int(floor(y-s));
//            int tempArrMaxY = int(floor(y+s));
//            if(tempArrMinY>=0 && tempArrMinY<sizeY && temp>=0 && temp<sizeX)
//                matr[temp][tempArrMinY]=signedMax(matr[temp][tempArrMinY],getSign(power)*(abs(power)-l));
//            if(tempArrMaxY>=0 && tempArrMinY<sizeY  && temp>=0 && temp<sizeX)
//                matr[temp][tempArrMaxY]=signedMax(matr[temp][tempArrMaxY],getSign(power)*(abs(power)-l));
//        }
//    }
//}
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

Vec2Double GetMinPotentialByRadius(int radius, double** matr, int sizeX, int sizeY, Vec2Double source) {

    double min = 10000;
    Vec2Double maxPos = source;
    int x = int(abs(floor(source.x)));
    int y = int(abs(floor(source.y)));
    for (int l = 0; l < radius; l++) {

        for (int temp = y - l; temp <= l + y; temp++) {
            if (x - l > 0 && x - l<sizeX && temp > 0 && temp < sizeY && matr[x - l][temp] < min) {
                min = matr[x - l][temp];
                maxPos.x = x - l;
                maxPos.y = temp;

            }
            if (x + l < sizeX && x + l>0 && temp > 0 && temp < sizeY && matr[x + l][temp] < min) {
                min = matr[x + l][temp];
                maxPos.x = x + l;
                maxPos.y = temp;
            }

        }
        for (int temp = x - l + 1; temp <= l + x - 1; temp++) {
            if (y - l > 0 && y - l < sizeY && temp > 0 && temp < sizeX && matr[temp][y - l] < min) {
                min = matr[temp][y - l];
                maxPos.x = temp;
                maxPos.y = y - l;
            }

            if (y + l < sizeY && y + l>0 && temp > 0 && temp < sizeX && matr[temp][y + l] < min) {
                min = matr[temp][y + l];
                maxPos.x = temp;
                maxPos.y = y + l;
            }

        }
    }
    return maxPos;

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
//    for (int i=0;i<width;i++) {
//        PotentialFields.push_back(std::vector<double>(height, 0));
//    }
    //std::cerr<<game.currentTick<<'\n';
    PutPotential(13, 1, a, width, height, nearestEnemy->position);
    for(auto bullet : game.bullets)
    {
        int step = 1;
        if(bullet.explosionParams.get()!=nullptr && bullet.explosionParams.get()->radius!=0)
            step = bullet.damage/bullet.explosionParams.get()->radius;
        PutPotential(bullet.damage, step, a, width,height, bullet.position);
    }
    bool isHealthPackFounded = false;
    for (const LootBox &lootBox : game.lootBoxes) {

        if (std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {
            isHealthPackFounded = true;
           // if (unit.health < game.properties.unitMaxHealth) {

                PutPotential(-7, 1, a, width, height, lootBox.position);
           // }
        }
    }
    for(int i = 0; i<width;i++)
    {
        bool isWallDetected = true;
        for(int j = 0; j<height;j++)
        {
            if(game.level.tiles[size_t(i)][size_t(j)] == Tile::WALL)
            {
                PutPotential(80, 80, a, width,height, Vec2Double(i,j));
                isWallDetected = false;
            }
            if(isWallDetected)
            {
                a[i][j] = 80;
            }
        }
    }
//    if(game.currentTick==147)
//    {
//        int stop = 1;
//    }
//    if(isHealthPackFounded) {
//        for (int j = height; j >= 0; j--) {
//            for (int i = 0; i < width; i++) {
//                std::cerr << a[i][j] << ' ';
////                if (a[i][j] == 0)
////                    std::cerr << "   ";
////                else{
////                    std::string sss =a[i][j]<10? " "+std::to_string(int(a[i][j])): std::to_string(int(a[i][j]));
////                    std::cerr << sss << ' ';
////                }
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

    if (unit.weapon == nullptr && nearestWeapon != nullptr) {
        targetPos = nearestWeapon->position;
    } else if (nearestEnemy != nullptr) {
        targetPos = nearestEnemy->position;
        if(distanceSqr(unit.position,nearestEnemy->position)<50)
            targetPos =  GetMinPotentialByRadius(10,  a, width,height, unit.position);


    }

    array_destroyer(a, width+1);

////    game.properties.weaponParams.first;
    if(unit.weapon != nullptr || unit.health<game.properties.unitMaxHealth){
        for (const LootBox &lootBox : game.lootBoxes) {
            if (std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)) {
                if (distanceSqr(unit.position, lootBox.position) < 7) {
                    if(game.properties.weaponParams.at(unit.weapon.get()->typ).bullet.damage< game.properties.weaponParams.at(std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)->weaponType).bullet.damage){
                        targetPos = lootBox.position;
                        swapWeapon = true;
                    }
                }

            }
            if (unit.health<game.properties.unitMaxHealth &&  std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {
                if (distanceSqr(unit.position, lootBox.position) < 5) {
                    targetPos = lootBox.position;
                }
            }
        }
    }

    bool jump = targetPos.y > unit.position.y;
    if (targetPos.x > unit.position.x &&
        game.level.tiles[size_t(unit.position.x + 1)][size_t(unit.position.y)] ==
        Tile::WALL) {
        jump = true;
    }
    if (targetPos.x < unit.position.x &&
        game.level.tiles[size_t(unit.position.x - 1)][size_t(unit.position.y)] ==
        Tile::WALL) {
        jump = true;
    }


    UnitAction action;
    bool isObstacleDetected =  isObstacleForAim(unit.position, Vec2Double(nearestEnemy->position.x, nearestEnemy->position.y), game);
    bool isShoot= !isObstacleDetected || (distanceSqr(unit.position, nearestEnemy->position) < 5);


    if(unit.weapon.get() == nullptr || isObstacleDetected || unit.health<game.properties.unitMaxHealth)
        action.velocity = (targetPos.x - unit.position.x)*game.properties.unitMaxHorizontalSpeed;
    else
        action.velocity = 0;



    action.jump = jump;
    action.jumpDown = !action.jump;
    action.aim = aim;
    action.reload = false;
    action.shoot = isShoot;
    action.swapWeapon = swapWeapon;
    action.plantMine = false;

    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
                                Vec2Float(nearestEnemy->position.x, nearestEnemy->position.y + (game.properties.unitSize.y/2)),
                                0.1,ColorFloat(50,50,50,50)));

    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
                                Vec2Float(targetPos.x, targetPos.y + (game.properties.unitSize.y/2)),
                                0.1,ColorFloat(100,0,0,50)));
    std::string tsxt = action.shoot? "true":"false";
    debug.draw(CustomData::Log(
            std::string("isShoot: ")+tsxt));
    //array_destroyer(a, width+1);
    return action;

}