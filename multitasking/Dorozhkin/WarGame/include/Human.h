#pragma once
#include "Armor.h"
#include "Weapon.h"
#include "stdafx.h"

enum class HumanProfession { Peasant, Archer, Knight, Human }; 

class Human
{
    const int forse_;
    const int speed_;
    const int skill_;
    const int endurance_;
public:
    int life_;
    const std::string name_;
    std::unique_ptr<Weapon> weapon_;
    std::unique_ptr<Armor> armor_;
    std::string generateName() const;
    int takeDamage(const Human& enemy) const;
    int dealDamage(const Human& human) const;
    int getOffensePower() const;
    int getDefensePower() const;
    Human(const std::string& name = "default", int forse = 0, int speed = 0, int skill = 0, int endurance = 0, int life = 100); 
    Human(const Human& copied) = delete;
    Human& operator= (const Human& copied) = delete;
    virtual ~Human();  
    virtual HumanProfession getProfession() const = 0;
    virtual int getDefaultLife() const = 0;
};

class Peasant : public Human
{
public:
    // getForse const override { return 100; }
    Peasant(const std::string& name = "default", int peasantForse = 50, int peasantSpeed = 20, int peasantSkill = 10, int peasantEndurance = 20); 
    Peasant(const Peasant& copied) = delete;
    Peasant& operator= (const Peasant& copied) = delete;
    HumanProfession getProfession() const override;
    int getDefaultLife() const override;
};

class BarbarianPeasant : public Peasant 
{
    
};

class Archer : public Human
{
public:
    Archer(const std::string& name = "default", int archerForse = 30, int archerSpeed = 40, int archerSkill = 30, int archerEndurance = 20); 
    Archer(const Archer& copied) = delete;
    Archer& operator= (const Archer& copied) = delete;
    HumanProfession getProfession() const override;
    int getDefaultLife() const override;
};

class RomanArcher : public Archer
{
    
};

class Knight : public Human
{
public:
    Knight(const std::string& name = "default", int knightForse = 70, int knightSpeed = 10, int knightSkill = 40, int knightEndurance = 50); 
    Knight(const Knight& copied) = delete;
    Knight& operator= (const Knight& copied) = delete;
    HumanProfession getProfession() const override;
    int getDefaultLife() const override;
};

class RomanKnight : public Knight 
{
    
};

class BarbarianKnight : public Knight
{
    
};
