#include <mutex>
#include <condition_variable>
#include <queue>
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
    std::shared_ptr<T> pop();
    std::shared_ptr<T> try_pop();
private:
    std::queue<T> data;
    std::mutex mtx;
    std::condition_variable cv;
};