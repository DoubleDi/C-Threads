#include "stdafx.h"
#include "../include/Human.h"
#include "../include/Armor.h"
#include "../include/Weapon.h"
#include "../include/Location.h"

TEST(SetupUniverse, Armor) {
    std::unique_ptr<Torso> torso(new class Torso());
    std::unique_ptr<Chain> chain(new class Chain());
    std::unique_ptr<Corslet> corslet(new class Corslet());
    EXPECT_EQ(ArmorName::Torso, torso->getName());
    EXPECT_EQ(ArmorName::Chain, chain->getName());
    EXPECT_EQ(ArmorName::Corslet, corslet->getName());
    
    std::unique_ptr<Weapon> hammer(new class Hammer());
    EXPECT_LT(torso->countBonus(*hammer), 0);
    EXPECT_LT(chain->countBonus(*hammer), 0);
    EXPECT_EQ(corslet->countBonus(*hammer), 0);
    
    EXPECT_EQ(torso->getPower(), 0);
    EXPECT_GT(chain->getPower(), 0);
    EXPECT_GT(corslet->getPower(), 0);
}

TEST(SetupUniverse, Weapon) {
    std::unique_ptr<Sword> sword(new class Sword());
    std::unique_ptr<Hammer> hammer(new class Hammer());
    std::unique_ptr<Bow> bow(new class Bow());
    std::unique_ptr<Nothing> nothing(new class Nothing());
    std::unique_ptr<Shovel> shovel(new class Shovel());
    EXPECT_EQ(WeaponName::Sword, sword->getName());
    EXPECT_EQ(WeaponName::Hammer, hammer->getName());
    EXPECT_EQ(WeaponName::Bow, bow->getName());
    EXPECT_EQ(WeaponName::Nothing, nothing->getName());
    EXPECT_EQ(WeaponName::Shovel, shovel->getName());
    
    std::unique_ptr<Armor> torso(new class Torso());
    EXPECT_GT(sword->countBonus(*torso), 0);
    EXPECT_GT(hammer->countBonus(*torso), 0);
    EXPECT_GT(bow->countBonus(*torso), 0);
    EXPECT_GT(nothing->countBonus(*torso), 0);
    EXPECT_GT(shovel->countBonus(*torso), 0);
    
    EXPECT_GT(sword->getPower(), 0);
    EXPECT_GT(hammer->getPower(), 0);
    EXPECT_GT(bow->getPower(), 0);
    EXPECT_GT(nothing->getPower(), 0);
    EXPECT_GT(shovel->getPower(), 0);
}

TEST(SetupUniverse, Human) {
    std::unique_ptr<Peasant> peasant(new class Peasant("Peasant"));
    std::unique_ptr<Knight> knight(new class Knight("Knight"));
    std::unique_ptr<Archer> archer(new class Archer("Archer"));
    std::unique_ptr<Archer> archerRandomName(new class Archer()); 
    std::unique_ptr<Archer> archerRandomName2(new class Archer()); 
    
    EXPECT_EQ(HumanProfession::Peasant, peasant->getProfession());
    EXPECT_EQ(HumanProfession::Archer, archer->getProfession());
    EXPECT_EQ(HumanProfession::Archer, archerRandomName->getProfession());
    EXPECT_EQ(HumanProfession::Archer, archerRandomName2->getProfession());
    EXPECT_EQ(HumanProfession::Knight, knight->getProfession());
    
    EXPECT_GT(peasant->getOffensePower(), 0);
    EXPECT_GT(archer->getOffensePower(), 0);
    EXPECT_GT(knight->getOffensePower(), 0);
    EXPECT_GT(archerRandomName->getOffensePower(), 0);
    EXPECT_GT(archerRandomName2->getOffensePower(), 0);
    
    EXPECT_GT(peasant->getDefensePower(), 0);
    EXPECT_GT(archer->getDefensePower(), 0);
    EXPECT_GT(knight->getDefensePower(), 0);
    EXPECT_GT(archerRandomName->getDefensePower(), 0);
    EXPECT_GT(archerRandomName2->getDefensePower(), 0);
    
    EXPECT_EQ(peasant->life_, peasant->getDefaultLife());
    EXPECT_EQ(archer->life_, archer->getDefaultLife());
    EXPECT_EQ(knight->life_, knight->getDefaultLife());
    EXPECT_EQ(archerRandomName->life_, archerRandomName->getDefaultLife());
    EXPECT_EQ(archerRandomName2->life_, archerRandomName2->getDefaultLife());
    
    EXPECT_EQ(peasant->name_, "Peasant");
    EXPECT_EQ(archer->name_, "Archer");
    EXPECT_EQ(knight->name_, "Knight");
    EXPECT_NE(archerRandomName->name_, archerRandomName2->name_);
    
    EXPECT_EQ(peasant->takeDamage(*archer), 0);
    EXPECT_GT(peasant->takeDamage(*knight), 0);
    EXPECT_GT(peasant->dealDamage(*knight), 0);
    EXPECT_GT(peasant->dealDamage(*archer), 0);
}

TEST(SetupUniverse, Location) {
    std::unique_ptr<Barrack> barrack(new class Barrack());
    std::unique_ptr<Stadium> stadium(new class Stadium());
    std::unique_ptr<Arsenal> arsenal(new class Arsenal());
    
    std::unique_ptr<Peasant> peasant(new class Peasant("Peasant"));
    std::unique_ptr<Knight> knight(new class Knight("Knight"));
    std::unique_ptr<Archer> archer(new class Archer("Archer"));
    
    EXPECT_FALSE(barrack->isInside("Peasant"));
    EXPECT_FALSE(barrack->isInside("Archer"));
    peasant->life_ = 1;
    EXPECT_EQ(peasant->life_, 1);
    barrack->enter(std::move(peasant));
    barrack->heal("Peasant");
    EXPECT_TRUE(barrack->isInside("Peasant"));
    EXPECT_FALSE(barrack->isInside("Archer"));
    auto newPeasant = barrack->leave("Peasant");
    EXPECT_EQ(newPeasant->name_, "Peasant");
    EXPECT_EQ(newPeasant->life_, newPeasant->getDefaultLife());
    EXPECT_FALSE(barrack->isInside("Peasant"));
    EXPECT_FALSE(barrack->isInside("Archer"));
    
    auto birthNamePeasant = barrack->birth<Peasant>("Peasant 2");
    auto birthNameArcher = barrack->birth<Archer>("Archer 2");
    auto birthNameKnight = barrack->birth<Knight>();
    EXPECT_EQ(birthNamePeasant, "Peasant 2");
    EXPECT_EQ(birthNameArcher, "Archer 2");
    newPeasant = barrack->leave(birthNamePeasant);
    auto newArcher = barrack->leave(birthNameArcher);
    auto newKnight = barrack->leave(birthNameKnight);
    EXPECT_EQ(newPeasant->getProfession(), HumanProfession::Peasant);
    EXPECT_EQ(newArcher->getProfession(), HumanProfession::Archer);
    
    arsenal->addArmor(ArmorName::Corslet);
    arsenal->addArmor(ArmorName::Chain);
    arsenal->addWeapon(WeaponName::Hammer);
    arsenal->addWeapon(WeaponName::Bow);
    arsenal->enter(std::move(newArcher));
    arsenal->enter(std::move(newKnight));
    arsenal->takeArmor<Chain>(birthNameArcher);
    arsenal->takeArmor<Corslet>(birthNameKnight);
    arsenal->takeWeapon<Bow>(birthNameArcher);
    arsenal->takeWeapon<Hammer>(birthNameKnight);
    newArcher = arsenal->leave(birthNameArcher);
    newKnight = arsenal->leave(birthNameKnight);
    EXPECT_EQ(newArcher->weapon_->getName(), WeaponName::Bow);
    EXPECT_EQ(newKnight->weapon_->getName(), WeaponName::Hammer);
    EXPECT_EQ(newArcher->armor_->getName(), ArmorName::Chain);
    EXPECT_EQ(newKnight->armor_->getName(), ArmorName::Corslet);
    
    arsenal->enter(std::move(newArcher));
    arsenal->enter(std::move(newKnight));
    arsenal->putArmor(birthNameArcher);
    arsenal->putArmor(birthNameKnight);
    arsenal->putWeapon(birthNameArcher);
    arsenal->putWeapon(birthNameKnight);
    newArcher = arsenal->leave(birthNameArcher);
    newKnight = arsenal->leave(birthNameKnight);
    EXPECT_EQ(newArcher->weapon_->getName(), WeaponName::Nothing);
    EXPECT_EQ(newKnight->weapon_->getName(), WeaponName::Nothing);
    EXPECT_EQ(newArcher->armor_->getName(), ArmorName::Torso);
    EXPECT_EQ(newKnight->armor_->getName(), ArmorName::Torso);
    
    stadium->enter(std::move(newArcher));
    stadium->enter(std::move(newKnight));
    EXPECT_EQ(stadium->battle(birthNameKnight, birthNameArcher), birthNameKnight);
}

TEST(SetupUniverse, Exception) {
    std::unique_ptr<Barrack> barrack(new class Barrack());
    std::unique_ptr<Stadium> stadium(new class Stadium());
    std::unique_ptr<Arsenal> arsenal(new class Arsenal());
    
    std::unique_ptr<Peasant> peasant1(new class Peasant("Peasant1"));
    std::unique_ptr<Peasant> peasant2(new class Peasant("Peasant2"));
    std::unique_ptr<Peasant> peasant3(new class Peasant("Peasant3"));
    barrack->enter(std::move(peasant1));
    stadium->enter(std::move(peasant2));
    arsenal->enter(std::move(peasant3));
    arsenal->addArmor(ArmorName::Corslet);
    arsenal->addWeapon(WeaponName::Hammer);
    
    EXPECT_THROW(barrack->heal("Peasant2"), std::logic_error);
    EXPECT_THROW(arsenal->putArmor("Peasant2"), std::logic_error);
    EXPECT_THROW(arsenal->putWeapon("Peasant2"), std::logic_error);
    EXPECT_THROW(stadium->battle("Peasant1", "Peasant2"), std::logic_error);
    EXPECT_THROW(arsenal->takeWeapon<Sword>("Peasant2"), std::logic_error);
    EXPECT_THROW(arsenal->takeWeapon<Hammer>("Peasant2"), std::logic_error);
    EXPECT_THROW(arsenal->takeArmor<Chain>("Peasant2"), std::logic_error);
    EXPECT_THROW(arsenal->takeArmor<Corslet>("Peasant2"), std::logic_error);
}

TEST(SetupUniverse, Random) {
    std::unique_ptr<Barrack> barrack(new class Barrack());
    std::unique_ptr<Stadium> stadium(new class Stadium());
    std::unique_ptr<Arsenal> arsenal(new class Arsenal());
    
    auto name1 = barrack->birthRandom();
    auto name2 = barrack->birthRandom();
    auto human1 = barrack->leave(name1);
    auto human2 = barrack->leave(name2);
    
    arsenal->addRandomWeapon();
    arsenal->addRandomWeapon();
    arsenal->addRandomArmor();
    arsenal->addRandomArmor();
    
    arsenal->enter(std::move(human1));
    arsenal->enter(std::move(human2));
    arsenal->takeRandomWeapon(name1);
    arsenal->takeRandomWeapon(name2);
    arsenal->takeRandomArmor(name1);
    arsenal->takeRandomArmor(name2);
    human1 = arsenal->leave(name1);
    human2 = arsenal->leave(name2);
    
    stadium->enter(std::move(human1));
    stadium->enter(std::move(human2));
    stadium->battleRandomEnemies();
    stadium->battleRandomEnemies();

}
