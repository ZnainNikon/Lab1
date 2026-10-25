#ifndef LIST_SEQUENCE_TPP
#define LIST_SEQUENCE_TPP

template <class T>
Sequence<T>* ListSequence<T>::AppendImpl(const T& item)
{
    items->Append(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAtImpl(const T& item, size_t index)
{
    if (index > items->GetLength())
    {
        throw IndexOutOfRangeException("Индекс выходит за границы в ListSequence::InsertAtImpl");
    }
    items->InsertAt(item, index);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::PrependImpl(const T& item)
{
    items->Prepend(item);
    return this;
}

template <class T>
ListSequence<T>::ListSequence() : items(new LinkedList<T>()) {}

template <class T>
ListSequence<T>::ListSequence(T* arr, size_t count) : items(new LinkedList<T>(arr, count)) {}

template <class T>
ListSequence<T>::ListSequence(const LinkedList<T>& list) : items(new LinkedList<T>(list)) {}

template <class T>
ListSequence<T>::ListSequence(const ListSequence<T>& other) : items(new LinkedList<T>(*other.items)) {}

template <class T>
ListSequence<T>::ListSequence(std::initializer_list<T> list) : items(new LinkedList<T>())
{
    for (const T& val : list)
    {
        items->Append(val);
    }
}

template <class T>
ListSequence<T>::~ListSequence()
{
    delete items;
}

template <class T>
T ListSequence<T>::GetFirst() const
{
    if (items->GetLength() == 0)
    {
        throw SequenceEmptyException("ListSequence пуста - невозможно получить первый элемент");
    }
    return items->GetFirst();
}

template <class T>
T ListSequence<T>::GetLast() const
{
    if (items->GetLength() == 0)
    {
        throw SequenceEmptyException("ListSequence пуста - невозможно получить последний элемент");
    }
    return items->GetLast();
}

template <class T>
T ListSequence<T>::Get(size_t index) const
{
    if (index >= items->GetLength())
    {
        throw IndexOutOfRangeException("Индекс выходит за границы в ListSequence::Get");
    }
    return items->Get(index);
}

template <class T>
Sequence<T>* ListSequence<T>::GetSubsequence(size_t start, size_t end) const
{
    size_t length = items->GetLength();
    if (start > end || end >= length)
    {
        throw IndexOutOfRangeException("Неверные начальный или конечный индексы в ListSequence::GetSubsequence");
    }
    
    ListSequence<T>* result = new ListSequence<T>();
    for (size_t i = start; i <= end; i++)
    {
        result->items->Append(items->Get(i));
    }
    return result;
}

template <class T>
size_t ListSequence<T>::GetLength() const
{
    return items->GetLength();
}

template <class T>
Sequence<T>* ListSequence<T>::Concat(Sequence<T>* list)
{
    if (list == nullptr)
    {
        throw NullPointerException("Невозможно объединить с нулевым указателем в ListSequence::Concat");
    }
    
    ListSequence<T>* result = new ListSequence<T>(*this);
    for (size_t i = 0; i < list->GetLength(); i++)
    {
        result->items->Append(list->Get(i));
    }
    return result;
}

#endif