#include "wrc.h"

// This function is thread safe with respect to weight but not with respect to src or dst.
void split_shares(size_t* src, size_t* dst, std::atomic<size_t>* weight) {
    size_t s = *src,
           d = *dst;

    if (__builtin_expect(s > 1, true)) {
        d = s / 2;
        s -= d;
        *dst = d;
        *src = s;
        return;
    }

    weight->fetch_add(W-1);
    *src = W/2;
    *dst = W/2;
}
