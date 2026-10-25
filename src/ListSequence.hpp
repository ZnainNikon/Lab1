#ifndef LIST_SEQUENCE_HPP
#define LIST_SEQUENCE_HPP

#include "Sequence.hpp"
#include "LinkedList.hpp"

template <class T> class ListSequence : public Sequence<T>
{
    private:
    LinkedList<T>* items;

    protected:
    Sequence<T>* AppendImpl(const T& item) override;
    Sequence<T>* InsertAtImpl(const T& item, size_t index) override;
    Sequence<T>* PrependImpl(const T& item) override;

    public:
    ListSequence();
    ListSequence(T* arr, size_t count);
    ListSequence(const LinkedList<T>& list);
    ListSequence(const ListSequence<T>& other);
    ListSequence(std::initializer_list<T> list);
    ~ListSequence();

    T GetFirst() const override;
    T GetLast() const override;
    T Get(size_t index) const override;
    Sequence<T>* GetSubsequence(size_t start, size_t end) const override;
    size_t GetLength() const override;
    Sequence<T>* Concat(Sequence<T>* list) override;
};

#include "ListSequence.tpp"

#endif