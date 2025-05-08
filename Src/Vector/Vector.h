#ifndef VECTOR_H
#define VECTOR_H

#include <Player/Player.h>
#include <Type/Type.h>

typedef struct
{
    void** objects;
    enum Type type;
    size_t count;

    void (*free)(struct Vector*);

} Vector;

#define INIT_VECTOR_METHODS(v) \
        (v)->free = vec_free;

void vec_free(Vector* vec);
void vec_push(Vector* vec, void* value);
Vector vector_new(enum Type type);

void* vec_retrieve(Vector* vec, size_t index);

#endif // VECTOR_H
