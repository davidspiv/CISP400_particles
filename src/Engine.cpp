#include "Engine.h"
#include "Particle.h"
#include "util.h"

Engine::Engine() { setup_window(m_Window, 1920, 1080); }

void Engine::input()
{

    Event event;
    while (m_Window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            m_Window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {

                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                for (size_t i = 0; i < 5; i++) {
                    m_particles.emplace_back(
                        Particle(m_Window, getRandInt(25, 50), mousePos));
                }
            }
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        m_Window.close();
    }
}

void Engine::update(float dtAsSeconds)
{
    vector<Particle>::iterator it = m_particles.begin();

    while (it != m_particles.end()) {
        if (it->getTTL() > 0.0) {
            it->update(dtAsSeconds);
            ++it;
        } else {
            it = m_particles.erase(it);
        }
    }
}

void Engine::draw()
{
    m_Window.clear();
    for (auto particle : m_particles) {
        m_Window.draw(particle);
    }
    m_Window.display();
}

void Engine::run()
{
    sf::Clock frameClock;

    // TESTS
    cout << "Starting Particle unit tests..." << endl;
    Particle p(m_Window, 4,
        { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete.  Starting engine..." << endl;

    // ENGINE
    while (m_Window.isOpen()) {
        float const dtAsSeconds = frameClock.restart().asSeconds();

        input();
        update(dtAsSeconds);
        draw();
    }
}
