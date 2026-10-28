#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <iostream>
#include <string>
#include <initializer_list>
#include <cstddef>
#include "Sequence.hpp"

template <class T> class LinkedList
{
    public:
    struct Node
    {
        T key;
        Node* next = nullptr;

        Node() = default;
        Node(const T& k) : key(k) {}
    };

    LinkedList();
    LinkedList(T* items, size_t count);
    LinkedList(const LinkedList<T>& list);
    LinkedList(std::initializer_list<T> list);
    ~LinkedList();

    T GetFirst() const;
    T GetLast() const;
    T Get(size_t index) const;
    LinkedList<T>* GetSubList(size_t start, size_t end) const;
    size_t GetLength() const;
    void Append(const T& item);
    void Prepend(const T& item);
    void InsertAt(const T& item, size_t index);
    LinkedList<T>* Concat(LinkedList<T>* list) const;
    void ClearList();

    private:
    Node* head = nullptr;
    Node* tail = nullptr;

    size_t calculateLength() const;
    Node* getNode(size_t index) const;
};

#include "LinkedList.tpp"

#endif