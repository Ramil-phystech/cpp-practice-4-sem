#include <iostream>
#include <vector>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <clocale>


template <typename T>
class ISetImpl {
public:
    virtual ~ISetImpl() = default;

    virtual void add(const T& element) = 0;
    virtual void remove(const T& element) = 0;
    virtual bool contains(const T& element) const = 0;
    virtual size_t size() const = 0;

    virtual std::vector<T> to_vector() const = 0;
    virtual bool is_array_based() const = 0;
};


template <typename T>
class ArraySetImpl : public ISetImpl<T> {
private:
    std::vector<T> data;
public:
    void add(const T& element) override {
        if (!contains(element)) {
            data.push_back(element);
        }
    }

    void remove(const T& element) override {
        auto it = std::find(data.begin(), data.end(), element);
        if (it != data.end()) {
            data.erase(it);
        }
    }

    bool contains(const T& element) const override {
        return std::find(data.begin(), data.end(), element) != data.end();
    }

    size_t size() const override { 
        return data.size(); 
    }

    std::vector<T> to_vector() const override { 
        return data; 
    }

    bool is_array_based() const override { 
        return true; 
    }
};


template <typename T>
class HashSetImpl : public ISetImpl<T> {
private:
    std::unordered_set<T> data;
public:
    void add(const T& element) override {
        data.insert(element);
    }

    void remove(const T& element) override {
        data.erase(element);
    }

    bool contains(const T& element) const override {
        return data.find(element) != data.end();
    }

    size_t size() const override { 
        return data.size(); 
    }

    std::vector<T> to_vector() const override {
        return std::vector<T>(data.begin(), data.end());
    }

    bool is_array_based() const override { 
        return false; 
    }
};


template <typename T>
class Set {
private:
    std::unique_ptr<ISetImpl<T>> impl;
    size_t threshold;

    void update_implementation() {
        size_t current_size = impl->size();
        bool is_array = impl->is_array_based();

        if (is_array && current_size >= threshold) {
            auto elements = impl->to_vector();
            impl = std::make_unique<HashSetImpl<T>>();
            for (const auto& el : elements) {
                impl->add(el);
            }
        }
        else if (!is_array && current_size < threshold) {
            auto elements = impl->to_vector();
            impl = std::make_unique<ArraySetImpl<T>>();
            for (const auto& el : elements) {
                impl->add(el);
            }
        }
    }

public:
    explicit Set(size_t switch_threshold = 10)
        : impl(std::make_unique<ArraySetImpl<T>>()), threshold(switch_threshold) {}

    Set(const Set& other) : threshold(other.threshold) {
        if (other.impl->is_array_based()) {
            impl = std::make_unique<ArraySetImpl<T>>();
        }
        else {
            impl = std::make_unique<HashSetImpl<T>>();
        }
        for (const auto& el : other.impl->to_vector()) {
            impl->add(el);
        }
    }

    Set& operator=(const Set& other) {
        if (this != &other) {
            Set temp(other);             
            std::swap(impl, temp.impl);  
            threshold = other.threshold;
        }
        return *this;
    }

    void add(const T& element) {
        impl->add(element);
        update_implementation();
    }

    void remove(const T& element) {
        impl->remove(element);
        update_implementation();
    }

    bool contains(const T& element) const {
        return impl->contains(element);
    }

    Set<T> unite(const Set<T>& other) const {
        Set<T> result(*this); 

        for (const auto& el : other.impl->to_vector()) {
            result.add(el);   
        }

        return result;
    }

    Set<T> intersect(const Set<T>& other) const {
        Set<T> result(threshold);
        for (const auto& el : impl->to_vector()) {
            if (other.contains(el)) {
                result.add(el);
            }
        }
        return result;
    }

    void print_state() const {
        std::cout << "Размер: " << impl->size() << " | Структура: "
            << (impl->is_array_based() ? "std::vector" : "std::unordered_set")
            << "\n";
    }
};


int main() {
    setlocale(LC_ALL, "Russian");
    Set<int> my_set(5);

    std::cout << "Добавляем элементы\n";
    for (int i = 1; i <= 4; ++i) {
        my_set.add(i);
        my_set.print_state();
    }

    std::cout << "\nПревышаем порог (добавляем 5-й элемент)...\n";
    my_set.add(5);
    my_set.print_state(); 

    std::cout << "\nУдаляем элементы...\n";
    my_set.remove(5);
    my_set.print_state();

    std::cout << "\nПроверка объединения множеств:\n";
    Set<int> set_a(5); 
    set_a.add(1); 
    set_a.add(2);

    Set<int> set_b(5); 
    set_b.add(2); 
    set_b.add(3); 
    set_b.add(4); 
    set_b.add(5); 
    set_b.add(6);

    Set<int> union_set = set_a.unite(set_b);
    std::cout << "Результат объединения: ";
