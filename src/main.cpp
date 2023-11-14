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
SDL_Texture* welcomeTexture;

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

SDL_Texture* loadTexture(const char* imagePath) {
  SDL_Surface* surface = SDL_LoadBMP(imagePath);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

void renderWelcomeScreen() {
  clear();

  // Render welcome image
  SDL_Rect welcomeRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_RenderCopy(renderer, welcomeTexture, NULL, &welcomeRect);

  SDL_RenderPresent(renderer);
}

int main() {
  print("Starting game");

  SDL_Init(SDL_INIT_VIDEO);
  ImageLoader::init();

  window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  ImageLoader::loadImage("+", "assets/map1/wall3.png");
  ImageLoader::loadImage("-", "assets/map1/wall1.png");
  ImageLoader::loadImage("|", "assets/map1/wall2.png");
  ImageLoader::loadImage("*", "assets/map1/wall4.png");
  ImageLoader::loadImage("g", "assets/map1/wall5.png");

  Raycaster raycaster = {renderer };
  raycaster.load_map("assets/map.txt");

  // Load welcome image
  welcomeTexture = loadTexture("assets/welcome.bmp");

  bool gameStarted = false;
  float speed = 10.0f;

  while (!gameStarted) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
      }
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
        gameStarted = true;
      }
    }

    renderWelcomeScreen();
  }

  // Game loop
  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      }
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
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

  SDL_DestroyTexture(welcomeTexture);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
