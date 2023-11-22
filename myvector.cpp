#include <iostream>
#include <string>
using namespace std;

template <typename T>
class SimpleVector
{
public:
    SimpleVector() : elements(nullptr), first_free(nullptr), cap(nullptr){};
    SimpleVector(const SimpleVector &other){
        auto res = alloc_n_copy(other.begin(), other.end());
        elements = res.first;
        cap = first_free = res.second;
    }
    SimpleVector(SimpleVector &&other) noexcept {
        elements = other.elements;
        first_free = other.first_free;
        cap = other.cap;
        other.elements = other.first_free = other.cap = nullptr;
    }
    SimpleVector &operator=(const SimpleVector &other) {
        if (this != &other) {
            auto res = alloc_n_copy(other.begin(), other.end());
            free();
            elements = res.first;
            cap = first_free = res.second;
        }
        return *this;
    }
    SimpleVector &operator=(SimpleVector &&other) noexcept {
        if (this != &other) {
            free();
            elements = other.elements;
            first_free = other.first_free;
            cap = other.cap;
            other.elements = other.first_free = other.cap = nullptr;
        }
        return *this;
    }
    size_t size() const {
      return first_free - elements;
    }
    size_t capacity() const {
        return cap - elements;
    }
    T *begin() const {
        return elements;
    }
    T *end() const {
        return first_free;
    }
    void push_back(const T & val) {
        chk_n_alloc();
        allocator_traits<allocator<T>>::construct(alloc, first_free++, val);
    }
    void push_back(T &&val) {
        chk_n_alloc();
        allocator_traits<allocator<T>>::construct(alloc, first_free++, std::move(val));
    }
    void pop_back() {
        if (first_free == elements)
            return;
        allocator_traits<allocator<T>>::destroy(alloc, --first_free);
    }
    void pop_back(T & val) {
        if (first_free == elements)
            return;
        val = *(--first_free);
        allocator_traits<allocator<T>>::destroy(alloc, first_free);
    }
    template <typename... Args>
    void emplace_back(Args &&...args) {
        chk_n_alloc();
        allocator_traits<allocator<T>>::construct(alloc, first_free++, std::forward<Args>(args)...);
    }

    ~SimpleVector() {
        free();
    }

private:
    T *elements;
    T *first_free;
    T *cap;
    static allocator<T> alloc;
    void chk_n_alloc();
    pair<T *, T *> alloc_n_copy(const T *, const T *);
    void free();
    void reallocate();
};

// static member initialization
template <typename T>
allocator<T> SimpleVector<T>::alloc;

template <typename T>
void SimpleVector<T>::chk_n_alloc() {
    if (first_free == cap)
        reallocate();
}

template <typename T>
void SimpleVector<T>::reallocate()
{
    size_t new_capacity = capacity() ? 2 * capacity() : 1;
    T *new_elements = allocator_traits<allocator<T>>::allocate(alloc, new_capacity);
    T *dest = new_elements;
    for (T *start = elements; start != cap; ++start)
        allocator_traits<allocator<T>>::construct(alloc, dest++, std::move(*start));
    free();
    elements = new_elements;
    first_free = dest;
    cap = elements + new_capacity;
}

template <typename T>
void SimpleVector<T>::free()
{
    if (elements)
    {
        for (T *start = elements; start != first_free; ++start)
            allocator_traits<allocator<T>>::destroy(alloc, start);
        allocator_traits<allocator<T>>::deallocate(alloc, elements, cap - elements);
        elements = first_free = cap = nullptr;
    }
}

template <typename T>
pair<T*, T*> SimpleVector<T>::alloc_n_copy(const T* org_start, const T* org_end) {
    T* data = allocator_traits<allocator<T>>::allocate(alloc, org_end - org_start);
    return {data, uninitialized_copy(org_start, org_end, data)};
}


int main()
{
    SimpleVector<string> sv;
    sv.emplace_back(10,'s');
    cout << *(sv.begin()) << endl;
    return 0;
}