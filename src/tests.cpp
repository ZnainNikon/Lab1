#include <gtest/gtest.h>
#include "BinaryTree.hpp"
#include "Sequence.hpp"
#include "ListSequence.hpp"

#include <string>

TEST(Constructor, EmptyOnCreate)
{
    BinaryTree<int> t;
    EXPECT_TRUE(t.empty());
    EXPECT_EQ(t.size(), 0u);
    EXPECT_EQ(t.height(), 0);
}

TEST(Basic, InsertAndContains)
{
    BinaryTree<int> t;
    t.insert(10);
    t.insert(5);
    t.insert(15);
    t.insert(1);
    EXPECT_EQ(t.size(), 4u);
    EXPECT_TRUE(t.contains(10));
    EXPECT_TRUE(t.contains(5));
    EXPECT_TRUE(t.contains(15));
    EXPECT_TRUE(t.contains(1));
    EXPECT_FALSE(t.contains(999));
}

TEST(Basic, DuplicateNotInserted)
{
    BinaryTree<int> t;
    t.insert(7);
    t.insert(7);
    t.insert(7);
    EXPECT_EQ(t.size(), 1u);
}

TEST(Basic, Remove)
{
    BinaryTree<int> t;
    t.insert(10); t.insert(5); t.insert(15); t.insert(1);
    t.remove(15);
    EXPECT_FALSE(t.contains(15));
    EXPECT_EQ(t.size(), 3u);
}

TEST(Basic, RemoveRoot)
{
    BinaryTree<int> t;
    t.insert(10); t.insert(5); t.insert(15);
    t.remove(10);
    EXPECT_FALSE(t.contains(10));
    EXPECT_EQ(t.size(), 2u);
}

TEST(Basic, Clear)
{
    BinaryTree<int> t;
    t.insert(1); t.insert(2); t.insert(3);
    t.clear();
    EXPECT_TRUE(t.empty());
}

TEST(Traversals, InorderIsSorted)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1, 7, 12, 20}) t.insert(x);
    ListSequence<int> in;
    t.inorder(in);
    ASSERT_EQ(in.GetLength(), 7u);
    for (size_t i = 0; i + 1 < in.GetLength(); i++)
        EXPECT_LT(in.Get(i), in.Get(i + 1));
}

TEST(Traversals, PreorderStartsWithRoot)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) t.insert(x);
    ListSequence<int> pre;
    t.preorder(pre);
    ASSERT_GT(pre.GetLength(), 0u);
    EXPECT_EQ(pre.Get(0), 10);
}

TEST(Traversals, PostorderEndsWithRoot)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) t.insert(x);
    ListSequence<int> post;
    t.postorder(post);
    ASSERT_GT(post.GetLength(), 0u);
    EXPECT_EQ(post.Get(post.GetLength() - 1), 10);
}

TEST(Traversals, LevelStartsAtRoot)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15}) t.insert(x);
    ListSequence<int> level;
    t.levelOrder(level);
    ASSERT_GT(level.GetLength(), 0u);
    EXPECT_EQ(level.Get(0), 10);
}

TEST(MapReduce, MapSquares)
{
    BinaryTree<int> t;
    for (int x : {1, 2, 3, 4, 5}) t.insert(x);
    auto sq = t.map<int>([](int x) { return x * x; });
    EXPECT_TRUE(sq.contains(25));
    EXPECT_TRUE(sq.contains(9));
    EXPECT_EQ(sq.size(), t.size());
}

TEST(MapReduce, ReduceSum)
{
    BinaryTree<int> t;
    for (int x : {1, 2, 3, 4, 5}) t.insert(x);
    int sum = t.reduce<int>([](const int& x, int acc) { return acc + x; }, 0);
    EXPECT_EQ(sum, 15);
}

TEST(Subtrees, ExtractAndContain)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1, 6}) t.insert(x);
    auto sub = t.extractSubtree(5);
    EXPECT_TRUE(sub.contains(5));
    EXPECT_TRUE(t.containsSubtree(sub));
}

TEST(Subtrees, ExtractMissingThrows)
{
    BinaryTree<int> t;
    t.insert(1);
    EXPECT_THROW(t.extractSubtree(999), std::out_of_range);
}

TEST(Subtrees, EmptySubtree)
{
    BinaryTree<int> t;
    t.insert(1);
    BinaryTree<int> empty;
    EXPECT_TRUE(t.containsSubtree(empty));
}

TEST(Path, FindByAbsolutePath)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) t.insert(x);
    int v = 0;
    EXPECT_TRUE(t.findByPath("10", v));
    EXPECT_EQ(v, 10);
    EXPECT_TRUE(t.findByPath("10.5", v));
    EXPECT_EQ(v, 5);
    EXPECT_TRUE(t.findByPath("10.5.1", v));
    EXPECT_EQ(v, 1);
    EXPECT_FALSE(t.findByPath("10.999", v));
    EXPECT_FALSE(t.findByPath("999", v));
}

TEST(Path, FindByRelativePath)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15}) t.insert(x);
    int v = 0;
    EXPECT_TRUE(t.findByRelativePath("10.15", v));
    EXPECT_EQ(v, 15);
}

TEST(SaveLoad, RoundTrip)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) t.insert(x);
    std::string s = t.saveToString();
    auto t2 = BinaryTree<int>::loadFromString(s);
    ListSequence<int> a, b;
    t.preorder(a);
    t2.preorder(b);
    ASSERT_EQ(a.GetLength(), b.GetLength());
    for (size_t i = 0; i < a.GetLength(); i++)
        EXPECT_EQ(a.Get(i), b.Get(i));
    EXPECT_EQ(t2.size(), t.size());
}

TEST(SaveLoad, EmptyTree)
{
    BinaryTree<int> t;
    std::string s = t.saveToString();
    EXPECT_EQ(s, "()");
    auto t2 = BinaryTree<int>::loadFromString(s);
    EXPECT_TRUE(t2.empty());
}

TEST(TypesDouble, Basic)
{
    BinaryTree<double> t;
    t.insert(2.5); t.insert(1.0); t.insert(3.14); t.insert(0.5);
    EXPECT_EQ(t.size(), 4u);
    EXPECT_TRUE(t.contains(2.5));
    EXPECT_FALSE(t.contains(9.99));
}

TEST(TypesString, Basic)
{
    BinaryTree<std::string> t;
    t.insert("m"); t.insert("a"); t.insert("z"); t.insert("b");
    EXPECT_EQ(t.size(), 4u);
    EXPECT_TRUE(t.contains("a"));
    EXPECT_FALSE(t.contains("x"));
}