#include "control_block.h"

control_block::control_block() : ref_count(0), weak_count(0) {}

void control_block::add_ref() noexcept{
    ++ref_count;
    add_weak();
}

void control_block::add_weak() noexcept{
    ++weak_count;
}

size_t control_block::get_ref_count() const noexcept {
    return ref_count;
}

size_t control_block::get_weak_count() const noexcept {
    return weak_count;
}

void control_block::release_ref() {
    --ref_count;
    if(ref_count == 0) {
        delete_object();
    }
    release_weak();
}

void control_block::release_weak() {
    --weak_count;
    if(weak_count == 0) {
        delete this;
    }
}
