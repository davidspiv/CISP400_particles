#include "Engine.h"
#include "Particle.h"
#include "util.h"

Engine::Engine()
{
    m_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);

    if (!m_window.isOpen()) {
        throw std::runtime_error("Failed to create SFML window");
    }

    auto desktop = sf::VideoMode::getDesktopMode();
    m_window.setPosition({ static_cast<int>(desktop.width / 2 - WINDOW_WIDTH / 2),
        static_cast<int>(desktop.height / 2 - WINDOW_HEIGHT / 2) });

    m_window.setFramerateLimit(TARGET_FPS);
}

void Engine::input()
{

    Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            m_window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed
            && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

            for (size_t i = 0; i < 5; i++) {
                m_particles.emplace_back(Particle(m_window, getRandInt(25, 50), mousePos));
            }
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        m_window.close();
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
    m_window.clear();
    for (auto particle : m_particles) {
        m_window.draw(particle);
    }
    m_window.display();
}

void Engine::run()
{
    sf::Clock frameClock;

    // TESTS
    cout << "Starting Particle unit tests..." << endl;
    Particle p(m_window, 4, { (int)m_window.getSize().x / 2, (int)m_window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete.  Starting engine..." << endl;

    // ENGINE
    while (m_window.isOpen()) {
        float const dtAsSeconds = frameClock.restart().asSeconds();

        input();
        update(dtAsSeconds);
        draw();
    }
}
