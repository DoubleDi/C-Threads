#include "../include/Army.h"

#include "stdafx.h"

#include "Human.h"
#include "stdafx.h"

void Army::save(std::ostream& out) const
{
    out << "LEADER" << std::endl;
    out << leader_->name_ << std::endl;
    out << "ARCHERS" << std::endl;
    for (auto& aa : archers_) {
        out << aa->name_ << std::endl;
    }
    out << "KNIGHTS" << std::endl;
    for (auto& kk : knights_) {
        out << kk->name_ << std::endl;
    }
    out << "PEASANTS" << std::endl;
    for (auto& pp : peasants_) {
        out << pp->name_ << std::endl;
    }
}

ArmyBuilder::~ArmyBuilder()
{
    
}

std::unique_ptr<Army> ArmyBuilder::getArmy()
{
    return std::move(army_);
}

void RomanArmyBuilder::buildArmy()
{
    army_ = std::unique_ptr<Army>(new Army());
}

void RomanArmyBuilder::buildArchers()
{
    for (int i = 0; i < 5; i++) {
        army_->archers_.push_back(std::move(std::unique_ptr<Archer>(new RomanArcher())));
    }
}

void RomanArmyBuilder::buildPeasants()
{
}

void RomanArmyBuilder::buildKnights()
{
    for (int i = 0; i < 3; i++) {
        army_->knights_.push_back(std::move(std::unique_ptr<Knight>(new RomanKnight())));
    }
}

void RomanArmyBuilder::buildLeader()
{
    army_->leader_ = std::move(std::unique_ptr<Human>(new RomanArcher()));
}

void BarbarianArmyBuilder::buildArmy()
{
    army_ = std::unique_ptr<Army>(new Army());
}

void BarbarianArmyBuilder::buildArchers()
{
}

void BarbarianArmyBuilder::buildPeasants()
{
    for (int i = 0; i < 2; i++) {
        army_->peasants_.push_back(std::move(std::unique_ptr<Peasant>(new BarbarianPeasant())));
    }
}

void BarbarianArmyBuilder::buildKnights()
{
    for (int i = 0; i < 6; i++) {
        army_->knights_.push_back(std::move(std::unique_ptr<Knight>(new BarbarianKnight())));
    }
}

void BarbarianArmyBuilder::buildLeader()
{
    army_->leader_ = std::move(std::unique_ptr<Human>(new BarbarianPeasant()));
}


std::unique_ptr<Army> ArmyDirector::createArmy(std::unique_ptr<ArmyBuilder> armyBuilder)
{
    armyBuilder->buildArmy();
    armyBuilder->buildArchers();
    armyBuilder->buildKnights();
    armyBuilder->buildPeasants();
    armyBuilder->buildLeader();
    return armyBuilder->getArmy();
}
