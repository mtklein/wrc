#pragma once

#include <atomic>
#include <stddef.h>
#include <stdint.h>
#include <utility>

static constexpr size_t W = static_cast<size_t>(1) << (sizeof(size_t)*4);

void split_shares(size_t* from, size_t* to, std::atomic<size_t>* weight);

template <typename T>
class wrc {
public:
    template <typename... Args>
    static wrc make(Args&&... args) {
        return wrc{ new weighted{std::forward<Args>(args)...} };
    }

    wrc(const wrc& other) : ptr_(other.ptr_) {
        split_shares(&other.share_, &share_, &ptr_->weight);
    }

    wrc(wrc&& other) : ptr_(other.ptr_), share_(other.share_) {
        other.ptr_   = nullptr;
        other.share_ = 0;
    }

    ~wrc() {
        if (ptr_ && share_ == ptr_->weight.fetch_sub(share_)) {
            delete ptr_;
        }
    }

    T*        get() const { return &ptr_->val; }
    T* operator->() const { return &ptr_->val; }
    T& operator *() const { return  ptr_->val; }

    explicit operator bool() const { return ptr_ != nullptr; }

private:
    struct weighted {
        template <typename... Args>
        weighted(Args&&... args) : weight{W}, val{std::forward<Args>(args)...} {}

        mutable std::atomic<size_t> weight;
        T val;
    };

    explicit wrc(weighted* ptr) : ptr_(ptr), share_(W) {}

    weighted*        ptr_;
    mutable size_t share_;
};
