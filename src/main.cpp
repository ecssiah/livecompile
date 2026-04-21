#include "game_api.h"

#include <dlfcn.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static long long get_last_write_time(const char *path)
{
    struct stat attr;
    if (stat(path, &attr) == 0)
    {
        return (long long)attr.st_mtimespec.tv_sec * 1000000000LL +
               (long long)attr.st_mtimespec.tv_nsec;
    }
    return 0;
}

static void copy_file(const char *src, const char *dst)
{
    FILE *in = fopen(src, "rb");
    FILE *out = fopen(dst, "wb");

    if (!in || !out)
    {
        return;
    }

    char buffer[4096];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0)
    {
        fwrite(buffer, 1, n, out);
    }

    fclose(in);
    fclose(out);
}

struct GameCode
{
    void *handle;

    game_update_fn *update;
    game_render_fn *render;

    long long last_write_time;
    int generation;

    bool is_valid;
};

static GameCode load_game_code(const char *source_path, int generation)
{
    GameCode code = {};

    char temp_path[256];
    
    snprintf(
        temp_path,
        sizeof(temp_path),
        "build/game_%d.dylib",
        generation
    );

    copy_file(source_path, temp_path);

    void *handle = dlopen(temp_path, RTLD_NOW);
    if (!handle)
    {
        printf("dlopen failed: %s\n", dlerror());
        return code;
    }

    code.update = (game_update_fn *)dlsym(handle, "game_update");
    code.render = (game_render_fn *)dlsym(handle, "game_render");

    if (code.update && code.render)
    {
        code.handle = handle;
        code.is_valid = true;
    }
    else
    {
        printf("dlsym failed\n");
        dlclose(handle);
    }

    return code;
}

static void unload_game_code(GameCode *code)
{
    if (code->handle)
    {
        dlclose(code->handle);
    }

    code->handle = nullptr;
    code->update = nullptr;
    code->render = nullptr;
    code->is_valid = false;
}

int main()
{
    const char *dylib_path = "build/libgame.dylib";

    GameMemory memory = {};
    memory.permanent_storage_size = 1024 * 1024;
    memory.permanent_storage = malloc(memory.permanent_storage_size);

    GameCode game = {};
    game.last_write_time = get_last_write_time(dylib_path);
    game = load_game_code(dylib_path, 0);

    int generation = 1;

    while (1)
    {
        long long new_time = get_last_write_time(dylib_path);

        if (new_time != game.last_write_time)
        {
            printf("reloading...\n");

            GameCode new_game = load_game_code(dylib_path, generation);

            if (new_game.is_valid)
            {
                unload_game_code(&game);
                game = new_game;
                game.last_write_time = new_time;
                generation++;
            }
        }

        GameInput input = {};
        input.delta_time = 0.016f;

        if (game.is_valid)
        {
            game.update(&memory, &input);
            game.render(&memory);
        }

        usleep(500000);
    }
}
