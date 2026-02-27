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

#include "../src/rbtree.h"
#include <stdio.h>

#define ASSERT(e) do { if (!(e)) { printf("Assert failed on line %d. Expected: %s\n", __LINE__, #e);} } while(0)

void print_tree(tree *t) {
    printf("%p = {%s %zu %s %p %p %p}\n",
            t, t->v, t->v_len,
            t->color == red ? "red" : "black",
            t->par, t->lhs, t->rhs);
}

void print_arena(void *v) {
    arena *ar = (arena*)v;
    printf("%zu:\n", ar->entries);
    size_t i=0;
    for (i = 0; i < ar->entries; i++) {
        print_tree(&ar->stack[i]);
    }
    printf("\n");
}

int check_rbprops(tree *t) {
    bool neither = t->lhs == NULL && t->rhs == NULL;
    bool both    = t->lhs != NULL && t->rhs != NULL;
    int bc = t->color == red ? 0 : 1;
    if (neither) {
        return 1 + bc;
    }
    if (t->color == red &&
        (!both || t->lhs->color == red ||
                  t->rhs->color == red)) {
        return 0;
    }
    int lc = t->lhs == NULL ? 1 : check_rbprops(t->lhs);
    if (lc == 0) {
        return 0;
    }
    int rc = t->rhs == NULL ? 1 : check_rbprops(t->rhs);
    if (rc == 0 || lc != rc) {
        return 0;
    }
    return lc + bc; 
}

int main() {
    const char *yv = "y";
    const char *xv = "x";
    const char *av = "a";
    const char *bv = "b";
    const char *gv = "g";
    tree y = { .v = yv, .v_len = 2, .color = black, .par = NULL };
    tree x = { .v = xv, .v_len = 2, .color = black, .par = &y };
    tree a = { .v = av, .v_len = 2, .color = black, .par = &x };
    tree b = { .v = bv, .v_len = 2, .color = black, .par = &x };
    tree g = { .v = gv, .v_len = 2, .color = black, .par = &y };
    y.lhs = &x;
    y.rhs = &g;
    x.lhs = &a;
    x.rhs = &b;
    tree root = y;
    tree_rotate_right(&root);
    ASSERT(root.v[0] == 'x');
    ASSERT(root.lhs->v[0] == 'a');
    ASSERT(root.rhs->v[0] == 'y');
    ASSERT(root.rhs->lhs->v[0] == 'b');
    ASSERT(root.rhs->rhs->v[0] == 'g');

    tree_rotate_left(&root);
    ASSERT(root.v[0] == 'y');
    ASSERT(root.lhs->v[0] == 'x');
    ASSERT(root.rhs->v[0] == 'g');
    ASSERT(root.lhs->lhs->v[0] == 'a');
    ASSERT(root.lhs->rhs->v[0] == 'b');

    tree stack[30] = {0};
    arena ar = { 30, 0, stack };
    ASSERT(tree_insert("abc", 4, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("def", 4, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("defg", 5, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("123", 4, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("happy days and jubilation", 26, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("something", 10, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("anything!", 10, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("something else", 15, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$%^&*(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$%^_*(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!_#$%^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@_$%^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#_%^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$_^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$%___(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$%^___){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$%^__(_{}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$%^__()_}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!@#$%^__(){_", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!__$%^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!___%^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!____^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!__$_^__(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!__$%___(){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!__$%^___){}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!__$%^__(_{}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!__$%^__()_}", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("!__$%^__(){_", 13, &ar)); ASSERT(check_rbprops(&stack[0]));
    ASSERT(tree_insert("should succeed", 15, &ar));
    ASSERT(!tree_insert("should succeed", 15, &ar)); /* shouldn't succeed :^) */
    ASSERT(!tree_insert("should fail", 12, &ar)); /* we ran out of space */

    printf("done\n");
}
