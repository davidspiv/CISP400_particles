#pragma once
#include "config.h"
#include <SFML/Graphics.hpp>
#include <random>

inline int getRandInt(int const min, int const max)
{
    return (std::rand() % (max - min + 1)) + min;
}

inline int getRandOddInt(int const min, int const max)
{
    int const randInt = getRandInt(min, max);
    return randInt % 2 ? randInt : randInt - 1;
}

// Mersenne Twister engine
inline double getRandDouble(double min, double max)
{
    static thread_local std::mt19937 generator(std::random_device {}());
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

inline sf::Color getRandColor()
{
    return sf::Color(getRandInt(0, 255), getRandInt(0, 255), getRandInt(0, 255));
}
