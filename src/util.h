#pragma once
#include "config.h"
#include <SFML/Graphics.hpp>

inline int getRandInt(int const min, int const max)
{
    return (std::rand() % (max - min + 1)) + min;
}

inline double getRandDouble(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

inline sf::Color getRandColor()
{
    return sf::Color(getRandInt(0, 255), getRandInt(0, 255), getRandInt(0, 255));
}

inline void setup_window(sf::RenderWindow& window, int const width,
    int const height)
{

    window.create(sf::VideoMode(width, height), WINDOW_TITLE);

    if (!window.isOpen()) {
        throw std::runtime_error("Failed to create SFML window");
    }

    auto desktop = sf::VideoMode::getDesktopMode();
    window.setPosition({ static_cast<int>(desktop.width / 2 - width / 2),
        static_cast<int>(desktop.height / 2 - height / 2) });

    window.setFramerateLimit(TARGET_FPS);
}
