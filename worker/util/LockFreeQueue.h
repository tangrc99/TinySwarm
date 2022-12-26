//
// Created by 唐仁初 on 2022/6/27.
//

#ifndef MONGO_LOCKFREEQUEUE_H
#define MONGO_LOCKFREEQUEUE_H

#include <memory>


template<typename T>
class LockFreeQueue {
private:
    struct node {
        std::shared_ptr<T> data;
        node *next;

        explicit node() : next(nullptr) {}
    };

    std::atomic<node *> head;
    std::atomic<node *> tail;

    node *pop_head() {
        node *const old_head = head.load();
        if (old_head == tail.load())
            return nullptr;
        head.store(old_head->next);
        return old_head;
    }

public:
    LockFreeQueue() noexcept: head(new node), tail(head.load()) {}

    LockFreeQueue(const LockFreeQueue &other) = delete;

    LockFreeQueue &operator=(const LockFreeQueue &other) = delete;

    ~LockFreeQueue() {
        while (node *const old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
    }

    std::shared_ptr<T> pop() {
        node *old_head = pop_head();
        if (!old_head) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(old_head->data);
        delete old_head;
        return res;
    }

    // 如果调用的时候使用std::move，则为浅拷贝，大大加快调用速度
    void push(T new_value) {
        std::shared_ptr<T> new_data = std::make_shared<T>(std::move(new_value));
        node *p = new node;
        node *const old_tail = tail.load();
        old_tail->data.swap(new_data);
        old_tail->next = p;
        tail.store(p);
    }

};


#endif //MONGO_LOCKFREEQUEUE_H
