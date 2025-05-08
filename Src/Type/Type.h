#ifndef TYPE_H
#define TYPE_H

#include <Player/Player.h>

int transform_value_int(void* value);

#define TRANSFORM_VALUE(type, v) \
        (type) == TYPE_INTEGER ? transform_value_int(v) : 0\

#define TYPE_SIZE(type) \
        (type) == TYPE_INTEGER ? sizeof(int) : \
        (type) == TYPE_FLOAT ? sizeof(float) : \
        (type) == TYPE_DOUBLE ? sizeof(double) : \
        (type) == TYPE_PLAYER ? sizeof(Player) : 0\

enum Type
{
    TYPE_INTEGER = 0,
    TYPE_FLOAT = 1,
    TYPE_DOUBLE = 2,
    TYPE_PLAYER = 3
};



#endif // TYPE_H
