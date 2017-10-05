#include "../include/Human.h"
#include "../include/Weapon.h"
#include "../include/Armor.h"
#include "stdafx.h"

//Human
Human::~Human()
{
}

Human::Human(const std::string& name, int forse, int speed, int skill, int endurance, int life)
    : forse_(forse), skill_(skill), endurance_(endurance), speed_(speed), life_(life),
    name_(name == "default" ? generateName() : name)
{
    this->weapon_.reset(new Nothing());
    this->armor_.reset(new Torso());
} 

int Human::takeDamage(const Human& enemy) const 
{
    int damage = enemy.getOffensePower() - this->getDefensePower() + this->armor_->countBonus(*enemy.weapon_) - enemy.weapon_->countBonus(*this->armor_);
    return damage > 0 ? damage : 0;
}

int Human::dealDamage(const Human& enemy) const 
{
    int damage = this->getOffensePower() - enemy.getDefensePower() + this->weapon_->countBonus(*enemy.armor_) - enemy.armor_->countBonus(*this->weapon_);
    return  damage > 0 ? damage : 0;
}
 
std::string Human::generateName() const
{
    return std::to_string(rand() + rand());
}

int Human::getOffensePower() const 
{
     return forse_ + skill_ + this->weapon_->getPower();
}

int Human::getDefensePower() const
{
     return endurance_ + speed_ - this->armor_->getPower();
}

//Peasant
//default values for Peasant
Peasant::Peasant(const std::string& name, int peasantForse, int peasantSpeed, int peasantSkill, int peasantEndurance) :
    Human(name, peasantForse, peasantSpeed, peasantSkill, peasantEndurance, getDefaultLife())
{
}

HumanProfession Peasant::getProfession() const 
{
    return HumanProfession::Peasant;
}

int Peasant::getDefaultLife() const 
{
    return 100;
}

//Archer
// default values for Archer
Archer::Archer(const std::string& name, int archerForse, int archerSpeed, int archerSkill, int archerEndurance) :
    Human(name, archerForse, archerSpeed, archerSkill, archerEndurance, getDefaultLife())
{    
}

HumanProfession Archer::getProfession() const 
{
    return HumanProfession::Archer;
}

int Archer::getDefaultLife() const 
{
    return 100;
}

//Knight
// default values for Knight
Knight::Knight(const std::string& name, int knightForse, int knightSpeed, int knightSkill, int knightEndurance) :
    Human(name, knightForse, knightSpeed, knightSkill, knightEndurance, getDefaultLife())
{    
}

HumanProfession Knight::getProfession() const 
{
    return HumanProfession::Knight;
}

int Knight::getDefaultLife() const 
{
    return 140;
}
