#pragma once

#include "Fluorescent.h"

template <typename T>
class Optional {
public:
    // The default constructor obviously sets no value.
    // FIXME: Maybe have some sort of `Default` system like Rust's `Default` trait?
    Optional()
        : m_is_set(false)
    {
    }

    // FIXME: Let's try to support moving values around.
    Optional(T& value)
        : m_value(value)
    {
    }

    // Returns a reference to the underlying value of this optional.
    // Please check if the optional has a value before doing this, otherwise you may reach unexpected behavior.
    T& get() { return m_value; }

    // It's recommended to use the `bool` operator instead.
    bool is_set() { return m_is_set; }

    // Allow assigning values into the optional.
    void operator=(T& value)
    {
        // FIXME: Let's try to support moving values around.
        m_value = value;
        m_is_set = true;
    }

    // Allow checking the set state via `if (optional)`.
    operator bool() const { return m_is_set; }

private:
    T m_value {};
    bool m_is_set { false };
};
