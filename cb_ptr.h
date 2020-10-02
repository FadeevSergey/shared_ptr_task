#pragma once

#include "control_block.h"

template<class Y, class Deleter>
class cb_ptr : control_block, Deleter {
public:
    cb_ptr(Y *pointer, Deleter deleter) : pointer(pointer), Deleter(std::move(deleter)) {
        add_ref();
    }

    void delete_object() override {
        static_cast<Deleter&>(*this)(pointer);
    }
private:
    Y *pointer;
};