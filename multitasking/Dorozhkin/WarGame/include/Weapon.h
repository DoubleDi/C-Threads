#pragma once
class Armor;

enum class WeaponName { Sword, Hammer, Bow, Nothing, Shovel, Weapon };

class Weapon {
    const int radius_;
    const int speed_;
public:
    Weapon(int radius = 0, int speed = 0);
    Weapon(const Weapon& copied) = delete;
    Weapon& operator= (const Weapon& copied) = delete;
    int countBonus(const Armor& armor) const;
    virtual ~Weapon();
    virtual WeaponName getName() const = 0;
    static constexpr WeaponName name = WeaponName::Weapon;
    int getPower() const;
};

class Nothing : public Weapon
{
public:
    Nothing(int nothingRadius = 0, int nothingSpeed = 30);
    Nothing(const Nothing& copied) = delete;
    Nothing& operator= (const Nothing& copied) = delete;
    WeaponName getName() const override;
    static constexpr WeaponName name = WeaponName::Nothing;
};

class Sword : public Weapon
{
public:
    Sword(int swordRadius = 20, int swordSpeed = 30);
    Sword(const Sword& copied) = delete;
    Sword& operator= (const Sword& copied) = delete;
    WeaponName getName() const override;
    static constexpr WeaponName name = WeaponName::Sword;
};

class Shovel : public Weapon
{
public:
    Shovel(int shovelRadius = 20, int shovelSpeed = 40);
    Shovel(const Shovel& copied) = delete;
    Shovel& operator= (const Shovel& copied) = delete;
    WeaponName getName() const override;
    static constexpr WeaponName name = WeaponName::Shovel;
};

class Bow : public Weapon
{
public:
    Bow(int bowRadius = 50, int bowSpeed = 10);
    Bow(const Bow& copied) = delete;
    Bow& operator= (const Bow& copied) = delete;
    WeaponName getName() const override;
    static constexpr WeaponName name = WeaponName::Bow;
};

class Hammer : public Weapon
{
public:
    Hammer(int hammerRadius = 20, int hammerSpeed = 10);
    Hammer(const Hammer& copied) = delete;
    Hammer& operator= (const Hammer& copied) = delete;
    WeaponName getName() const override;
    static constexpr WeaponName name = WeaponName::Hammer;
};
