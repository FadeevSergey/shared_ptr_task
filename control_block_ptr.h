#pragma once

#include "control_block.h"

template<class Y, class Deleter>
class control_block_ptr : control_block, Deleter {
public:
    control_block_ptr(Y *pointer, Deleter deleter) : pointer(pointer), Deleter(std::move(deleter)) {
        add_ref();
    }

    void delete_object() override {
        static_cast<Deleter&>(*this)(pointer);
    }
private:
    Y *pointer;
};