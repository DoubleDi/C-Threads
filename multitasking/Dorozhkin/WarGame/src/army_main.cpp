#include "../include/Army.h"
#include "stdafx.h"


int main(int argc, char const *argv[]) {
    std::unique_ptr<ArmyBuilder> romanArmyBuilder(new RomanArmyBuilder);
    std::unique_ptr<ArmyBuilder> barbarianArmyBuilder(new BarbarianArmyBuilder);
    
    auto romanArmy = ArmyDirector::createArmy(std::move(romanArmyBuilder));
    auto barbarianArmy = ArmyDirector::createArmy(std::move(barbarianArmyBuilder));
    
    romanArmy->save(std::cout);
    std::cout << "===============" << std::endl;
    barbarianArmy->save(std::cout);
    return 0;
}
