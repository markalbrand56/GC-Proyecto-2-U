#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL_mixer.h>
#include <iostream>

#include "color.h"
#include "imageloader.h"
#include "raycaster.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* welcomeTexture;
Mix_Music* backgroundMusic;
Mix_Chunk* footstepSound;

void clear() {
  SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255);
  SDL_RenderClear(renderer);
}

void draw_floor() {
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
  if (!surface) {
    std::cerr << "Error al cargar la textura desde " << imagePath << ": " << SDL_GetError() << std::endl;
    return nullptr;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

void renderWelcomeScreen() {
  clear();
  SDL_Rect welcomeRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_RenderCopy(renderer, welcomeTexture, NULL, &welcomeRect);
  SDL_RenderPresent(renderer);
}

void load_map(int id){
  switch (id)
  {
  case 1: // Map 1
    ImageLoader::loadImage("+", "assets/map1/wall3.png");
    ImageLoader::loadImage("-", "assets/map1/wall1.png");
    ImageLoader::loadImage("|", "assets/map1/wall2.png");
    ImageLoader::loadImage("*", "assets/map1/wall4.png");
    ImageLoader::loadImage("g", "assets/map1/wall5.png");
    break;
  
  case 2: // Map 2
    ImageLoader::loadImage("+", "assets/map2/wall1.png");
    ImageLoader::loadImage("-", "assets/map2/wall2.png");
    ImageLoader::loadImage("|", "assets/map2/wall5.png");
    ImageLoader::loadImage("*", "assets/map2/wall4.png");
    ImageLoader::loadImage("g", "assets/map2/wall3.png");
    break;
  
  default: // Default map
    ImageLoader::loadImage("+", "assets/map1/wall3.png");
    ImageLoader::loadImage("-", "assets/map1/wall1.png");
    ImageLoader::loadImage("|", "assets/map1/wall2.png");
    ImageLoader::loadImage("*", "assets/map1/wall4.png");
    ImageLoader::loadImage("g", "assets/map1/wall5.png");
    break;
  }
}

void cleanup() {
  Mix_FreeMusic(backgroundMusic);
  Mix_FreeChunk(footstepSound);
  Mix_CloseAudio();
  SDL_DestroyTexture(welcomeTexture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main() {
  print("Starting game");

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
    return 1;
  }

  ImageLoader::init();

  window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Error al crear la ventana: " << SDL_GetError() << std::endl;
    cleanup();
    return 1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Error al crear el renderizador: " << SDL_GetError() << std::endl;
    cleanup();
    return 1;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Error al inicializar SDL_mixer: " << Mix_GetError() << std::endl;
    cleanup();
    return 1;
  }

  backgroundMusic = Mix_LoadMUS("assets/Density & Time - MAZE.mp3");
  if (!backgroundMusic) {
    std::cerr << "Error al cargar la música: " << Mix_GetError() << std::endl;
    cleanup();
    return 1;
  }

  footstepSound = Mix_LoadWAV("assets/sounds/walking.wav");
  if (!footstepSound) {
    std::cerr << "Error al cargar el efecto de sonido: " << Mix_GetError() << std::endl;
    cleanup();
    return 1;
  }

  Mix_PlayMusic(backgroundMusic, -1);

  ImageLoader::loadImage("+", "assets/map1/wall3.png");
  ImageLoader::loadImage("-", "assets/map1/wall1.png");
  ImageLoader::loadImage("|", "assets/map1/wall2.png");
  ImageLoader::loadImage("*", "assets/map1/wall4.png");
  ImageLoader::loadImage("g", "assets/map1/wall5.png");

  Raycaster raycaster = {renderer };
  raycaster.load_map("assets/map.txt");

  welcomeTexture = loadTexture("assets/welcome.bmp");
  if (!welcomeTexture) {
    cleanup();
    return 1;
  }

  bool gameStarted = false;
  float speed = 10.0f;
  int selectedMap = 1;

  Uint32 frameStart, frameTime;

  while (!gameStarted) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        gameStarted = true;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE) {
          gameStarted = true;
        } else if (event.key.keysym.sym == SDLK_1) {
          selectedMap = 1;
          load_map(selectedMap);
          std::cout << "Map 1 loaded" << std::endl;
        } else if (event.key.keysym.sym == SDLK_2) {
          selectedMap = 2;
          load_map(selectedMap);
          std::cout << "Map 2 loaded" << std::endl;
        }
      }
    }

    renderWelcomeScreen();
  }

  bool walkingForward = false;
  bool walkingBackward = false;
  bool mouseActive = false;

  bool running = true;
  while (running) {
    frameStart = SDL_GetTicks();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;        
        break;
      }

      if (event.type == SDL_MOUSEMOTION && mouseActive) {
        raycaster.player.a += event.motion.xrel * 0.005;
      }

      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            running = false;
            break;
          case SDLK_m:
            mouseActive = !mouseActive;
            break;
          case SDLK_LEFT:
            raycaster.player.a += 3.14 / 24;
            break;
          case SDLK_RIGHT:
            raycaster.player.a -= 3.14 / 24;
            break;
          case SDLK_UP:
            raycaster.player.x += speed * cos(raycaster.player.a);
            raycaster.player.y += speed * sin(raycaster.player.a);

            if(!walkingForward) {
              Mix_PlayChannel(-1, footstepSound, -1);  // Reproduce el sonido en bucle
            }

            walkingForward = true;
            break;
          case SDLK_DOWN:
            raycaster.player.x -= speed * cos(raycaster.player.a);
            raycaster.player.y -= speed * sin(raycaster.player.a);

            if(!walkingBackward) {
              Mix_PlayChannel(-1, footstepSound, -1);  // Reproduce el sonido en bucle
            }

            walkingBackward = true;
            break;
          default:
            break;
        }
      }
      

      if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
          case SDLK_UP:
            walkingForward = false;
            Mix_HaltChannel(-1);  // Detiene todos los canales
            break;
          case SDLK_DOWN:
            walkingBackward = false;
            Mix_HaltChannel(-1);  // Detiene todos los canales
            break;
          default:
            break;
        }
      }
    }

    clear();
    draw_floor();

    raycaster.render();

    SDL_RenderPresent(renderer);
    
    frameTime = SDL_GetTicks() - frameStart;
    char title[32];
    sprintf(title, "DOOM | FPS: %d", (int)(1000.0f / frameTime));
    SDL_SetWindowTitle(window, title);
  }

  cleanup();
  return 0;
}
