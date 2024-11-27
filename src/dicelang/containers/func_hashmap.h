
#ifndef __FUNC_HASHMAP_H__
#define __FUNC_HASHMAP_H__

#include <ustd/range.h>

#include "distribution.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef void (*dicelang_script_func)(struct dicelang_distrib *input, struct dicelang_distrib *output, struct allocator alloc);

/**
 * @brief
 *
 */
struct dicelang_function {
    u32 hash;

    size_t nb_args;
    dicelang_script_func func_impl;
};

/**
 * @brief
 *
 */
struct dicelang_function_map {
    RANGE(struct dicelang_function) *funcs;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct dicelang_function_map dicelang_function_map_create(size_t size, struct allocator alloc);
void dicelang_function_map_destroy(struct dicelang_function_map *map, struct allocator alloc);

bool dicelang_function_map_get(struct dicelang_function_map map, const char *name, size_t len_name, struct dicelang_function *func);
bool dicelang_function_map_set(struct dicelang_function_map *map, const char *name, size_t len_name, dicelang_script_func func, size_t nb_args, struct allocator alloc);

#endif