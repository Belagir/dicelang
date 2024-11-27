
#ifndef __VAR_HASHMAP_H__
#define __VAR_HASHMAP_H__

#include <ustd/range.h>

#include "distribution.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
struct dicelang_variable {
    u32 hash;
    struct dicelang_distrib val;
};

/**
 * @brief
 *
 */
struct dicelang_variable_map {
    RANGE(struct dicelang_variable) *vars;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct dicelang_variable_map dicelang_variable_map_create(size_t size, struct allocator alloc);
void dicelang_variable_map_destroy(struct dicelang_variable_map *map, struct allocator alloc);

bool dicelang_variable_map_get(struct dicelang_variable_map map, const char *name, size_t len_name, struct dicelang_distrib *out_val);
bool dicelang_variable_map_set(struct dicelang_variable_map map, const char *name, size_t len_name, struct dicelang_distrib *new_val, struct allocator alloc);

#endif