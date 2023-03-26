/*  Jumping into paintings is still cool!
    TO DO:
        Calculate map cells window position and area limits.
        Adjust player position offset.
        Collision detection between player and road.
        Get map size dinamically. */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define CELL_SIZE 32
#define WIDTH CELL_SIZE * 16
#define HEIGHT CELL_SIZE * 9
#define TILE_STEP CELL_SIZE / 16
#define TILE_SIZE 256

#define GRASS 0
#define ROAD 1
#define WATER 2
#define START_POS 3

int not_quit = 1;

SDL_Window* win = NULL;
SDL_Renderer* rend = NULL;

typedef struct {
    int x;
    int y;
}Pos;

typedef struct {
    int size;
    int **tiles;
}TileMap;

typedef struct {
    Pos mapSize;
    Pos spawn;
    int* map;
    TileMap tileMap;
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
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &win, &rend);
    SDL_SetWindowTitle(win, "2DGame");

    // Level

    // TileMap
    level.tileMap.size = 4;
    level.tileMap.tiles = malloc(sizeof(int *) * level.tileMap.size);
    for(int t = 0; t < level.tileMap.size; t++){
        level.tileMap.tiles[t] = malloc(sizeof(int) * TILE_SIZE);
    }

    int grassImg[] = {//    0 green/ 1 dark green
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
    int roadImg[] = {// 2 black/ 3 yellow
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    };
    int waterImg[] = {//    4 blue/ 0 green/ 5 red
                4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                4, 0, 4, 0, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4,
                4, 4, 0, 4, 5, 4, 4, 0, 4, 0, 4, 4, 4, 5, 4, 4,
                4, 4, 4, 4, 4, 4, 5, 4, 0, 4, 0, 4, 4, 4, 4, 4,
                4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4,
                4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 0, 4, 0,
                4, 4, 0, 4, 0, 4, 4, 4, 4, 4, 4, 5, 4, 4, 0, 4,
                4, 4, 4, 0, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4,
                4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 5, 0,
                4, 4, 5, 4, 4, 0, 5, 0, 4, 4, 4, 4, 4, 0, 4, 4,
                4, 4, 4, 4, 4, 4, 0, 4, 4, 5, 4, 4, 4, 5, 4, 4,
                4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                4, 4, 4, 5, 4, 5, 4, 4, 4, 4, 0, 4, 0, 4, 5, 4,
                4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0, 4, 0, 4, 4,
                4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
    };
    int playerPosImg[] = {//    6 grey/ 2 black
                6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                6, 2, 2, 6, 2, 2, 2, 6, 6, 6, 6, 6, 6, 2, 2, 2,
                6, 2, 6, 6, 6, 2, 6, 6, 2, 6, 6, 2, 2, 6, 2, 6,
                6, 2, 2, 6, 6, 2, 6, 2, 6, 2, 6, 2, 6, 6, 2, 6,
                6, 6, 2, 6, 6, 2, 6, 2, 2, 2, 6, 2, 6, 6, 2, 6,
                6, 2, 2, 6, 6, 2, 6, 2, 6, 2, 6, 2, 6, 6, 2, 6,
                6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                6, 2, 2, 6, 6, 6, 2, 6, 6, 2, 2, 6, 6, 6, 6, 6,
                6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 6, 6, 6, 6, 6,
                6, 2, 2, 6, 6, 2, 6, 2, 6, 2, 2, 6, 6, 6, 6, 6,
                6, 2, 6, 6, 6, 2, 6, 2, 6, 6, 2, 6, 6, 6, 6, 6,
                6, 2, 6, 6, 6, 6, 2, 6, 6, 2, 2, 6, 6, 6, 6, 6,
                6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
    };

    int cell = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            level.tileMap.tiles[0][cell] = grassImg[cell];
            level.tileMap.tiles[1][cell] = roadImg[cell];
            level.tileMap.tiles[2][cell] = waterImg[cell];
            level.tileMap.tiles[3][cell] = playerPosImg[cell];
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

    level.mapSize.x = 16; level.mapSize.y = 8;
    level.map = malloc(sizeof(int) * (level.mapSize.x * level.mapSize.y));

    cell = 0;
    for (int i = 0; i < level.mapSize.y; i++) {
        for (int j = 0; j < level.mapSize.x; j++) {
            level.map[cell] = map[cell];
            if (level.map[cell] == START_POS) {
                level.spawn.x = j * CELL_SIZE;
                level.spawn.y = i * CELL_SIZE;
            }
            cell++;
        }
    }
    p.rect.x = level.spawn.x; p.rect.y = level.spawn.y;

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

    //  Setting position manually
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
    if (p.rect.y > HEIGHT - p.size) { p.rect.y = HEIGHT - CELL_SIZE; }
    if (p.rect.y < 0) { p.rect.y = 0; }
    if (p.rect.x > WIDTH - p.size) { p.rect.x = WIDTH - CELL_SIZE; }
    if (p.rect.x < 0) { p.rect.x = 0; }

    // Pick up Items
    for (int i = 0; i < items.size; i++) {
        if (p.rect.x == items.item[i].x && p.rect.y == items.item[i].y) {
            items.item[i].x = -CELL_SIZE; items.item[i].y = -CELL_SIZE;
        }
    }
}

void display() {
    SDL_SetRenderDrawColor(rend, 110, 110, 255, 255);
    SDL_RenderClear(rend);

    // Map
    int cell = 0;
    for (int i = 0; i < level.mapSize.y; i++) {
        for (int j = 0; j < level.mapSize.x; j++) {
            tileDraw(j * CELL_SIZE, i * CELL_SIZE, level.map[cell]);
            cell++;
        }
    }
    
    // Items
    SDL_Rect rect = { 0, 0, TILE_STEP, TILE_STEP};
    cell = 0;
    SDL_SetRenderDrawColor(rend, 0, 50, 255, 255);
    for (int x = 0; x < items.size; x++) {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                if (items.itemImg[cell] == 1) {
                    rect.x = j * TILE_STEP + items.item[x].x; rect.y = i * TILE_STEP + items.item[x].y;
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
    SDL_Rect rect = { 0, 0, TILE_STEP, TILE_STEP};
    int cell = 0;

    //  Assign color and draw

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            switch (level.tileMap.tiles[tile][cell]) {
                case 0://   green
                    SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
                    break;
                case 1://   dark green
                    SDL_SetRenderDrawColor(rend, 0, 150, 0, 255);
                    break;
                case 2://   black
                    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
                    break;
                case 3://   yellow
                    SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
                    break;
                case 4://   blue
                    SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
                    break;
                case 5://   red
                    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
                    break;
                case 6://   grey
                    SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);
                    break;
                default://  transparent
                    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
                    break;
            }
            rect.x = j * TILE_STEP + x; rect.y = i * TILE_STEP + y;
            SDL_RenderFillRect(rend, &rect);
            cell++;
        }
    }
}