#pragma once
#include "basic_queue.h"
#include <mutex>
#include <queue>
#include <condition_variable> 

template<typename T>
class std_queue_with_mutex : public basic_queue<T> {
private:
    mutable std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cond; 
    
public:
    void push(const T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(value);
        m_cond.notify_one();
    }
    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]() { return !m_queue.empty(); }); 
        T item = m_queue.front(); 
        m_queue.pop(); 
        return item; 
    }
};