/* URIPathFinder: A simple parser for URIs
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Nate Bragg
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef URI_PATH_FINDER_RBTREE_H
#define URI_PATH_FINDER_RBTREE_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

typedef enum color {
    red, black
} color;

typedef struct tree {
    const char *v;
    size_t v_len;
    color color;
    struct tree *par;
    struct tree *lhs;
    struct tree *rhs;
} tree;

typedef struct arena {
    size_t size;
    size_t entries;
    tree *stack;
} arena;

static tree *arena_alloc(arena *a) {
    if (a->entries == a->size) {
        return NULL;
    }
    size_t entry = a->entries++;
    static tree def_tree = {
        .v = NULL,
        .v_len = 0,
        .color = black,
        .par = NULL,
        .lhs = NULL,
        .rhs = NULL,
    };
    a->stack[entry] = def_tree;
    return &a->stack[entry];
}

/*    y         x
    x   g  -> a   y
   a b           b g */
static void tree_rotate_right(tree *y) {
    /* Double check that rotation is possible */
    if (y == NULL || y->lhs == NULL) {
        return;
    }
    /* In case y is the root we don't swap, we overwrite */
    tree *x = y->lhs;
    tree *g = y->rhs;
    tree *a = x->lhs;
    tree *b = x->rhs;
    tree x_val = *x;
    tree y_val = *y;
    tree *old_y = y;
    tree *old_x = x;
    /* Make x called y and vice versa */
    *x = y_val;
    x = old_y;
    *y = x_val;
    y = old_x;
    /* Perform the rotation and fix up */
    x->par = y->par;
    y->par = x;
    y->lhs = b;
    x->rhs = y;
    if (a != NULL) {
        a->par = x;
    }
    if (b != NULL) {
        b->par = y;
    }
    if (g != NULL) {
        g->par = y;
    }
}

/*   x           y
   a   y   ->  x   g
      b g     a b    */
static void tree_rotate_left(tree *x) {
    /* Double check that rotation is possible */
    if (x == NULL || x->rhs == NULL) {
        return;
    }
    /* In case x is the root we don't swap, we overwrite */
    tree *a = x->lhs;
    tree *y = x->rhs;
    tree *b = y->lhs;
    tree *g = y->rhs;
    tree x_val = *x;
    tree y_val = *y;
    tree *old_y = y;
    tree *old_x = x;
    /* Now x is called y and vice versa */
    *x = y_val;
    x = old_y;
    *y = x_val;
    y = old_x;
    /* Perform the rotation and fix up */
    y->par = x->par;
    x->par = y;
    x->rhs = b;
    y->lhs = x;
    if (a != NULL) {
        a->par = x;
    }
    if (b != NULL) {
        b->par = x;
    }
    if (g != NULL) {
        g->par = y;
    }
}

static bool tree_insert(const char *needle, size_t needle_len, arena *a) {
    tree *which = arena_alloc(a);
    if (which == NULL) {
        return false;
    }
    which->v = needle;
    which->v_len = needle_len;
    which->color = black;
    
    /* Inside baseball, index 0 is always the root */
    tree *root = &a->stack[0];
    if (root == which) {
        return true;
    }

    tree **current = &root;
    tree *previous = NULL;
    while (*current != NULL) {
        previous = *current;
        int cmp = strncmp(needle, previous->v, needle_len);
        if (cmp == 0) {
            if (needle_len == previous->v_len) {
                /* Already in the table! Couldn't insert. */
                return false;
            }
            current = needle_len < previous->v_len ?
                      &previous->lhs : &previous->rhs;
        } else {
            current = cmp < 0 ? &previous->lhs : &previous->rhs;
        }
    }
    which->par = previous;
    which->color = red;
    *current = which;

    /* This is more or less riffed off of CLRS ch 13 */
    tree *z = which;
    while (z->par != NULL && z->par->color == red) {
        tree *zp = z->par;
        tree *zpp = zp->par;
        tree *y = zp == zpp->lhs ? zpp->rhs : zpp->lhs;
        if (y != NULL && y->color == red) {
            /* This preserves black height for the grandparent */
            zp->color = black;
            y->color = black;
            zpp->color = red;
            z = zpp;
        } else if (zp == zpp->lhs) {
            if (z == zp->rhs) {
                /* In order to rotate the grandparent right,
                   we first must rotate the parent left */
                tree_rotate_left(zp);
            }
            zp = z->par;
            zpp = zp->par;
            zp->color = black;
            zpp->color = red;
            tree_rotate_right(zpp);
        } else {
            if (z == zp->lhs) {
                /* In order to rotate the grandparent left,
                   we first must rotate the parent right */
                tree_rotate_right(zp);
            }
            zp = z->par;
            zpp = zp->par;
            zp->color = black;
            zpp->color = red;
            tree_rotate_left(zpp);
        }
    }
    root->color = black;

    return true;
}

static tree *tree_max(tree *t) {
    for (; t && t->rhs; t = t->rhs);
    return t;
}

#endif /* URI_PATH_FINDER_RBTREE_H */
