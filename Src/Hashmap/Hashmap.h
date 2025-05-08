#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdbool.h>
#include <Vector/Vector.h>

typedef struct {
    void *key;
    void *value;
} KV;

typedef struct {
    KV* kvs;
    enum Type key_type;
    enum Type value_type;
    size_t count;
} Hashmap;

Hashmap hashmap_new(enum Type key_type, enum Type value_type);
void hashmap_push(Hashmap* map, void* key, void* value);
int hashmap_key_does_exist(Hashmap* map, void* key);
KV* hashmap_retrieve(Hashmap* map, size_t index);

#endif // HASHMAP_H
