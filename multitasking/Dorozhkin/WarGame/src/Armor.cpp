#include "../include/Armor.h"
#include "../include/Weapon.h"
#include "stdafx.h"

//Armor
Armor::~Armor()
{
}

Armor::Armor(int speedPenalty)
    : speedPenalty_(speedPenalty) 
{
}

int Armor::countBonus(const Weapon& weapon) const
{
    return this->getPower() - weapon.getPower(); 
}

int Armor::getPower() const 
{
    return speedPenalty_;
}

//Torso
//default values for Torso
Torso::Torso(int torsoSpeedPenalty) : 
    Armor(torsoSpeedPenalty)
{
}

ArmorName Torso::getName() const 
{
    return ArmorName::Torso;
}

//Chain
//default values for Chain
Chain::Chain(int chainSpeedPenalty) :
    Armor(chainSpeedPenalty)
{
}

ArmorName Chain::getName() const
{
    return ArmorName::Chain;
}

//Corslet
//default values for Corslet
Corslet::Corslet(int corsletSpeedPenalty) :
    Armor(corsletSpeedPenalty)
{
}

ArmorName Corslet::getName() const
{
    return ArmorName::Corslet;
}
