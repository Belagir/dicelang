
#include <ustd/sorting.h>

#include "var_hashmap.h"

/**
 * @brief
 *
 * @param size
 * @param alloc
 * @return struct dicelang_variable_map
 */
struct dicelang_variable_map dicelang_variable_map_create(size_t size, struct allocator alloc)
{
    struct dicelang_variable_map new_map = { };

    if (size == 0) {
        return (struct dicelang_variable_map) { };
    }

    new_map = (struct dicelang_variable_map) {
            .vars = range_create_dynamic(alloc, sizeof(*new_map.vars->data), size),
    };

    return new_map;
}

/**
 * @brief
 *
 * @param map
 * @param alloc
 */
void dicelang_variable_map_destroy(struct dicelang_variable_map *map, struct allocator alloc)
{
    if (!map) {
        return;
    }

    for (size_t i = 0 ; i < map->vars->length ; i++) {
        dicelang_distrib_destroy(&map->vars->data[i].val, alloc);
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(map->vars));
    *map = (struct dicelang_variable_map) { };
}

/**
 * @brief
 *
 * @param map
 * @param name
 * @param len_name
 * @return struct dicelang_distrib
 */

bool dicelang_variable_map_get(struct dicelang_variable_map map, const char *name, size_t len_name, struct dicelang_distrib *out_val, struct allocator alloc)
{
    u32 hash = 0;
    size_t pos = 0;

    if (!name || (len_name == 0)) {
        return false;
    }

    hash = hash_jenkins_one_at_a_time((const byte *) name, len_name, 0);

    if (sorted_range_find_in(RANGE_TO_ANY(map.vars), &hash_compare, &hash, &pos)) {
        *out_val = dicelang_distrib_copy(map.vars->data[pos].val, alloc);
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
 * @param new_val
 */
bool dicelang_variable_map_set(struct dicelang_variable_map *map, const char *name, size_t len_name, struct dicelang_distrib *new_val, struct allocator alloc)
{
    u32 hash = 0;
    size_t pos = 0;

    if (!name || (len_name == 0)) {
        return false;
    }

    hash = hash_jenkins_one_at_a_time((const byte *) name, len_name, 0);

    if (sorted_range_find_in(RANGE_TO_ANY(map->vars), &hash_compare, &hash, &pos)) {
        dicelang_distrib_destroy(&map->vars->data[pos].val, alloc);
        goto lbl_dicelang_variable_map_set_assume_ownership;
    }

    range_ensure_capacity(alloc, RANGE_TO_ANY(map->vars), 1);
    range_insert_value(RANGE_TO_ANY(map->vars), pos, &(struct dicelang_variable) { .hash = hash, .val = { } });

lbl_dicelang_variable_map_set_assume_ownership:
    map->vars->data[pos].val = *new_val;
    *new_val = (struct dicelang_distrib) { };
    return true;
}
