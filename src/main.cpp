#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <print.h>

#include "color.h"
#include "imageloader.h"
#include "raycaster.h"

SDL_Window* window;
SDL_Renderer* renderer;

void clear() {
  SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255);
  SDL_RenderClear(renderer);
}

void draw_floor() {
  // floor color
  SDL_SetRenderDrawColor(renderer, 112, 122, 122, 255);
  SDL_Rect rect = {
    SCREEN_WIDTH, 
    SCREEN_HEIGHT / 2,
    SCREEN_WIDTH,
    SCREEN_HEIGHT / 2
  };
  SDL_RenderFillRect(renderer, &rect);
}

int main() {
  print("hello world");

  SDL_Init(SDL_INIT_VIDEO);
  ImageLoader::init();

  window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  ImageLoader::loadImage("+", "assets/wall3.png");
  ImageLoader::loadImage("-", "assets/wall1.png");
  ImageLoader::loadImage("|", "assets/wall2.png");
  ImageLoader::loadImage("*", "assets/wall4.png");
  ImageLoader::loadImage("g", "assets/wall5.png");

  Raycaster raycaster = {renderer };
  raycaster.load_map("assets/map.txt");

  bool running = true;
  float speed = 10.0f;

  while(running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      }
      if (event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym ){
          case SDLK_LEFT:
              raycaster.player.a += 3.14 / 24;
            break;
          case SDLK_RIGHT:
              raycaster.player.a -= 3.14 / 24;
            break;
          case SDLK_UP:
              raycaster.player.x += speed * cos(raycaster.player.a);
                raycaster.player.y += speed * sin(raycaster.player.a);
            break;
          case SDLK_DOWN:
              raycaster.player.x -= speed * cos(raycaster.player.a);
                raycaster.player.y -= speed * sin(raycaster.player.a);
            break;
           default:
            break;
        }
      }
    }

    clear();
    draw_floor();

    raycaster.render();

    // render

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
}
