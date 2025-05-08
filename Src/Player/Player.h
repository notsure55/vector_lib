#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
    int health;
    char* name;

    void(*free)(struct Player*);

} Player;

#define INIT_PLAYER_METHODS(p) \
    (p)->free = player_free;

Player player_new(int health, const char* name);
void player_free(Player* player);

#endif PLAYER_H
