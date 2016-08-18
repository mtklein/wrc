#include "wrc.h"

// This function is thread safe with respect to weight but not with respect to from or to.
void split_shares(size_t* from, size_t* to, std::atomic<size_t>* weight) {
    size_t f = *from,
           t = *to;

    if (__builtin_expect(f > 1, true)) {
        t = f / 2;
        f -= t;
        *to   = t;
        *from = f;
        return;
    }

    weight->fetch_add(W-1);
    *from = W/2;
    *to   = W/2;
}
