
#include <ustd/sorting.h>

#include "func_hashmap.h"

/**
 * @brief
 *
 * @param size
 * @param alloc
 * @return struct dicelang_function_map
 */
struct dicelang_function_map dicelang_function_map_create(size_t size, struct allocator alloc)
{
    struct dicelang_function_map new_map = { };

    if (size == 0) {
        return (struct dicelang_function_map) { };
    }

    new_map = (struct dicelang_function_map) {
            .funcs = range_create_dynamic(alloc, sizeof(*new_map.funcs->data), size),
    };

    return new_map;
}

/**
 * @brief
 *
 * @param map
 * @param alloc
 */
void dicelang_function_map_destroy(struct dicelang_function_map *map, struct allocator alloc)
{
    if (!map) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(map->funcs));
    *map = (struct dicelang_function_map) { };
}

/**
 * @brief
 *
 * @param map
 * @param name
 * @param len_name
 * @param func
 * @return
 */
bool dicelang_function_map_get(struct dicelang_function_map map, const char *name, size_t len_name, struct dicelang_function *func)
{
    u32 hash = 0;
    size_t pos = 0;

    if (!name || (len_name == 0)) {
        return false;
    }

    hash = hash_jenkins_one_at_a_time((const byte *) name, len_name, 0);

    if (sorted_range_find_in(RANGE_TO_ANY(map.funcs), &hash_compare, &hash, &pos)) {
        *func = map.funcs->data[pos];
        return true;
    }

    return false;
}

/**
 * @brief
 *
 * @param map
 * @param name
 * @param len_name
 * @param func
 * @param nb_args
 * @param alloc
 * @return
 */
bool dicelang_function_map_set(struct dicelang_function_map *map, const char *name, size_t len_name, dicelang_script_func func, size_t nb_args, bool returns_something, struct allocator alloc)
{
    u32 hash = 0;
    size_t pos = 0;

    if (!name || (len_name == 0) || !func) {
        return false;
    }

    hash = hash_jenkins_one_at_a_time((const byte *) name, len_name, 0);

    if (sorted_range_find_in(RANGE_TO_ANY(map->funcs), &hash_compare, &hash, &pos)) {
        return false;
    }

    range_ensure_capacity(alloc, RANGE_TO_ANY(map->funcs), 1);
    range_insert_value(RANGE_TO_ANY(map->funcs), pos, &(struct dicelang_function) { .hash = hash, .func_impl = func, .nb_args = nb_args, .returns_value = returns_something });

    return true;
}
