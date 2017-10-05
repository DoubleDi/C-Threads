#pragma once
#include "Human.h"

class Weapon;
class Armor;

class Location
{
public:
    std::mutex peopleMutex;
    std::condition_variable peopleCV;
    std::unique_lock<std::mutex> peopleLock;
    std::map<std::string, std::unique_ptr<Human>> people;
    Location() = default;
    const std::string& enter(std::unique_ptr<Human>&& human);
    std::unique_ptr<Human> leave(const std::string& name);
    bool isInside(const std::string& humanName) const;
    void heal(const std::string& name);
    Location(const Location& copied) = delete;
    Location& operator= (const Location& copied) = delete;
    const std::string& getRandomName(const std::string& withOut = "");
    const std::string& getRandomNameParallel();
    virtual ~Location();
};

class Barrack : public Location
{
public:
    Barrack() = default;
    template <class profession>
    std::string birth(const std::string& birthName = "default")
    {
        auto human = std::unique_ptr<Human>(new profession(birthName));
        std::string name = human->name_;
        people[name].swap(human);
        peopleCV.notify_one();
        return name;
    }
    std::string birthRandom(const std::string& birthName = "default");
    Barrack(const Barrack& copied) = delete;
    Barrack& operator= (const Barrack& copied) = delete;
};

class Arsenal : public Location
{
    std::map<ArmorName, int> armors;
    std::map<WeaponName, int> weapons;
    std::mutex armorMutex; 
    std::mutex weaponMutex;
    std::condition_variable armorCV;
    std::condition_variable weaponCV;
    std::unique_lock<std::mutex> armorLock;
    std::unique_lock<std::mutex> weaponLock;
public:
    Arsenal() = default;
    void addArmor(const ArmorName& armorName);
    void addRandomArmor();
    void addWeapon(const WeaponName& weaponName);
    void addRandomWeapon();
    template <class armorType>
    void takeArmor(const std::string& humanName)
    {
        // armorMutex.try_lock();
        // peopleMutex.try_lock();
        auto at = armorType::name;
        if (at != ArmorName::Torso) {
            auto armor_it = armors.find(at);
            if (armor_it != armors.end()&& armor_it->second > 0) {
                auto armor = std::unique_ptr<Armor>(new armorType);
                if (this->isInside(humanName)) {
                    if (people[humanName]->armor_->getName() != ArmorName::Torso) {
                        this->putArmor(humanName);
                    }
                    people[humanName]->armor_.swap(armor);
                } else {
        
                    throw std::logic_error{"No human in Arsenal"};
                }
            } else {
                throw std::logic_error{"No armor in Arsenal"};
            }
        }
        // armorMutex.unlock();
        // peopleMutex.unlock();
    }
    void takeRandomArmor(const std::string& humanName);
    template <class weaponType>
    void takeWeapon(const std::string& humanName)
    {
        // weaponMutex.try_lock();
        // peopleMutex.try_lock();
        auto wt = weaponType::name;
        if (wt != WeaponName::Nothing) {
            auto weapon_it = weapons.find(wt);
            if (weapon_it != weapons.end()&& weapon_it->second > 0) {
                auto weapon = std::unique_ptr<Weapon>(new weaponType);
                if (this->isInside(humanName)) {
                    if (people[humanName]->weapon_->getName() != WeaponName::Nothing) {
                        this->putArmor(humanName);
                    }
                    people[humanName]->weapon_.swap(weapon);
                } else {
                    throw std::logic_error{"No human in Arsenal"};
                }
            } else {
                throw std::logic_error{"No weapon in Arsenal"};
            }
        }
        // peopleMutex.unlock();
        // weaponMutex.unlock();
    }
    void takeRandomWeapon(const std::string& humanName);
    void putArmor(const std::string& humanName);
    void putWeapon(const std::string& humanName);
};

class Stadium : public Location
{
public:
    Stadium() = default;
    const std::string& battle(const std::string& humanName, const std::string& opponentName);
    const std::string& battleRandomEnemies();
};
