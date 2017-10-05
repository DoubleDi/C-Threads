#include "../include/Armor.h"
#include "../include/Weapon.h"
#include "../include/Location.h"
#include "../include/Human.h"
#include "stdafx.h"

Arsenal arsenal;
Barrack barrack;
Stadium stadium;

void fillArsenalWeapons() 
{
    std::cout << "filling arsenal weapons START\n" << std::flush;
    for (auto i = 0; i < 15; i++) {
        arsenal.addRandomWeapon();
    }
    std::cout << "filling arsenal weapons END\n" << std::flush;
}

void fillArsenalArmors() 
{
    std::cout << "filling arsenal armors START\n" << std::flush;
    for (auto i = 0; i < 15; i++) {
        arsenal.addRandomArmor();
    }
    std::cout << "filling arsenal armors END\n" << std::flush;
}

void fillBarracks()
{
    std::cout << "filling barracks START\n" << std::flush;
    for (auto i = 0; i < 20; i++) {
        barrack.birthRandom();
    }
    std::cout << "filling barracks END\n" << std::flush;
}

void battle() 
{
    for (int i = 0; i < 40; i++) {
        std::cout << "battle " << i << " START\n" << std::flush;
        auto winner = stadium.battleRandomEnemies();
        std::cout << "battle " << i << " WINNER " << winner << "\n" << std::flush;
    }
}

void play(int i) 
{
    std::cout << std::to_string(i) +  " PLAY START\n" << std::flush;
    std::cout << std::to_string(i) +  " BARRACK START\n" << std::flush;
    auto humanName = barrack.getRandomName();
    auto human = barrack.leave(humanName);
    std::cout << std::to_string(i) +  " BARRACK END\n" << std::flush;
    std::cout << std::to_string(i) + " ARSENAL START\n" << std::flush;
    arsenal.enter(std::move(human));
    arsenal.takeRandomWeapon(humanName);
    arsenal.takeRandomArmor(humanName);
    human = arsenal.leave(humanName);
    std::cout << std::to_string(i) + " ARSENAL END\n" << std::flush;
    
    std::cout << std::to_string(i) +  " STADIUM START\n" << std::flush;
    stadium.enter(std::move(human));
    
}

int main(int argc, char const*argv[]) 
{
    try {
        std::thread arsenalFillWeaponThread(fillArsenalWeapons);
        std::thread arsenalFillArmorThread(fillArsenalArmors);
        std::thread barrackFillPeopleThread(fillBarracks);
        std::thread battleThread(battle);
        
        for (auto i = 0; i < 1; i++) {
            std::thread player1(play, 4 * i);
            std::thread player2(play, 4 * i + 1);
            std::thread player3(play, 4 * i + 2);
            std::thread player4(play, 4 * i + 3);
            player1.join();
            player2.join();
            player3.join();
            player4.join();
        }
        arsenalFillArmorThread.join();
        arsenalFillWeaponThread.join();
        barrackFillPeopleThread.join();
        battleThread.join();
        std::cout << "END ALL\n" << std::flush;
    } catch(std::logic_error&s) {
        std::cerr << s.what() << std::endl;
    } catch(...) {
        std::cerr << "Oups" << std::endl;
    }
    return 0;
}
