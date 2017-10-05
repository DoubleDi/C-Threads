#include "../include/Location.h"
#include "stdafx.h"

//Location
const std::string& Location::enter(std::unique_ptr<Human>&& human)
{
    std::unique_lock<std::mutex> lock(peopleMutex);
    std::string name = human->name_;
    people[name].swap(human);
    peopleCV.notify_one();
    return people[name]->name_;
}

std::unique_ptr<Human> Location::leave(const std::string& name)
{
    std::unique_lock<std::mutex> lock(peopleMutex);
    Human* freehuman = people[name].release();
    people.erase(name);
    return std::move(std::unique_ptr<Human>(freehuman));
}

const std::string& Location::getRandomName(const std::string& withOut)
{
    std::unique_lock<std::mutex> lock(peopleMutex, std::adopt_lock);
    std::vector<std::string> names;
    for (auto p = people.begin(); p != people.end(); p++) {
        if (p->first != withOut) 
        names.push_back(p->first);
    }
    if (names.empty()) {
        peopleCV.wait(lock);
        // throw std::logic_error{"No humans on this location to get name"};
        return people.begin()->first;
    } else {
        return names[floor(static_cast<int>(rand() / static_cast<float>(RAND_MAX) * names.size()))]; 
    }
}

const std::string& Location::getRandomNameParallel()
{
    std::unique_lock<std::mutex> lock(peopleMutex);
    return this->getRandomName();
}

Location::~Location()
{
    
}

bool Location::isInside(const std::string& humanName) const
{
    if (people.find(humanName) != people.end()) {
        return true;
    } else {
        return false;
    }
}

void Location::heal(const std::string& name)
{
    std::unique_lock<std::mutex> lock(peopleMutex, std::adopt_lock);
    if (this->isInside(name)) {
        people[name]->life_ = people[name]->getDefaultLife();
    } else {
        throw std::logic_error{"No human in Location to heal"};
    }
}

//Barrack
std::string Barrack::birthRandom(const std::string& birthName)
{
    std::unique_lock<std::mutex> lock(peopleMutex);
    int randomProfession = rand() % static_cast<int>(HumanProfession::Human);
    auto humanProfession = static_cast<HumanProfession>(randomProfession);
    if (humanProfession == HumanProfession::Peasant) {
        return this->birth<Peasant>(birthName);
    } else if (humanProfession == HumanProfession::Archer) {
        return this->birth<Archer>(birthName);
    } else {
        return this->birth<Knight>(birthName);
    }
}

//Arsenal
void Arsenal::addArmor(const ArmorName& armorName)
{
    if (armorName != ArmorName::Torso) {
        armors[armorName]++;
        armorCV.notify_one();
    }
}

void Arsenal::addWeapon(const WeaponName& weaponName)
{
    if (weaponName != WeaponName::Nothing) {
        weapons[weaponName]++;
        weaponCV.notify_one();
    }
}

void Arsenal::putArmor(const std::string& humanName)
{
    std::unique_lock<std::mutex> lock1(armorMutex, std::defer_lock);
    std::unique_lock<std::mutex> lock2(peopleMutex, std::defer_lock);
    std::lock(lock1, lock2);
    
    if (this->isInside(humanName)) {
        if (people[humanName]->armor_->getName() != ArmorName::Torso) {
            armors[people[humanName]->armor_->getName()]++;
            armorCV.notify_one();
            people[humanName]->armor_.reset(new class Torso());
        }
    } else {
        throw std::logic_error{"No human in Arsenal"};
    }
}

void Arsenal::putWeapon(const std::string& humanName)
{
    std::unique_lock<std::mutex> lock1(weaponMutex, std::defer_lock);
    std::unique_lock<std::mutex> lock2(peopleMutex, std::defer_lock);
    std::lock(lock1, lock2);
    
    if (this->isInside(humanName)) {
        if (people[humanName]->weapon_->getName() != WeaponName::Nothing) {
            weapons[people[humanName]->weapon_->getName()]++;
            weaponCV.notify_one();
            people[humanName]->weapon_.reset(new class Nothing());
        }
    } else {
        throw std::logic_error{"No human in Arsenal"};
    }
}

void Arsenal::takeRandomArmor(const std::string& humanName)
{
    std::unique_lock<std::mutex> lock1(armorMutex, std::defer_lock);
    std::unique_lock<std::mutex> lock2(peopleMutex, std::defer_lock);
    std::lock(lock1, lock2);
    
    std::vector<ArmorName> names;
    for (auto w = armors.begin(); w != armors.end(); w++) {
        if (w->second) {
            names.push_back(w->first);
        }
    }
    ArmorName armorName;
    if (names.empty()) {
        armorCV.wait(lock1);
        armorName = armors.begin()->first;
    } else {
        int randomArmor = rand() % names.size();
        armorName = names[randomArmor];
    }
    if (armorName == ArmorName::Corslet) {
        this->takeArmor<Corslet>(humanName);
    } else if (armorName == ArmorName::Chain) {
        this->takeArmor<Chain>(humanName);
    }
}

void Arsenal::takeRandomWeapon(const std::string& humanName)
{
    std::unique_lock<std::mutex> lock1(weaponMutex, std::defer_lock);
    std::unique_lock<std::mutex> lock2(peopleMutex, std::defer_lock);
    std::lock(lock1, lock2);
    
    std::vector<WeaponName> names;
    for (auto w = weapons.begin(); w != weapons.end(); w++) {
        if (w->second) {
            names.push_back(w->first);
        }
    }
    WeaponName weaponName;
    if (names.empty()) {
        weaponCV.wait(lock1);
        weaponName = weapons.begin()->first;
    } else {
        int randomWeapon = rand() % names.size();
        weaponName = names[randomWeapon];
    }
    if (weaponName == WeaponName::Sword) {
        this->takeWeapon<Sword>(humanName);
    } else if (weaponName == WeaponName::Hammer) {
        this->takeWeapon<Hammer>(humanName);
    } else if (weaponName == WeaponName::Bow) {
        this->takeWeapon<Bow>(humanName);
    } else if (weaponName == WeaponName::Shovel) {
        this->takeWeapon<Shovel>(humanName);
    }
}

void Arsenal::addRandomWeapon()
{
    std::unique_lock<std::mutex> lock(weaponMutex, std::adopt_lock);
    int randomWeapon = rand() % static_cast<int>(WeaponName::Weapon);
    auto weaponName = static_cast<WeaponName>(randomWeapon);
    if (weaponName == WeaponName::Nothing) {
        this->addRandomWeapon();
    } else {
        this->addWeapon(weaponName);
    }
}

void Arsenal::addRandomArmor()
{
    std::unique_lock<std::mutex> lock(armorMutex, std::adopt_lock);
    int randomArmor = rand() % static_cast<int>(ArmorName::Armor);
    auto armorName = static_cast<ArmorName>(randomArmor);
    if (armorName == ArmorName::Torso) {
        this->addRandomArmor();
    } else {
        this->addArmor(armorName);
    }
}

//Stadium
const std::string& Stadium::battle(const std::string& humanName, const std::string& opponentName)
{
    this->heal(humanName);
    this->heal(opponentName);
    while (people[humanName]->life_ >= 0 && people[opponentName]->life_ >= 0)
    {
        
        auto humanDamage = people[humanName]->takeDamage(*people[opponentName]);
        auto opponentDamage = people[opponentName]->takeDamage(*people[humanName]);
        people[humanName]->life_ -= humanDamage > 0 ? humanDamage : 0;
        people[opponentName]->life_ -= opponentDamage > 0 ? opponentDamage : 0;
    }
    return people[humanName]->life_ > 0 ? humanName : opponentName;
    // throw std::logic_error{"No human in Stadium"};
}

const std::string& Stadium::battleRandomEnemies()
{
    std::unique_lock<std::mutex> lock(peopleMutex); 
    auto humanName = this->getRandomName();
    auto opponentName = this->getRandomName(humanName);
    return this->battle(humanName, opponentName);
}
