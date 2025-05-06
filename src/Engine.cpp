#include "Engine.h"

void Engine::input() {
  while (m_Window.isOpen()) {

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
}
