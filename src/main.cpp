#include <iostream>

#include "lumen/version.hpp"

int main() {
    std::cout << "Lumen v" << lumen::kVersion << '\n';
    return 0;
}