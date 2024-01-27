#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;
const int INITIAL_LENGTH = 5;
const int BORDER_SIZE = 20; 

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

struct Snake {
    vector<pair<int, int>> body;
    char direction;
};

struct Food {
    int x, y;
};

struct BonusFood {
    int x, y;
    bool active;
};

struct Obstacle {
    int x, y;
};

int score = 0;
bool gameOver = false;
Snake snake;
Food food;
BonusFood bonusFood;
vector<Obstacle> obstacles;

int foodsEaten = 0;
const int BONUS_FOOD_INTERVAL = 5;

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("arial.ttf", 30);

    snake.body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    snake.direction = 'R';
     food.x = rand() % (SCREEN_WIDTH / GRID_SIZE -2) * GRID_SIZE + GRID_SIZE;
     food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE -2) * GRID_SIZE + GRID_SIZE;

   

    bonusFood.active = false; 

    obstacles.push_back({100, 100});
    obstacles.push_back({200, 200});
    obstacles.push_back({300, 300});
}

void closeWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void close() {
    closeWindow();
}

void generateFood() {
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE - 2) * GRID_SIZE + GRID_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE - 2) * GRID_SIZE + GRID_SIZE;
}

bool checkBorderCollision(pair<int, int> position) {
    
    if (position.first < 0 || position.first >= SCREEN_WIDTH -BORDER_SIZE || position.second < 0 || position.second >= SCREEN_HEIGHT - BORDER_SIZE) {
        return true;
    }

    return false;
}







bool checkCollision(pair<int, int> position) {
   
    for (const auto& segment : snake.body) {
        if (position.first == segment.first && position.second == segment.second) {
            return true;
        }
    }

  
    for (const auto& obstacle : obstacles) {
       
        if (position.first >= obstacle.x && position.first < obstacle.x + 100 &&
            position.second >= obstacle.y && position.second < obstacle.y + 20) {
            return true;
        }
    }

    return false;
}
void renderBorder() {
    SDL_SetRenderDrawColor(renderer,128,128,0,0.6); 
    SDL_Rect topBorder = {0, 0, SCREEN_WIDTH, BORDER_SIZE};
    SDL_Rect bottomBorder = {0, SCREEN_HEIGHT - BORDER_SIZE, SCREEN_WIDTH, BORDER_SIZE};
    SDL_Rect leftBorder = {0, 0, BORDER_SIZE, SCREEN_HEIGHT};
    SDL_Rect rightBorder = {SCREEN_WIDTH - BORDER_SIZE, 0, BORDER_SIZE, SCREEN_HEIGHT};

    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
}

void updateScore(int increment) {
    score += increment; 
    
}

void renderScore() {
    SDL_Color textColor = {255, 255, 255, 255};
    string scoreText = "Score: " + to_string(score);
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect scoreRect = {280, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &scoreRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void renderGameOver() {
    SDL_SetRenderDrawColor(renderer, 76, 175, 80, 0.1);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 255, 255, 255};
    string gameOverText = "Game Over! Score: " + to_string(score);
    SDL_Surface* surface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textWidth = 200;
    int textHeight = 100;

    SDL_Rect gameOverRect = {(SCREEN_WIDTH - surface->w) / 2, (SCREEN_HEIGHT - surface->h) / 2, textWidth, textHeight};
    SDL_RenderCopy(renderer, texture, NULL, &gameOverRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderObstacles() {
    SDL_SetRenderDrawColor(renderer, 233, 30, 99, 0.7);
    for (const auto& obstacle : obstacles) {
        SDL_Rect rect = {obstacle.x, obstacle.y, 100, 20};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void generateBonusFood() {
    bonusFood.x = rand() % (SCREEN_WIDTH / GRID_SIZE - 2) * GRID_SIZE + GRID_SIZE;
    bonusFood.y = rand() % (SCREEN_HEIGHT / GRID_SIZE - 2) * GRID_SIZE + GRID_SIZE;
    bonusFood.active = true;
}

void checkBonusFoodCollision() {
    if (snake.body.front().first == bonusFood.x && snake.body.front().second == bonusFood.y) {
        updateScore(20); 
        bonusFood.active = false; 
    }
}

void renderBonusFood() {
    if (bonusFood.active) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0.5); 
        SDL_Rect bonusFoodRect = {bonusFood.x, bonusFood.y, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &bonusFoodRect);
    }
}

void update() {
    if (gameOver) return;
    pair<int, int> newHead = snake.body.front();

    switch (snake.direction) {
        case 'U':
            newHead.second -= GRID_SIZE;
            break;
        case 'D':
            newHead.second += GRID_SIZE;
            break;
        case 'L':
            newHead.first -= GRID_SIZE;
            break;
            
        case 'R':
            newHead.first += GRID_SIZE;
            break;
    }

   
    if (checkCollision(newHead) || checkBorderCollision(newHead)) {
        gameOver = true;
        return;
    }

    snake.body.insert(snake.body.begin(), newHead);

    checkBonusFoodCollision();

    if (newHead.first == food.x && newHead.second == food.y) {
        generateFood();
        updateScore(10); 
        foodsEaten++;

        if (foodsEaten % BONUS_FOOD_INTERVAL == 0) {
            generateBonusFood(); 
        }

        checkBonusFoodCollision(); 
    } else {
        snake.body.pop_back();
    }

    if (newHead.first < 0 || newHead.first >= SCREEN_WIDTH-BORDER_SIZE || newHead.second < 0 || newHead.second >= SCREEN_HEIGHT - BORDER_SIZE) {
        gameOver = true;
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 76, 175, 80, 0.1);
    SDL_RenderClear(renderer);


renderBorder();


    SDL_SetRenderDrawColor(renderer, 121, 85, 72, 0.1);
    for (const auto& segment : snake.body) {
        SDL_Rect rect = {segment.first, segment.second, GRID_SIZE, GRID_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
     
   
    SDL_SetRenderDrawColor(renderer, 245, 141, 26, 0.2);
    SDL_Rect foodRect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);

    
    renderBonusFood();

    renderObstacles();

    renderScore();
    if (gameOver) {
        renderGameOver();
    }

    SDL_RenderPresent(renderer);

    if (gameOver) {
        SDL_Delay(1000); 
        closeWindow();   
    }
}

int main(int argc, char* argv[]) {
    initialize();

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (gameOver && e.key.keysym.sym == SDLK_RETURN) {
                    gameOver = false;
                    score = 0;
                    snake.body.clear();
                    snake.body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
                    snake.direction = 'R';
                    generateFood();
                    foodsEaten = 0; 
                } else {
                    switch (e.key.keysym.sym) {
                        case SDLK_UP:
                            if (snake.direction != 'D') snake.direction = 'U';
                            break;
                        case SDLK_DOWN:
                            if (snake.direction != 'U') snake.direction = 'D';
                            break;
                        case SDLK_LEFT:
                            if (snake.direction != 'R') snake.direction = 'L';
                            break;
                        case SDLK_RIGHT:
                            if (snake.direction != 'L') snake.direction = 'R';
                            break;
                    }
                }
            }
        }

        update();
        render();

        SDL_Delay(200);
    }

    close();
    return 0;
}
