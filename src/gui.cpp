#include "gui.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <math.h>

using namespace std;

GUI::GUI(mission::Mission mission) {
  this->mission = mission;
}

void GUI::addState(state::State state) {
  states.push_back(state);
}


int width = 800, height = 600;
float sw; //width of one square on screen
sf::Vector2f mTopRight;
const int LHEIGHT = 25, LMARGIN = 2;
const int RWIDTH = 150;
sf::Color BACKGROUNDCOL = sf::Color::Black,
  FLOORCOL = sf::Color::White,
  ROBOTCOL = sf::Color::Red;

sf::Vector2f onMap(int x, int y) {
  return mTopRight + sf::Vector2f(x*sw, y*sw);
}
sf::Vector2f onMapC(int x, int y) {
  return mTopRight + sf::Vector2f(x*sw + sw/2, y*sw + sw/2);
}
sf::Vector2f onMapC(pair<int, int> p) {
  return onMapC(p.first, p.second);
}

sf::Vector2f interp(sf::Vector2f a, sf::Vector2f b, float f) {
  return a*(1-f) + b*f;
}

void GUI::show() {
  if (states.size() == 0)
    states.push_back(state::State(mission));

  sf::RenderWindow window(sf::VideoMode(width, height), "GUI");
  window.setFramerateLimit(60);
  sf::Font font;
  font.loadFromFile("res/SourceSansPro-Regular.ttf");

  int stateId = 0;
  float trans = 0;

  while(window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    trans += 0.04;

    while(trans > 1)
      stateId = min(int(states.size()) - 1, stateId + 1), trans -= 1;
    while(trans < 0)
      stateId = max(0, stateId - 1), trans += 1;

    state::State curS = states[stateId];
    state::State nextS = stateId+1 < int(states.size()) ? states[stateId+1] : curS;

    {
      float mw = width - RWIDTH, mh = height - LHEIGHT;
      sw = min(mw/mission.width, mh/mission.height);
      mTopRight = sf::Vector2f((mw-sw*mission.width)/2, (mh-sw*mission.height)/2);
    }

    window.clear();
    {
      sf::RectangleShape rectangle;

      rectangle = sf::RectangleShape();
      rectangle.setSize(sf::Vector2f(width-RWIDTH, height-LHEIGHT));
      rectangle.setPosition(0, 0);
      rectangle.setFillColor(BACKGROUNDCOL);
      window.draw(rectangle);

      rectangle = sf::RectangleShape();
      rectangle.setSize(sf::Vector2f(width, LHEIGHT));
      rectangle.setPosition(0, height-LHEIGHT);
      rectangle.setFillColor(sf::Color(0x333333ff));
      window.draw(rectangle);

      rectangle = sf::RectangleShape();
      rectangle.setSize(sf::Vector2f(RWIDTH, height-LHEIGHT));
      rectangle.setPosition(width-RWIDTH, 0);
      rectangle.setFillColor(sf::Color(0x333333ff));
      window.draw(rectangle);
    }

    {
      int curWidth = LHEIGHT/2;
      auto drawButton = [&](int w, string label) {
        int x = curWidth + LMARGIN, y = height-LHEIGHT+LMARGIN, h = LHEIGHT-LMARGIN;
        bool mouseOver = false;
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        mouseOver = (mp.x >= x && mp.y >= y && mp.x <= x+w && mp.y <= y+h);

        sf::Text text(label, font, (LHEIGHT-LMARGIN*2));
        text.setPosition(x, y);
        text.setColor(sf::Color(0xccccccff));
        if (mouseOver)
          text.setColor(sf::Color(0xffffffff));
        window.draw(text);
        curWidth += w;

        return mouseOver && sf::Mouse::isButtonPressed(sf::Mouse::Left);
      };
      int w = LHEIGHT*3;
      drawButton(w, "<");
      drawButton(w, "play");
      drawButton(w, "pause");
      drawButton(w, ">");
    }

    for (int x = 0; x < mission.width; x++)
      for (int y = 0; y < mission.height; y++) {
        sf::RectangleShape rectangle;
        rectangle.setFillColor(
            mission.toPos({x, y}) != -1 ? FLOORCOL : BACKGROUNDCOL);
        rectangle.setPosition(onMap(x, y));
        rectangle.setSize(sf::Vector2f(sw, sw));
        window.draw(rectangle);
      } 

    for (int r = 0; r < int(curS.robots.size()); r++) {
      float rad = sw*0.4f;
      sf::Vector2f pos = interp(
        onMapC(mission.toCoors(curS.robots[r].pos)), 
        onMapC(mission.toCoors(nextS.robots[r].pos)), trans)
          - sf::Vector2f(rad, rad);

      sf::CircleShape circle;
      circle.setFillColor(ROBOTCOL);
      circle.setRadius(rad);
      circle.setPosition(pos);
      window.draw(circle);
    }

    window.display();
  }
}