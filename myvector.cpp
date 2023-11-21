#include <iostream>
#include <string>
using namespace std;

class StringVector
{
public:
    StringVector() : elements(nullptr), first_free(nullptr), cap(nullptr){}; // default construct
    StringVector(const StringVector &);                                      // copy construct
    StringVector &operator=(const StringVector &);                           // copy assign
    StringVector(StringVector &&);                                           // move construct
    StringVector &operator=(StringVector &&);                                // move assign
    ~StringVector();                                                         // deconstruct
    size_t size() const
    {
        return first_free - elements;
    }
    size_t capacity() const
    {
        return cap - elements;
    }
    void push_back(const string &);
    void push_back(string &&);
    string *begin() const
    {
        return elements;
    };
    string *end() const
    {
        return first_free;
    };

private:
    string *elements;
    string *first_free;
    string *cap;
    static allocator<string> alloc;
    void chk_n_alloc();
    pair<string *, string *> alloc_n_copy(const string *, const string *);
    void free();
    void reallocate();
};

inline void StringVector::chk_n_alloc()
{
    if (first_free == cap)
        reallocate();
}

void StringVector::reallocate()
{
    size_t new_capacity = capacity() ? 2 * capacity() : 1;
    string *new_elements = alloc.allocate(new_capacity);
    string *dest = new_elements;
    for (string *start = elements; start != cap; ++start)
        alloc.construct(dest++, move(*start));
    free();
    elements = new_elements;
    first_free = dest;
    cap = elements + new_capacity;
}

void StringVector::free()
{
    if (elements)
    {
        for (string *start = elements; start != first_free; ++start)
            alloc.destroy(start);
        alloc.deallocate(elements, cap - elements);
        elements = first_free = cap = nullptr;
    }
}

template <typename T>
class SimpleVector
{
public:
    SimpleVector() : ele(nullptr), last(nullptr), capcity(nullptr){};
    size_t size() const
    {
        return last - ele;
    };

private:
    T *ele;
    T *last;
    T *capcity;
    static allocator<T> alloc;
};

int main()
{
    SimpleVector<int> sv;
    cout << sv.size() << endl;
    return 0;
}