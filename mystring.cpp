#include <iostream>
#include <string>
using namespace std;

class StringVector
{
public:
    // default construct
    StringVector() : elements(nullptr), first_free(nullptr), cap(nullptr){}; 

    // copy construct
    StringVector(const StringVector & other){
        auto res = alloc_n_copy(other.begin(), other.end());
        elements = res.first;
        cap = first_free = res.second;
    };                                          

     // copy assign                             
    StringVector &operator=(const StringVector & other){
        if (this != &other) {
            auto res = alloc_n_copy(other.begin(), other.end());
            free();
            elements = res.first;
            cap = first_free = res.second;
        }
        return *this;
    };                                                

    // move construct                      
    StringVector(StringVector && other) noexcept{
        elements = other.elements;
        first_free = other.first_free;
        cap = other.cap;
        other.elements = other.first_free = other.cap = nullptr;
    };                                           

    // move assign
    StringVector &operator=(StringVector && other) noexcept{
        if (this != &other) {
            free();
            elements = other.elements;
            first_free = other.first_free;
            cap = other.cap;
            other.elements = other.first_free = other.cap = nullptr;
        }
        return *this;
    };                       
    // deconstruct
    ~StringVector(){free();};                                                    
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
    void pop_back(string &);
    void pop_back();
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





std::allocator<std::string> StringVector::alloc;
inline void StringVector::chk_n_alloc()
{
    if (first_free == cap)
        reallocate();
}

void StringVector::reallocate()
{
    size_t new_capacity = capacity() ? 2 * capacity() : 1;
    string *new_elements = allocator_traits<allocator<string>>::allocate(alloc, new_capacity);
    string *dest = new_elements;
    for (string *start = elements; start != cap; ++start)
        allocator_traits<allocator<string>>::construct(alloc, dest++, std::move(*start));
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
            allocator_traits<allocator<string>>::destroy(alloc, start);
        allocator_traits<allocator<string>>::deallocate(alloc, elements, cap - elements);
        elements = first_free = cap = nullptr;
    }
}

pair<string*, string*> StringVector::alloc_n_copy(const string* org_start, const string* org_end) {
    string* data = allocator_traits<allocator<string>>::allocate(alloc, org_end - org_start);
    return {data, uninitialized_copy(org_start, org_end, data)};
}


void StringVector::push_back(const string& str) {
    chk_n_alloc();
    allocator_traits<allocator<string>>::construct(alloc, first_free++, str);
}

void StringVector::push_back(string&& str) {
    chk_n_alloc();
    allocator_traits<allocator<string>>::construct(alloc, first_free++, std::move(str));
}

void StringVector::pop_back() {
    if (first_free == elements)
        return;
    allocator_traits<allocator<string>>::destroy(alloc, --first_free);
}

void StringVector::pop_back(string & str) {
    if (first_free == elements)
        return;
    str = *(--first_free);
    allocator_traits<allocator<string>>::destroy(alloc, first_free);
}
int main() {
    StringVector v1;
    string s1 = "456";
    
    v1.push_back(s1);
    v1.push_back("123");
    StringVector v2(std::move(v1));
    v2.pop_back(s1);
    cout << s1 << endl;
    return 0;
}