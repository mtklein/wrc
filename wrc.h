#pragma once

#include <atomic>
#include <stdint.h>
#include <utility>

static constexpr size_t W = static_cast<size_t>(1) << (sizeof(size_t)*4);

class weighted_refcount {
public:
    std::atomic<size_t>* weight() const { return &weight_; }

private:
    mutable std::atomic<size_t> weight_{ W };
};

void split_shares(size_t* from, size_t* to, std::atomic<size_t>* weight);

template <typename T>
class wrc {
public:
    template <typename... Args>
    static wrc make(Args&&... args) {
        return wrc{ new T{std::forward<Args>(args)...} };
    }

    wrc(const wrc& other) : ptr_(other.ptr_) {
        split_shares(&other.share_, &share_, ptr_->weight());
    }

    wrc(wrc&& other) : ptr_(other.ptr_), share_(other.share_) {
        other.ptr_   = nullptr;
        other.share_ = 0;
    }

    ~wrc() {
        if (ptr_ && share_ == ptr_->weight()->fetch_sub(share_)) {
            delete ptr_;
        }
    }

    T*        get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T* operator *() const { return ptr_; }

private:
    explicit wrc(T* ptr) : ptr_(ptr), share_(W) {}

    T*             ptr_;
    mutable size_t share_;
};
