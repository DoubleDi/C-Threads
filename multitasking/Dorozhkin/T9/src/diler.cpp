#include "stdafx.h"
#include "diler.h"

std::string dile(std::string word) {
    std::string res{}, add{};
    char let; char num;
    for (auto w : word) {
        add = "";
        if (w >= 'a' && w <= 'c') {
            let = 'a'; num = '2';
        } else if (w >= 'd' && w <= 'f') {
            let = 'd'; num = '3';
        } else if (w >= 'g' && w <= 'i') {
            let = 'g'; num = '4';
        } else if (w >= 'j' && w <= 'l') {
            let = 'j'; num = '5';
        } else if (w >= 'm' && w <= 'o') {
            let = 'm'; num = '6';
        } else if (w >= 'p' && w <= 's') {
            let = 'p'; num = '7';
        } else if (w >= 't' && w <= 'v') {
            let = 't'; num = '8';
        } else if (w >= 'w' && w <= 'z') {
            let = 'w'; num = '9';
        } else if (w == ' ') {
            let = ' '; num = '0';
        }
        for (auto i = 0; i < (w - let + 1); i++) {
            add += num;
        }
        if (let != ' ' && *(--res.end()) == *(add.begin())) {
            res = res + " " + add;
        } else {
            res += add;
        }
    }
    return res;
}
