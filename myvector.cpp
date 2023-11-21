#include <iostream>
#include <string>
using namespace std;


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