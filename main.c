/*  Jumping into paintings is still cool!
    TO DO:
        Calculate map cells window position and area limits.
        Adjust player position offset.
        Collision detection between player and road.
        Get map size dinamically. */

#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_W 1024
#define SCREEN_H 512
#define CELL_SIZE 64

int not_quit = 1;

SDL_Window* win = NULL;
SDL_Renderer* rend = NULL;

typedef struct {
    int x;
    int y;
}Pos;

typedef struct {
    int grass;
    int grassImg[256];
    int road;
    int roadImg[256];
    int water;
    int waterImg[256];
    int playerPos;
    int playerPosImg[256];
    int endPos;
    int endPosImg[256];
}Tile;

typedef struct {
    Pos mapSize;
    int mapLength;
    int spawnX;
    int spawnY;
    int* map;
    Tile tile;
}Level; Level level;

typedef struct {
    SDL_Rect rect;
    int size;
    int stepSize;
}Player; Player p;

typedef struct {
    SDL_Rect item[4];
    int size;
    int itemImg[256];
}Item; Item items;

int map[] = {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 2, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 2, 0, 0, 0
};

/*  Level Startup */
void init();

/*  Main Loop */
void events();
void update();
void display();

void tileDraw(int x, int y, int tile);

int main(void) {

    init();

    while (not_quit)
    {
        events();
        update();
        display();
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

void init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &win, &rend);
    SDL_SetWindowTitle(win, "2DGame");

    // Level
    level.tile.grass = 0;
    level.tile.road = 1;
    level.tile.water = 2;
    level.tile.playerPos = 3;
    level.tile.endPos = 4;
    level.mapSize.x = 16; level.mapSize.y = 8;
    
    // Tile Images
    int grassImg[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
                0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
                0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    int roadImg[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    int waterImg[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0,
                0, 0, 1, 0, 2, 0, 0, 1, 0, 1, 0, 0, 0, 2, 0, 0,
                0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 1, 0, 0, 0, 0, 0,
                0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 1,
                0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0,
                0, 0, 0, 1, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 1,
                0, 0, 2, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 2, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 1, 0, 1, 0, 2, 0,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    int playerPosImg[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0,
                0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0,
                0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0,
                0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0,
                0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
                0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0,
                0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
                0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int cell = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            level.tile.grassImg[cell] = grassImg[cell];
            level.tile.roadImg[cell] = roadImg[cell];
            level.tile.waterImg[cell] = waterImg[cell];
            level.tile.playerPosImg[cell] = playerPosImg[cell];
            cell++;
        }
    }

    /*  load map from file
        get mapX, mapY, mapLength
        set spawnX, spawnY
        calculate cells real pos,
        edge limits and center pos */

    // Player
    p.rect.x = 0; p.rect.y = 0;
    p.size = CELL_SIZE;
    p.rect.w = p.size; p.rect.h = p.size;
    p.stepSize = CELL_SIZE;

    // Map
    cell = 0;
    for (int i = 0; i < level.mapSize.y; i++) {
        for (int j = 0; j < level.mapSize.x; j++) {
            if (map[cell] == level.tile.playerPos) {
                level.spawnX = j * CELL_SIZE;
                level.spawnY = i * CELL_SIZE;
            }
            cell++;
        }
    }
    p.rect.x = level.spawnX; p.rect.y = level.spawnY;

    // Items
    int itemImg[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    items.size = 4;
    for (int i = 0; i < items.size; i++) {
        items.item[i].w = CELL_SIZE; items.item[i].h = CELL_SIZE;
        items.item[i].x = i * CELL_SIZE * 2 + (CELL_SIZE * 5); items.item[i].y = 4 * CELL_SIZE;
    }

    cell = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            items.itemImg[cell] = itemImg[cell];
            cell++;
        }
    }
}

void events(){
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            not_quit = 0;
            break;
        case SDL_KEYDOWN:
            switch (ev.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                not_quit = 0;
                break;
            case SDL_SCANCODE_W:
                p.rect.y -= p.stepSize;
                break;
            case SDL_SCANCODE_S:
                p.rect.y += p.stepSize;
                break;
            case SDL_SCANCODE_A:
                p.rect.x -= p.stepSize;
                break;
            case SDL_SCANCODE_D:
                p.rect.x += p.stepSize;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void update() {
    // Player
    if (p.rect.y > SCREEN_H - p.size) { p.rect.y = SCREEN_H - CELL_SIZE; }
    if (p.rect.y < 0) { p.rect.y = 0; }
    if (p.rect.x > SCREEN_W - p.size) { p.rect.x = SCREEN_W - CELL_SIZE; }
    if (p.rect.x < 0) { p.rect.x = 0; }

    // Pick up Items
    for (int i = 0; i < items.size; i++) {
        if (p.rect.x == items.item[i].x && p.rect.y == items.item[i].y) {
            items.item[i].x = -CELL_SIZE; items.item[i].y = -CELL_SIZE;
        }
    }
}

void display() {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);

    // Map
    int cell = 0;
    for (int i = 0; i < level.mapSize.y; i++) {
        for (int j = 0; j < level.mapSize.x; j++) {
            if (map[cell] == level.tile.grass) {
                tileDraw(j * CELL_SIZE, i * CELL_SIZE, level.tile.grass);
            }
            else if (map[cell] == level.tile.road) {
                tileDraw(j * CELL_SIZE, i * CELL_SIZE, level.tile.road);
            }
            else if (map[cell] == level.tile.water) {
                tileDraw(j * CELL_SIZE, i * CELL_SIZE, level.tile.water);
            }
            else if (map[cell] == level.tile.playerPos) {
                tileDraw(j * CELL_SIZE, i * CELL_SIZE, level.tile.playerPos);
            }
            cell++;
        }
    }
    
    // Items
    SDL_Rect rect = { 0, 0, 4, 4 };
    cell = 0;
    SDL_SetRenderDrawColor(rend, 0, 50, 255, 255);
    for (int x = 0; x < items.size; x++) {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                if (items.itemImg[cell] == 1) {
                    rect.x = j * 4 + items.item[x].x; rect.y = i * 4 + items.item[x].y;
                    SDL_RenderFillRect(rend, &rect);
                }
                cell++;
            }
        }
        cell = 0;
    }

    // Player
    SDL_SetRenderDrawColor(rend, 200, 125, 0, 255);
    SDL_RenderFillRect(rend, &p.rect);

    SDL_RenderPresent(rend);
}

void tileDraw(int x, int y, int tile) {
    SDL_Rect rect = { 0, 0, 4, 4 };
    int cell = 0;
    switch (tile){
        case 0:
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    if (level.tile.grassImg[cell] == 0) {
                        SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
                    }
                    if (level.tile.grassImg[cell] == 1) {
                        SDL_SetRenderDrawColor(rend, 0, 150, 0, 255);
                    }
                    rect.x = j * 4 + x; rect.y = i * 4 + y;
                    SDL_RenderFillRect(rend, &rect);
                    cell++;
                }
            }
            break;
        case 1:
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    if (level.tile.roadImg[cell] == 0) {
                        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
                    }
                    if (level.tile.roadImg[cell] == 1) {
                        SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
                    }
                    rect.x = j * 4 + x; rect.y = i * 4 + y;
                    SDL_RenderFillRect(rend, &rect);
                    cell++;
                }
            }
            break;
        case 2:
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    if (level.tile.waterImg[cell] == 0) {
                        SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
                    }
                    if (level.tile.waterImg[cell] == 1) {
                        SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
                    }
                    if (level.tile.waterImg[cell] == 2) {
                        SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
                    }
                    rect.x = j * 4 + x; rect.y = i * 4 + y;
                    SDL_RenderFillRect(rend, &rect);
                    cell++;
                }
            }
            break;
        case 3:
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    if (level.tile.playerPosImg[cell] == 0) {
                        SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);
                    }
                    if (level.tile.playerPosImg[cell] == 1) {
                        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
                    }
                    rect.x = j * 4 + x; rect.y = i * 4 + y;
                    SDL_RenderFillRect(rend, &rect);
                    cell++;
                }
            }
            break;
        default:
            break;
    }
}