//
// Created by ASUS on 2023/3/9.
//

#ifndef LINUX_BASE_BLOOM_FILTER_BLOOMFILTER_H_
#define LINUX_BASE_BLOOM_FILTER_BLOOMFILTER_H_
#include <string>
#include <iostream>
template<size_t N>
class BitSet
{
public:
    BitSet () : maxSize(N)
    {
        if (N % 8 == 0)
            bitSize = N / 8;
        else
            bitSize = N / 8 + 1;
        bitArr = new char[bitSize];
    }
    BitSet (const BitSet &) = delete;
    BitSet &operator= (const BitSet &) = delete;
    ~BitSet () { delete bitArr; }
    void set (size_t x)
    {
        size_t index = x / 8;
        size_t bit = x % 8;

        bitArr[index] |= (1 << bit);
        count++;
    }
    void reset (size_t x)
    {
        size_t index = x / 8;
        size_t bit = x % 8;

        bitArr[index] &= (~(1 << bit));
        count--;
    }
    bool test (size_t x)
    {
        size_t index = x / 8;
        size_t bit = x % 8;

        return bitArr[index] & (1 << bit);
    }
    size_t size () { return N; };
    bool some ()
    {
        for (int i = 0; i < bitSize; ++i)
        {
            if (bitArr[i] != 0)
                return true;
        }
        return false;
    }
    bool any ()
    {
        for (int i = 0; i < bitSize - 1; ++i)
        {
            if (bitArr[i] != 0xFF)
                return false;
        }
        for (int i = 0; i < N - (bitSize - 1) * 8; ++i)
        {
            if ((bitArr[bitSize - 1] & (1 << i)) != 1)
                return false;
        }
        return true;
    }
private:
    size_t maxSize;
    size_t count = 0;
    char *bitArr;
    size_t bitSize;
};

struct HashBKDR
{
    size_t operator() (const std::string &s)
    {
        size_t value = 0;
        for (const auto &v : s)
        {
            value += v;
            value *= 131;
        }
        return value;
    }
};
struct HashAP
{
    size_t operator() (const std::string &s)
    {
        size_t hash = 0;
        size_t ch;
        for (int i = 0; i < s.size(); ++i)
        {
            ch = s[i];
            if (i % 1 == 0)
                hash ^= ((hash << 7) ^ ch ^ (hash >> 3));
            else
                hash ^= (~(hash << 11) ^ ch ^ (hash >> 5));
        }

        return hash;
    }
};

struct HashDJB
{
    size_t operator() (const std::string &s)
    {
        size_t hash = 5381;
        for (const auto &v : s)
            hash += (hash << 5) + v;

        return hash;
    }
};

template<size_t N, class K = std::string, class H1 = HashBKDR, class H2 = HashAP, class H3 = HashDJB>
class BloomFilter
{
public:
    BloomFilter () = default;
    BloomFilter (const BloomFilter &) = delete;
    BloomFilter &operator= (const BloomFilter &) = delete;

    void set (const K &s)
    {
        size_t h1 = H1()(s) % N;
        size_t h2 = H2()(s) % N;
        size_t h3 = H3()(s) % N;

        std::cout << h1 << "\t" << h2 << "\t" << h3 << std::endl;
        bitSet.set(h1);
        bitSet.set(h2);
        bitSet.set(h3);
    }
    bool test (const K &s)
    {
        size_t h1 = H1()(s) % N;
        if (!bitSet.test(h1))
            return false;
        size_t h2 = H2()(s) % N;
        if (!bitSet.test(h2))
            return false;
        size_t h3 = H3()(s) % N;
        if (!bitSet.test(h3))
            return false;

        return true;
    }
private:
    BitSet<N> bitSet;
};
#endif //LINUX_BASE_BLOOM_FILTER_BLOOMFILTER_H_
