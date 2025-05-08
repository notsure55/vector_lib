#include <windows.h>
#include <stdio.h>

#include <Vector/Vector.h>
#include <Hashmap/Hashmap.h>
#include <Type/Type.h>

void vector_example()
{
    Vector players = vector_new(TYPE_PLAYER);

    Player player = player_new(100, "black");
    Player player1 = player_new(50, "fan");

    vec_push(&players, player);
    vec_push(&players, player1);

    for (size_t i = 0; i < players.count; ++i)
    {
        Player* p = (Player*)players.objects[i];
        printf("First: %s\n", p->name);
        p->free(p);
        printf("Second: %s\n", p->name);
    }

    players.free(&players);
}

int main()
{
    Hashmap map = hashmap_new(TYPE_INTEGER, TYPE_PLAYER);
    Player player = player_new(100, "fan");
    Player player1 = player_new(50, "who.ru");
    int black = 5;
    hashmap_push(&map, &black, &player);

    printf("%d %s\n", TRANSFORM_VALUE(map.key_type, map.kvs[0].key), ((Player*)map.kvs[0].value)->name);

    if (hashmap_insert(&map, &black, &player1))
    {
        printf("Key %d already exists\n", black);
    }
    else
    {
        printf("Pushed onto map\n");
    }

    printf("%d %s\n", TRANSFORM_VALUE(map.key_type, map.kvs[0].key), ((Player*)map.kvs[0].value)->name);
    return 0;
}
