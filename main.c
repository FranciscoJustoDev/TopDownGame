/*  Jumping into paintings is still cool!
    TO DO:
        Calculate map cells window position and area limits.
        Adjust player position offset.
        Collision detection between player and road.
        Get map size dinamically. */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define CELL_SIZE 64
#define WIDTH CELL_SIZE * 16
#define HEIGHT CELL_SIZE * 9
#define TILE_STEP CELL_SIZE / 16
#define TILE_SIZE 256

#define GRASS 0
#define ROAD 1
#define WATER 2

#define PLAYER 1

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
    Pos size;           // Same W,H for all maps
    int *layout;        // draw order: 1st
    int *objects;       // 2nd
    int *items;         // 3rd
    int *creatures;     // 4th
    int *events;        // 5th
}Map;

typedef struct {
    int type;
    Pos pos;
}Item;

typedef struct {
    int itemCount;
    int tileCount;
    Item *items;
    int **tiles;
}ItemList;

typedef struct {
    Map map;
    Pos playerSpawn;
    TileMap tileMap;
    ItemList itemList;
}Level; Level level;

typedef struct {
    Pos pos;            // grid position
    SDL_Rect rect;      // pixel position
    int size;
    int stepSize;
}Player; Player p;

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

    // TileMap
    level.tileMap.size = 3;
    level.tileMap.tiles = malloc(sizeof(int *) * level.tileMap.size);
    for(int t = 0; t < level.tileMap.size; t++){
        level.tileMap.tiles[t] = malloc(sizeof(int) * TILE_SIZE);
    }

    int cell = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            level.tileMap.tiles[0][cell] = grassImg[cell];
            level.tileMap.tiles[1][cell] = roadImg[cell];
            level.tileMap.tiles[2][cell] = waterImg[cell];
            cell++;
        }
    }

    /*  load map from file
        get mapX, mapY, mapLength
        set spawnX, spawnY
        calculate cells real pos,
        edge limits and center pos */

    // Maps
    int layoutMap[] = {
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 2, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 2, 0, 0, 0
    };

    int objectMap[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int itemMap[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int creatureMap[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int eventMap[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    level.map.size.x = 16; level.map.size.y = 8;
    level.map.layout = malloc(sizeof(int) * (level.map.size.x * level.map.size.y));
    level.map.objects = malloc(sizeof(int) * (level.map.size.x * level.map.size.y));
    level.map.items = malloc(sizeof(int) * (level.map.size.x * level.map.size.y));
    level.map.creatures = malloc(sizeof(int) * (level.map.size.x * level.map.size.y));
    level.map.events = malloc(sizeof(int) * (level.map.size.x * level.map.size.y));

    cell = 0;
    for (int i = 0; i < level.map.size.y; i++) {
        for (int j = 0; j < level.map.size.x; j++) {
            level.map.layout[cell] = layoutMap[cell];
            level.map.objects[cell] = objectMap[cell];
            level.map.items[cell] = itemMap[cell];
            level.map.creatures[cell] = creatureMap[cell];
            level.map.events[cell] = eventMap[cell];

            if (level.map.events[cell] == PLAYER) {
                level.playerSpawn.x = j;
                level.playerSpawn.y = i;
            }
            cell++;
        }
    }

    // Player
    p.pos.x = level.playerSpawn.x; p.pos.y = level.playerSpawn.y;
    p.rect.x = p.pos.x * CELL_SIZE; p.rect.y = p.pos.y * CELL_SIZE;
    p.size = CELL_SIZE;
    p.rect.w = p.size; p.rect.h = p.size;
    p.stepSize = CELL_SIZE;

    // Items
    int itemImg[] = {//     7 transparent / 4 blue
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 4, 4, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 4, 4, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
    };

    // Load item sprites
    level.itemList.tileCount = 1;
    level.itemList.tiles = malloc(sizeof(int *) * level.itemList.tileCount);
    for(int i = 0; i < level.itemList.tileCount; i++){
        level.itemList.tiles[i] = malloc(sizeof(int) * TILE_SIZE);
    }

    cell = 0;
    for(int itm = 0; itm < level.itemList.tileCount; itm++){
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                level.itemList.tiles[itm][cell] = itemImg[cell];
                cell++;
            }
        }
    }

    //  Count items, set size of item array
    cell = 0;
    level.itemList.itemCount = 0;
    for (int i = 0; i < level.map.size.y; i++) {
        for (int j = 0; j < level.map.size.x; j++) {
            if(level.map.items[cell] != 0){
                level.itemList.itemCount++;
            }
            cell++;
        }
    }
    
    if(level.itemList.itemCount == 0){
        // Allocate memory for at least 1 item
        level.itemList.items = malloc(sizeof(Item) * 1);
    }else{
        // Allocate memory for total of items
        level.itemList.items = malloc(sizeof(Item) * level.itemList.itemCount);

        cell = 0;
        int itm = 0;
        for (int i = 0; i < level.map.size.y; i++) {
            for (int j = 0; j < level.map.size.x; j++) {
                if(level.map.items[cell] != 0){
                    level.itemList.items[itm].type = level.map.items[cell];
                    level.itemList.items[itm].pos.y = i;
                    level.itemList.items[itm].pos.x = j;
                    itm++;
                }
                cell++;
            }
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
                p.pos.y -= 1;
                break;
            case SDL_SCANCODE_S:
                p.pos.y += 1;
                break;
            case SDL_SCANCODE_A:
                p.pos.x -= 1;
                break;
            case SDL_SCANCODE_D:
                p.pos.x += 1;
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
    if (p.pos.y > level.map.size.y - 1) { p.pos.y = level.map.size.y - 1; }
    if (p.rect.y < 0) { p.pos.y = 0; }
    if (p.pos.x > level.map.size.x - 1) { p.pos.x = level.map.size.x - 1; }
    if (p.pos.x < 0) { p.pos.x = 0; }

    p.rect.x = p.pos.x * CELL_SIZE;
    p.rect.y = p.pos.y * CELL_SIZE;

    // Pick up Items
    for (int i = 0; i < level.itemList.itemCount; i++) {
        if (p.pos.x == level.itemList.items[i].pos.x && p.pos.y == level.itemList.items[i].pos.y) {
            level.itemList.items[i].pos.x = -1; level.itemList.items[i].pos.y = -1;
        }
    }
}

void display() {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);

    // Map
    int cell = 0;
    for (int i = 0; i < level.map.size.y; i++) {
        for (int j = 0; j < level.map.size.x; j++) {
            tileDraw(j * CELL_SIZE, i * CELL_SIZE, level.map.layout[cell]);
            cell++;
        }
    }
    
    // Items
    SDL_Rect rect = { 0, 0, TILE_STEP, TILE_STEP};
    cell = 0;
    SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);   // blue
    for (int itm = 0; itm < level.itemList.itemCount; itm++) {
        if(level.itemList.items[itm].type == 1){
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    if (level.itemList.tiles[0][cell] == 4) {
                        rect.x = j * TILE_STEP + level.itemList.items[itm].pos.x * CELL_SIZE;
                        rect.y = i * TILE_STEP + level.itemList.items[itm].pos.y * CELL_SIZE;
                        SDL_RenderFillRect(rend, &rect);
                    }
                    cell++;
                }
            }
        }
        cell = 0;
    }

    // Creatures
        // go here ...

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
                case 7://  transparent
                    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
                    break;
            }
            rect.x = j * TILE_STEP + x; rect.y = i * TILE_STEP + y;
            SDL_RenderFillRect(rend, &rect);
            cell++;
        }
    }
}