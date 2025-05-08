#include <Vector.h>
#include <stdio.h>
#include <stdlib.h>

Vector vector_new(enum Type type)
{
    size_t size_of_type;

    switch (type) {
        // integer
    case TYPE_INTEGER:
        {
            size_of_type = sizeof(int);
        };
        // float
    case TYPE_FLOAT:
        {
            size_of_type = sizeof(float);
        }
        // double
    case TYPE_DOUBLE:
        {
            size_of_type = sizeof(double);
        }
    case TYPE_PLAYER:
        {
            size_of_type = sizeof(Player);
        }
    }

    Vector new_vec;
    new_vec.objects = (void**)malloc(1);
    new_vec.type = type;
    new_vec.count = 0;
    new_vec.size = size_of_type;

    INIT_VECTOR_METHODS(&new_vec);

    return new_vec;
}

void vec_push_int(Vector* vec, int value)
{
    // compares type using c generics
    if (vec->type != _Generic(
        value,
        int: TYPE_INTEGER,
        float: TYPE_FLOAT,
        double: TYPE_DOUBLE,
        Player: TYPE_PLAYER
    ))
    {
        fprintf(stderr, "ERROR: Tried pushing the wrong type into a vector!\n");
        exit(EXIT_FAILURE);
    }

    // incrementing the count of our vector
    vec->count += 1;
    // allocating more size for our vector
    vec->objects = (void**)realloc(vec->objects, vec->count);

    // allocating memory to the heap for the value
    void* heap_allocated_value = malloc(sizeof(value));
    memcpy(heap_allocated_value, &value, sizeof(value));
    vec->objects[vec->count - 1] = heap_allocated_value;
}

void vec_push_player(Vector* vec, Player value)
{
    // compares type using c generics
    if (vec->type != _Generic(
        value,
        int: TYPE_INTEGER,
        float: TYPE_FLOAT,
        double: TYPE_DOUBLE,
        Player: TYPE_PLAYER
    ))
    {
        fprintf(stderr, "ERROR: Tried pushing the wrong type into a vector!\n");
        exit(EXIT_FAILURE);
    }

    vec->count += 1;
    // allocating more size for our vector
    vec->objects = (void*)realloc(vec->objects, vec->size * vec->count);

    void* heap_allocated_value = malloc(sizeof(value));
    memcpy(heap_allocated_value, &value, sizeof(value));
    vec->objects[vec->count - 1] = heap_allocated_value;
}

// free our vecs used memory
// doesnt free everything tho depending on the type.
void vec_free(Vector* vec)
{
    for (size_t i = 0; i < vec->count; ++i)
    {
        free(vec->objects[i]);
        vec->objects[i] = NULL;

        if (vec->objects[i] != NULL)
        {
            fprintf(stderr, "ERROR: Failed to freed memory of vec");
            exit(EXIT_FAILURE);
        }
    }

    free(vec->objects);
}
