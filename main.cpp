#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct WidthHeightSpeed {
  int width = 0;
  int height = 0;
  int speed = 0;
};
WidthHeightSpeed menu();

enum GameState { PAUSE, PLAY };

void computeNextGeneration(std::vector<std::vector<bool>> &grid,
                           int GridHeightGame, int GridWidthGame) {
  std::vector<std::vector<bool>> newGrid = grid;
  for (int y = 0; y < GridHeightGame; ++y) {
    for (int x = 0; x < GridWidthGame; ++x) {
      int liveNeighbors = 0;
      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          if (dy == 0 && dx == 0)
            continue;
          int ny = y + dy;
          int nx = x + dx;
          if (ny >= 0 && ny < GridHeightGame && nx >= 0 && nx < GridWidthGame) {
            liveNeighbors += grid[ny][nx];
          }
        }
      }
      if (grid[y][x]) {
        if (liveNeighbors < 2 || liveNeighbors > 3) {
          newGrid[y][x] = false;
        }
      } else {
        if (liveNeighbors == 3) {
          newGrid[y][x] = true;
        }
      }
    }
  }
  grid = newGrid;
}

void drawGrid(SDL_Renderer *rendererGame,
              const std::vector<std::vector<bool>> &grid, int GridHeightGame,
              int GridWidthGame, int CellSize) {
  SDL_SetRenderDrawColor(rendererGame, 0, 0, 0, 255);
  SDL_RenderClear(rendererGame);

  SDL_SetRenderDrawColor(rendererGame, 255, 255, 255, 255);
  for (int y = 0; y < GridHeightGame; ++y) {
    for (int x = 0; x < GridWidthGame; ++x) {
      if (grid[y][x]) {
        SDL_Rect cellRect = {x * CellSize, y * CellSize, CellSize, CellSize};
        SDL_RenderFillRect(rendererGame, &cellRect);
      }
    }
  }
  SDL_RenderPresent(rendererGame);
}

void handleMouseInput(std::vector<std::vector<bool>> &grid, int mouseX,
                      int mouseY, bool status, int GridWidthGame,
                      int GridHeightGame, int CellSize) {
  int x = mouseX / CellSize;
  int y = mouseY / CellSize;
  if (x >= 0 && x < GridWidthGame && y >= 0 && y < GridHeightGame) {
    grid[y][x] = status;
  }
}

bool game(int speed, int WindowsWidthGame, int WindowsHeightGame,
          int GridWidthGame, int GridHeightGame, int CellSize) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    abort;  
  }

  SDL_Window *windowGame = SDL_CreateWindow(
      "Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WindowsWidthGame, WindowsHeightGame, SDL_WINDOW_SHOWN);
  if (!windowGame) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    abort;  
  }

  SDL_Renderer *rendererGame =
      SDL_CreateRenderer(windowGame, -1, SDL_RENDERER_ACCELERATED);
  if (!rendererGame) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(windowGame);
    SDL_Quit();
    abort;  
  }

  std::vector<std::vector<bool>> grid(GridHeightGame,
                                      std::vector<bool>(GridWidthGame, false));
  GameState gameState = PAUSE;

  bool quit = false;
  bool mouseLeftDown = false;
  bool mouseRightDown = false;
  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_SPACE:
          gameState = (gameState == PAUSE) ? PLAY : PAUSE;
          break;
        case SDLK_r:
          SDL_DestroyRenderer(rendererGame);
          SDL_DestroyWindow(windowGame);
          SDL_Quit();
          return true;
          break;
        }
      } else if (e.type == SDL_MOUSEBUTTONDOWN && gameState == PAUSE) {
        if (e.button.button == SDL_BUTTON_LEFT) {
          mouseLeftDown = true;
          int mouseX, mouseY;
          SDL_GetMouseState(&mouseX, &mouseY);
          handleMouseInput(grid, mouseX, mouseY, 1, GridWidthGame,
                           GridHeightGame, CellSize);
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
          mouseRightDown = true;
          int mouseX, mouseY;
          SDL_GetMouseState(&mouseX, &mouseY);
          handleMouseInput(grid, mouseX, mouseY, 0, GridWidthGame,
                           GridHeightGame, CellSize);
        }
      } else if (e.type == SDL_MOUSEBUTTONUP &&
                 e.button.button == SDL_BUTTON_LEFT) {
        mouseLeftDown = false;
      } else if (e.type == SDL_MOUSEBUTTONUP &&
                 e.button.button == SDL_BUTTON_RIGHT) {
        mouseRightDown = false;
      } else if (e.type == SDL_MOUSEMOTION && mouseLeftDown &&
                 gameState == PAUSE) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        handleMouseInput(grid, mouseX, mouseY, 1, GridWidthGame, GridHeightGame,
                         CellSize);
      } else if (e.type == SDL_MOUSEMOTION && mouseRightDown &&
                 gameState == PAUSE) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        handleMouseInput(grid, mouseX, mouseY, 0, GridWidthGame, GridHeightGame,
                         CellSize);
      }
    }

    if (gameState == PLAY) {
      computeNextGeneration(grid, GridHeightGame, GridWidthGame);
    }

    drawGrid(rendererGame, grid, GridHeightGame, GridWidthGame, CellSize);
    SDL_Delay(speed);
  }

  SDL_DestroyRenderer(rendererGame);
  SDL_DestroyWindow(windowGame);
  SDL_Quit();
  return false;
}

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 400;
const int TEXTBOX_WIDTH = 150;
const int TEXTBOX_HEIGHT = 40;
const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 40;

const int sWidth = 125;
const int sHeight = 185;
const int sSpeed = 245;

void renderText(SDL_Renderer *rendererMenu, TTF_Font *font,
                const std::string &text, int x, int y) {
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(rendererMenu, surface);
  int texW = 0;
  int texH = 0;
  SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
  SDL_Rect dstrect = {x, y, texW, texH};
  SDL_RenderCopy(rendererMenu, texture, NULL, &dstrect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

bool handleEvents(SDL_Event &e, std::string &stringWidth,
                  std::string &stringHeight, std::string &stringSpeed,
                  bool &buttonPressed, SDL_Rect buttonRect, int &activeInput) {
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      return true;
    } else if (e.type == SDL_TEXTINPUT) {
      if (activeInput == 1) {
        stringWidth += e.text.text;
      } else if (activeInput == 2)
        stringHeight += e.text.text;
      else if (activeInput == 3)
        stringSpeed += e.text.text;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE) {
      if (activeInput == 1 && !stringWidth.empty())
        stringWidth.pop_back();
      else if (activeInput == 2 && !stringHeight.empty())
        stringHeight.pop_back();
      else if (activeInput == 3 && !stringSpeed.empty())
        stringSpeed.pop_back();
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
      int x, y;
      SDL_GetMouseState(&x, &y);
      if (x >= 140 && x <= 140 + TEXTBOX_WIDTH) {
        if (y >= sWidth && y <= sWidth + TEXTBOX_HEIGHT)
          activeInput = 1;
        else if (y >= sHeight && y <= sHeight + TEXTBOX_HEIGHT)
          activeInput = 2;
        else if (y >= sSpeed && y <= sSpeed + TEXTBOX_HEIGHT)
          activeInput = 3;
        else
          activeInput = 0;
      }
      if (x >= buttonRect.x  && x <= buttonRect.x + BUTTON_WIDTH &&
                 y >= buttonRect.y  &&
                 y <= buttonRect.y + BUTTON_HEIGHT) { 
        buttonPressed = true;
      } else {
        activeInput = 0;
      }
    }
  }
  return false;
}

void renderTextBox(SDL_Renderer *rendererMenu, TTF_Font *font,
                   const std::string &text, int x, int y, bool isActive) {
  SDL_Rect textBox = {x, y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT};
  if (isActive) {
    SDL_SetRenderDrawColor(rendererMenu, 0, 255, 0, 255);
  } else {
    SDL_SetRenderDrawColor(rendererMenu, 255, 255, 255, 255);
  }
  SDL_RenderDrawRect(rendererMenu, &textBox);
  renderText(rendererMenu, font, text, x + 5, y + 5);
}

WidthHeightSpeed menu() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    abort;  
  }

  if (TTF_Init() == -1) {
    std::cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError()
              << std::endl;
    abort;  
  }

  SDL_Window *windowMenu = SDL_CreateWindow(
      "Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (windowMenu == nullptr) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    abort;  
  }

  SDL_Renderer *rendererMenu =
      SDL_CreateRenderer(windowMenu, -1, SDL_RENDERER_ACCELERATED);
  if (rendererMenu == nullptr) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    abort;  
  }

  TTF_Font *font = TTF_OpenFont("Hack-Regular.ttf", 24);
  TTF_Font *fontRules = TTF_OpenFont("Hack-Regular.ttf", 15);
  if (font == nullptr) {
    std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError()
              << std::endl;
    abort;  
  }

  bool quit = false;
  SDL_Event e;
  std::string stringWidth("1000"), stringHeight("700"), stringSpeed("20");

  bool buttonPressed = false;
  WidthHeightSpeed temp;
  int activeInput = 0;

  SDL_StartTextInput();

  SDL_Rect buttonRect = {WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
                         WINDOW_HEIGHT - 60, BUTTON_WIDTH, BUTTON_HEIGHT};
  
  while (!quit) {
    quit = handleEvents(e, stringWidth, stringHeight, stringSpeed,
                        buttonPressed, buttonRect, activeInput);

    SDL_SetRenderDrawColor(rendererMenu, 0, 0, 0, 255);
    SDL_RenderClear(rendererMenu);

    renderText(rendererMenu, font, "Welcome to \"Game of Life\" ",
               WINDOW_WIDTH / 2 - 150, 10);
    renderText(rendererMenu, font, "Enter settings:", 10, 70);
    renderText(rendererMenu, font, "Width:", 10, 130);
    renderText(rendererMenu, font, "Height:", 10, 190);
    renderText(rendererMenu, font, "Speed:", 10, 250);

    renderText(rendererMenu, fontRules, "          Control:", 350, 150);
    renderText(rendererMenu, fontRules, "         LMB - Life", 350, 170);
    renderText(rendererMenu, fontRules, "         RMB - Death", 350, 190);
    renderText(rendererMenu, fontRules, "Space button - Play/Pause", 350, 210);
    renderText(rendererMenu, fontRules, "    r button - Restart", 350, 230);

    renderTextBox(rendererMenu, font, stringWidth, 140, sWidth,
                  activeInput == 1);
    renderTextBox(rendererMenu, font, stringHeight, 140, sHeight,
                  activeInput == 2);
    renderTextBox(rendererMenu, font, stringSpeed, 140, sSpeed,
                  activeInput == 3);

    SDL_SetRenderDrawColor(rendererMenu, 255, 255, 255, 255);
    renderText(rendererMenu, font, "Apply", buttonRect.x + 10,
               buttonRect.y + 10);

    SDL_RenderDrawRect(rendererMenu, &buttonRect);

    SDL_RenderPresent(rendererMenu);

    if (buttonPressed) {
      temp.width = std::stoi(stringWidth);
      temp.height = std::stoi(stringHeight);
      temp.speed = std::stoi(stringSpeed);
      quit = true;
      SDL_StopTextInput();
      TTF_CloseFont(font);
      SDL_DestroyRenderer(rendererMenu);
      SDL_DestroyWindow(windowMenu);
      font = nullptr;
      rendererMenu = nullptr;
      windowMenu = nullptr;
      TTF_Quit();
      SDL_Quit();
      return temp;
    }
  }
  SDL_StopTextInput();
  TTF_CloseFont(font);
  SDL_DestroyRenderer(rendererMenu);
  SDL_DestroyWindow(windowMenu);
  font = nullptr;
  rendererMenu = nullptr;
  windowMenu = nullptr;
  TTF_Quit();
  SDL_Quit();
  return temp;
}
int main(int argc, char *args[]) {
  WidthHeightSpeed wHS;
  bool quitMain(true);

  while (quitMain) {
    wHS = menu();
    if (wHS.height > 0 && wHS.width > 0 && wHS.speed > 0)
      quitMain = game(wHS.speed, wHS.width, wHS.height, wHS.width / 10,
                      wHS.height / 10, 10);
    else
      break;
  }
  return 0;
}