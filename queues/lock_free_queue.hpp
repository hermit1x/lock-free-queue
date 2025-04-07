#pragma once

#include <atomic>
#include <memory>

template<typename T>
struct lock_free_node {
    T data;
    std::atomic<lock_free_node<T>*> next;

    lock_free_node(const T& value) : data(value), next(nullptr) {}
    lock_free_node(T&& value) : data(std::move(value)), next(nullptr) {}
};

template<typename T>
class lock_free_queue {
private:
    std::atomic<lock_free_node<T>*> head;
    std::atomic<lock_free_node<T>*> tail;

public:
    lock_free_queue() {
        lock_free_node<T>* dummy = new lock_free_node<T>(T());
        head.store(dummy, std::memory_order_relaxed);
        tail.store(dummy, std::memory_order_relaxed);
    }

    ~lock_free_queue() {
        T val;
        while (try_pop(val));
        delete head.load(std::memory_order_relaxed);
    }

    void push(const T& value) {
        lock_free_node<T>* newNode = new lock_free_node<T>(value);
        lock_free_node<T>* currentTail = nullptr;
        lock_free_node<T>* currentNext = nullptr;

        while (true) {
            currentTail = tail.load(std::memory_order_acquire);
            currentNext = currentTail->next.load(std::memory_order_acquire);

            if (currentTail != tail.load(std::memory_order_relaxed)) continue;
            if (currentNext != nullptr) {
                tail.compare_exchange_weak(
                    currentTail,
                    currentNext,
                    std::memory_order_release,
                    std::memory_order_relaxed
                );
                continue;
            }
            if (currentTail->next.compare_exchange_weak(
                    currentNext,
                    newNode,
                    std::memory_order_release,
                    std::memory_order_relaxed
                )
            ) {
                break;
            }
        }

        tail.compare_exchange_weak(
            currentTail,
            newNode,
            std::memory_order_release,
            std::memory_order_relaxed
        );
    }


    bool try_pop(T& value) {
        lock_free_node<T>* currentHead = nullptr;
        lock_free_node<T>* currentTail = nullptr;
        lock_free_node<T>* nextNode = nullptr;

        while (true) {
            currentHead = head.load(std::memory_order_acquire);
            currentTail = tail.load(std::memory_order_acquire);
            nextNode = currentHead->next.load(std::memory_order_acquire);

            if (currentHead != head.load(std::memory_order_relaxed)) continue;

            if (currentHead == currentTail) {
                if (nextNode == nullptr) {
                    return false;
                }
                tail.compare_exchange_weak(
                    currentTail,
                    nextNode, 
                    std::memory_order_release,
                    std::memory_order_relaxed
                );
            }
            else {
                value = nextNode->data;

                if (head.compare_exchange_weak(
                    currentHead,
                    nextNode,
                    std::memory_order_release,
                    std::memory_order_relaxed
                )) {
                    break;
                }
            }
        }

        delete currentHead;
        return true;
    }
};