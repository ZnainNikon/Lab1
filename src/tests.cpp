#include <gtest/gtest.h>
#include "Sequence.hpp"
#include "ListSequence.hpp"
#include "NTree.hpp"

#include <string>

TEST(ListSequence, BasicOps)
{
    ListSequence<int> s;
    s.Append(10);
    s.Append(20);
    s.Append(30);

    EXPECT_EQ(s.GetLength(), 3u);
    EXPECT_EQ(s.Get(0), 10);
    EXPECT_EQ(s.Get(1), 20);
    EXPECT_EQ(s.Get(2), 30);
}

TEST(Constructor, InvalidArityThrows)
{
    EXPECT_THROW(NTree<int>(0), std::invalid_argument);
    EXPECT_THROW(NTree<int>(-3), std::invalid_argument);
}

TEST(Constructor, ValidArity)
{
    NTree<int> t(3);
    EXPECT_EQ(t.arity(), 3);
    EXPECT_TRUE(t.empty());
    EXPECT_EQ(t.size(), 0u);
}

TEST(Basic, InsertRootAndChildren)
{
    NTree<int> t(3);
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
    NTree<int> t(3);
    t.insert(7);
    t.insert(7);
    t.insert(7);
    EXPECT_EQ(t.size(), 1u);
}

TEST(Basic, Remove)
{
    NTree<int> t(3);
    t.insert(10);
    t.insert(5);
    t.insert(15);
    t.insert(1);

    t.remove(15);
    EXPECT_FALSE(t.contains(15));
    EXPECT_EQ(t.size(), 3u);
}

TEST(Basic, Clear)
{
    NTree<int> t(3);
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.clear();
    EXPECT_TRUE(t.empty());
}

TEST(Traversals, PreorderPostorderLevel)
{
    NTree<int> t(3);
    t.insert(10);
    t.insert(5);
    t.insert(15);
    t.insert(1);

    ListSequence<int> pre;
    t.preorder(pre);
    ASSERT_GT(pre.GetLength(), 0u);
    EXPECT_EQ(pre.Get(0), 10);

    ListSequence<int> post;
    t.postorder(post);
    ASSERT_GT(post.GetLength(), 0u);
    EXPECT_EQ(post.Get(post.GetLength() - 1), 10);

    ListSequence<int> lvl;
    t.levelOrder(lvl);
    ASSERT_GT(lvl.GetLength(), 0u);
    EXPECT_EQ(lvl.Get(0), 10);
}

TEST(MapReduce, MapSquares)
{
    NTree<int> t(3);
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.insert(4);
    t.insert(5);

    auto sq = t.map<int>([](int x) { return x * x; });
    EXPECT_TRUE(sq.contains(25));
    EXPECT_TRUE(sq.contains(9));
}

TEST(MapReduce, ReduceSum)
{
    NTree<int> t(3);
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.insert(4);
    t.insert(5);

    int sum = t.reduce<int>([](const int& x, int acc) { return acc + x; }, 0);
    EXPECT_EQ(sum, 15);
}

TEST(Subtrees, ExtractAndContain)
{
    NTree<int> t(3);
    t.insert(10);
    t.insert(5);
    t.insert(15);
    t.insert(1);
    t.insert(6);

    auto sub = t.extractSubtree(5);
    EXPECT_TRUE(sub.contains(5));
    EXPECT_TRUE(t.containsSubtree(sub));
}

TEST(Subtrees, ExtractMissingThrows)
{
    NTree<int> t(3);
    t.insert(1);
    EXPECT_THROW(t.extractSubtree(999), std::out_of_range);
}

TEST(Subtrees, EmptySubtree)
{
    NTree<int> t(3);
    t.insert(1);
    NTree<int> empty(3);
    EXPECT_TRUE(t.containsSubtree(empty));
}

TEST(Path, FindByAbsolutePath)
{
    NTree<int> t(3);
    t.insert(10);
    t.insert(5);
    t.insert(15);
    t.insert(1);

    int v = 0;
    EXPECT_TRUE(t.findByPath("10", v));
    EXPECT_EQ(v, 10);

    EXPECT_TRUE(t.findByPath("10.5", v));
    EXPECT_EQ(v, 5);

    EXPECT_TRUE(t.findByPath("10.1", v));   // 1 — ребёнок 10, а не 5
    EXPECT_EQ(v, 1);

    EXPECT_FALSE(t.findByPath("10.999", v));
    EXPECT_FALSE(t.findByPath("999", v));
}

TEST(Path, FindByRelativePath)
{
    NTree<int> t(3);
    t.insert(10);
    t.insert(5);
    t.insert(15);

    int v = 0;
    EXPECT_TRUE(t.findByRelativePath("10.15", v));
    EXPECT_EQ(v, 15);
}

TEST(SaveLoad, RoundTrip)
{
    NTree<int> t(3);
    t.insert(10);
    t.insert(5);
    t.insert(15);
    t.insert(1);

    std::string s = t.saveToString();
    auto t2 = NTree<int>::loadFromString(s, 3);

    ListSequence<int> a;
    ListSequence<int> b;
    t.preorder(a);
    t2.preorder(b);

    ASSERT_EQ(a.GetLength(), b.GetLength());
    for (size_t i = 0; i < a.GetLength(); i++)
    {
        EXPECT_EQ(a.Get(i), b.Get(i));
    }
    EXPECT_EQ(t2.size(), t.size());
}

TEST(SaveLoad, EmptyTree)
{
    NTree<int> t(3);
    std::string s = t.saveToString();
    EXPECT_EQ(s, "()");

    auto t2 = NTree<int>::loadFromString(s, 3);
    EXPECT_TRUE(t2.empty());
}

TEST(DifferentArity, Binary)
{
    NTree<int> t(2);
    t.insert(5);
    t.insert(3);
    t.insert(7);
    t.insert(2);
    t.insert(4);
    EXPECT_EQ(t.size(), 5u);
    EXPECT_TRUE(t.contains(4));
}

TEST(DifferentArity, Four)
{
    NTree<int> t(4);
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.insert(4);
    t.insert(5);
    t.insert(6);
    EXPECT_EQ(t.size(), 6u);
    EXPECT_TRUE(t.contains(6));
}

TEST(TypesDouble, Basic)
{
    NTree<double> t(3);
    t.insert(2.5);
    t.insert(1.0);
    t.insert(3.14);
    t.insert(0.5);

    EXPECT_EQ(t.size(), 4u);
    EXPECT_TRUE(t.contains(2.5));
    EXPECT_FALSE(t.contains(9.99));
}

TEST(TypesString, Basic)
{
    NTree<std::string> t(3);
    t.insert("m");
    t.insert("a");
    t.insert("z");
    t.insert("b");

    EXPECT_EQ(t.size(), 4u);
    EXPECT_TRUE(t.contains("a"));
    EXPECT_FALSE(t.contains("x"));
}