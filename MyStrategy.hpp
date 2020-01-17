#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "Debug.hpp"
#include "model/CustomData.hpp"
#include "model/Game.hpp"
#include "model/Unit.hpp"
#include "model/UnitAction.hpp"
#include <map>
//#include "helpers/Geometry.h"
#include "helpers/InfluenceMapBuilder.h"
#include "helpers/AStar.h"


class MyStrategy {
public:
  MyStrategy();
  UnitAction getAction(const Unit &unit, const Game &game, Debug &debug);
  void SetPotentialField(const Game &game);
  std::vector<std::vector<double>> PotentialField;
  std::map<int,std::vector<Vec2Double>> SavedPath;
};

#endif