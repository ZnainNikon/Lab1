#include <gtest/gtest.h>
#include "BinaryTree.hpp"
#include "array.hpp"
#include <string>

TEST(Constructor, EmptyOnCreate)
{
    BinaryTree<int> t;
    EXPECT_TRUE(t.Empty());
    EXPECT_EQ(t.Size(), 0u);
    EXPECT_EQ(t.Height(), 0);
}

TEST(Basic, InsertAndContains)
{
    BinaryTree<int> t;
    t.Insert(10);
    t.Insert(5);
    t.Insert(15);
    t.Insert(1);
    EXPECT_EQ(t.Size(), 4u);
    EXPECT_TRUE(t.Contains(10));
    EXPECT_TRUE(t.Contains(5));
    EXPECT_TRUE(t.Contains(15));
    EXPECT_TRUE(t.Contains(1));
    EXPECT_FALSE(t.Contains(999));
}

TEST(Basic, DuplicateNotInserted)
{
    BinaryTree<int> t;
    t.Insert(7);
    t.Insert(7);
    t.Insert(7);
    EXPECT_EQ(t.Size(), 1u);
}

TEST(Basic, Remove)
{
    BinaryTree<int> t;
    t.Insert(10);
    t.Insert(5);
    t.Insert(15);
    t.Insert(1);
    t.Remove(15);
    EXPECT_FALSE(t.Contains(15));
    EXPECT_EQ(t.Size(), 3u);
}

TEST(Basic, RemoveRoot)
{
    BinaryTree<int> t;
    t.Insert(10);
    t.Insert(5);
    t.Insert(15);
    t.Remove(10);
    EXPECT_FALSE(t.Contains(10));
    EXPECT_EQ(t.Size(), 2u);
}

TEST(Basic, Clear)
{
    BinaryTree<int> t;
    t.Insert(1);
    t.Insert(2);
    t.Insert(3);
    t.Clear();
    EXPECT_TRUE(t.Empty());
}

TEST(Traversals, InorderIsSorted)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1, 7, 12, 20}) 
    {
        t.Insert(x);
    }
    
    MyVector<int> result;
    t.Inorder([&](int v) { result.push_back(v); });
    
    ASSERT_EQ(result.size(), 7u);
    for (size_t i = 0; i + 1 < result.size(); i++)
        EXPECT_LT(result[i], result[i + 1]);
}

TEST(Traversals, PreorderStartsWithRoot)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) 
    { 
        t.Insert(x);
    }

    MyVector<int> result;
    t.Preorder([&](int v) { result.push_back(v); });
    ASSERT_GT(result.size(), 0u);
    EXPECT_EQ(result[0], 10);
}

TEST(Traversals, PostorderEndsWithRoot)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) 
    {
        t.Insert(x);
    }
    MyVector<int> result;
    t.Postorder([&](int v) { result.push_back(v); });
    ASSERT_GT(result.size(), 0u);
    EXPECT_EQ(result[result.size() - 1], 10);
}

TEST(Traversals, LevelStartsAtRoot)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15}) 
    {
        t.Insert(x);
    }
    MyVector<int> result;
    t.LevelOrder([&](int v) { result.push_back(v); });
    ASSERT_GT(result.size(), 0u);
    EXPECT_EQ(result[0], 10);
}

TEST(MapReduce, MapSquares)
{
    BinaryTree<int> t;
    for (int x : {1, 2, 3, 4, 5}) 
    {
        t.Insert(x);
    }
    auto sq = t.Map<int>([](int x) { return x * x; });
    EXPECT_TRUE(sq.Contains(25));
    EXPECT_TRUE(sq.Contains(9));
    EXPECT_EQ(sq.Size(), t.Size());
}

TEST(MapReduce, WhereEvens)
{
    BinaryTree<int> t;
    for (int x : {1, 2, 3, 4, 5, 6, 7, 8}) 
    {
        t.Insert(x);
    }
    auto evens = t.Where([](const int& x) { return x % 2 == 0; });
    EXPECT_EQ(evens.Size(), 4u);
    EXPECT_TRUE(evens.Contains(2));
    EXPECT_TRUE(evens.Contains(4));
    EXPECT_TRUE(evens.Contains(6));
    EXPECT_TRUE(evens.Contains(8));
    EXPECT_FALSE(evens.Contains(1));
}

TEST(MapReduce, WhereGreaterThan3)
{
    BinaryTree<int> t;
    for (int x : {1, 2, 3, 4, 5, 6}) 
    {
        t.Insert(x);
    }
    auto big = t.Where([](const int& x) { return x > 3; });
    EXPECT_EQ(big.Size(), 3u);
    EXPECT_TRUE(big.Contains(4));
    EXPECT_TRUE(big.Contains(5));
    EXPECT_TRUE(big.Contains(6));
    EXPECT_FALSE(big.Contains(1));
}

TEST(MapReduce, WhereEmptyResult)
{
    BinaryTree<int> t;
    for (int x : {1, 2, 3}) 
    {
        t.Insert(x);
    }
    auto nothing = t.Where([](const int& x) { return x > 100; });
    EXPECT_TRUE(nothing.Empty());
}

TEST(MapReduce, ReduceSum)
{
    BinaryTree<int> t;
    for (int x : {1, 2, 3, 4, 5}) 
    {
        t.Insert(x);
    }
    int sum = t.Reduce<int>([](const int& x, int acc) { return acc + x; }, 0);
    EXPECT_EQ(sum, 15);
}

TEST(Subtrees, ExtractAndContain)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1, 6}) 
    {
        t.Insert(x);
    }
    auto sub = t.ExtractSubtree(5);
    EXPECT_TRUE(sub.Contains(5));
    EXPECT_TRUE(t.ContainsSubtree(sub));
}

TEST(Subtrees, ExtractMissingThrows)
{
    BinaryTree<int> t;
    t.Insert(1);
    EXPECT_THROW(t.ExtractSubtree(999), std::out_of_range);
}

TEST(Subtrees, EmptySubtree)
{
    BinaryTree<int> t;
    t.Insert(1);
    BinaryTree<int> empty;
    EXPECT_TRUE(t.ContainsSubtree(empty));
}

TEST(Path, FindByAbsolutePath)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) 
    {
        t.Insert(x);
    }
    int v = 0;
    EXPECT_TRUE(t.FindByPath("10", v));
    EXPECT_EQ(v, 10);
    EXPECT_TRUE(t.FindByPath("10.5", v));
    EXPECT_EQ(v, 5);
    EXPECT_TRUE(t.FindByPath("10.5.1", v));
    EXPECT_EQ(v, 1);
    EXPECT_FALSE(t.FindByPath("10.999", v));
    EXPECT_FALSE(t.FindByPath("999", v));
}

TEST(Path, FindByRelativePath)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15}) 
    {
        t.Insert(x);
    }
    int v = 0;
    EXPECT_TRUE(t.FindByRelativePath("10.15", v));
    EXPECT_EQ(v, 15);
}

TEST(SaveLoad, RoundTrip)
{
    BinaryTree<int> t;
    for (int x : {10, 5, 15, 1}) 
    {
        t.Insert(x);
    }
    std::string s = t.SaveToString();
    auto t2 = BinaryTree<int>::LoadFromString(s);
    
    MyVector<int> a, b;
    t.Preorder([&](int v) { a.push_back(v); });
    t2.Preorder([&](int v) { b.push_back(v); });
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); i++)
        EXPECT_EQ(a[i], b[i]);
    EXPECT_EQ(t2.Size(), t.Size());
}

TEST(TypesDouble, Basic)
{
    BinaryTree<double> t;
    t.Insert(2.5);
    t.Insert(1.0);
    t.Insert(3.14);
    t.Insert(0.5);
    EXPECT_EQ(t.Size(), 4u);
    EXPECT_TRUE(t.Contains(2.5));
    EXPECT_FALSE(t.Contains(9.99));
}

TEST(TypesString, Basic)
{
    BinaryTree<std::string> t;
    t.Insert("m");
    t.Insert("a");
    t.Insert("z");
    t.Insert("b");
    EXPECT_EQ(t.Size(), 4u);
    EXPECT_TRUE(t.Contains("a"));
    EXPECT_FALSE(t.Contains("x"));
}