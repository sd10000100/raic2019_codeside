#include "MyStrategy.hpp"
#include <iostream>
#include <cmath>
#include <cstring>

MyStrategy::MyStrategy() {
    //PotentialField = {};
}

//void MyStrategy::SetPotentialField(const Game &game){
//    int width = game.level.tiles.size();
//    int height = game.level.tiles[0].size();
//
//    for (int i=0;i<width;i++) {
//        PotentialField.push_back(std::vector<double>(height, 0));
//
//    }
//};

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

UnitAction MyStrategy::getAction(const Unit &unit, const Game &game,
                                 Debug &debug) {

    AStar aStarPathfinder = AStar();
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
    if(game.currentTick%(10*unit.id)==0)
    {
        for(auto unitItem : game.units)
        {
            if(unitItem.playerId == unit.playerId)
            {
                SavedPath[unitItem.id]=aStarPathfinder.FindPath(unitItem.position, (unit.weapon==nullptr)? nearestWeapon->position: nearestEnemy->position, game, unit);
            }
        }
    }

    int width = game.level.tiles.size();
    int height = game.level.tiles[0].size();
    double **a = array_generator(width+1,height+1);
    for (int i=0;i<width;i++) {
        for (int j=0;j<height;j++) {
            a[i][j] = 0;
        }
    }


    for (const Mine &mine : game.mines) {
        PutPotential(60, 10, a, width, height, nearestEnemy->position);
    }

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

    if (unit.weapon == nullptr && nearestWeapon != nullptr) {
        targetPos = nearestWeapon->position;
    } else if (nearestEnemy != nullptr) {
        //targetPos = nearestEnemy->position;
        if(isMeOnFire) // или если на меня летит пуля
        {
            targetPos =   GetMinPotentialByRadius(3, a, width,height, unit.position);
        }else {
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

    action.velocity = (targetPos.x - unit.position.x)*game.properties.unitMaxHorizontalSpeed;

    // bool isReloadMyGun = false;
    // isReloadMyGun = (unit.weapon != nullptr && unit.weapon.get()->fireTimer!= nullptr && isObstacleDetected);
    if(unit.weapon != nullptr && unit.weapon.get()->lastAngle.get()!=nullptr && nearestEnemy!=nullptr)
    {
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

//    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
//                                Vec2Float(targetPos.x, targetPos.y + (game.properties.unitSize.y/2)),
//                                0.1,ColorFloat(100,0,0,50)));
    std::string tsxt = isMyUnitOnAimLine? "true":"false";
    std::string isMeOnFireS = isMeOnFire? "true":"false";
    debug.draw(CustomData::Log(
            std::string("isMyUnitOnAimLine: ")+tsxt+" isMeOnFire:" + isMeOnFireS));

    return action;

}