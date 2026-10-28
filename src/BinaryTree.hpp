#ifndef BINARY_TREE_HPP
#define BINARY_TREE_HPP

#include <functional>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>

#include "ListSequence.hpp"

template <class T> class BinaryTree
{
    public:
    using value_type = T;

    struct Node
    {
        T value;
        Node* left = nullptr;
        Node* right = nullptr;

        explicit Node(const T& v) : value(v) {}
    };

    BinaryTree() = default;
    BinaryTree(const BinaryTree& other);
    BinaryTree& operator=(const BinaryTree& other);
    BinaryTree(BinaryTree&& other) noexcept;
    BinaryTree& operator=(BinaryTree&& other) noexcept;
    ~BinaryTree();

    void insert(const T& value);
    bool contains(const T& value) const;
    void remove(const T& value);
    void clear();

    void preorder(ListSequence<T>& out) const;
    void postorder(ListSequence<T>& out) const;
    void inorder(ListSequence<T>& out) const;
    void levelOrder(ListSequence<T>& out) const;

    template <class R, class F>
    BinaryTree<R> map(F f) const;

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
    int height() const { return heightOf(root); }
    const Node* getRoot() const { return root; }

    private:
    Node* root = nullptr;
    size_t sz = 0;

    static Node* cloneNode(const Node* src);
    static void destroy(Node* n);
    static size_t countNodes(const Node* n);
    static int heightOf(const Node* n);

    static void preorderFill(const Node* n, ListSequence<T>& out);
    static void postorderFill(const Node* n, ListSequence<T>& out);
    static void inorderFill(const Node* n, ListSequence<T>& out);

    static Node* findNode(Node* n, const T& v);
    static const Node* findNode(const Node* n, const T& v);
    static Node* minNode(Node* n);

    Node* insertNode(Node* n, const T& v, bool& inserted);
    Node* removeNode(Node* n, const T& v, bool& removed);

    static bool sameStructure(const Node* a, const Node* b);

    static void serialize(const Node* n, std::ostream& os);
    static Node* deserialize(std::istream& is);
};

#include "BinaryTree.tpp"

#endif