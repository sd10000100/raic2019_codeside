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
//    for (int tempY = y+radius; tempY > y-radius-1; tempY--) {
//        for (int tempX = x-radius+1; tempX < x+radius; tempX++) {
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
//
//
//Vec2Double GetMinPotentialByRadiusDirection(int radius, double** matr, int sizeX, int sizeY, Vec2Double source) {
//    Vec2Double maxPos = source;
//    double minPotential = 10000;
//    int x = int(abs(floor(source.x)));
//    int y = int(abs(floor(source.y)));
//
//    for(int i = -radius+1; i<radius;i++)
//    {
//
//        for(int j = 0; j<radius*2-1;j++)
//        {
//            if(game.level.tiles[size_t(i)][size_t(j)] != Tile::EMPTY)
//            {
//                if(intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i), size_t(j)+1))
//                   ||
//                   intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)), Vec2Double(size_t(i)+1, size_t(j)))
//                   ||
//                   intersect(unitPos,aim,Vec2Double(size_t(i)+1, size_t(j)), Vec2Double(size_t(i)+1, size_t(j)+1))
//                   ||
//                   intersect(unitPos,aim,Vec2Double(size_t(i), size_t(j)+1), Vec2Double(size_t(i)+1, size_t(j)+1))
//                        ){
//                    PutPotential(damage, 1, matr, sizeX, sizeY,Vec2Double(size_t(i)+0.5, size_t(j)+0.5));
//                }
//
//            }
//        }
//    }
//
//}

Vec2Double GetMinPotentialByRadiusDirection2(int radius, double** matr, int sizeX, int sizeY, Vec2Double source) {

    //double min = 10000;
    Vec2Double maxPos = source;
    int x = int(abs(floor(source.x)));
    int y = int(abs(floor(source.y)));

    double ToLeft = 0;
    int stepsToLeft = 0;

    double ToRight = 0;
    int stepsToRight = 0;

    double ToUp = 0;
    int stepsToUp = 0;

    double ToDown = 0;
    int stepsToDown = 0;

//------------------------------

    double ToLeftUp = 0;
    int stepsToLeftUp = 0;

    double ToRightUp = 0;
    int stepsToRightUp = 0;

    double ToLeftDown = 0;
    int stepsToLeftDown = 0;

    double ToRightDown = 0;
    int stepsToRightDown = 0;

    Vec2Double PosToLeft = source;
    Vec2Double PosToRight = source;
    Vec2Double PosToUp = source;
    Vec2Double PosToDown = source;

    Vec2Double PosToLeftUp = source;
    Vec2Double PosToRightUp = source;
    Vec2Double PosToLeftDown = source;
    Vec2Double PosToRightDown = source;

    for (int l = 1; l < radius; l++) {
        if(isCorrectCoordinate(x,y-l, sizeX,sizeY))//  && matr[x][y-l]!=80
        {
            ToLeft+=matr[x][y-l];
            stepsToLeft ++;
            PosToLeft.x = x;
            PosToLeft.y = y-l;
        }
        if(isCorrectCoordinate(x,y+l, sizeX,sizeY))//  && matr[x][y+l]!=80
        {
            ToRight+=matr[x][y+l];
            stepsToRight ++;
            PosToRight.x = x;
            PosToRight.y = y+l;
        }

        if(isCorrectCoordinate(x+l,y, sizeX,sizeY))//  && matr[x+l][y]!=80
        {
            ToUp+=matr[x+l][y];
            stepsToUp ++;
            PosToUp.x = x+l;
            PosToUp.y = y;
        }

        if(isCorrectCoordinate(x-l,y, sizeX,sizeY))//  && matr[x+l][y]!=80
        {
            ToDown+=matr[x-l][y];
            stepsToDown ++;
            PosToDown.x = x-l;
            PosToDown.y = y;
        }
// ------------------------------
        if(isCorrectCoordinate(x-l,y+l, sizeX,sizeY))//  && matr[x+l][y]!=80
        {
            ToLeftUp+=matr[x-l][y+l];
            stepsToLeftUp ++;
            PosToLeftUp.x = x-l;
            PosToLeftUp.y = y+l;
        }

        if(isCorrectCoordinate(x+l,y+l, sizeX,sizeY))//  && matr[x+l][y]!=80
        {
            ToRightUp+=matr[x+l][y+l];
            stepsToRightUp ++;
            PosToRightUp.x = x+l;
            PosToRightUp.y = y+l;
        }

        if(isCorrectCoordinate(x-l,y-l, sizeX,sizeY))//  && matr[x+l][y]!=80
        {
            ToLeftDown+=matr[x-l][y-l];
            stepsToLeftDown ++;
            PosToLeftDown.x = x-l;
            PosToLeftDown.y = y-l;
        }

        if(isCorrectCoordinate(x+l,y-l, sizeX,sizeY))//  && matr[x+l][y]!=80
        {
            ToRightDown+=matr[x+l][y-l];
            stepsToRightDown ++;
            PosToRightDown.x = x+l;
            PosToRightDown.y = y-l;
        }
    }

    int min = fmin(fmin(fmin(ToLeft,ToRight), fmin(ToUp,ToDown)), fmin(fmin(ToLeftUp,ToRightUp), fmin(ToLeftDown,ToRightDown)));

    if(min==ToLeft)
        return PosToLeft;
    else if(min==ToRight)
        return PosToRight;
    else if(min==ToUp)
        return PosToUp;
    else if(min==ToDown)
        return PosToDown;

    else if(min==ToLeftUp)
        return PosToLeftUp;
    else if(min==ToRightUp)
        return PosToRightUp;
    else if(min==ToLeftDown)
        return PosToLeftDown;
    else if(min==ToRightDown)
        return PosToRightDown;

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
    PutPotential(80, 20, a, width, height, nearestEnemy->position);
    PutPotential(isRocketInMyHand?5:3, 1, a, width, height, unit.position);
    for(auto bullet : game.bullets)
    {
        int step = bullet.damage/2;
        if(bullet.explosionParams.get()!=nullptr && bullet.explosionParams.get()->radius!=0)
            step = bullet.damage/((bullet.explosionParams.get()->radius)*2);
        PutPotential(bullet.damage, step, a, width,height, bullet.position);
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
                if(isRocketInMyHand)
                    PutPotential(80, 40, a, width,height, Vec2Double(i,j));
                else
                    PutPotential(80, 80, a, width,height, Vec2Double(i,j));
                isWallDetected = false;
            }
            if(isWallDetected)
            {
                a[i][j] = 80;
            }
        }
    }
    if(game.bullets.size()>0) {
        for (int j = height; j >= 0; j--) {
            for (int i = 0; i < width; i++) {
                //std::cerr << a[i][j] << ' ';
                if (a[i][j] == 0)
                    std::cerr << "  ";
                else{
                    std::string sss = "";
                    if(a[i][j]<0)
                        sss = std::to_string(int(a[i][j]));
                    else sss =a[i][j]<10? " "+std::to_string(int(a[i][j])): std::to_string(int(a[i][j]));
                    std::cerr << sss;
                }

            }
            std::cerr << '\n';
        }
    }
   // targetPos =  GetMinPotentialByRadius(int radius, const double** matr, int sizeX, int sizeY, Vec2Double source)

    Vec2Double targetPos = unit.position;




    Vec2Double aim = Vec2Double(0, 0);
    if (nearestEnemy != nullptr) {
        aim = Vec2Double(nearestEnemy->position.x - unit.position.x,
                         nearestEnemy->position.y - unit.position.y);
    }




    bool isObstacleDetected =  isObstacleForAim(unit.position, Vec2Double(nearestEnemy->position.x, nearestEnemy->position.y), game);
    if (unit.weapon.get() != nullptr && !isObstacleDetected)
    {
      // PutRayPotential(unit.position, aim, game, unit.weapon.get()->params.bullet.damage, a, width,height);
    }



    bool isShoot= !isObstacleDetected || (distanceSqr(unit.position, nearestEnemy->position) < 9);
    if (unit.weapon == nullptr && nearestWeapon != nullptr) {
        targetPos = nearestWeapon->position;
    } else if (nearestEnemy != nullptr) {
        targetPos = nearestEnemy->position;
        if(!isObstacleDetected)
        //{
            targetPos =   GetMinPotentialByRadius(5, a, width,height, unit.position);
        //   targetPos =  GetMinPotentialByRadiusDirection(7,  a, width,height, unit.position);
        //}
            //targetPos =  GetMinPotentialByRadius(5,  a, width,height, unit.position);
    }
    array_destroyer(a, width+1);

    Vec2Double nearestHealthPack = Vec2Double(0, 0);
    if(unit.weapon != nullptr || unit.health<game.properties.unitMaxHealth){
        for (const LootBox &lootBox : game.lootBoxes) {
            if (std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)) {
                if (distanceSqr(unit.position, lootBox.position) < 5) {
                    if(game.properties.weaponParams.at(unit.weapon.get()->typ).bullet.damage< game.properties.weaponParams.at(std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)->weaponType).bullet.damage){
                        targetPos = lootBox.position;
                        swapWeapon = true;
                    }
                }

            }
            if (unit.health<game.properties.unitMaxHealth &&  std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {
                if (distanceSqr(unit.position, lootBox.position) < 40) {
                //if ((nearestHealthPack.x==0 && nearestHealthPack.y==0) || distanceSqr(unit.position, lootBox.position)< distanceSqr(unit.position, nearestHealthPack)) {
                    nearestHealthPack = lootBox.position;
                    targetPos = lootBox.position;
                }
            }
        }
    }
////    game.properties.weaponParams.first;


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



    //if(unit.weapon.get() == nullptr || isObstacleDetected || unit.health<game.properties.unitMaxHealth)
        action.velocity = (targetPos.x - unit.position.x)*game.properties.unitMaxHorizontalSpeed;
   // else
    //    action.velocity = 0;



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