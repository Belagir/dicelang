
#ifndef __DISTRIBUTION_H__
#define __DISTRIBUTION_H__

#include <ustd/range.h>

#include <dicelang.h>

struct dicelang_entry { f32 val, weight; };
struct dicelang_distrib { RANGE(struct dicelang_entry) *values; RANGE(const char *) *formula; };

struct dicelang_distrib dicelang_distrib_create(struct dicelang_token token, struct allocator alloc);
void dicelang_distrib_destroy(struct dicelang_distrib *distrib, struct allocator alloc);

void dicelang_distrib_add(struct dicelang_distrib *lhs, struct dicelang_distrib rhs);
void dicelang_distrib_substract(struct dicelang_distrib *lhs, struct dicelang_distrib rhs);

void dicelang_distrib_multiply(struct dicelang_distrib *lhs, struct dicelang_distrib rhs);
void dicelang_distrib_divide(struct dicelang_distrib *lhs, struct dicelang_distrib rhs);

void dicelang_distrib_dice(struct dicelang_distrib *lhs, struct dicelang_distrib rhs);

void dicelang_distrib_test(void);

#endif
