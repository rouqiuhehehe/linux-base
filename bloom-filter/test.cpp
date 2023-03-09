//
// Created by ASUS on 2023/3/10.
//
#include "bloomfilter.h"
int main ()
{
    BloomFilter<10000> bloomFilter;

    bloomFilter.set("狗蛋");
    bloomFilter.set("宋宪宇");

    std::cout << std::boolalpha << bloomFilter.test("狗蛋") << std::endl6 ;
}