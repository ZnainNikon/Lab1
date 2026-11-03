#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include "array.hpp"

template <class T>
struct Node
{
    T value;
    Node* left = nullptr;
    Node* right = nullptr;
    explicit Node(const T& v) : value(v) {}
};

template <class T> class BinaryTree
{
    public:
    BinaryTree() = default;
    BinaryTree(const BinaryTree& other);
    BinaryTree& operator=(const BinaryTree& other);
    BinaryTree(BinaryTree&& other) noexcept;
    BinaryTree& operator=(BinaryTree&& other) noexcept;
    ~BinaryTree();

    void Insert(const T& value);
    bool Contains(const T& value) const;
    void Remove(const T& value);
    void Clear();

    void Preorder(std::function<void(const T&)> visitor) const;
    void Postorder(std::function<void(const T&)> visitor) const;
    void Inorder(std::function<void(const T&)> visitor) const;
    void LevelOrder(std::function<void(const T&)> visitor) const;

    template <class R, class F>
    BinaryTree<R> Map(F f) const;

    BinaryTree<T> Where(std::function<bool(const T&)> predicate) const;

    template <class R>
    R Reduce(std::function<R(const T&, R)> f, R start) const;

    BinaryTree<T> ExtractSubtree(const T& rootValue) const;
    bool ContainsSubtree(const BinaryTree<T>& other) const;

    bool FindByPath(const std::string& path, T& out) const;
    bool FindByRelativePath(const std::string& pathFromRoot, T& out) const;

    // Сериализация через пробелы (preorder)
    std::string SaveToString() const;
    static BinaryTree<T> LoadFromString(const std::string& s);

    bool Empty() const { return root == nullptr; }
    size_t Size() const { return sz; }
    int Height() const { return HeightOf(root); }

    const Node<T>* GetRoot() const { return root; }

    private:
    Node<T>* root = nullptr;
    size_t sz = 0;

    static Node<T>* CloneNode(const Node<T>* src);
    static void Destroy(Node<T>* n);
    static int HeightOf(const Node<T>* n);

    static void PreorderFill(const Node<T>* n, std::function<void(const T&)>& visitor);
    static void PostorderFill(const Node<T>* n, std::function<void(const T&)>& visitor);
    static void InorderFill(const Node<T>* n, std::function<void(const T&)>& visitor);

    static const Node<T>* FindNode(const Node<T>* n, const T& v);
    static Node<T>* InsertNode(Node<T>* n, const T& v, bool& inserted);
    static Node<T>* RemoveNode(Node<T>* n, const T& v, bool& removed);
    static bool SameStructure(const Node<T>* a, const Node<T>* b);
};

#include "BinaryTree.tpp"