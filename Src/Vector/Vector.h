#ifndef VECTOR_H
#define VECTOR_H

#include <Player.h>
#include <Type.h>

// macros

#define vec_push(vec, val) _Generic(val, \
        int: vec_push_int, \
        Player: vec_push_player) \
        (vec, val)

typedef struct
{
    void** objects;
    enum Type type;
    size_t count;
    size_t size;

    void (*free)(struct Vector*);

} Vector;

#define INIT_VECTOR_METHODS(v) \
        (v)->free = vec_free;

void vec_free(Vector* vec);
void vec_push_player(Vector* vec, Player value);
void vec_push_int(Vector* vec, int value);
Vector vector_new(enum Type type);

#endif // VECTOR_H
