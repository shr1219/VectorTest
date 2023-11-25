#include<iostream>
using namespace std;

int main() {
    auto ptr = shared_ptr<int>();
    if (ptr)
        cout << "yes";
    else
        cout <<"no";
    return 0;

}