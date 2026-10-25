#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <iostream>
#include <string>
#include <initializer_list>
#include <cstddef>
#include "Sequence.hpp"

template <class T> class Node
{
    public:
    T key;
    Node* next{nullptr};

    Node();
    Node(const T& key);
};

template <class T> class LinkedList
{
    private:
    Node<T>* head;
    Node<T>* tail;

    size_t calculateLength() const;
    Node<T>* getNode(size_t index) const;

    public:
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
};

#include "LinkedList.tpp"

#endif