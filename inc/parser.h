
#ifndef __DICELANG_PARSER_H__
#define __DICELANG_PARSER_H__

#include <dicelang.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

struct dicelang_parse_node {
    struct dicelang_token token;

    struct dicelang_parse_node *parent;
    RANGE(struct dicelang_parse_node *) *children;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void dicelang_parse_node_print(struct dicelang_parse_node *node, FILE *to_file);
struct dicelang_parse_node *dicelang_parse(RANGE_TOKEN *tokens, allocator alloc);
void dicelang_parse_node_destroy(struct dicelang_parse_node **node, struct allocator alloc);

#endif