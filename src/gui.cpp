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
  FLOORCOL = sf::Color::White;

sf::Vector2f onMap(int x, int y) {
  return mTopRight + sf::Vector2f(x*sw, y*sw);
}

void GUI::show() {
  if (states.size() == 0)
    states.push_back(state::State(mission));

  sf::RenderWindow window(sf::VideoMode(width, height), "GUI");
  sf::Font font;
  font.loadFromFile("res/SourceSansPro-Regular.ttf");

  while(window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    {
      float mw = width - RWIDTH, mh = height - LHEIGHT;
      sw = min(mw/mission.width, mh/mission.height);
      mTopRight = sf::Vector2f((mw-sw*mission.width)/2, (mh-sw*mission.height)/2);
      cout << mTopRight.x << " " << mTopRight.y << " " << sw << endl;
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
        sf::Color color = mission.toPos({x, y}) != -1 ? 
            sf::Color::White : sf::Color::Black;

        sf::RectangleShape rectangle;
        rectangle.setFillColor(
            mission.toPos({x, y}) != -1 ? FLOORCOL : BACKGROUNDCOL);
        rectangle.setPosition(onMap(x, y));
        rectangle.setSize(sf::Vector2f(sw, sw));
        window.draw(rectangle);
      }

    window.display();
  }
}