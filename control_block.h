#pragma once

#include <cstddef>

class control_block {
public:
    control_block();

    void release_ref();
    void release_weak() noexcept;

    void add_ref() noexcept;
    void add_weak() noexcept;

    size_t get_ref_count() const noexcept;
    size_t get_weak_count() const noexcept;

    virtual ~control_block() = default;
    virtual void delete_object() = 0;

private:
    size_t ref_count;
    size_t weak_count;
};