#include <iostream>
#include "varvector.h"

int main(int argc, char* argv[])
{
    stable_varvector<int, char, bool> v;
    v.push_back(10);
    v.push_back('c');
    v.push_back(30);
    v.push_back(true);
    v.push_back(20);
    v.push_back(false);
    v.push_back('a');
    v.push_back(42);

    std::cout << "vector<int> size: " << sizeof(std::vector<int>) << std::endl;
    std::cout << "vector<char> size: " << sizeof(std::vector<char>) << std::endl;
    std::cout << "vector<bool> size: " << sizeof(std::vector<bool>) << std::endl;
    std::cout << "varvector size: " << sizeof(varvector<int, char, bool>) << std::endl;

    std::cout << "Nb elements: " << v.size() << std::endl;
    v.foreach([](const auto& el) {
        std::cout << el << '\n';
    });

    return 0;
}
