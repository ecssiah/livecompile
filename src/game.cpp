#include "game_api.h"
#include <stdio.h>

struct GameState
{
    f32 value;
};

extern "C" {

__attribute__((visibility("default")))
void game_update(GameMemory *memory, const GameInput *input)
{
    GameState *state = (GameState *)memory->permanent_storage;

    if (!memory->is_initialized)
    {
        state->value = 0.0f;
        memory->is_initialized = 1;
        printf("initialized\n");
    }

    state->value += 100.0f * input->delta_time;
}

__attribute__((visibility("default")))
void game_render(GameMemory *memory)
{
    GameState *state = (GameState *)memory->permanent_storage;

    printf("value: %.2f\n", state->value);
}

}
