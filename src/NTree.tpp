#ifndef NTREE_TPP
#define NTREE_TPP

// ============================================================
//  Служебные
// ============================================================

template <class T>
int NTree<T>::childCount(const Node* n)
{
    if (!n)
    {
        return 0;
    }
    int c = 0;
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        if (n->children->Get(i) != nullptr)
        {
            c++;
        }
    }
    return c;
}

template <class T>
int NTree<T>::heightOf(const Node* n)
{
    if (!n)
    {
        return 0;
    }
    int best = 0;
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        int h = heightOf(n->children->Get(i));
        if (h > best)
        {
            best = h;
        }
    }
    return best + 1;
}

template <class T>
typename NTree<T>::Node* NTree<T>::cloneNode(const Node* src)
{
    if (!src)
    {
        return nullptr;
    }
    Node* copy = new Node(src->value);
    size_t len = src->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        const Node* child = src->children->Get(i);
        if (!child)
        {
            continue;
        }
        copy->children->Append(cloneNode(child));
    }
    return copy;
}

template <class T>
void NTree<T>::destroy(Node* n)
{
    if (!n)
    {
        return;
    }
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        destroy(n->children->Get(i));
    }
    delete n;
}

template <class T>
size_t NTree<T>::countNodes(const Node* n)
{
    if (!n)
    {
        return 0;
    }
    size_t c = 1;
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        if (n->children->Get(i) != nullptr)
        {
            c += countNodes(n->children->Get(i));
        }
    }
    return c;
}

template <class T>
typename NTree<T>::Node* NTree<T>::findNode(Node* n, const T& v)
{
    if (!n)
    {
        return nullptr;
    }
    if (!(n->value < v) && !(v < n->value))
    {
        return n;
    }
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        Node* child = n->children->Get(i);
        if (!child)
        {
            continue;
        }
        Node* found = findNode(child, v);
        if (found)
        {
            return found;
        }
    }
    return nullptr;
}

template <class T>
const typename NTree<T>::Node* NTree<T>::findNode(const Node* n, const T& v)
{
    if (!n)
    {
        return nullptr;
    }
    if (!(n->value < v) && !(v < n->value))
    {
        return n;
    }
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        const Node* child = n->children->Get(i);
        if (!child)
        {
            continue;
        }
        const Node* found = findNode(child, v);
        if (found)
        {
            return found;
        }
    }
    return nullptr;
}

// ============================================================
//  Конструкторы / копирование / перемещение
// ============================================================

template <class T>
NTree<T>::NTree(int n) : maxChildren(n)
{
    if (n < 1)
    {
        throw std::invalid_argument("NTree: arity must be >= 1");
    }
}

template <class T>
NTree<T>::NTree(const NTree& other)
    : maxChildren(other.maxChildren),
      root(cloneNode(other.root)),
      sz(other.sz)
{
}

template <class T>
NTree<T>& NTree<T>::operator=(const NTree& other)
{
    if (this != &other)
    {
        clear();
        maxChildren = other.maxChildren;
        root = cloneNode(other.root);
        sz = other.sz;
    }
    return *this;
}

template <class T>
NTree<T>::NTree(NTree&& other) noexcept
    : maxChildren(other.maxChildren),
      root(other.root),
      sz(other.sz)
{
    other.root = nullptr;
    other.sz = 0;
    other.maxChildren = 0;
}

template <class T>
NTree<T>& NTree<T>::operator=(NTree&& other) noexcept
{
    if (this != &other)
    {
        clear();
        maxChildren = other.maxChildren;
        root = other.root;
        sz = other.sz;
        other.root = nullptr;
        other.sz = 0;
        other.maxChildren = 0;
    }
    return *this;
}

template <class T>
NTree<T>::~NTree()
{
    clear();
}

// ============================================================
//  Вставка (BST-спуск по диапазонам)
// ============================================================

template <class T>
typename NTree<T>::Node*
NTree<T>::insertNode(Node* n, const T& v, bool& inserted)
{
    if (!n)
    {
        inserted = true;
        return new Node(v);
    }

    // дубликат
    if (!(n->value < v) && !(v < n->value))
    {
        inserted = false;
        return n;
    }

    size_t len = n->children->GetLength();
    
    // Ищем правильную ветку для спуска
    Node* targetChild = nullptr;
    Node* previousChild = nullptr;

    for (size_t i = 0; i < len; i++)
    {
        Node* child = n->children->Get(i);
        if (!child) continue;

        // Если v меньше текущего ребенка — значит, мы нашли нужную ветку
        if (v < child->value)
        {
            targetChild = child;
            break;
        }
        
        previousChild = child;
    }

    // Если нашли подходящего ребенка, спускаемся в него
    if (targetChild != nullptr)
    {
        insertNode(targetChild, v, inserted);
        return n;
    }

    // Если не нашли ветку (v больше всех детей или детей нет), 
    // но у текущего узла есть свободное место — добавляем сюда
    if (childCount(n) < maxChildren)
    {
        Node* child = new Node(v);
        
        // Вставляем с сохранением сортировки
        size_t insertPos = n->children->GetLength();
        for (size_t i = 0; i < n->children->GetLength(); i++)
        {
            Node* current = n->children->Get(i);
            if (!current) continue;
            
            if (current->value < v)
            {
                insertPos = i + 1;
            }
            else
            {
                if (insertPos == n->children->GetLength())
                {
                    insertPos = i;
                }
                break;
            }
        }

        if (insertPos >= n->children->GetLength())
        {
            n->children->Append(child);
        }
        else
        {
            n->children->InsertAt(child, insertPos);
        }
        
        inserted = true;
        return n;
    }

    // Если места нет, спускаемся в последнего подходящего (самого правого) ребенка
    // или в первого, если предыдущего не было
    Node* fallback = previousChild;
    if (!fallback)
    {
        for (size_t i = 0; i < len; i++)
        {
            if (n->children->Get(i))
            {
                fallback = n->children->Get(i);
                break;
            }
        }
    }

    if (fallback)
    {
        insertNode(fallback, v, inserted);
    }
    else
    {
        // Страховка (не должно случиться)
        Node* child = new Node(v);
        n->children->Append(child);
        inserted = true;
    }

    return n;
}

// ============================================================
//  Удаление
// ============================================================

template <class T>
typename NTree<T>::Node*
NTree<T>::removeNode(Node* n, const T& v, bool& removed)
{
    if (!n)
    {
        return n;
    }

    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        Node* child = n->children->Get(i);
        if (!child)
        {
            continue;
        }
        if (!(child->value < v) && !(v < child->value))
        {
            // удаляем ребёнка целиком (вместе с поддеревом)
            ListSequence<Node*>* newChildren = new ListSequence<Node*>();
            for (size_t j = 0; j < len; j++)
            {
                if (j != i)
                {
                    newChildren->Append(n->children->Get(j));
                }
            }
            delete n->children;
            n->children = newChildren;

            destroy(child);
            removed = true;
            return n;
        }
    }

    len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        Node* child = n->children->Get(i);
        if (!child)
        {
            continue;
        }
        removeNode(child, v, removed);
        if (removed)
        {
            return n;
        }
    }
    return n;
}

// ============================================================
//  Обходы
// ============================================================

template <class T>
void NTree<T>::preorderFill(const Node* n, ListSequence<T>& out)
{
    if (!n)
    {
        return;
    }
    out.Append(n->value);
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        preorderFill(n->children->Get(i), out);
    }
}

template <class T>
void NTree<T>::postorderFill(const Node* n, ListSequence<T>& out)
{
    if (!n)
    {
        return;
    }
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        postorderFill(n->children->Get(i), out);
    }
    out.Append(n->value);
}

// ============================================================
//  Публичные
// ============================================================

template <class T>
void NTree<T>::insert(const T& value)
{
    bool inserted = false;
    root = insertNode(root, value, inserted);
    if (inserted)
    {
        sz++;
    }
}

template <class T>
bool NTree<T>::contains(const T& value) const
{
    return findNode(root, value) != nullptr;
}

template <class T>
void NTree<T>::remove(const T& value)
{
    bool removed = false;
    root = removeNode(root, value, removed);
    if (removed)
    {
        sz--;
    }
}

template <class T>
void NTree<T>::clear()
{
    destroy(root);
    root = nullptr;
    sz = 0;
}

template <class T>
void NTree<T>::preorder(ListSequence<T>& out) const
{
    preorderFill(root, out);
}

template <class T>
void NTree<T>::postorder(ListSequence<T>& out) const
{
    postorderFill(root, out);
}

template <class T>
void NTree<T>::levelOrder(ListSequence<T>& out) const
{
    if (!root)
    {
        return;
    }

    size_t head = 0;
    size_t tail = 0;
    size_t capacity = 16;
    const Node** queue = new const Node*[capacity];
    queue[tail++] = root;

    while (head < tail)
    {
        const Node* current = queue[head++];
        out.Append(current->value);

        size_t len = current->children->GetLength();
        for (size_t i = 0; i < len; i++)
        {
            const Node* child = current->children->Get(i);
            if (!child)
            {
                continue;
            }
            if (tail == capacity)
            {
                capacity *= 2;
                const Node** bigger = new const Node*[capacity];
                for (size_t j = 0; j < tail; j++)
                {
                    bigger[j] = queue[j];
                }
                delete[] queue;
                queue = bigger;
            }
            queue[tail++] = child;
        }
    }
    delete[] queue;
}

// ============================================================
//  map / reduce
// ============================================================

template <class T>
template <class R, class F>
NTree<R> NTree<T>::map(F f) const
{
    NTree<R> result(maxChildren);
    ListSequence<T> values;
    preorder(values);
    for (size_t i = 0; i < values.GetLength(); i++)
    {
        result.insert(f(values.Get(i)));
    }
    return result;
}

template <class T>
template <class R>
R NTree<T>::reduce(std::function<R(const T&, R)> f, R start) const
{
    R accumulator = start;
    ListSequence<T> values;
    preorder(values);
    for (size_t i = 0; i < values.GetLength(); i++)
    {
        accumulator = f(values.Get(i), accumulator);
    }
    return accumulator;
}

// ============================================================
//  Поддеревья
// ============================================================

template <class T>
NTree<T> NTree<T>::extractSubtree(const T& rootValue) const
{
    const Node* found = findNode(root, rootValue);
    if (!found)
    {
        throw std::out_of_range("extractSubtree: element not found");
    }
    NTree<T> result(maxChildren);
    result.root = cloneNode(found);
    result.sz = countNodes(result.root);
    return result;
}

template <class T>
bool NTree<T>::sameStructure(const Node* a, const Node* b)
{
    if (!a && !b)
    {
        return true;
    }
    if (!a || !b)
    {
        return false;
    }
    if (a->value != b->value)
    {
        return false;
    }
    if (childCount(a) != childCount(b))
    {
        return false;
    }

    size_t ia = 0;
    size_t ib = 0;
    size_t la = a->children->GetLength();
    size_t lb = b->children->GetLength();

    while (true)
    {
        while (ia < la && a->children->Get(ia) == nullptr)
        {
            ia++;
        }
        while (ib < lb && b->children->Get(ib) == nullptr)
        {
            ib++;
        }
        if (ia >= la || ib >= lb)
        {
            break;
        }
        if (!sameStructure(a->children->Get(ia), b->children->Get(ib)))
        {
            return false;
        }
        ia++;
        ib++;
    }
    return ia >= la && ib >= lb;
}

template <class T>
bool NTree<T>::containsSubtree(const NTree<T>& other) const
{
    if (other.empty())
    {
        return true;
    }

    std::function<bool(const Node*, const Node*)> find =
        [&](const Node* n, const Node* target) -> bool
        {
            if (!n)
            {
                return false;
            }
            if (sameStructure(n, target))
            {
                return true;
            }
            size_t len = n->children->GetLength();
            for (size_t i = 0; i < len; i++)
            {
                if (find(n->children->Get(i), target))
                {
                    return true;
                }
            }
            return false;
        };

    return find(root, other.root);
}

// ============================================================
//  Поиск по пути
// ============================================================

template <class T>
bool NTree<T>::findByPath(const std::string& path, T& out) const
{
    if (!root)
    {
        return false;
    }

    ListSequence<T> parts;
    std::istringstream reader(path);
    std::string token;
    while (std::getline(reader, token, '.'))
    {
        if (token.empty())
        {
            continue;
        }
        T value{};
        std::istringstream tokenReader(token);
        if (!(tokenReader >> value))
        {
            return false;
        }
        parts.Append(value);
    }
    if (parts.GetLength() == 0)
    {
        return false;
    }

    if (!(root->value == parts.Get(0)))
    {
        return false;
    }

    const Node* current = root;
    for (size_t i = 1; i < parts.GetLength(); i++)
    {
        const Node* next = nullptr;
        size_t len = current->children->GetLength();
        for (size_t j = 0; j < len; j++)
        {
            const Node* child = current->children->Get(j);
            if (!child)
            {
                continue;
            }
            if (child->value == parts.Get(i))
            {
                next = child;
                break;
            }
        }
        if (!next)
        {
            return false;
        }
        current = next;
    }
    out = current->value;
    return true;
}

template <class T>
bool NTree<T>::findByRelativePath(const std::string& pathFromRoot, T& out) const
{
    return findByPath(pathFromRoot, out);
}

// ============================================================
//  Сериализация
// ============================================================

template <class T>
void NTree<T>::serialize(const Node* n, std::ostream& os)
{
    if (!n)
    {
        os << "()";
        return;
    }
    os << '(' << n->value;
    size_t len = n->children->GetLength();
    for (size_t i = 0; i < len; i++)
    {
        const Node* child = n->children->Get(i);
        if (!child)
        {
            continue;
        }
        os << ' ';
        serialize(child, os);
    }
    os << ')';
}

template <class T>
typename NTree<T>::Node*
NTree<T>::deserialize(std::istream& is, int n)
{
    char c = 0;
    is >> c;
    if (c != '(')
    {
        throw std::invalid_argument("deserialize: expected '('");
    }

    is >> std::ws;
    if (is.peek() == ')')
    {
        is.get();
        return nullptr;
    }

    T value;
    if (!(is >> value))
    {
        throw std::invalid_argument("deserialize: cannot read value");
    }

    Node* node = new Node(value);
    while (true)
    {
        is >> std::ws;
        if (is.peek() == ')')
        {
            is.get();
            break;
        }
        Node* child = deserialize(is, n);
        if (child)
        {
            node->children->Append(child);
        }
    }
    return node;
}

template <class T>
std::string NTree<T>::saveToString() const
{
    std::ostringstream oss;
    serialize(root, oss);
    return oss.str();
}

template <class T>
NTree<T> NTree<T>::loadFromString(const std::string& s, int n)
{
    std::istringstream iss(s);
    NTree<T> result(n);
    result.root = deserialize(iss, n);
    result.sz = countNodes(result.root);
    return result;
}

#endif