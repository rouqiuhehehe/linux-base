//
// Created by ASUS on 2023/2/26.
//
#ifdef TEST

#include <iostream>
#include "connectionpool.h"
#include <functional>
#include <random>
template <class ...Arg>
inline void checkClock (const std::function <void (Arg ...)> &fn, Arg...arg)
{
    clock_t begin = clock();
    fn(arg...);
    clock_t end = clock();
    std::cout << begin - end << "ms" << std::endl;
}

inline int getRandomInt (int a, int b)
{
    std::random_device rd;
    std::default_random_engine eng(rd());
    return std::uniform_int_distribution <>(a, b)(eng);
}
int main ()
{
    // auto *connectionPool = ConnectionPool::createConnectionPool();

    for (int i = 0; i < 100; ++i)
    {
        std::string name;
        for (int j = 0; j < getRandomInt(4, 10); ++j)
        {
            char s = getRandomInt(97, 122);
            name += s;
        }
        std::cout << name << std::endl;
        name.clear();
    }
}

#endif