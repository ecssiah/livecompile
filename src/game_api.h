#pragma once

#include <stdint.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef int32_t  i32;
typedef float    f32;
typedef int32_t  b32;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GameMemory GameMemory;
struct GameMemory
{
    void *permanent_storage;
    u32 permanent_storage_size;

    b32 is_initialized;
};

typedef struct GameInput GameInput;
struct GameInput
{
    f32 delta_time;
};

typedef void game_update_fn(GameMemory *memory, const GameInput *input);
typedef void game_render_fn(GameMemory *memory);

#ifdef __cplusplus
}
#endif
