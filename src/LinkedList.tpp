#ifndef LINKED_LIST_TPP
#define LINKED_LIST_TPP

template <class T>
Node<T>::Node() {}

template <class T>
Node<T>::Node(const T& key) : key(key), next(nullptr) {}

template <class T>
size_t LinkedList<T>::calculateLength() const
{
    size_t len = 0;
    Node<T>* current = head;
    while (current != nullptr)
    {
        len++;
        current = current->next;
    }
    return len;
}

template <class T>
Node<T>* LinkedList<T>::getNode(size_t index) const
{
    Node<T>* current = head;
    for (size_t i = 0; i < index && current != nullptr; i++)
    {
        current = current->next;
    }
    return current;
}

template <class T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr) {}

template <class T>
LinkedList<T>::LinkedList(T* items, size_t count) : head(nullptr), tail(nullptr)
{
    for (size_t i = 0; i < count; i++)
    {
        Append(items[i]);
    }
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T>& list) : head(nullptr), tail(nullptr)
{
    Node<T>* current = list.head;
    while (current != nullptr)
    {
        Append(current->key);
        current = current->next;
    }
}

template <class T>
LinkedList<T>::LinkedList(std::initializer_list<T> list) : head(nullptr), tail(nullptr)
{
    for (const T& val : list)
    {
        Append(val);
    }
}

template <class T>
LinkedList<T>::~LinkedList()
{
    ClearList();
}

template <class T>
T LinkedList<T>::GetFirst() const
{
    if (head == nullptr)
    {
        throw SequenceEmptyException("Список пуст");
    }
    return head->key;
}

template <class T>
T LinkedList<T>::GetLast() const
{
    if (tail == nullptr)
    {
        throw SequenceEmptyException("Список пуст");
    }
    return tail->key;
}

template <class T>
T LinkedList<T>::Get(size_t index) const
{
    Node<T>* node = getNode(index);
    if (node == nullptr)
    {
        throw IndexOutOfRangeException("Индекс выходит за границы");
    }
    return node->key;
}

template <class T>
LinkedList<T>* LinkedList<T>::GetSubList(size_t start, size_t end) const
{
    size_t length = calculateLength();
    if (start > end || end >= length)
    {
        throw IndexOutOfRangeException("Неверные начальный или конечный индексы");
    }
    
    LinkedList<T>* result = new LinkedList<T>();
    for (size_t i = start; i <= end; i++)
    {
        result->Append(Get(i));
    }
    return result;
}

template <class T>
size_t LinkedList<T>::GetLength() const
{
    return calculateLength();
}

template <class T>
void LinkedList<T>::Append(const T& item)
{
    Node<T>* newNode = new Node<T>(item);
    if (head == nullptr)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }
}

template <class T>
void LinkedList<T>::Prepend(const T& item)
{
    Node<T>* newNode = new Node<T>(item);
    if (head == nullptr)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        newNode->next = head;
        head = newNode;
    }
}

template <class T>
void LinkedList<T>::InsertAt(const T& item, size_t index)
{
    size_t length = calculateLength();
    if (index > length)
    {
        throw IndexOutOfRangeException("Индекс выходит за границы");
    }
    
    if (index == 0)
    {
        Prepend(item);
    }
    else if (index == length)
    {
        Append(item);
    }
    else
    {
        Node<T>* newNode = new Node<T>(item);
        Node<T>* prev = getNode(index - 1);
        if (prev != nullptr)
        {
            newNode->next = prev->next;
            prev->next = newNode;
        }
    }
}

template <class T>
LinkedList<T>* LinkedList<T>::Concat(LinkedList<T>* list) const
{
    if (list == nullptr)
    {
        throw NullPointerException("Невозможно объединить с нулевым указателем");
    }
    
    LinkedList<T>* result = new LinkedList<T>(*this);
    Node<T>* current = list->head;
    while (current != nullptr)
    {
        result->Append(current->key);
        current = current->next;
    }
    return result;
}

template <class T>
void LinkedList<T>::ClearList()
{
    Node<T>* current = head;
    while (current != nullptr)
    {
        Node<T>* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
}

#endif