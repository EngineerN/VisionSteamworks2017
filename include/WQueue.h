#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

template <typename T> class wqueue
{ 
    std::queue<std::unique_ptr<T>>   m_queue;
    std::mutex m_mutex;
    std::condition_variable  m_cv;

public:
    wqueue() {}
    ~wqueue() {}
    void add(T item) {
        std::unique_ptr<T> item_ptr = std::make_unique<T>(item);
        std::lock_guard<std::mutex> mutex_lock(m_mutex);
        m_queue.push(std::move(item_ptr));
        m_cv.notify_one();
    }
    std::unique_ptr<T> remove() {
        std::unique_lock<std::mutex> mutex_lock(m_mutex);
        while (m_queue.empty()) {
            m_cv.wait(mutex_lock);
        }
        std::unique_ptr<T> item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }
    int size() {
        std::unique_lock<std::mutex> mutex_lock(m_mutex);
        int size = m_queue.size();
        return size;
    }
};
