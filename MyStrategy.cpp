#include "MyStrategy.hpp"
#include <cmath>

MyStrategy::MyStrategy() {}

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
    if(distanceSqr(unitPos, aim)<2) return false;
    int sqareStartX =  fmin(floor(unitPos.x), floor(aim.x));
    int sqareFinishX = fmax(ceil(unitPos.x),ceil(aim.x));
    int sqareStartY = fmin(floor(unitPos.y),floor(aim.y));
    int sqareFinishY = fmax(ceil(unitPos.y),int(aim.y));

    for(int i = sqareStartX; i<sqareFinishX;i++)
    {
        for(int j = sqareStartY; j<sqareFinishY;j++)
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

UnitAction MyStrategy::getAction(const Unit &unit, const Game &game,
                                 Debug &debug) {
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
    Vec2Double targetPos = unit.position;



    if (unit.weapon == nullptr && nearestWeapon != nullptr) {
        targetPos = nearestWeapon->position;
    } else if (nearestEnemy != nullptr) {
        targetPos = nearestEnemy->position;
    }
    //debug.draw(CustomData::Log(
    //    std::string("Target pos: ") + targetPos.toString()));


    Vec2Double aim = Vec2Double(0, 0);
    if (nearestEnemy != nullptr) {
        aim = Vec2Double(nearestEnemy->position.x - unit.position.x,
                         nearestEnemy->position.y - unit.position.y);
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

//    game.properties.weaponParams.first;
    if(unit.weapon != nullptr || unit.health<game.properties.unitMaxHealth){
        for (const LootBox &lootBox : game.lootBoxes) {
            if (std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)) {
                if (distanceSqr(unit.position, lootBox.position) < 2) {
                    // for (const WeaponParat &weaponTemp : game.properties.weaponParams) {
                    if(game.properties.weaponParams.at(unit.weapon.get()->typ).bullet.damage< game.properties.weaponParams.at(std::dynamic_pointer_cast<Item::Weapon>(lootBox.item)->weaponType).bullet.damage){
                        targetPos = lootBox.position;
                        swapWeapon = true;
                    }
                }

            }
            if (unit.health<game.properties.unitMaxHealth &&  std::dynamic_pointer_cast<Item::HealthPack>(lootBox.item)) {
                // if (distanceSqr(unit.position, lootBox.position) < 5) {
                targetPos = lootBox.position;
                // }
            }
        }
    }


    UnitAction action;
    bool isObstacleDetected =  isObstacleForAim(unit.position, Vec2Double(nearestEnemy->position.x, nearestEnemy->position.y), game);
    //if(unit.weapon == nullptr && distanceSqr(targetPos, unit.position)>unit.weapon->)
    if(unit.weapon.get() == nullptr || isObstacleDetected || unit.health<game.properties.unitMaxHealth)
        action.velocity = (targetPos.x - unit.position.x)*game.properties.unitMaxHorizontalSpeed;
    else
        action.velocity = 0;
    action.jump = jump;
    action.jumpDown = !action.jump;
    action.aim = aim;
    action.shoot = !isObstacleDetected || (distanceSqr(unit.position, nearestEnemy->position) < 3);
    action.swapWeapon = swapWeapon;
    action.plantMine = false;

    debug.draw(CustomData::Line(Vec2Float(unit.position.x ,unit.position.y),
                                Vec2Float(nearestEnemy->position.x, nearestEnemy->position.y + (game.properties.unitSize.y/2)),
                                0.1,ColorFloat(50,50,50,50)));

//    debug.draw(CustomData::Log(
//            std::string("isObstacleForAim: ")+tsxt));

    return action;
}