#ifndef BINARY_TREE_TPP
#define BINARY_TREE_TPP

template <class T>
int BinaryTree<T>::heightOf(const Node* n)
{
    if (!n) return 0;
    int leftH = heightOf(n->left);
    int rightH = heightOf(n->right);
    return 1 + (leftH > rightH ? leftH : rightH);
}

template <class T>
typename BinaryTree<T>::Node* BinaryTree<T>::cloneNode(const Node* src)
{
    if (!src) return nullptr;
    Node* copy = new Node(src->value);
    copy->left = cloneNode(src->left);
    copy->right = cloneNode(src->right);
    return copy;
}

template <class T>
void BinaryTree<T>::destroy(Node* n)
{
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

template <class T>
size_t BinaryTree<T>::countNodes(const Node* n)
{
    if (!n) return 0;
    return 1 + countNodes(n->left) + countNodes(n->right);
}

template <class T>
typename BinaryTree<T>::Node* BinaryTree<T>::findNode(Node* n, const T& v)
{
    while (n)
    {
        if (v < n->value) n = n->left;
        else if (n->value < v) n = n->right;
        else return n;
    }
    return nullptr;
}

template <class T>
const typename BinaryTree<T>::Node* BinaryTree<T>::findNode(const Node* n, const T& v)
{
    while (n)
    {
        if (v < n->value) n = n->left;
        else if (n->value < v) n = n->right;
        else return n;
    }
    return nullptr;
}

template <class T>
typename BinaryTree<T>::Node* BinaryTree<T>::minNode(Node* n)
{
    while (n && n->left) n = n->left;
    return n;
}

template <class T>
BinaryTree<T>::BinaryTree(const BinaryTree& other)
    : root(cloneNode(other.root)), sz(other.sz) {}

template <class T>
BinaryTree<T>& BinaryTree<T>::operator=(const BinaryTree& other)
{
    if (this != &other)
    {
        clear();
        root = cloneNode(other.root);
        sz = other.sz;
    }
    return *this;
}

template <class T>
BinaryTree<T>::BinaryTree(BinaryTree&& other) noexcept
    : root(other.root), sz(other.sz)
{
    other.root = nullptr;
    other.sz = 0;
}

template <class T>
BinaryTree<T>& BinaryTree<T>::operator=(BinaryTree&& other) noexcept
{
    if (this != &other)
    {
        clear();
        root = other.root;
        sz = other.sz;
        other.root = nullptr;
        other.sz = 0;
    }
    return *this;
}

template <class T>
BinaryTree<T>::~BinaryTree() { clear(); }

template <class T>
typename BinaryTree<T>::Node*
BinaryTree<T>::insertNode(Node* n, const T& v, bool& inserted)
{
    if (!n)
    {
        inserted = true;
        return new Node(v);
    }

    if (v < n->value)
    {
        n->left = insertNode(n->left, v, inserted);
    }
    else if (n->value < v)
    {
        n->right = insertNode(n->right, v, inserted);
    }
    else
    {
        inserted = false;
    }
    return n;
}

template <class T>
typename BinaryTree<T>::Node*
BinaryTree<T>::removeNode(Node* n, const T& v, bool& removed)
{
    if (!n) return nullptr;

    if (v < n->value)
    {
        n->left = removeNode(n->left, v, removed);
    }
    else if (n->value < v)
    {
        n->right = removeNode(n->right, v, removed);
    }
    else
    {
        if (!n->left && !n->right)
        {
            delete n;
            removed = true;
            return nullptr;
        }
        if (!n->left)
        {
            Node* right = n->right;
            delete n;
            removed = true;
            return right;
        }
        if (!n->right)
        {
            Node* left = n->left;
            delete n;
            removed = true;
            return left;
        }
        Node* successor = minNode(n->right);
        n->value = successor->value;
        n->right = removeNode(n->right, successor->value, removed);
        return n;
    }
    return n;
}

template <class T>
void BinaryTree<T>::preorderFill(const Node* n, ListSequence<T>& out)
{
    if (!n) return;
    out.Append(n->value);
    preorderFill(n->left, out);
    preorderFill(n->right, out);
}

template <class T>
void BinaryTree<T>::postorderFill(const Node* n, ListSequence<T>& out)
{
    if (!n) return;
    postorderFill(n->left, out);
    postorderFill(n->right, out);
    out.Append(n->value);
}

template <class T>
void BinaryTree<T>::inorderFill(const Node* n, ListSequence<T>& out)
{
    if (!n) return;
    inorderFill(n->left, out);
    out.Append(n->value);
    inorderFill(n->right, out);
}

template <class T>
void BinaryTree<T>::insert(const T& value)
{
    bool inserted = false;
    root = insertNode(root, value, inserted);
    if (inserted) sz++;
}

template <class T>
bool BinaryTree<T>::contains(const T& value) const
{
    return findNode(root, value) != nullptr;
}

template <class T>
void BinaryTree<T>::remove(const T& value)
{
    bool removed = false;
    root = removeNode(root, value, removed);
    if (removed) sz--;
}

template <class T>
void BinaryTree<T>::clear()
{
    destroy(root);
    root = nullptr;
    sz = 0;
}

template <class T>
void BinaryTree<T>::preorder(ListSequence<T>& out) const
{
    preorderFill(root, out);
}

template <class T>
void BinaryTree<T>::postorder(ListSequence<T>& out) const
{
    postorderFill(root, out);
}

template <class T>
void BinaryTree<T>::inorder(ListSequence<T>& out) const
{
    inorderFill(root, out);
}

template <class T>
void BinaryTree<T>::levelOrder(ListSequence<T>& out) const
{
    if (!root) return;
    size_t head = 0, tail = 0, capacity = 16;
    const Node** queue = new const Node*[capacity];
    queue[tail++] = root;

    while (head < tail)
    {
        const Node* current = queue[head++];
        out.Append(current->value);

        if (current->left)
        {
            if (tail == capacity)
            {
                capacity *= 2;
                const Node** bigger = new const Node*[capacity];
                for (size_t j = 0; j < tail; j++) bigger[j] = queue[j];
                delete[] queue;
                queue = bigger;
            }
            queue[tail++] = current->left;
        }
        if (current->right)
        {
            if (tail == capacity)
            {
                capacity *= 2;
                const Node** bigger = new const Node*[capacity];
                for (size_t j = 0; j < tail; j++) bigger[j] = queue[j];
                delete[] queue;
                queue = bigger;
            }
            queue[tail++] = current->right;
        }
    }
    delete[] queue;
}

template <class T>
template <class R, class F>
BinaryTree<R> BinaryTree<T>::map(F f) const
{
    BinaryTree<R> result;
    ListSequence<T> values;
    preorder(values);
    for (size_t i = 0; i < values.GetLength(); i++)
        result.insert(f(values.Get(i)));
    return result;
}

template <class T>
template <class R>
R BinaryTree<T>::reduce(std::function<R(const T&, R)> f, R start) const
{
    R accumulator = start;
    ListSequence<T> values;
    preorder(values);
    for (size_t i = 0; i < values.GetLength(); i++)
        accumulator = f(values.Get(i), accumulator);
    return accumulator;
}

template <class T>
BinaryTree<T> BinaryTree<T>::extractSubtree(const T& rootValue) const
{
    const Node* found = findNode(root, rootValue);
    if (!found) throw std::out_of_range("extractSubtree: element not found");

    BinaryTree<T> result;
    result.root = cloneNode(found);
    result.sz = countNodes(result.root);
    return result;
}

template <class T>
bool BinaryTree<T>::sameStructure(const Node* a, const Node* b)
{
    if (!a && !b) return true;
    if (!a || !b) return false;
    if (!(a->value == b->value)) return false;
    return sameStructure(a->left, b->left)
        && sameStructure(a->right, b->right);
}

template <class T>
bool BinaryTree<T>::containsSubtree(const BinaryTree<T>& other) const
{
    if (other.empty()) return true;

    std::function<bool(const Node*, const Node*)> find =
        [&](const Node* n, const Node* target) -> bool
        {
            if (!n) return false;
            if (sameStructure(n, target)) return true;
            return find(n->left, target) || find(n->right, target);
        };
    return find(root, other.root);
}

template <class T>
bool BinaryTree<T>::findByPath(const std::string& path, T& out) const
{
    if (!root) return false;

    ListSequence<T> parts;
    std::istringstream reader(path);
    std::string token;
    while (std::getline(reader, token, '.'))
    {
        if (token.empty()) continue;
        T value{};
        std::istringstream tokenReader(token);
        if (!(tokenReader >> value)) return false;
        parts.Append(value);
    }
    if (parts.GetLength() == 0) return false;
    if (!(root->value == parts.Get(0))) return false;

    const Node* current = root;
    for (size_t i = 1; i < parts.GetLength(); i++)
    {
        const Node* next = nullptr;
        if (current->left && current->left->value == parts.Get(i))
            next = current->left;
        else if (current->right && current->right->value == parts.Get(i))
            next = current->right;
        if (!next) return false;
        current = next;
    }
    out = current->value;
    return true;
}

template <class T>
bool BinaryTree<T>::findByRelativePath(const std::string& pathFromRoot, T& out) const
{
    return findByPath(pathFromRoot, out);
}

template <class T>
void BinaryTree<T>::serialize(const Node* n, std::ostream& os)
{
    if (!n)
    {
        os << "()";
        return;
    }
    os << '(' << n->value;
    os << ' ';
    serialize(n->left, os);
    os << ' ';
    serialize(n->right, os);
    os << ')';
}

template <class T>
typename BinaryTree<T>::Node*
BinaryTree<T>::deserialize(std::istream& is)
{
    char c = 0;
    is >> c;
    if (c != '(')
        throw std::invalid_argument("deserialize: expected '('");

    is >> std::ws;
    if (is.peek() == ')')
    {
        is.get();
        return nullptr;
    }

    T value;
    if (!(is >> value))
        throw std::invalid_argument("deserialize: cannot read value");

    Node* node = new Node(value);
    node->left = deserialize(is);
    node->right = deserialize(is);

    is >> std::ws;
    if (is.peek() != ')')
        throw std::invalid_argument("deserialize: expected ')'");
    is.get();
    return node;
}

template <class T>
std::string BinaryTree<T>::saveToString() const
{
    std::ostringstream oss;
    serialize(root, oss);
    return oss.str();
}

template <class T>
BinaryTree<T> BinaryTree<T>::loadFromString(const std::string& s)
{
    std::istringstream iss(s);
    BinaryTree<T> result;
    result.root = deserialize(iss);
    result.sz = countNodes(result.root);
    return result;
}

#endif