#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include "array.hpp"

template <class T> class BinaryTree
{
    public:
    using value_type = T;

    struct Node
    {
        T value;
        std::unique_ptr<Node> left = nullptr;
        std::unique_ptr<Node> right = nullptr;
        explicit Node(const T& v) : value(v) {}
    };

    BinaryTree() = default;
    BinaryTree(const BinaryTree& other);
    BinaryTree& operator=(const BinaryTree& other);
    BinaryTree(BinaryTree&& other) noexcept = default;
    BinaryTree& operator=(BinaryTree&& other) noexcept = default;
    ~BinaryTree() = default;

    void insert(const T& value);
    bool contains(const T& value) const;
    void remove(const T& value);
    void clear();

    void preorder(std::function<void(const T&)> visitor) const;
    void postorder(std::function<void(const T&)> visitor) const;
    void inorder(std::function<void(const T&)> visitor) const;
    void levelOrder(std::function<void(const T&)> visitor) const;

    template <class R, class F>
    BinaryTree<R> map(F f) const;

    BinaryTree<T> where(std::function<bool(const T&)> predicate) const;

    template <class R>
    R reduce(std::function<R(const T&, R)> f, R start) const;

    BinaryTree<T> extractSubtree(const T& rootValue) const;
    bool containsSubtree(const BinaryTree<T>& other) const;

    bool findByPath(const std::string& path, T& out) const;
    bool findByRelativePath(const std::string& pathFromRoot, T& out) const;

    std::string saveToString() const;
    static BinaryTree<T> loadFromString(const std::string& s);

    bool empty() const { return root == nullptr; }
    size_t size() const { return sz; }
    int height() const { return heightOf(root.get()); }
    const std::unique_ptr<Node>& getRoot() const { return root; }

    private:
    std::unique_ptr<Node> root = nullptr;
    size_t sz = 0;

    // Владеющие операции (unique_ptr по значению)
    std::unique_ptr<Node> insertNode(std::unique_ptr<Node> n, const T& v, bool& inserted);
    std::unique_ptr<Node> removeNode(std::unique_ptr<Node> n, const T& v, bool& removed);

    // Навигационные операции (сырые указатели по значению, никаких ссылок на указатели)
    static std::unique_ptr<Node> cloneNode(Node* src);   // получает сырой, возвращает unique_ptr (владение)
    static int heightOf(const Node* n);
    static void preorderFill(const Node* n, std::function<void(const T&)>& visitor);
    static void postorderFill(const Node* n, std::function<void(const T&)>& visitor);
    static void inorderFill(const Node* n, std::function<void(const T&)>& visitor);
    static const Node* findNode(const Node* n, const T& v); // возвращает сырой указатель по значению
    static bool sameStructure(const Node* a, const Node* b);
    static void serialize(const Node* n, std::ostream& os);
    static std::unique_ptr<Node> deserialize(std::istream& is);
};

#include "BinaryTree.tpp"