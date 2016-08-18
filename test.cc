#include "wrc.h"
#include <stdio.h>
#include <utility>

struct foo {
    foo(int v) : x(v) { printf("hi\n"); }
    ~foo() { printf("bye\n"); }

    int x;
};

int main(int, char**) {

    wrc<foo> f = wrc<foo>::make(5);

    printf("x: %d\n", f->x);

    wrc<foo> other{ f };
    other->x++;

    printf("x: %d\n", f->x);

    wrc<foo> moved{ std::move(f) };
    if (f) {
        printf("f is still alive\n");
    } else {
        printf("f is a husk\n");
    }
    printf("x: %d\n", moved->x);

    return 0;
}
