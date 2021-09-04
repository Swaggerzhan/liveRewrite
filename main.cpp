#include <iostream>
#include "base/timeval.h"


int main() {

    DelayInterval time(10, 20);
    std::cout << time.seconds() << std::endl;
    std::cout << time.useconds() << std::endl;

}
