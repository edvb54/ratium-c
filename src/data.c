/* See LICENSE for licence details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ratium.h"

static struct _Keys player_keys[MAX_PLAYERS] = {
{ 'h', 'j', 'k', 'l', 'b', 'y', 'n', 'u', '.', 'o', 'p', 'i' },
{ 'a', 'x', 'w', 'd', 'z', 'q', 'c', 'e', 's', 'f', 't', 'v' },
};

struct Ent_t {
	char *name, *drop, *msg; /* TODO: Make msg (& drop?) an array */
	char face;
	int color;
	int hp;
	int damage;
	int sight;
	int speed;
	ENT_TYPE type;
	ENT_AI ai;
	int rarity;
};

static struct Ent_t player_t[MAX_PLAYERS] = {
{ "player1", NULL, NULL, '@', 3, 10, 1, 61, 0, TYPE_ALL, AI_PLAYER, 0 },
{ "player2", NULL, NULL, '@', 1, 10, 1, 16, 0, TYPE_ALL, AI_PLAYER, 0 },
};

static struct Ent_t ent_t[MAX_ENTITIES] = {
{ "rat", "rat meat", NULL,
  'r', 5, 2,  1, 4, 6, TYPE_CAVE,  AI_HOSTILE, 5 },
{ "supper rat", "rat meat", NULL,
  'R', 5, 4,  2, 8, 5,  TYPE_CAVE, AI_HOSTILE, 3 },
{ "gnu", "gnu meat", NULL,
  'G', 6, 6,  1, 3, 8,  TYPE_GRASS, AI_PEACEFUL, 2 },
{ "cow", "beef", NULL,
  'c', 5, 2,  0, 3, 6,  TYPE_GRASS, AI_PEACEFUL, 5 },
{ "King Arthur", "gold", "I am King Arthur",
  '@', 4, 10, 0, 16, 12, TYPE_ALL, AI_PEACEFUL, 1 },
{ "knight", "sword", "Who goes there?",
  '@', 6, 10, 0, 16, 12, TYPE_ALL, AI_PEACEFUL, 2 },
{ "peasant", NULL, "Oh. How'd you do",
  '@', 5, 10, 0, 16, 12, TYPE_ALL, AI_PEACEFUL, 2 },
{ "peasant", NULL, "lovely filth down here",
  '@', 5, 10, 0, 16, 12, TYPE_ALL, AI_PEACEFUL, 2 },
};
int entqty_t = 8;

struct Item_t {
	char *name;
	ITEM_TYPE type;
	char face;
	int color;
	int stat;
	int rarity;
};

static struct Item_t item_t[MAX_ITEMS] = {
{ "gold",     0, '$', 4,  0,  5 },
{ "spam",     1, '=', 8,  2,  4 },
{ "haggis",   1, 'o', 11, 10, 1 },
{ "sword",    2, '/', 6,  2,  3 },
{ "shield",   3, '0', 6,  4,  2 },
{ "bow",      4, ')', 5,  0,  3 },
{ "arrow",    5, '|', 5,  5,  4 },
{ "rat meat", 1, '%', 5,  -1, 0 },
{ "gnu meat", 1, '%', 6,  4,  0 },
{ "beef",     1, '%', 5,  22, 0 },
};
int itemqty_t = 8;

/* TODO: Improve and implement in other functions */
/* TODO: Add min and max parameters */
/* calc_rarity: change rarity to make it random */
static void
calc_rarity(int *rarity) {
	if (*rarity != 0)
		switch (rand() % 2) {
		case 0: *rarity += rand() % 3;
		case 1: *rarity -= rand() % 3;
		}
}

/* gen_ent: change x and y values to a valid place for entity to be generated
 *          based on type */
static void
gen_ent(int *x, int *y, ENT_TYPE type) {
	int spawntile;

	switch(type) {
	case TYPE_ALL:                    break;
	case TYPE_CAVE:  spawntile = '.'; break;
	case TYPE_GRASS: spawntile = 'g'; break;
	case TYPE_WATER: spawntile = 'w'; break;
	}

	do {
		*x = rand() % MAX_X;
		*y = rand() % MAX_Y;
	} while ((type == TYPE_ALL) ?
		  !is_floor(*x, *y) :
		  get_map(*x, *y) != spawntile);
}

/* init_item: copies values from item_t[] to item[] */
bool init_item(void) {
	itemqty = 0;

	for (int num = 0; num < itemqty_t; num++) {
		item[itemqty].name = malloc(MAX_NAME * sizeof(char));
		strcpy(item[itemqty].name, item_t[itemqty].name);
		item[itemqty].face = item_t[itemqty].face;
		item[itemqty].color = item_t[itemqty].color;
		item[itemqty].type = item_t[itemqty].type;
		item[itemqty].stat = item_t[itemqty].stat;

		for (int i = 0; i < MAX_X; i++)
			for (int j = 0; j < MAX_Y; j++)
				item[itemqty].map[j][i] = 0;

		calc_rarity(&item_t[itemqty].rarity);
		for (int x, y, i = 0; i < item_t[itemqty].rarity; i++) {
			do {
				x = rand() % MAX_X;
				y = rand() % MAX_Y;
			} while (get_map(x, y) != '.');
			item[itemqty].map[y][x]++;
		}

		itemqty++;
	}

	itemqty++;

	return true;
}

bool /* copies values from ent_t[] to entity[] */
init_entity(void) {
	entqty = 0;
	for (int i = 0; i < entqty_t; i++) {
		calc_rarity(&ent_t[i].rarity);
		for (int num = 0; num < ent_t[i].rarity; num++, entqty++) {
			entity[entqty].name = malloc(MAX_NAME * sizeof(char));
			strcpy(entity[entqty].name, ent_t[i].name);
			entity[entqty].type = ent_t[i].type;
			entity[entqty].ai = ent_t[i].ai;
			entity[entqty].face = ent_t[i].face;
			entity[entqty].color = ent_t[i].color;
			entity[entqty].maxhp = ent_t[i].hp;
			entity[entqty].hp = ent_t[i].hp;
			entity[entqty].isdead = false;
			entity[entqty].damage = ent_t[i].damage;
			entity[entqty].sight = ent_t[i].sight;

			entity[entqty].speed = ent_t[i].speed;

			entity[entqty].hand = -1;

			entity[entqty].msg = malloc(MAX_NAME * sizeof(char));
			if (ent_t[i].msg != NULL) {
				strcpy(entity[entqty].msg, ent_t[i].msg);
			} else
				entity[entqty].msg = NULL;

			for (int j = 0; j < MAX_INV; j++) {
				entity[entqty].inv[j].name = malloc(MAX_NAME * sizeof(char));
				entity[entqty].inv[j].face = ' ';
				entity[entqty].inv[j].color = 0;
				entity[entqty].inv[j].map[0][0] = 0;
			}

			if (ent_t[i].drop != NULL) {
				strcpy(entity[entqty].inv[0].name, ent_t[i].drop);
				entity[entqty].inv[0].map[0][0] = rand() % 3;
			}

			gen_ent(&entity[entqty].x, &entity[entqty].y, ent_t[i].type);

			switch(ent_t[i].ai) {
			case AI_PLAYER: break;
			case AI_HOSTILE:
				entity[entqty].run = dumb_ai;
				break;
			case AI_PEACEFUL:
				entity[entqty].run = rand_ai;
				break;
			}

		}
	}

	return true;
}

/* init_player: copies values from player_t[] to player[] */
bool init_player(int count) {
	int x_0, y_0;

	for (int num = 0; num < count; num++) {
		player[num].name = malloc(MAX_NAME * sizeof(char));
		strcpy(player[num].name, player_t[num].name);
		player[num].type = TYPE_ALL;
		player[num].ai = AI_PLAYER;
		player[num].face = '@';
		player[num].color = player_t[num].color;

		player[num].maxhp = player_t[num].hp;
		player[num].hp = player[num].maxhp;
		player[num].isdead = false;
		player[num].damage = player_t[num].damage;
		player[num].sight = player_t[num].sight;
		player[num].speed = player_t[num].speed;

		player[num].direc = RIGHT;
		do {
			x_0 = rand() % MAX_X;
			y_0 = rand() % MAX_Y;
		} while (!is_floor(x_0, y_0));
		player[num].x = x_0;
		player[num].y = y_0;
		player[num].bary = num;

		player[num].keys = player_keys[num];

		player[num].msg = malloc(MAX_NAME * sizeof(char));

		for (int i = 0; i < MAX_INV; i++) {
			player[num].inv[i].name = malloc(MAX_NAME * sizeof(char));
			player[num].inv[i].face = ' ';
			player[num].inv[i].color = 0;
			player[num].inv[i].map[0][0] = 0;
		}

		player[num].hand = -1;

		player[num].run = NULL;
	}

	playerqty = count-1;

	return true;
}

