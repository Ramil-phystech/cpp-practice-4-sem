#include <iostream>
#include <cassert>


template <typename T>
struct less_than_comparable {
    friend bool operator>(const T& lhs, const T& rhs) {
        return rhs < lhs;
    }
    friend bool operator<=(const T& lhs, const T& rhs) {
        return !(rhs < lhs);
    }
    friend bool operator>=(const T& lhs, const T& rhs) {
        return !(lhs < rhs);
    }
    friend bool operator==(const T& lhs, const T& rhs) {
        return !(lhs < rhs) && !(rhs < lhs);
    }
    friend bool operator!=(const T& lhs, const T& rhs) {
        return !(lhs == rhs);
    }
};


template <typename T>
class counter {
protected:
    counter() { 
        ++instances_count; 
    }
    counter(const counter&) { 
        ++instances_count; 
    }
    counter(counter&&) noexcept { 
        ++instances_count; 
    }
    ~counter() { 
        --instances_count; 
    }

public:
    static int count() { 
        return instances_count; 
    }

private:
    static inline int instances_count{ 0 };
};


class Number: public less_than_comparable<Number>, public counter<Number> {
public:
    Number(int value): m_value{ value } {}

    int value() const { 
        return m_value; 
    }

    bool operator<(Number const& other) const {
        return m_value < other.m_value;
    }

private:
    int m_value;
};


int main() {
    Number one{ 1 };
    Number two{ 2 };
    Number three{ 3 };
    Number four{ 4 };

    assert(one >= one);
    assert(three <= four);
    assert(two == two);
    assert(three > two);
    assert(one < two);

    std::cout << "Count: " << counter<Number>::count() << std::endl;

    return 0;
}
