#include "gui.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <math.h>
#include <sstream>

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
  ROBOTCOL = sf::Color(0xff3333ff),
  ITEMCOL =  sf::Color(0x3333ffff),
  ITEMEMPTYCOL = sf::Color(0x333333ff),
  ITEMFRAMECOL = sf::Color::Black,
  STATIONCOL = sf::Color(0x337733ff);
const float ITEMRADROBOT = 0.15, ITEMRADSHELF = 0.2, ITEMRADSTATION = 0.1;

sf::Vector2f onMap(int x, int y) {
  return mTopRight + sf::Vector2f(x*sw, y*sw);
}
sf::Vector2f onMap(pair<int, int> p) {
  return onMap(p.first, p.second);
}
sf::Vector2f onMapC(int x, int y) {
  return mTopRight + sf::Vector2f(x*sw + sw/2, y*sw + sw/2);
}
sf::Vector2f onMapC(pair<int, int> p) {
  return onMapC(p.first, p.second);
}

float smooth(float f) {
  return 3*f*f - 2*f*f*f;
}

sf::Vector2f interp(sf::Vector2f a, sf::Vector2f b, float f) {
  f = smooth(f);
  return a*(1-f) + b*f;
}
float interp(float a, float b, float f){
  f = smooth(f);
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

  bool playing = true;

  while(window.isOpen()) {
    bool mouseClicked = false;
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::Resized) {
        width = window.getSize().x, height = window.getSize().y;
        window.setView(sf::View(sf::FloatRect(0, 0, width, height)));
      } else if (event.type == sf::Event::MouseButtonPressed) {
        mouseClicked = event.mouseButton.button == sf::Mouse::Left;
      }
    }

    float dT = 0.0167f;

    if (playing)
      trans += 1*dT;
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

    auto drawText = [&](sf::Vector2f pos, sf::Color color, string str, int sz) {
      sf::Text text(str, font, sz);
      text.setColor(color);
      text.setPosition(pos);
      window.draw(text);
    };
    auto drawItem = [&](sf::Vector2f pos, float rad, int item, bool empty=false) {
      sf::CircleShape circle;
      circle.setFillColor(empty ? ITEMEMPTYCOL : ITEMCOL);
      circle.setOutlineColor(ITEMFRAMECOL);
      circle.setRadius(rad);
      circle.setOutlineThickness(rad*0.1);
      circle.setPosition(pos - sf::Vector2f(rad, rad));
      window.draw(circle);
      
      drawText(pos - sf::Vector2f(rad*0.2, rad*0.5),
        ITEMFRAMECOL,
        to_string(item),
        int(rad));
    };

    {
      int curWidth = LHEIGHT/2;
      auto drawButton = [&](int w, string label) {
        int x = curWidth + LMARGIN, y = height-LHEIGHT+LMARGIN, h = LHEIGHT-LMARGIN;
        bool mouseOver = false;
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        mouseOver = (mp.x >= x && mp.y >= y && mp.x <= x+w && mp.y <= y+h);

        drawText(sf::Vector2f(x, y), 
            mouseOver ? sf::Color(0xccccccff) : sf::Color(0xffffffff),
            label,
            (LHEIGHT-LMARGIN*2));
        curWidth += w;

        return mouseOver && mouseClicked;
      };
      int w = LHEIGHT*3;
      if (drawButton(w/2, "<<"))
        trans = 0, stateId = 0;
      if (drawButton(w/3, "<")){
        if (trans != 0)
          trans = 0;
        else
          stateId = max(0, stateId-1);
      }
      if (drawButton(w, "play")) {
        playing = true;
      }
      if (drawButton(w, "pause")) {
        playing = false;
      }
      if (drawButton(w/3, ">")) {
        stateId = min(int(states.size() - 1), stateId+1);
        trans = 0;
      }
      if (drawButton(w/2, ">>"))
        trans = 0, stateId = int(states.size() - 1);

      string stepText = "State: ";
      stepText += to_string(stateId);
      //stepText += " ("; stepText += to_string(trans); stepText += ")";
      drawButton(0, stepText);
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

    {
      int curHeight = 5, lineHeight = LHEIGHT/2, x = width-RWIDTH+LMARGIN;

      {
        drawText(sf::Vector2f(x, curHeight), sf::Color::White, "Orders", lineHeight*2);
        curHeight += lineHeight*3;
      }

      for (int i = 0; i < int(mission.orders.size()); i ++) {
        mission::order order = mission.orders[i];
        stringstream ss;
        for (int j = 0; j < int(curS.stations.size()); j++)
          if (curS.stations[j].order == i) {
            ss << j << "> ";
          }
        ss << "[";
        for (int j = 0; j < int(order.items.size()); j++)
          ss << (j == 0 ? "" : ", ") << order.items[j];
        ss << "]";
        drawText(sf::Vector2f(x, curHeight), sf::Color::White, ss.str(), lineHeight);
        curHeight += lineHeight;
      }
    }

    for (int i = 0; i < int(curS.stations.size()); i++) {
      sf::RectangleShape rectangle;
      rectangle.setFillColor(STATIONCOL);
      rectangle.setPosition(onMap(mission.stations[i].coors));
      rectangle.setSize(sf::Vector2f(sw, sw));
      window.draw(rectangle);

      state::station curSt = curS.stations[i], nextSt = nextS.stations[i];
      sf::Vector2f pos = onMapC(mission.stations[i].coors);
      drawText(pos + sf::Vector2f(-sw*0.37, sw*0.15), sf::Color::Black, "station " + to_string(i), int(sw*0.2));
      if (curSt.order == -1)
        continue;
      mission::order order = mission.orders[curSt.order];
      for (int j = 0; j < int(order.items.size()); j++) {
        double ang = 2*M_PI*j/order.items.size();
        sf::Vector2f itemPos = pos + sf::Vector2f(float(cos(ang)), float(sin(ang)))*sw*0.3f;
        if (order.items.size() == 1)
          itemPos = pos;
        drawItem(itemPos, ITEMRADSTATION*sw, order.items[j], !curSt.fulfilled[j]);
        if (!curSt.fulfilled[j] && (curSt.order != nextSt.order || nextSt.fulfilled[j]))
          drawItem(itemPos, interp(0, ITEMRADSTATION, trans)*sw, order.items[j]);
      }
    }

    for (int r = 0; r < int(curS.robots.size()); r++) {
      state::robot curR = curS.robots[r], nextR = nextS.robots[r];
      float rad = sw*0.4f;
      sf::Vector2f pos = interp(
        onMapC(mission.toCoors(curR.pos)), 
        onMapC(mission.toCoors(nextR.pos)), trans);

      sf::CircleShape circle;
      circle.setFillColor(ROBOTCOL);
      circle.setRadius(rad);
      circle.setPosition(pos - sf::Vector2f(rad, rad));
      window.draw(circle);

      if (curR.item == -1 && nextR.item != -1) {
        float itemR = sw*interp(ITEMRADSHELF, ITEMRADROBOT, trans);
        sf::Vector2f from = onMapC(mission.items[nextR.item].shelfCoors);
        sf::Vector2f itemPos = interp(from, pos, trans);
        drawItem(itemPos, itemR, nextR.item);
      } else if (curR.item != -1 && nextR.item == -1) {
        drawItem(pos, sw*interp(ITEMRADROBOT, 0, trans), curR.item);
      } else if (curR.item != -1){
        drawItem(pos, sw*ITEMRADROBOT, curR.item);
      }
    }

    for (int i = 0; i < int(mission.items.size()); i ++) {
      mission::item item = mission.items[i];
      sf::Vector2f pos = onMapC(item.shelfCoors);
      drawItem(pos, sw*ITEMRADSHELF, i);
      drawText(pos += sf::Vector2f(-sw*0.3, sw*0.15), sf::Color(0x444444ff), "shelf", int(sw*0.2));
    }

    window.display();
  }
}