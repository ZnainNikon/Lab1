#ifndef SEQUENCE_TPP
#define SEQUENCE_TPP

template <class T>
Sequence<T>* Sequence<T>::Append(T item)
{
    return AppendImpl(item);
}

template <class T>
Sequence<T>* Sequence<T>::Prepend(T item)
{
    return PrependImpl(item);
}

template <class T>
Sequence<T>* Sequence<T>::InsertAt(T item, size_t index)
{
    return InsertAtImpl(item, index);
}

#endif