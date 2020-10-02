#pragma once

#include <memory>
#include <utility>
#include "control_block.h"
#include "control_block_ptr.h"
#include "control_block_object.h"

template<class T>
class shared_ptr {
public:
    //1
    constexpr shared_ptr() noexcept : pointer(nullptr), control_block(nullptr) {}

    //2
    constexpr shared_ptr(std::nullptr_t) noexcept : shared_ptr() {}

    //3
    template<class Y>
    explicit shared_ptr(Y *ptr) : shared_ptr(ptr, std::default_delete<T>()) {}

    //4
    template<class Y, class Deleter>
    explicit shared_ptr(Y *ptr, Deleter deleter) {
        try {
            pointer = ptr;
            control_block = new control_block_ptr<Y, Deleter>(ptr, deleter);
        } catch (...) {
            deleter(ptr);
            throw;
        }
    }

    //5
    template<class Deleter>
    explicit shared_ptr(std::nullptr_t ptr, Deleter deleter) : shared_ptr() {}

    ~shared_ptr() {
        if (control_block != nullptr) {
            control_block->release_ref();
        }
    }

    //8
    template<class Y>
    shared_ptr(const shared_ptr<Y> &r, T *ptr) noexcept : pointer(ptr), control_block(r.control_block) {
        if (control_block != nullptr) {
            control_block->add_ref();
        }
    }

    //9.1
    shared_ptr(const shared_ptr &r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        if (control_block != nullptr) {
            control_block->add_ref();
        }
    }

    //9.2
    template<class Y>
    shared_ptr(const shared_ptr<Y> &r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        if (control_block != nullptr) {
            control_block->add_ref();
        }
    }

    //10
    shared_ptr(shared_ptr &&r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        r.pointer = nullptr;
        r.control_block = nullptr;
    }

    //10
    template<class Y>
    shared_ptr(shared_ptr<Y> &&r) noexcept : pointer(r), control_block(r.control_block) {
        r.pointer = nullptr;
        r.control_block = nullptr;
    }

    void swap(shared_ptr &r) noexcept {
        std::swap(r.pointer, pointer);
        std::swap(r.control_block, control_block);
    }

    //1
    void reset() noexcept {
        reset <T> (nullptr);
    }

    //2
    template<class Y>
    void reset(Y *ptr) {
        reset(ptr, std::default_delete<Y>());
    }

    //3
    template<class Y, class Deleter>
    void reset(Y *ptr, Deleter d) {
        shared_ptr<T>(ptr, d).swap(*this);
    }

    T *get() const noexcept {
        return pointer;
    }

    T &operator*() const noexcept {
        return *pointer;
    }

    T *operator->() const noexcept {
        return pointer;
    }

    size_t use_count() const noexcept {
        if (control_block != nullptr) {
            return control_block->get_ref_count();
        } else {
            return 0;
        }
    }

    explicit operator bool() const noexcept {
        return pointer != nullptr;
    }

    shared_ptr &operator=(const shared_ptr &r) noexcept {
        if (this != &r) {
            shared_ptr(r).swap(*this);
        }
        return *this;
    }

    template<class Y>
    shared_ptr &operator=(const shared_ptr<Y> &r) noexcept {
        if (this != &r) {
            shared_ptr<Y>(r).swap(*this);
        }
        return *this;
    }

    shared_ptr &operator=(shared_ptr &&r) noexcept {
        if (this != &r) {
            shared_ptr(std::move(r)).swap(*this);
        }
        return *this;
    }

    template<class Y>
    shared_ptr &operator=(shared_ptr<Y> &&r) noexcept {
        if (this != &r) {
            shared_ptr<Y>(std::move(r)).swap(*this);
        }
        return *this;
    }

    template<class Y, class... Args>
    friend shared_ptr<Y> make_shared(Args &&... args);

    template<class Y, class U>
    friend bool operator==(const shared_ptr<Y> &lhs, const shared_ptr<U> &rhs) noexcept;

    template<class Y, class U>
    friend bool operator!=(const shared_ptr<Y> &lhs, const shared_ptr<U> &rhs) noexcept;

    template<class Y>
    friend bool operator==(const shared_ptr<Y> &lhs, std::nullptr_t) noexcept;

    template<class Y>
    friend bool operator==(std::nullptr_t, const shared_ptr<Y> &rhs) noexcept;


    template<class Y>
    friend bool operator!=(const shared_ptr<Y> &lhs, std::nullptr_t) noexcept;

    template<class Y>
    friend bool operator!=(std::nullptr_t, const shared_ptr<Y> &rhs) noexcept;


//private:
    T *pointer;
    control_block *control_block;
};

template<class T, class... Args>
shared_ptr<T> make_shared(Args &&... args) {
    auto *cb_object = new control_block_object<T>(std::forward<Args>(args)...);
    shared_ptr<T> new_shared_pointer;
    new_shared_pointer.pointer = cb_object->get_pointer();
    new_shared_pointer.control_block = cb_object;
    return new_shared_pointer;
}

template<class T, class U>
bool operator==(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return lhs.get() == rhs.get();
}

template<class T>
bool operator==(const shared_ptr<T> &lhs, nullptr_t) noexcept {
    return lhs.get() == nullptr;
}

template<class T>
bool operator==(std::nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return nullptr == rhs;
}

template<class T, class U>
bool operator!=(const shared_ptr<T> &lhs, const shared_ptr<U> &rhs) noexcept {
    return !(lhs == rhs);
}

template<class T>
bool operator!=(const shared_ptr<T> &lhs, std::nullptr_t) noexcept {
    return lhs != nullptr;
}

template<class T>
bool operator!=(std::nullptr_t, const shared_ptr<T> &rhs) noexcept {
    return rhs != nullptr;
}

template<class T>
struct weak_ptr {
    //1
    constexpr weak_ptr() noexcept : pointer(nullptr), control_block(nullptr) {}

    //2
    weak_ptr(const weak_ptr &r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        if (control_block != nullptr) {
            control_block->add_weak();
        }
    }

    //3
    template<class Y>
    weak_ptr(const weak_ptr<Y> &r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        if (control_block != nullptr) {
            control_block->add_weak();
        }
    }

    //4
    template<class Y>
    weak_ptr(const shared_ptr<Y> &r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        if (control_block != nullptr) {
            control_block->add_weak();
        }
    }

    //5
    weak_ptr(weak_ptr &&r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        r.pointer = nullptr;
        r.control_block = nullptr;
    }

    //6
    template<class Y>
    weak_ptr(weak_ptr<Y> &&r) noexcept : pointer(r.pointer), control_block(r.control_block) {
        r.pointer = nullptr;
        r.control_block = nullptr;
    }


    //1
    weak_ptr &operator=(const weak_ptr &r) noexcept {
        if (this != &r) {
            weak_ptr<T>(r).swap(*this);
        }
        return *this;
    }

    //2
    template<class Y>
    weak_ptr &operator=(const weak_ptr<Y> &r) noexcept {
        weak_ptr<Y>(r).swap(*this);
        return *this;
    }

    //3
    template<class Y>
    weak_ptr &operator=(const shared_ptr<Y> &r) noexcept {
        weak_ptr<Y>(r).swap(*this);
        return *this;
    }

    //4
    weak_ptr &operator=(weak_ptr &&r) noexcept {
        if (this != &r) {
            weak_ptr(std::move(r)).swap(*this);
        }
        return *this;
    }

    //5
    template<class Y>
    weak_ptr &operator=(weak_ptr<Y> &&r) noexcept {
        if (this != &r) {
            weak_ptr(std::move(r)).swap(*this);
        }
        return *this;
    }

    ~weak_ptr() {
        if (control_block != nullptr) {
            control_block->release_ref();
        }
    }

    void swap(weak_ptr &r) noexcept {
        std::swap(control_block, r.control_block);
        std::swap(pointer, r.pointer);
    }

    shared_ptr<T> lock() const noexcept {
        shared_ptr<T> new_shared_ptr;
        if (control_block != nullptr && control_block->get_ref_count() != 0) {
            new_shared_ptr.pointer = pointer;
            new_shared_ptr.control_block = control_block;
            control_block->add_ref();
        }
        return new_shared_ptr;
    }

private:
    T *pointer;
    control_block *control_block;
};