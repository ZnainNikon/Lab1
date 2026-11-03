#pragma once

template <class T>
int BinaryTree<T>::heightOf(const Node* n)
{
    if (!n) return 0;
    int leftH = heightOf(n->left.get());
    int rightH = heightOf(n->right.get());
    return 1 + std::max(leftH, rightH);
}

template <class T>
std::unique_ptr<typename BinaryTree<T>::Node> BinaryTree<T>::cloneNode(Node* src)
{
    if (!src) return nullptr;
    auto copy = std::make_unique<Node>(src->value);
    copy->left = cloneNode(src->left.get());
    copy->right = cloneNode(src->right.get());
    return copy;
}

template <class T>
const typename BinaryTree<T>::Node* BinaryTree<T>::findNode(const Node* n, const T& v)
{
    while (n)
    {
        if (v < n->value) n = n->left.get();
        else if (n->value < v) n = n->right.get();
        else return n;
    }
    return nullptr;
}


template <class T>
BinaryTree<T>::BinaryTree(const BinaryTree& other)
    : root(cloneNode(other.root.get())), sz(other.sz) {}

template <class T>
BinaryTree<T>& BinaryTree<T>::operator=(const BinaryTree& other)
{
    if (this != &other)
    {
        root = cloneNode(other.root.get());
        sz = other.sz;
    }
    return *this;
}


template <class T>
std::unique_ptr<typename BinaryTree<T>::Node>
BinaryTree<T>::insertNode(std::unique_ptr<Node> n, const T& v, bool& inserted)
{
    if (!n)
    {
        inserted = true;
        return std::make_unique<Node>(v);
    }
    if (v < n->value)
        n->left = insertNode(std::move(n->left), v, inserted);
    else if (n->value < v)
        n->right = insertNode(std::move(n->right), v, inserted);
    else
        inserted = false;
    return n;
}

template <class T>
std::unique_ptr<typename BinaryTree<T>::Node>
BinaryTree<T>::removeNode(std::unique_ptr<Node> n, const T& v, bool& removed)
{
    if (!n) return nullptr;

    if (v < n->value)
        n->left = removeNode(std::move(n->left), v, removed);
    else if (n->value < v)
        n->right = removeNode(std::move(n->right), v, removed);
    else
    {
        removed = true;
        if (!n->left) return std::move(n->right);
        if (!n->right) return std::move(n->left);
        
        Node* successor = n->right.get();
        while (successor->left) successor = successor->left.get();
        n->value = successor->value;
        n->right = removeNode(std::move(n->right), successor->value, removed);
    }
    return n;
}


template <class T>
void BinaryTree<T>::preorderFill(const Node* n, std::function<void(const T&)>& visitor)
{
    if (!n) return;
    visitor(n->value);
    preorderFill(n->left.get(), visitor);
    preorderFill(n->right.get(), visitor);
}

template <class T>
void BinaryTree<T>::postorderFill(const Node* n, std::function<void(const T&)>& visitor)
{
    if (!n) return;
    postorderFill(n->left.get(), visitor);
    postorderFill(n->right.get(), visitor);
    visitor(n->value);
}

template <class T>
void BinaryTree<T>::inorderFill(const Node* n, std::function<void(const T&)>& visitor)
{
    if (!n) return;
    inorderFill(n->left.get(), visitor);
    visitor(n->value);
    inorderFill(n->right.get(), visitor);
}


template <class T>
void BinaryTree<T>::insert(const T& value)
{
    bool inserted = false;
    root = insertNode(std::move(root), value, inserted);
    if (inserted) sz++;
}

template <class T>
bool BinaryTree<T>::contains(const T& value) const
{
    return findNode(root.get(), value) != nullptr;
}

template <class T>
void BinaryTree<T>::remove(const T& value)
{
    bool removed = false;
    root = removeNode(std::move(root), value, removed);
    if (removed) sz--;
}

template <class T>
void BinaryTree<T>::clear()
{
    root.reset();
    sz = 0;
}

template <class T>
void BinaryTree<T>::preorder(std::function<void(const T&)> visitor) const
{
    preorderFill(root.get(), visitor);
}

template <class T>
void BinaryTree<T>::postorder(std::function<void(const T&)> visitor) const
{
    postorderFill(root.get(), visitor);
}

template <class T>
void BinaryTree<T>::inorder(std::function<void(const T&)> visitor) const
{
    inorderFill(root.get(), visitor);
}

template <class T>
void BinaryTree<T>::levelOrder(std::function<void(const T&)> visitor) const
{
    if (!root) return;
    MyVector<const Node*> queue;
    queue.push_back(root.get());
    size_t head = 0;
    while (head < queue.size())
    {
        const Node* current = queue[head++];
        visitor(current->value);
        if (current->left) queue.push_back(current->left.get());
        if (current->right) queue.push_back(current->right.get());
    }
}


template <class T>
template <class R, class F>
BinaryTree<R> BinaryTree<T>::map(F f) const
{
    BinaryTree<R> result;
    preorder([&](const T& value) { result.insert(f(value)); });
    return result;
}

template <class T>
BinaryTree<T> BinaryTree<T>::where(std::function<bool(const T&)> predicate) const
{
    BinaryTree<T> result;
    preorder([&](const T& value) {
        if (predicate(value)) result.insert(value);
    });
    return result;
}

template <class T>
template <class R>
R BinaryTree<T>::reduce(std::function<R(const T&, R)> f, R start) const
{
    R accumulator = start;
    preorder([&](const T& value) { accumulator = f(value, accumulator); });
    return accumulator;
}


template <class T>
BinaryTree<T> BinaryTree<T>::extractSubtree(const T& rootValue) const
{
    const Node* found = findNode(root.get(), rootValue);
    if (!found) throw std::out_of_range("extractSubtree: element not found");
    
    BinaryTree<T> result;
    result.root = cloneNode(const_cast<Node*>(found));
    
    size_t count = 0;
    result.preorder([&](const T&) { count++; });
    result.sz = count;
    return result;
}

template <class T>
bool BinaryTree<T>::sameStructure(const Node* a, const Node* b)
{
    if (!a && !b) return true;
    if (!a || !b) return false;
    if (!(a->value == b->value)) return false;
    return sameStructure(a->left.get(), b->left.get()) && 
           sameStructure(a->right.get(), b->right.get());
}

template <class T>
bool BinaryTree<T>::containsSubtree(const BinaryTree<T>& other) const
{
    if (other.empty()) return true;
    std::function<bool(const Node*, const Node*)> find =
        [&](const Node* n, const Node* target) -> bool {
            if (!n) return false;
            if (sameStructure(n, target)) return true;
            return find(n->left.get(), target) || find(n->right.get(), target);
        };
    return find(root.get(), other.root.get());
}

template <class T>
bool BinaryTree<T>::findByPath(const std::string& path, T& out) const
{
    if (!root) return false;
    std::istringstream reader(path);
    std::string token;
    const Node* current = root.get();
    bool first = true;
    while (std::getline(reader, token, '.'))
    {
        if (token.empty()) continue;
        T value{};
        std::istringstream tokenReader(token);
        if (!(tokenReader >> value)) return false;
        if (first)
        {
            if (!(current->value == value)) return false;
            first = false;
        }
        else
        {
            if (current->left && current->left->value == value) current = current->left.get();
            else if (current->right && current->right->value == value) current = current->right.get();
            else return false;
        }
    }
    if (first) return false;
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
    if (!n) { os << "()"; return; }
    os << '(' << n->value << ' ';
    serialize(n->left.get(), os);
    os << ' ';
    serialize(n->right.get(), os);
    os << ')';
}

template <class T>
std::unique_ptr<typename BinaryTree<T>::Node> BinaryTree<T>::deserialize(std::istream& is)
{
    char c = 0; is >> c;
    if (c != '(') throw std::invalid_argument("deserialize: expected '('");
    is >> std::ws;
    if (is.peek() == ')') { is.get(); return nullptr; }
    T value;
    if (!(is >> value)) throw std::invalid_argument("deserialize: cannot read value");
    auto node = std::make_unique<Node>(value);
    node->left = deserialize(is);
    node->right = deserialize(is);
    is >> std::ws;
    if (is.peek() != ')') throw std::invalid_argument("deserialize: expected ')'");
    is.get();
    return node;
}

template <class T>
std::string BinaryTree<T>::saveToString() const
{
    std::ostringstream oss;
    serialize(root.get(), oss);
    return oss.str();
}

template <class T>
BinaryTree<T> BinaryTree<T>::loadFromString(const std::string& s)
{
    std::istringstream iss(s);
    BinaryTree<T> result;
    result.root = deserialize(iss);
    size_t count = 0;
    result.preorder([&](const T&) { count++; });
    result.sz = count;
    return result;
}