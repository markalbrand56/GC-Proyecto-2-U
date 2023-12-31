#pragma once

#include <print.h>
#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <string>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <unordered_map>
#include "color.h"
#include "imageloader.h"


const Color B = {0, 0, 0};
const Color W = {255, 255, 255};

const int WIDTH = 16;
const int HEIGHT = 11;
const int BLOCK = 75;
const int SCREEN_WIDTH = WIDTH * BLOCK;
const int SCREEN_HEIGHT = HEIGHT * BLOCK;


struct Player {
  int x;
  int y;
  float a;
  float fov;
}; 

struct Impact {
  float d;
  std::string mapHit;  // + | -
  int tx;
};

class Raycaster {
public:
  Raycaster(SDL_Renderer* renderer)
    : renderer(renderer) {

    player.x = BLOCK + BLOCK / 2;
    player.y = BLOCK + BLOCK / 2;

    player.a = M_PI / 4.0f;
    player.fov = M_PI / 3.0f;

    scale = 50;
    tsize = 128;
  }

  void load_map(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (getline(file, line)) {
      map.push_back(line);
    }
    file.close();
  }

  void point(int x, int y, Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(renderer, x, y);
  }

  Impact cast_ray(float a) {
    float d = 0;
    std::string mapHit;
    int tx;

    while(true) {
      int x = static_cast<int>(player.x + d * cos(a)); 
      int y = static_cast<int>(player.y + d * sin(a)); 
      
      int i = static_cast<int>(x / BLOCK);
      int j = static_cast<int>(y / BLOCK);


      if (map[j][i] != ' ') {
        mapHit = map[j][i];

        int hitx = x - i * BLOCK;
        int hity = y - j * BLOCK;
        int maxhit;

        if (hitx == 0 || hitx == BLOCK - 1) {
          maxhit = hity;
        } else {
          maxhit = hitx;
        }

        tx = maxhit * tsize / BLOCK;

        break;
      }
      d += 1;
    }
    return Impact{d, mapHit, tx};
  }

  void draw_stake(int x, float h, Impact i) {
    float start = SCREEN_HEIGHT/2.0f - h/2.0f;
    float end = start + h;

    for (int y = start; y < end; y++) {
      int ty = (y - start) * tsize / h;
      Color c = ImageLoader::getPixelColor(i.mapHit, i.tx, ty);
      SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

      SDL_RenderDrawPoint(renderer, x, y);
    }
  }

  bool render() {
    for (int i = 0; i < SCREEN_WIDTH; i++) {
      double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
      Impact impact = cast_ray(a);
      float d = impact.d;
      Color c = Color(255, 0, 0);

      if (d < 1.0f) {
        print("you lose");
        return true;
      }
      int x = i;
      float h = static_cast<float>(SCREEN_HEIGHT)/static_cast<float>(d) * static_cast<float>(scale);
      draw_stake(x, h, impact);
    }

    // draw 2d map
    int miniMapScale = 15;
    int miniMapX = 0;
    int miniMapY = SCREEN_HEIGHT - HEIGHT * miniMapScale;

    for (int j = 0; j < HEIGHT; j++) {
      for (int i = 0; i < WIDTH; i++) {
        if (map[j][i] != ' ') {
          SDL_Rect rect = {
            miniMapX + i * miniMapScale,
            miniMapY + j * miniMapScale,
            miniMapScale,
            miniMapScale
          };
          SDL_RenderFillRect(renderer, &rect);
        }
      }
    }

    // draw player on map
    SDL_Rect rect = {
      miniMapX + player.x / BLOCK * miniMapScale,
      miniMapY + player.y / BLOCK * miniMapScale,
      miniMapScale,
      miniMapScale
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // set color to red
    SDL_RenderFillRect(renderer, &rect);

    // draw line showing where the player is looking at
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // set color to green
    SDL_RenderDrawLine(renderer, rect.x + miniMapScale / 2, rect.y + miniMapScale / 2, rect.x + miniMapScale / 2 + cos(player.a) * miniMapScale, rect.y + miniMapScale / 2 + sin(player.a) * miniMapScale);

    return false;
  }

  Player player{};
private:
  int scale;
  SDL_Renderer* renderer;
  std::vector<std::string> map;
  int tsize;
};
