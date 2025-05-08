#pragma once
#include "Particle.h"
#include <SFML/Graphics.hpp>

class Engine {
public:
    Engine();
    void run();

private:
    sf::RenderWindow m_window;
    std::vector<Particle> m_particles;
    size_t m_currColorIdx;

    std::vector<sf::Color> m_colors;

    // Private functions for internal use only
    void input();
    void update(float dtAsSeconds);
    void draw();
};
