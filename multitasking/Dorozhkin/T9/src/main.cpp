#include "stdafx.h"
#include "diler.h"

int main(int argc, char const *argv[]) {
    int n; // count of tests
    std::string s;
    std::vector<std::string> out;
    
    std::getline(std::cin, s);
    try {
        n = stoi(s);
    } catch (...) {
        std::cout << "BAD FIRST ARGUMENT" << std::endl;
        return EXIT_FAILURE;
    }
    
    try {
        for (auto i = 0; i < n; i++) {
            std::getline(std::cin, s);
            out.push_back("Case #" + std::to_string(i + 1) + ":" + dile(s));
        }
    } catch (...) {
        std::cout << "ERROR" << std::endl;
        return EXIT_FAILURE;
    }
    
    for (auto o : out) {
        std::cout << o << std::endl;
    }
    return EXIT_SUCCESS;
}
