/* See LICENSE for licence details. */
#ifndef RATIUM_H
#define RATIUM_H

/* settings for ratium */
#define DEF_PLAYERS 1 /* default number of players */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

/* max constants */
#define MAX_X 48
#define MAX_Y 32
#define MAX_NAME 16
#define MAX_PLAYERS 8
#define MAX_ENTITIES 256
#define MAX_ITEMS 256
#define MAX_INV 16

#define U 16
#define FONT_W 20
#define FONT_H 40

#undef false /* damn ncurses.... */
#undef true
#undef bool
typedef enum { false, true } bool;

/* direction entities can face */
typedef enum {
	LEFT,
	DOWN,
	UP,
	RIGHT,
	LEFTDOWN,
	LEFTUP,
	RIGHTDOWN,
	RIGHTUP
} DIREC;

typedef enum {
	ITEM_MISC,
	ITEM_FOOD,
	ITEM_SWORD,
	ITEM_SHIELD,
	ITEM_SHOOTER,
	ITEM_AMMO
} ITEM_TYPE;

/* where the entity can spawn */
typedef enum {
	TYPE_ALL,
	TYPE_CAVE,
	TYPE_GRASS,
	TYPE_WATER
} ENT_TYPE;

/* type of entity ai, or if entity is a player */
typedef enum {
	AI_PLAYER,
	AI_HOSTILE,
	AI_PEACEFUL
} ENT_AI;

typedef struct {
	char *name;
	char face;

	SDL_Texture *img;
	SDL_Rect src; /* ent position on sprite sheet */
	/* SDL_Rect dst; /1* ent position *1/ */

	int stat;  /* universal variable for damage of sword, heath healed for food, etc. */
} Block;

typedef struct {
	char *name;
	ITEM_TYPE type;
	int map[MAX_Y][MAX_X]; /* position and count of item in world */
	char face; /* char that gets displayed */

	SDL_Texture *img;
	SDL_Rect src; /* ent position on sprite sheet */
	/* SDL_Rect dst; /1* ent position *1/ */

	int stat;  /* universal variable for damage of sword, heath healed for food, etc. */
} Item;

/* player keys */
typedef struct _Keys Keys;
struct _Keys {
	Uint8 left, down, up, right;
	Uint8 act, drop, inv;
};

/* different armor slots that entities could have items in */
typedef struct _Armor Armor;
struct _Armor {
	/* int which points to ent inventory slot */
	int body, head, chest, hands, legs, feet;
};

/* universal struct for players and other entities */
typedef struct _Ent Ent;
struct _Ent {
	char *name;    /* name of ent */
	ENT_TYPE type; /* used mainly for where to spawn */
	ENT_AI ai;     /* how ent should move around */

	SDL_Texture *img;
	SDL_Rect src; /* ent position on sprite sheet */
	/* SDL_Rect dst; /1* ent position *1/ */
	SDL_RendererFlip flip;

	DIREC direc; /* direction ent is facing */
	int x, y;    /* ent position */
	int bary;    /* position of stat bar, mainly for player */

	int maxhp, hp; /* ent heath and max heath */
	bool isdead;   /* set to true after entity death stuff it run */
	int damage;    /* how much damage entity deals to others */
	int sight;     /* how far entity can see */
	int speed;     /* how fast entity ai can move */

	struct _Keys keys; /* player keys */
	char *msg;   /* message player will display, or message to tell player */
	Item inv[MAX_INV]; /* inventory of ent */
	int hand;
	Armor armor;

	void (*run)(Ent *e); /* function containing entity movement logic */
};

/* game.c: game functions */
bool rat_init(void);
void rat_loop(void);
void rat_cleanup(void);

/* map.c: handle the map */
void init_map(void);
char get_map(int x, int y);
void set_map(int x, int y, char newch);
bool is_floor(int x, int y);
bool is_floor_range(int x, int y, int dx, int dy);
int  floor_count(char ch);
void toggle_door(int x, int y);
void draw_map(Ent e, int r);
void draw_map_floor(Ent e, int r);

/* item.c: handle items */
int  query_item(char *name);
void clear_item(Item *item, int x, int y);
void add_item(Item *item, int x, int y);
void draw_item(Item item, Ent e, int r);

/* entity.c: handle anything with entities */
void draw_ent(Ent e, Ent oe, int r);

/* ai.c: different entity AIs */
void rand_ai(Ent *e);
void dumb_ai(Ent *e);

/* player.c: handle the player */
void add_msg(char *msg, char *message);
void draw_msg(char *msg);
void player_run(Ent *e);

/* data.c: handle reading from data/ directory */
bool init_block(void);
bool init_item(void);
bool init_entity(void);
bool init_player(int count);

/* gfx.c: SDL functions */
SDL_Texture *load_img(char *path);
void draw_text(char *str, SDL_Color color, int x, int y);
void draw_img(SDL_Texture *img, SDL_Rect *src, int x, int y, SDL_RendererFlip flip);

int ZOOM;
SDL_Window *win;
SDL_Renderer *ren;
TTF_Font *font;

SDL_Texture *nullimg; /* fall back if image file cant be loaded */

int blockqty;
int itemqty;
int playerqty;
int entqty;

Block block[MAX_ITEMS];
Item item[MAX_ITEMS];
Ent player[MAX_PLAYERS]; /* TODO combine player with entity */
Ent entity[MAX_ENTITIES];

#endif /* RATIUM_H */
