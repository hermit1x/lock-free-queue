#pragma once
#include <mutex>
#include <queue>
#include <chrono>
#include <condition_variable> 

template<typename T>
class std_queue_with_mutex {
private:
    mutable std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cond; 
    
public:
    std_queue_with_mutex() {}
    std_queue_with_mutex(const std_queue_with_mutex&) = delete;
    std_queue_with_mutex& operator=(const std_queue_with_mutex&) = delete;

    void push(T&& value) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(value));
        }
        m_cond.notify_one();
    }

    void push(const T& value) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(value);
        }
        m_cond.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this] { return !m_queue.empty(); });

        T value = std::move(m_queue.front());
        m_queue.pop();
        return value;
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }

        value = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
};