#include "Engine.h"
#include "util.h"


Engine::Engine() { setup_window(m_Window, 1920, 1080); }


void Engine::input() {

  Event event;
  while (m_Window.pollEvent(event)) {
    if (event.type == Event::Closed) {
      m_Window.close();
    }

    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        // make particle at
        event.mouseButton.x;
        event.mouseButton.y;
      }
    }
  }

  if (Keyboard::isKeyPressed(Keyboard::Escape)) {
    m_Window.close();
  }
}


void Engine::update(float dtAsSeconds) {
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


void Engine::draw() {
  m_Window.clear();
  for (auto particle : m_particles) {
    m_Window.draw(particle);
  }
  m_Window.display();
}


void Engine::run() {
  sf::Clock frameClock;

  // TESTS
  cout << "Starting Particle unit tests..." << endl;
  Particle p(m_Window, 4,
             {(int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2});
  p.unitTests();
  cout << "Unit tests complete.  Starting engine..." << endl;

  // ENGINE
  while (m_Window.isOpen()) {
    const float dtAsSeconds = frameClock.restart().asSeconds();

    input();
    update(dtAsSeconds);
    draw();
  }
};
