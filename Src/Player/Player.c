#include <Player.h>
#include <stdlib.h>

Player player_new(int health, const char* name)
{
    Player player;
    player.health = health;
    player.name = (char*)malloc(strlen(name));
    strcpy(player.name, name);
    INIT_PLAYER_METHODS(&player);
    return player;
}

void player_free(Player* player)
{
    free(player->name);
    free(player);
}
