#include "BinaryTree.hpp"
#include "Sequence.hpp"
#include "ListSequence.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std::chrono;

template <typename Func>
double measureTimeMs(Func func)
{
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration<double, std::milli>(end - start).count();
}

void printTableRow(size_t n, double insertTime, double findTime, double removeTime)
{
    std::cout << "| " 
              << std::left << std::setw(8) << n << " | "
              << std::fixed << std::setprecision(2) << std::setw(12) << insertTime << " | "
              << std::fixed << std::setprecision(2) << std::setw(12) << findTime << " | "
              << std::fixed << std::setprecision(2) << std::setw(12) << removeTime << " |\n";
}

int main()
{

    size_t sizes[] = { 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 5000000 };
    size_t sizesCount = sizeof(sizes) / sizeof(sizes[0]);

    std::string separator = "+----------+--------------+--------------+--------------+";
    std::cout << separator << "\n";
    std::cout << "| " 
              << std::left << std::setw(8) << "N" << " | "
              << std::left << std::setw(12) << "Вставка (мс)" << " | "
              << std::left << std::setw(12) << "Поиск (мс)" << " | "
              << std::left << std::setw(12) << "Удаление (мс)" << "|\n";
    std::cout << separator << "\n";

    for (size_t idx = 0; idx < sizesCount; idx++)
    {
        size_t n = sizes[idx];
        BinaryTree<int> tree;

        double insertTime = measureTimeMs([&]() {
            size_t val = 25;
            for (size_t i = 0; i < n; i++)
            {
                val = (val * 1103515245 + 12345) % n; // LCG
                tree.insert((int)val);
            }
        });

        double findTime = measureTimeMs([&]() {
            for (size_t i = 0; i < n; i++)
                tree.contains((int)i);
            tree.contains(-999);
        });

        double removeTime = measureTimeMs([&]() {
            for (size_t i = 0; i < n; i++)
                tree.remove((int)i);
        });

        printTableRow(n, insertTime, findTime, removeTime);
    }
    std::cout << separator << std::endl;
    return 0;
}