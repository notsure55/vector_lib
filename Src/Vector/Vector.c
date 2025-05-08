#include <Vector/Vector.h>
#include <stdio.h>
#include <stdlib.h>

Vector vector_new(enum Type type)
{
    Vector new_vec;
    new_vec.objects = NULL;
    new_vec.type = type;
    new_vec.count = 0;

    INIT_VECTOR_METHODS(&new_vec);

    return new_vec;
}

void vec_push(Vector* vec, void* value)
{
    // incrementing the count of our vector
    vec->count += 1;
    // allocating more size for our vector
    vec->objects = (void**)realloc(vec->objects, vec->count * sizeof(void*));
    // doesnt check type, so make sure your pushing right type on the vec
    vec->objects[vec->count - 1] = (void*)malloc(TYPE_SIZE(vec->type));
    memcpy(vec->objects[vec->count - 1], value, TYPE_SIZE(vec->type));
}

void* vec_retrieve(Vector* vec, size_t index)
{
    return vec->objects[index];
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
