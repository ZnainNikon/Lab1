#ifndef BINARY_TREE_TPP
#define BINARY_TREE_TPP

template <class T>
int BinaryTree<T>::heightOf(const std::unique_ptr<Node>& n)
{
    if (!n) return 0;
    int leftH = heightOf(n->left);
    int rightH = heightOf(n->right);
    return 1 + std::max(leftH, rightH);
}

template <class T>
std::unique_ptr<typename BinaryTree<T>::Node> 
BinaryTree<T>::cloneNode(const std::unique_ptr<Node>& src)
{
    if (!src) return nullptr;
    auto copy = std::make_unique<Node>(src->value);
    copy->left = cloneNode(src->left);
    copy->right = cloneNode(src->right);
    return copy;
}

template <class T>
bool BinaryTree<T>::findNode(const std::unique_ptr<Node>& n, const T& v, const Node** outNode)
{
    if (!n) return false;
    if (v < n->value) return findNode(n->left, v, outNode);
    if (n->value < v) return findNode(n->right, v, outNode);
    if (outNode) *outNode = n.get();
    return true;
}

template <class T>
BinaryTree<T>::BinaryTree(const BinaryTree& other)
    : root(cloneNode(other.root)), sz(other.sz) {}

template <class T>
BinaryTree<T>& BinaryTree<T>::operator=(const BinaryTree& other)
{
    if (this != &other)
    {
        root = cloneNode(other.root);
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
    {
        n->left = insertNode(std::move(n->left), v, inserted);
    }
    else if (n->value < v)
    {
        n->right = insertNode(std::move(n->right), v, inserted);
    }
    else
    {
        inserted = false;
    }
    return n;
}

template <class T>
std::unique_ptr<typename BinaryTree<T>::Node>
BinaryTree<T>::removeNode(std::unique_ptr<Node> n, const T& v, bool& removed)
{
    if (!n) return nullptr;

    if (v < n->value)
    {
        n->left = removeNode(std::move(n->left), v, removed);
    }
    else if (n->value < v)
    {
        n->right = removeNode(std::move(n->right), v, removed);
    }
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
void BinaryTree<T>::preorderFill(const std::unique_ptr<Node>& n, std::function<void(const T&)>& visitor)
{
    if (!n) return;
    visitor(n->value);
    preorderFill(n->left, visitor);
    preorderFill(n->right, visitor);
}

template <class T>
void BinaryTree<T>::postorderFill(const std::unique_ptr<Node>& n, std::function<void(const T&)>& visitor)
{
    if (!n) return;
    postorderFill(n->left, visitor);
    postorderFill(n->right, visitor);
    visitor(n->value);
}

template <class T>
void BinaryTree<T>::inorderFill(const std::unique_ptr<Node>& n, std::function<void(const T&)>& visitor)
{
    if (!n) return;
    inorderFill(n->left, visitor);
    visitor(n->value);
    inorderFill(n->right, visitor);
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
    return findNode(root, value, nullptr);
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
    preorderFill(root, visitor);
}

template <class T>
void BinaryTree<T>::postorder(std::function<void(const T&)> visitor) const
{
    postorderFill(root, visitor);
}

template <class T>
void BinaryTree<T>::inorder(std::function<void(const T&)> visitor) const
{
    inorderFill(root, visitor);
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
        if (predicate(value)) {
            result.insert(value);
        }
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
    const Node* found = nullptr;
    if (!findNode(root, rootValue, &found)) 
        throw std::out_of_range("extractSubtree: element not found");
    
    BinaryTree<T> result;
    
    std::function<std::unique_ptr<Node>(const Node*)> cloneFromPtr = 
        [&](const Node* n) -> std::unique_ptr<Node> {
            if (!n) return nullptr;
            auto copy = std::make_unique<Node>(n->value);
            copy->left = cloneFromPtr(n->left.get());
            copy->right = cloneFromPtr(n->right.get());
            return copy;
        };
    result.root = cloneFromPtr(found);
    
    std::function<size_t(const Node*)> count = [&](const Node* n) -> size_t {
        return n ? 1 + count(n->left.get()) + count(n->right.get()) : 0;
    };
    result.sz = count(result.root.get());
    return result;
}

template <class T>
bool BinaryTree<T>::sameStructure(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b)
{
    if (!a && !b) return true;
    if (!a || !b) return false;
    if (!(a->value == b->value)) return false;
    return sameStructure(a->left, b->left) && sameStructure(a->right, b->right);
}

template <class T>
bool BinaryTree<T>::containsSubtree(const BinaryTree<T>& other) const
{
    if (other.empty()) return true;
    
    std::function<bool(const std::unique_ptr<Node>&, const std::unique_ptr<Node>&)> find =
        [&](const std::unique_ptr<Node>& n, const std::unique_ptr<Node>& target) -> bool {
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
void BinaryTree<T>::serialize(const std::unique_ptr<Node>& n, std::ostream& os)
{
    if (!n) { os << "()"; return; }
    os << '(' << n->value << ' ';
    serialize(n->left, os);
    os << ' ';
    serialize(n->right, os);
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
    serialize(root, oss);
    return oss.str();
}

template <class T>
BinaryTree<T> BinaryTree<T>::loadFromString(const std::string& s)
{
    std::istringstream iss(s);
    BinaryTree<T> result;
    result.root = deserialize(iss);
    
    std::function<size_t(const Node*)> count = [&](const Node* n) -> size_t {
        return n ? 1 + count(n->left.get()) + count(n->right.get()) : 0;
    };
    result.sz = count(result.root.get());
    return result;
}

#endif