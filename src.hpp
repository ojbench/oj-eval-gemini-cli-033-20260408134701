#pragma once
#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <memory>
#include <cstddef>
#include <stdexcept>

namespace sjtu {

template <typename T>
class list {
protected:
    struct node_base {
        node_base* prev;
        node_base* next;
        node_base() : prev(this), next(this) {}
    };

    struct node : public node_base {
        alignas(T) unsigned char data[sizeof(T)];
        T* valptr() { return reinterpret_cast<T*>(data); }
        const T* valptr() const { return reinterpret_cast<const T*>(data); }
    };

    node_base dummy;
    size_t sz;

    void init() {
        dummy.prev = dummy.next = &dummy;
        sz = 0;
    }

public:
    class const_iterator;
    class iterator {
        friend class list<T>;
        friend class const_iterator;
    private:
        node_base* ptr;
    public:
        iterator(node_base* p = nullptr) : ptr(p) {}
        
        iterator &operator++() {
            ptr = ptr->next;
            return *this;
        }
        iterator &operator--() {
            ptr = ptr->prev;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        T &operator*() const noexcept {
            return *(static_cast<node*>(ptr)->valptr());
        }
        T *operator->() const noexcept {
            return static_cast<node*>(ptr)->valptr();
        }

        friend bool operator==(const iterator &a, const iterator &b) {
            return a.ptr == b.ptr;
        }
        friend bool operator!=(const iterator &a, const iterator &b) {
            return a.ptr != b.ptr;
        }
    };

    class const_iterator {
        friend class list<T>;
    private:
        const node_base* ptr;
    public:
        const_iterator(const node_base* p = nullptr) : ptr(p) {}
        const_iterator(const iterator& other) : ptr(other.ptr) {}
        
        const_iterator &operator++() {
            ptr = ptr->next;
            return *this;
        }
        const_iterator &operator--() {
            ptr = ptr->prev;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        const T &operator*() const noexcept {
            return *(static_cast<const node*>(ptr)->valptr());
        }
        const T *operator->() const noexcept {
            return static_cast<const node*>(ptr)->valptr();
        }

        friend bool operator==(const const_iterator &a, const const_iterator &b) {
            return a.ptr == b.ptr;
        }
        friend bool operator!=(const const_iterator &a, const const_iterator &b) {
            return a.ptr != b.ptr;
        }
    };

    list() {
        init();
    }

    list(const list &other) {
        init();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }

    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
        return *this;
    }

    ~list() {
        clear();
    }

    T &front() noexcept {
        return *(static_cast<node*>(dummy.next)->valptr());
    }
    T &back() noexcept {
        return *(static_cast<node*>(dummy.prev)->valptr());
    }
    const T &front() const noexcept {
        return *(static_cast<const node*>(dummy.next)->valptr());
    }
    const T &back() const noexcept {
        return *(static_cast<const node*>(dummy.prev)->valptr());
    }

    iterator begin() noexcept { return iterator(dummy.next); }
    const_iterator cbegin() const noexcept { return const_iterator(dummy.next); }
    iterator end() noexcept { return iterator(&dummy); }
    const_iterator cend() const noexcept { return const_iterator(&dummy); }

    bool empty() const noexcept { return sz == 0; }
    size_t size() const noexcept { return sz; }

    void clear() noexcept {
        node_base* curr = dummy.next;
        while (curr != &dummy) {
            node_base* next = curr->next;
            node* n = static_cast<node*>(curr);
            n->valptr()->~T();
            delete n;
            curr = next;
        }
        init();
    }

    iterator insert(iterator pos, const T &value) {
        node* new_node = new node();
        try {
            new (new_node->valptr()) T(value);
        } catch (...) {
            delete new_node;
            throw;
        }
        new_node->prev = pos.ptr->prev;
        new_node->next = pos.ptr;
        pos.ptr->prev->next = new_node;
        pos.ptr->prev = new_node;
        sz++;
        return iterator(new_node);
    }

    iterator erase(iterator pos) noexcept {
        if (pos.ptr == &dummy) return pos;
        node_base* next_node = pos.ptr->next;
        pos.ptr->prev->next = pos.ptr->next;
        pos.ptr->next->prev = pos.ptr->prev;
        node* n = static_cast<node*>(pos.ptr);
        n->valptr()->~T();
        delete n;
        sz--;
        return iterator(next_node);
    }

    void push_front(const T &value) {
        insert(begin(), value);
    }
    void push_back(const T &value) {
        insert(end(), value);
    }

    void pop_front() noexcept {
        if (!empty()) erase(begin());
    }
    void pop_back() noexcept {
        if (!empty()) erase(iterator(dummy.prev));
    }
};

} // namespace sjtu

#endif // SJTU_LIST_HPP