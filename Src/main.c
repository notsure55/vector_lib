#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include <Vector/Vector.h>
#include <Hashmap/Hashmap.h>
#include <Type/Type.h>
#include <Hook/InlineHook.h>
#include <Hook/TrampolineHook.h>

void vector_example()
{
    Vector players = vector_new(TYPE_PLAYER);

    Player player = player_new(100, "black");
    Player player1 = player_new(50, "fan");

    vec_push(&players, &player);
    vec_push(&players, &player1);

    for (size_t i = 0; i < players.count; ++i)
    {
        Player* p = (Player*)vec_retrieve(&players, i);
        printf("Name: %s\n", p->name);
    }

    players.free(&players);
}

void hashmap_example()
{
    Hashmap map = hashmap_new(TYPE_INTEGER, TYPE_PLAYER);
    Player player = player_new(100, "fan");
    Player player1 = player_new(50, "who.ru");
    int black = 5;
    hashmap_push(&map, &black, &player);

    printf("%d %s\n", *(int*)map.kvs[0].key, ((Player*)map.kvs[0].value)->name);

    if (hashmap_insert(&map, &black, &player1))
    {
        printf("Key %d already exists\n", black);
    }
    else
    {
        printf("Pushed onto map\n");
    }

    KV* object = hashmap_retrieve(&map, 0);

    printf("%d %s\n", *(int*)object->key, ((Player*)object->value)->name);
}

void print_input(const char* input)
{
    printf("%s\n", input);
    fflush(stdout);
}

typedef void (*trampoline_t)(const char* input);
trampoline_t trampoline_function = NULL;

void print_hook(const char* input) {
    printf("Your not black!\n");
    fflush(stdout);

    trampoline_function(input);
}

int main()
{
    install_trampoline_hook((void*)&print_input, (void*)&print_hook, (void**)&trampoline_function);

    while (true)
    {
        print_input("Im black in real life.\n");
        Sleep(1000);
    }

    return 0;
}
