#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 800;
constexpr int PLAYER_SIZE = 50;
constexpr int ENEMY_SIZE = 50;
constexpr int SPEED = 5;
constexpr int ENEMY_SPEED = 2;
constexpr int NUM_OF_ENEMIES = 5;
struct Entity
{
    SDL_Rect rect;
    int speedX, speedY;
};

bool checkCollision(const SDL_Rect &a, const SDL_Rect &b)
{
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}

void renderDeathCounter(SDL_Renderer *renderer, int deathCount)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White

    // Simple approach: render death count as a series of vertical bars
    for (int i = 0; i < deathCount; i++)
    {
        SDL_Rect deathBar = {10 + (i * 60), 10, 5, 30}; // Each "death" is represented as a rectangle
        SDL_RenderFillRect(renderer, &deathBar);
    }
}

int main()
{
    srand(time(0));
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL Game with Death Counter",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    Entity player = {{100, 250, PLAYER_SIZE, PLAYER_SIZE}, 0, 0};
    std::vector<Entity> enemies;
    int deathCount = 0; // Player death counter

    // Create NUM_OF_ENEMIES random enemies
    for (int i = 0; i < NUM_OF_ENEMIES; i++)
    {
        Entity enemy = {
            {rand() % (SCREEN_WIDTH - ENEMY_SIZE), rand() % (SCREEN_HEIGHT - ENEMY_SIZE), ENEMY_SIZE, ENEMY_SIZE},
            (rand() % 2 == 0 ? ENEMY_SPEED : -ENEMY_SPEED),
            (rand() % 2 == 0 ? ENEMY_SPEED : -ENEMY_SPEED)};
        enemies.push_back(enemy);
    }

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Event Handling
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    player.speedY = -SPEED;
                    break;
                case SDLK_DOWN:
                    player.speedY = SPEED;
                    break;
                case SDLK_LEFT:
                    player.speedX = -SPEED;
                    break;
                case SDLK_RIGHT:
                    player.speedX = SPEED;
                    break;
                }
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                    player.speedY = -SPEED;
                    break;
                case SDL_SCANCODE_S:
                    player.speedY = SPEED;
                    break;
                case SDL_SCANCODE_A:
                    player.speedX = -SPEED;
                    break;
                case SDL_SCANCODE_D:
                    player.speedX = SPEED;
                    break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                case SDLK_DOWN:
                    player.speedY = 0;
                    break;
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    player.speedX = 0;
                    break;
                }
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                    player.speedY = 0;
                    break;
                case SDL_SCANCODE_S:
                    player.speedY = 0;
                    break;
                case SDL_SCANCODE_A:
                    player.speedX = 0;
                case SDL_SCANCODE_D:
                    player.speedX = 0;
                    break;
                }
            }
        }

        // Move player
        player.rect.x += player.speedX;
        player.rect.y += player.speedY;

        // Keep player inside screen bounds
        if (player.rect.x < 0)
            player.rect.x = 0;
        if (player.rect.x > SCREEN_WIDTH - PLAYER_SIZE)
            player.rect.x = SCREEN_WIDTH - PLAYER_SIZE;
        if (player.rect.y < 0)
            player.rect.y = 0;
        if (player.rect.y > SCREEN_HEIGHT - PLAYER_SIZE)
            player.rect.y = SCREEN_HEIGHT - PLAYER_SIZE;

        // Move enemies
        for (auto &enemy : enemies)
        {
            enemy.rect.x += enemy.speedX;
            enemy.rect.y += enemy.speedY;

            // Bounce off screen edges
            if (enemy.rect.x <= 0 || enemy.rect.x >= SCREEN_WIDTH - ENEMY_SIZE)
                enemy.speedX *= -1;
            if (enemy.rect.y <= 0 || enemy.rect.y >= SCREEN_HEIGHT - ENEMY_SIZE)
                enemy.speedY *= -1;

            // Check collision with player
            if (checkCollision(player.rect, enemy.rect))
            {
                deathCount++; // Increase death counter
                std::cout << "Collision! Deaths: " << deathCount << std::endl;
                if (deathCount > 8)
                {
                    running = false;
                    break;
                }
                player.rect.x = 100;
                player.rect.y = 250;
            }
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw Player
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
        SDL_RenderFillRect(renderer, &player.rect);

        // Draw Enemies
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
        for (const auto &enemy : enemies)
        {
            SDL_RenderFillRect(renderer, &enemy.rect);
        }

        // Render Death Counter
        renderDeathCounter(renderer, deathCount);

        SDL_RenderPresent(renderer); // Show updated frame

        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
