#pragma once

#include "control_block.h"

template<class Y>
class control_block_object : public control_block {
public:
    template<class... Args>
    explicit control_block_object(Args &&... args) {
        new(&data) Y(std::forward<Args>(args)...);
    }

    Y *get_pointer() {
        return reinterpret_cast<Y *>(&data);
    }

    void delete_object() override {
        get_pointer()->~Y();
    }

private:
    typename std::aligned_storage_t<sizeof(Y), alignof(Y)>::type data;
};