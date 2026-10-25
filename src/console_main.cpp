#include "NTree.hpp"
#include "Sequence.hpp"
#include "ListSequence.hpp"

#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

// ============================================================
//  Вспомогательные функции ввода
// ============================================================

// Читает целое число с консоли.
// Возвращает false, если ввод пустой, не число, содержит «хвост»
// (например "12abc") или не помещается в int.
bool readInteger(const std::string& prompt, int& result)
{
    std::cout << prompt;

    std::string line;
    if (!std::getline(std::cin, line))
    {
        return false;
    }

    if (line.empty())
    {
        return false;
    }

    try
    {
        size_t consumed = 0;
        long value = std::stol(line, &consumed);

        while (consumed < line.size() && std::isspace((unsigned char)line[consumed]))
        {
            consumed++;
        }

        if (consumed != line.size())
        {
            return false;
        }

        if (value < std::numeric_limits<int>::min() ||
            value > std::numeric_limits<int>::max())
        {
            return false;
        }

        result = (int)value;
        return true;
    }
    catch (std::exception&)
    {
        return false;
    }
}

// Читает строку целиком (может быть пустой).
std::string readLine(const std::string& prompt)
{
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line))
    {
        return "";
    }
    return line;
}

// Ждёт Enter.
void waitForEnter()
{
    std::cout << "\nНажмите Enter, чтобы продолжить...";
    std::string ignored;
    std::getline(std::cin, ignored);
}

// ============================================================
//  Печать значения и последовательности
// ============================================================
template <typename T>
void printValue(const T& value)
{
    std::cout << value;
}

template <typename T>
void printSequence(const std::string& name, ListSequence<T>& sequence)
{
    std::cout << name << ": ";
    size_t length = sequence.GetLength();
    if (length == 0)
    {
        std::cout << "(пусто)";
    }
    for (size_t i = 0; i < length; i++)
    {
        printValue(sequence.Get(i));
        if (i + 1 < length)
        {
            std::cout << " ";
        }
    }
    std::cout << "\n";
}

// ============================================================
//  Парсинг значения под тип T
// ============================================================
template <typename T>
bool parseValue(const std::string& text, T& parsed)
{
    if (text.empty())
    {
        return false;
    }

    std::istringstream reader(text);
    if (!(reader >> parsed))
    {
        return false;
    }

    reader >> std::ws;
    return reader.eof();
}

// ============================================================
//  Меню
// ============================================================
void printMenu(const char* typeName, int n, size_t size)
{
    std::cout << "\n===== n-арное дерево (" << typeName << ", n = " << n
              << ", узлов: " << size << ") =====\n";
    std::cout << " 1. Вставить значения\n";
    std::cout << " 2. Поиск значения\n";
    std::cout << " 3. Удалить значение\n";
    std::cout << " 4. Показать обходы\n";
    std::cout << " 5. Найти по пути\n";
    std::cout << " 6. Извлечь поддерево\n";
    std::cout << " 7. Сохранить в строку\n";
    std::cout << " 8. Загрузить из строки\n";
    std::cout << " 9. Очистить дерево\n";
    std::cout << " 0. Выход\n";
}

// ============================================================
//  Вставка нескольких значений
// ============================================================
template <typename T>
void handleInsert(NTree<T>& tree)
{
    std::string line = readLine("Введите значения через пробел: ");
    if (line.empty())
    {
        std::cout << "Пустой ввод — ничего не добавлено.\n";
        return;
    }

    std::istringstream reader(line);
    int added = 0;
    int duplicates = 0;
    int invalid = 0;

    while (true)
    {
        T value{};
        if (reader >> value)
        {
            size_t before = tree.size();
            tree.insert(value);
            if (tree.size() > before)
            {
                added++;
            }
            else
            {
                duplicates++;
            }
            continue;
        }

        if (reader.eof())
        {
            break;
        }

        reader.clear();
        std::string garbage;
        reader >> garbage;
        invalid++;
    }

    std::cout << "Добавлено: " << added;
    if (duplicates > 0)
    {
        std::cout << ", дубликатов: " << duplicates;
    }
    if (invalid > 0)
    {
        std::cout << ", некорректных токенов: " << invalid;
    }
    std::cout << "\n";
}

// ============================================================
//  Поиск
// ============================================================
template <typename T>
void handleFind(NTree<T>& tree)
{
    std::string text = readLine("Значение для поиска: ");
    T value{};
    if (!parseValue<T>(text, value))
    {
        std::cout << "Некорректное значение.\n";
        return;
    }
    if (tree.contains(value))
    {
        std::cout << "Найдено.\n";
    }
    else
    {
        std::cout << "Не найдено.\n";
    }
}

// ============================================================
//  Удаление
// ============================================================
template <typename T>
void handleRemove(NTree<T>& tree)
{
    std::string text = readLine("Значение для удаления: ");
    T value{};
    if (!parseValue<T>(text, value))
    {
        std::cout << "Некорректное значение.\n";
        return;
    }

    size_t before = tree.size();
    tree.remove(value);
    if (tree.size() < before)
    {
        std::cout << "Удалено.\n";
    }
    else
    {
        std::cout << "Не найдено.\n";
    }
}

// ============================================================
//  Обходы
// ============================================================
template <typename T>
void handleTraversals(NTree<T>& tree)
{
    ListSequence<T> pre;
    tree.preorder(pre);
    printSequence("Preorder ", pre);

    ListSequence<T> post;
    tree.postorder(post);
    printSequence("Postorder", post);

    ListSequence<T> level;
    tree.levelOrder(level);
    printSequence("Level    ", level);
}

// ============================================================
//  Поиск по пути
// ============================================================
template <typename T>
void handleFindByPath(NTree<T>& tree)
{
    std::string path = readLine("Путь через точку (например 10.5.1): ");
    if (path.empty())
    {
        std::cout << "Путь не может быть пустым.\n";
        return;
    }

    T found{};
    if (tree.findByPath(path, found))
    {
        std::cout << "Найдено: ";
        printValue(found);
        std::cout << "\n";
    }
    else
    {
        std::cout << "Путь не найден.\n";
    }
}

// ============================================================
//  Извлечение поддерева
// ============================================================
template <typename T>
void handleExtractSubtree(NTree<T>& tree)
{
    std::string text = readLine("Значение корня поддерева: ");
    T value{};
    if (!parseValue<T>(text, value))
    {
        std::cout << "Некорректное значение.\n";
        return;
    }

    if (!tree.contains(value))
    {
        std::cout << "Такого узла в дереве нет.\n";
        return;
    }

    NTree<T> subtree = tree.extractSubtree(value);
    std::cout << "Поддерево: узлов " << subtree.size() << "\n";

    ListSequence<T> values;
    subtree.preorder(values);
    printSequence("Preorder ", values);
}

// ============================================================
//  Сохранение
// ============================================================
template <typename T>
void handleSave(const NTree<T>& tree)
{
    std::cout << "Строка: " << tree.saveToString() << "\n";
}

// ============================================================
//  Загрузка
// ============================================================
template <typename T>
void handleLoad(NTree<T>& tree)
{
    std::string text = readLine("Строка в формате (10 (5 (1)) (15)): ");
    if (text.empty())
    {
        std::cout << "Строка пуста.\n";
        return;
    }

    try
    {
        NTree<T> loaded = NTree<T>::loadFromString(text, tree.arity());
        tree = loaded;
        std::cout << "Загружено, узлов: " << tree.size() << "\n";
    }
    catch (std::exception& exception)
    {
        std::cout << "Ошибка загрузки: " << exception.what() << "\n";
    }
}

// ============================================================
//  Очистка
// ============================================================
template <typename T>
void handleClear(NTree<T>& tree)
{
    tree.clear();
    std::cout << "Дерево очищено.\n";
}

// ============================================================
//  Выбор n с проверкой
// ============================================================
int askArity()
{
    int n = 0;
    while (true)
    {
        if (!readInteger("Введите n (макс. детей у узла, 1..10): ", n))
        {
            std::cout << "Ошибка: введите целое число от 1 до 10.\n";
            continue;
        }
        if (n < 1 || n > 10)
        {
            std::cout << "Ошибка: n должно быть от 1 до 10.\n";
            continue;
        }
        return n;
    }
}

// ============================================================
//  Выбор пункта меню
// ============================================================
int askMenuChoice()
{
    while (true)
    {
        int choice = -1;
        if (!readInteger("Выбор: ", choice))
        {
            std::cout << "Ошибка: введите целое число от 0 до 9.\n";
            continue;
        }
        if (choice < 0 || choice > 9)
        {
            std::cout << "Ошибка: пункт меню от 0 до 9.\n";
            continue;
        }
        return choice;
    }
}

// ============================================================
//  Основной цикл для конкретного T
// ============================================================
template <typename T>
void runTree(const char* typeName)
{
    int n = askArity();
    NTree<T> tree(n);

    while (true)
    {
        printMenu(typeName, tree.arity(), tree.size());
        int choice = askMenuChoice();

        if (choice == 0)
        {
            std::cout << "Выход.\n";
            return;
        }

        try
        {
            switch (choice)
            {
                case 1: handleInsert(tree); break;
                case 2: handleFind(tree); break;
                case 3: handleRemove(tree); break;
                case 4: handleTraversals(tree); break;
                case 5: handleFindByPath(tree); break;
                case 6: handleExtractSubtree(tree); break;
                case 7: handleSave(tree); break;
                case 8: handleLoad(tree); break;
                case 9: handleClear(tree); break;
            }
        }
        catch (std::exception& exception)
        {
            std::cout << "Ошибка: " << exception.what() << "\n";
        }

        waitForEnter();
    }
}

// ============================================================
//  Выбор типа
// ============================================================
int askValueType()
{
    std::cout << "Выберите тип значений:\n";
    std::cout << " 1. int\n";
    std::cout << " 2. double\n";
    std::cout << " 3. string\n";

    while (true)
    {
        int choice = -1;
        if (!readInteger("Выбор: ", choice))
        {
            std::cout << "Ошибка: введите 1, 2 или 3.\n";
            continue;
        }
        if (choice < 1 || choice > 3)
        {
            std::cout << "Ошибка: выберите 1 (int), 2 (double) или 3 (string).\n";
            continue;
        }
        return choice;
    }
}

// ============================================================
//  Точка входа
// ============================================================
int main()
{
    std::cout << "n-арное дерево — консольный интерфейс\n\n";

    int typeChoice = askValueType();

    switch (typeChoice)
    {
        case 1: runTree<int>("int"); break;
        case 2: runTree<double>("double"); break;
        case 3: runTree<std::string>("string"); break;
    }

    return 0;
}