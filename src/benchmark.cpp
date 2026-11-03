#include "BinaryTree.hpp"
#include "array.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using namespace std::chrono;

template <typename Func>
double Time(Func func)
{
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration<double, std::milli>(end - start).count();
}

void Print(size_t n, double InsertTime, double FindTime, double RemoveTime)
{
    std::cout << "| " 
        << std::left << std::setw(8) << n << " | "
        << std::fixed << std::setprecision(2) << std::setw(12) << InsertTime << " | "
        << std::fixed << std::setprecision(2) << std::setw(12) << FindTime << " | "
        << std::fixed << std::setprecision(2) << std::setw(12) << RemoveTime << " |\n";
}

int main()
{
    size_t sizes[] {1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    size_t sizesCount = sizeof(sizes) / sizeof(sizes[0]);

    std::string separator = "+----------+--------------+--------------+--------------+";
    std::cout << separator << "\n";
    std::cout << "| " 
              << std::left << std::setw(8) << "N" << " | "
              << std::left << std::setw(12) << "Вставка" << " | "
              << std::left << std::setw(12) << "Поиск" << " | "
              << std::left << std::setw(12) << "Удаление" << " |\n";
    std::cout << separator << "\n";

    for (size_t idx = 0; idx < sizesCount; idx++)
    {
        size_t n = sizes[idx];
        BinaryTree<int> tree;

        double InsertTime = Time([&]() {
            size_t val = 42;
            for (size_t i = 0; i < n; i++)
            {
                val = (val * 1103515245 + 12345) % n;
                tree.Insert((int)val);
            }
        });
        double FindTime = Time([&]() {
            for (size_t i = 0; i < n; i++)
                tree.Contains((int)i);
            tree.Contains(-999);
        });
        double RemoveTime = Time([&]() {
            for (size_t i = 0; i < n; i++)
                tree.Remove((int)i);
        });
        Print(n, InsertTime, FindTime, RemoveTime);
    }
    std::cout << separator;
    return 0;
}