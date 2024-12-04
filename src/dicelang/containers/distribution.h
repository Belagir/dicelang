
#ifndef __DISTRIBUTION_H__
#define __DISTRIBUTION_H__

#include <ustd/range.h>

#include <dicelang.h>

struct dicelang_entry { i32 val; u32 count; };
struct dicelang_distrib { RANGE(struct dicelang_entry) *values; /* RANGE(const char *) *formula; */ };

struct dicelang_distrib dicelang_distrib_create(struct dicelang_token token, struct allocator alloc);
struct dicelang_distrib dicelang_distrib_copy(struct dicelang_distrib from, struct allocator alloc);
void dicelang_distrib_destroy(struct dicelang_distrib *distrib, struct allocator alloc);

bool dicelang_distrib_is_empty(struct dicelang_distrib d);

struct dicelang_distrib dicelang_distrib_add      (struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc);
struct dicelang_distrib dicelang_distrib_substract(struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc);
struct dicelang_distrib dicelang_distrib_multiply (struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc);
struct dicelang_distrib dicelang_distrib_divide   (struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc);
struct dicelang_distrib dicelang_distrib_union    (struct dicelang_distrib lhs, struct dicelang_distrib rhs, struct allocator alloc);
struct dicelang_distrib dicelang_distrib_dice     (struct dicelang_distrib from, struct allocator alloc);

void dicelang_distrib_test(void);

#endif
