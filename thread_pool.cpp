#include <iostream>
#include <thread>
#include <future>
#include "myqueue.h"
using namespace std;

class ThreadPool {
public:
    static ThreadPool& instance() {
        static ThreadPool ins;
        return ins;
    }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    void stop() {
        isStop = true;
        tasks.shutdown(); //保证被阻塞的线程退出
        for (auto& t: threads)
            if (t.joinable())
                t.join();
    }
    using Task = function<void()>;
    template <typename F, typename... Args>
    auto commit(F&& f, Args&& ...args) -> future<decltype(f(args...))>{
        using RetType = decltype(f(args...));
        function<RetType()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = make_shared<packaged_task<RetType()>>(func);
        future<RetType> res = task_ptr->get_future();
        Task task = [task_ptr] {
            (*task_ptr)();
        };
        tasks.push(task);
        return res;
    }


private:
    ThreadPool(const int num = 8):isStop(false) {
        for (int i = 0; i < num; ++i) {
            threads.emplace_back([this]{
                while(!isStop) {
                    auto task_ptr = tasks.pop();
                    if (task_ptr)
                        try {
                            (*task_ptr)();
                        }
                        catch(exception e) {
                            cout << "catch ";
                        }
                }
            });
        }
    }
    ~ThreadPool() {
        stop();
    }
    SafeQueue<Task> tasks;
    vector<thread> threads;
    bool isStop;
};


int func() {
    cout << "this is func1" << endl;
    return 10;
}



int main() {
    
    ThreadPool& tp = ThreadPool::instance();
    auto res = tp.commit(func);
    cout << res.get();
    return 0;
    

}