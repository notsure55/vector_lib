#include <stdlib.h>
#include <stdio.h>

#include <Hashmap/Hashmap.h>

Hashmap hashmap_new(enum Type key_type, enum Type value_type)
{
    Hashmap hp;
    hp.kvs = (struct KV*)malloc((hp.count + 1) * sizeof(struct KV));
    hp.count = 0;
    hp.key_type = key_type;
    hp.value_type = value_type;

    return hp;
}

// searches for key in map and replaces the value associated with it.
// returns true if it finds a key and replaces it, returns false if inserts new key value pair
bool hashmap_insert(Hashmap* map, void* key, void* value)
{
    int index = -1;
    if ((index = hashmap_key_does_exist(map, key)) != -1)
    {
        memcpy(map->kvs[index].value, value, TYPE_SIZE(map->value_type));
        return true;
    }
    else
    {
        hashmap_push(map, key, value);
    }

    return false;
}

int hashmap_key_does_exist(Hashmap* map, void* key)
{
    // cant do pointer checks because im copying memory into my hashmap
    switch (map->key_type) {
    case TYPE_INTEGER:
        {
            for (size_t i = 0; i < map->count; ++i)
            {
                if (*(int*)map->kvs[i].key == *(int*)key) {
                    return (int)i;
                }
            }
        }
    case TYPE_FLOAT:
        {
            for (size_t i = 0; i < map->count; ++i)
            {
                if (*(float*)map->kvs[i].key == *(float*)key) {
                    return (int)i;
                }
            }
        }
    case TYPE_DOUBLE:
        {
            for (size_t i = 0; i < map->count; ++i)
            {
                if (*(double*)map->kvs[i].key == *(double*)key) {
                    return (int)i;
                }
            }
        }
    case TYPE_PLAYER:
        {
            for (size_t i = 0; i < map->count; ++i)
            {
                // TODO:
                //*if (*(Player*)map->kvs[i].key == *(Player*)key) {
                    return i;
                //}
            }
        }
    }

    return -1;
}

void hashmap_push(Hashmap* map, void* key, void* value)
{
    map->count += 1;
    map->kvs = (struct KV*)realloc(map->kvs, map->count * sizeof(struct KV));
    // copying key
    map->kvs[map->count - 1].key = (void*)malloc(TYPE_SIZE(map->key_type));
    memcpy(map->kvs[map->count - 1].key, key, TYPE_SIZE(map->key_type));
    // copying value
    map->kvs[map->count - 1].value = (void*)malloc(TYPE_SIZE(map->value_type));
    memcpy(map->kvs[map->count - 1].value, value, TYPE_SIZE(map->value_type));
}
