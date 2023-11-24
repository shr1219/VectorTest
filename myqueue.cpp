#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;


template <typename T>
class SafeQueue {
public:
    SafeQueue(){};
    SafeQueue(const SafeQueue& rhs) = delete;
    SafeQueue& operator= (const SafeQueue& rhs) = delete;
    SafeQueue(SafeQueue&& rhs) = delete;
    SafeQueue& operator= (SafeQueue&& rhs) = delete;
    ~SafeQueue() = default;
    size_t size() const;
    bool empty() const;
    void push(const T&);
    void push(T&&);
    shared_ptr<T> pop();
    shared_ptr<T> try_pop();
private:
    queue<T> data;
    mutex mtx;
    condition_variable cv;
};

template <typename T>
inline size_t SafeQueue<T>::size() const {
    lock_guard lg(mtx);
    return data.size();
}

template <typename T>
inline bool SafeQueue<T>::empty() const {
    lock_guard lg(mtx);
    return data.empty();
}


template <typename T>
inline void SafeQueue<T>::push(const T& ele) {
    {
        lock_guard lg(mtx);
        data.push(ele);
    }
    cv.notify_one();
}

template <typename T>
inline void SafeQueue<T>::push(T&& ele) {
    {
        lock_guard lg(mtx);
        data.push(std::move(ele));
    }
    cv.notify_one();
}


template <typename T>
inline shared_ptr<T> SafeQueue<T>::try_pop() {
    lock_guard lg(mtx);
    if (data.empty())
        return make_shared<T>();
    auto ele = make_shared<T>(std::move(data.front()));
    data.pop();
    return ele;
}

template <typename T>
inline shared_ptr<T> SafeQueue<T>::pop() {
    unique_lock lg(mtx);
    cv.wait(lg, [this] {
        return !data.empty();
    });
    auto ele = make_shared<T>(std::move(data.front()));
    data.pop();
    return ele;
}


SafeQueue<int> sq;
void producer() {
    for (int i = 0; i < 100; ++i) {
        sq.push(i);
        
        cout << "produce " << i << endl;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}
void consumer() {
    while(1) {
        auto ele = sq.pop();
        
        cout << "consume " << *ele << endl;
        this_thread::sleep_for(chrono::milliseconds(300));
    }
}
int main() {
    
    thread t1(producer);
    thread t2(consumer);
    t1.join();
    t2.join();
    
    return 0;
}
