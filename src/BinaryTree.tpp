#pragma once

template <class T>
int BinaryTree<T>::HeightOf(const Node<T>* n)
{
    if (!n) 
        return 0;
    int leftH = HeightOf(n->left);
    int rightH = HeightOf(n->right);
    return 1 + std::max(leftH, rightH);
}

template <class T>
Node<T>* BinaryTree<T>::CloneNode(const Node<T>* src)
{
    if (!src) 
        return nullptr;
    Node<T>* copy = new Node<T>(src->value);
    copy->left = CloneNode(src->left);
    copy->right = CloneNode(src->right);
    return copy;
}

template <class T>
void BinaryTree<T>::Destroy(Node<T>* n)
{
    if (!n) 
        return;
    Destroy(n->left);
    Destroy(n->right);
    delete n;
}

template <class T>
const Node<T>* BinaryTree<T>::FindNode(const Node<T>* n, const T& v)
{
    while (n)
    {
        if (v < n->value) 
            n = n->left;
        else if (n->value < v) 
            n = n->right;
        else 
            return n;
    }
    return nullptr;
}


template <class T>
BinaryTree<T>::BinaryTree(const BinaryTree& other) : root(CloneNode(other.root)), sz(other.sz) {}

template <class T>
BinaryTree<T>& BinaryTree<T>::operator=(const BinaryTree& other)
{
    if (this != &other)
    {
        Destroy(root);
        root = CloneNode(other.root);
        sz = other.sz;
    }
    return *this;
}

template <class T>
BinaryTree<T>::BinaryTree(BinaryTree&& other) noexcept : root(other.root), sz(other.sz)
{
    other.root = nullptr;
    other.sz = 0;
}

template <class T>
BinaryTree<T>& BinaryTree<T>::operator=(BinaryTree&& other) noexcept
{
    if (this != &other)
    {
        Destroy(root);
        root = other.root;
        sz = other.sz;
        other.root = nullptr;
        other.sz = 0;
    }
    return *this;
}

template <class T>
BinaryTree<T>::~BinaryTree()
{
    Destroy(root);
}


template <class T>
Node<T>* BinaryTree<T>::InsertNode(Node<T>* n, const T& v, bool& inserted)
{
    if (!n)
    {
        inserted = true;
        return new Node<T>(v);
    }
    if (v < n->value)
    {
        n->left = InsertNode(n->left, v, inserted);
    }
    else if (n->value < v)
    {
        n->right = InsertNode(n->right, v, inserted);
    }
    else
    {
        inserted = false;
    }
    return n;
}

template <class T>
Node<T>* BinaryTree<T>::RemoveNode(Node<T>* n, const T& v, bool& removed)
{
    if (!n) 
        return nullptr;

    if (v < n->value)
    {
        n->left = RemoveNode(n->left, v, removed);
    }
    else if (n->value < v)
    {
        n->right = RemoveNode(n->right, v, removed);
    }
    else
    {
        removed = true;
        if (!n->left)
        {
            Node<T>* right = n->right;
            delete n;
            return right;
        }
        if (!n->right)
        {
            Node<T>* left = n->left;
            delete n;
            return left;
        }
        Node<T>* successor = n->right;
        while (successor->left) 
        {
            successor = successor->left;
        }
        n->value = successor->value;
        n->right = RemoveNode(n->right, successor->value, removed);
    }
    return n;
}


template <class T>
void BinaryTree<T>::PreorderFill(const Node<T>* n, std::function<void(const T&)>& visitor)
{
    if (!n) 
        return;
    visitor(n->value);
    PreorderFill(n->left, visitor);
    PreorderFill(n->right, visitor);
}

template <class T>
void BinaryTree<T>::PostorderFill(const Node<T>* n, std::function<void(const T&)>& visitor)
{
    if (!n) 
        return;
    PostorderFill(n->left, visitor);
    PostorderFill(n->right, visitor);
    visitor(n->value);
}

template <class T>
void BinaryTree<T>::InorderFill(const Node<T>* n, std::function<void(const T&)>& visitor)
{
    if (!n) 
        return;
    InorderFill(n->left, visitor);
    visitor(n->value);
    InorderFill(n->right, visitor);
}


template <class T>
void BinaryTree<T>::Insert(const T& value)
{
    bool inserted = false;
    root = InsertNode(root, value, inserted);
    if (inserted) 
        sz++;
}

template <class T>
bool BinaryTree<T>::Contains(const T& value) const
{
    return FindNode(root, value) != nullptr;
}

template <class T>
void BinaryTree<T>::Remove(const T& value)
{
    bool removed = false;
    root = RemoveNode(root, value, removed);
    if (removed) 
        sz--;
}

template <class T>
void BinaryTree<T>::Clear()
{
    Destroy(root);
    root = nullptr;
    sz = 0;
}

template <class T>
void BinaryTree<T>::Preorder(std::function<void(const T&)> visitor) const
{
    PreorderFill(root, visitor);
}

template <class T>
void BinaryTree<T>::Postorder(std::function<void(const T&)> visitor) const
{
    PostorderFill(root, visitor);
}

template <class T>
void BinaryTree<T>::Inorder(std::function<void(const T&)> visitor) const
{
    InorderFill(root, visitor);
}

template <class T>
void BinaryTree<T>::LevelOrder(std::function<void(const T&)> visitor) const
{
    if (!root) 
        return;
    
    MyVector<const Node<T>*> queue;
    queue.push_back(root);
    size_t head = 0;
    while (head < queue.size())
    {
        const Node<T>* current = queue[head++];
        visitor(current->value);
        if (current->left) 
            queue.push_back(current->left);
        if (current->right) 
            queue.push_back(current->right);
    }
}


template <class T>
template <class R, class F>
BinaryTree<R> BinaryTree<T>::Map(F f) const
{
    BinaryTree<R> result;
    Preorder([&](const T& value) { 
        result.Insert(f(value)); 
    });
    return result;
}

template <class T>
BinaryTree<T> BinaryTree<T>::Where(std::function<bool(const T&)> predicate) const
{
    BinaryTree<T> result;
    Preorder([&](const T& value) {
        if (predicate(value)) 
            result.Insert(value);
    });
    return result;
}

template <class T>
template <class R>
R BinaryTree<T>::Reduce(std::function<R(const T&, R)> f, R start) const
{
    R accumulator = start;
    Preorder([&](const T& value) { 
        accumulator = f(value, accumulator); 
    });
    return accumulator;
}


template <class T>
BinaryTree<T> BinaryTree<T>::ExtractSubtree(const T& rootValue) const
{
    const Node<T>* found = FindNode(root, rootValue);
    if (!found) throw std::out_of_range("ExtractSubtree: element not found");
    BinaryTree<T> result;
    result.root = CloneNode(found);
    
    size_t count = 0;
    result.Preorder([&](const T&) { 
        count++; 
    });
    result.sz = count;
    return result;
}

template <class T>
bool BinaryTree<T>::SameStructure(const Node<T>* a, const Node<T>* b)
{
    if (!a && !b) 
        return true;
    if (!a || !b) 
        return false;
    if (!(a->value == b->value)) 
        return false;
    return SameStructure(a->left, b->left) && SameStructure(a->right, b->right);
}

template <class T>
bool BinaryTree<T>::ContainsSubtree(const BinaryTree<T>& other) const
{
    if (other.Empty()) 
        return true;
    
    std::function<bool (const Node<T>*, const Node<T>*)> find =
        [&](const Node<T>* n, const Node<T>* target) -> bool {
            if (!n) 
                return false;
            if (SameStructure(n, target)) 
                return true;
            return find(n->left, target) || find(n->right, target);
        };
    return find(root, other.root);
}

template <class T>
bool BinaryTree<T>::FindByPath(const std::string& path, T& out) const
{
    if (!root) 
        return false;
    std::istringstream reader(path);
    std::string token;
    const Node<T>* current = root;
    bool first = true;
    while (std::getline(reader, token, '.'))
    {
        if (token.empty()) 
            continue;
        T value {};
        std::istringstream TokenReader(token);
        if (!(TokenReader >> value))    
            return false;
        if (first)
        {
            if (!(current->value == value)) 
                return false;
            first = false;
        }
        else
        {
            if (current->left && current->left->value == value) 
                current = current->left;
            else if (current->right && current->right->value == value) 
                current = current->right;
            else 
                return false;
        }
    }
    if (first) 
        return false;
    out = current->value;
    return true;
}

template <class T>
bool BinaryTree<T>::FindByRelativePath(const std::string& pathFromRoot, T& out) const
{
    return FindByPath(pathFromRoot, out);
}


template <class T>
std::string BinaryTree<T>::SaveToString() const
{
    std::ostringstream oss;
    bool first = true;
    Preorder([&](const T& value) {
        if (!first) 
            oss << ' ';
        oss << value;
        first = false;
    });
    return oss.str();
}

template <class T>
BinaryTree<T> BinaryTree<T>::LoadFromString(const std::string& s)
{
    BinaryTree<T> result;
    std::istringstream iss(s);
    T value;
    while (iss >> value)
    {
        result.Insert(value);
    }
    return result;
}