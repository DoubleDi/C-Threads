#include "../include/Weapon.h"
#include "../include/Armor.h"
#include "stdafx.h"

//Weapon
Weapon::~Weapon()
{
    
}

Weapon::Weapon(int radius, int speed)
    : radius_(radius), speed_(speed)
{
}

int Weapon::countBonus(const Armor& armor) const
{
    return this->getPower() - armor.getPower(); 
}

int Weapon::getPower() const 
{
    return radius_ + speed_;
}

//Nothing
//default values for Nothing
Nothing::Nothing(int nothingRadius, int nothingSpeed) :
    Weapon(nothingRadius, nothingSpeed)
{
}

WeaponName Nothing::getName() const 
{
    return WeaponName::Nothing;
}

//Sword
//default values for Sword
Sword::Sword(int swordRadius, int swordSpeed) :
    Weapon(swordRadius, swordSpeed)
{
}

WeaponName Sword::getName() const 
{
    return WeaponName::Sword;
}

//Shovel
//default values for Shovel
Shovel::Shovel(int shovelRadius, int shovelSpeed) :
    Weapon(shovelRadius, shovelSpeed)
{
}

WeaponName Shovel::getName() const
{
    return WeaponName::Shovel;
}

//Bow
//default values for Bow
Bow::Bow(int bowRadius, int bowSpeed) :
    Weapon(bowRadius, bowSpeed)
{
}

WeaponName Bow::getName() const 
{
    return WeaponName::Bow;
}

//Hammer
//default values for Hammer
Hammer::Hammer(int hammerRadius, int hammerSpeed) :
    Weapon(hammerRadius, hammerSpeed)
{
}

WeaponName Hammer::getName() const 
{
    return WeaponName::Hammer;
}
