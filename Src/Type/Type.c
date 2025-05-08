#include <Type/Type.h>

int transform_value_int(void* value)
{
    return *(int*)value;
}
float transform_value_float(void* value)
{
    return *(float*)value;
}
double transform_value_double(void* value)
{
    return *(double*)value;
}
Player transform_value_player(void* value)
{
    return *(Player*)value;
}
