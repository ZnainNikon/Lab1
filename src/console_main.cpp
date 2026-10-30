#include "BinaryTree.hpp"
#include "array.hpp"
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

bool readInteger(const std::string& prompt, int& result)
{
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) return false;
    if (line.empty()) return false;
    try
    {
        size_t consumed = 0;
        long value = std::stol(line, &consumed);
        while (consumed < line.size() && std::isspace((unsigned char)line[consumed])) consumed++;
        if (consumed != line.size()) return false;
        if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) return false;
        result = (int)value;
        return true;
    }
    catch (std::exception&) { return false; }
}

std::string readLine(const std::string& prompt)
{
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) return "";
    return line;
}

void waitForEnter()
{
    std::cout << "\nНажмите Enter, чтобы продолжить...";
    std::string ignored;
    std::getline(std::cin, ignored);
}


template <typename T>
void printValue(const T& value) { std::cout << value; }

template <typename T>
std::string valueForTree(const T& value)
{
    std::ostringstream oss;
    oss << value;
    std::string text = oss.str();
    if (text.length() > 12) text = text.substr(0, 11) + "…";
    return text;
}

template <typename T>
bool parseValue(const std::string& text, T& parsed)
{
    if (text.empty()) return false;
    std::istringstream reader(text);
    if (!(reader >> parsed)) return false;
    reader >> std::ws;
    return reader.eof();
}


template <typename T>
void printTraversal(const std::string& name, const BinaryTree<T>& tree, int type)
{
    std::cout << name << ": ";
    bool isEmpty = true;
    auto printer = [&](const T& value) {
        std::cout << value << " ";
        isEmpty = false;
    };

    if (type == 1) tree.preorder(printer);
    else if (type == 2) tree.inorder(printer);
    else if (type == 3) tree.postorder(printer);
    else if (type == 4) tree.levelOrder(printer);

    if (isEmpty) std::cout << "(пусто)";
    std::cout << "\n";
}


template <typename T>
void printTree(const BinaryTree<T>& tree)
{
    if (tree.empty()) {
        std::cout << "(дерево пусто)\n";
        return;
    }

    struct PosNode {
        const typename BinaryTree<T>::Node* node;
        int x;
        int y;
    };

    MyVector<PosNode> nodes; 
    int maxWidth = 0;
    int maxHeight = 0;

    auto calcPos = [&](auto&& self, const typename BinaryTree<T>::Node* n, int x, int y) -> void {
        if (!n) return;
        std::string text = valueForTree(n->value);
        int textWidth = text.length();
        
        nodes.push_back({n, x, y});
        if (x + textWidth > maxWidth) maxWidth = x + textWidth;
        if (y > maxHeight) maxHeight = y;
        
        int shift = textWidth + 2;
        if (n->left) self(self, n->left.get(), x - shift, y + 1);
        if (n->right) self(self, n->right.get(), x + shift, y + 1);
    };

    calcPos(calcPos, tree.getRoot().get(), 80, 0);

    int rows = maxHeight * 2 + 1;
    int cols = maxWidth + 40;
    
    char** canvas = new char*[rows];
    for (int r = 0; r < rows; r++) {
        canvas[r] = new char[cols];
        for (int c = 0; c < cols; c++) canvas[r][c] = ' ';
    }

    for (size_t i = 0; i < nodes.size(); i++) {
        const auto& pos = nodes[i];
        if (pos.node->left) {
            int startX = pos.x - 1; 
            int startY = pos.y * 2 + 1;
            for (size_t j = 0; j < nodes.size(); j++) {
                const auto& childPos = nodes[j];
                if (childPos.node == pos.node->left.get()) {
                    int endX = childPos.x + valueForTree(childPos.node->value).length() / 2;
                    int endY = childPos.y * 2;
                    for (int k = 0; k < 10; k++) {
                        int cx = startX - (startX - endX) * k / 10;
                        int cy = startY - (startY - endY) * k / 10;
                        if (cy >= 0 && cy < rows && cx >= 0 && cx < cols) {
                            if (canvas[cy][cx] == ' ') canvas[cy][cx] = '/';
                        }
                    }
                }
            }
        }
        if (pos.node->right) {
            int startX = pos.x + valueForTree(pos.node->value).length();
            int startY = pos.y * 2 + 1;
            for (size_t j = 0; j < nodes.size(); j++) {
                const auto& childPos = nodes[j];
                if (childPos.node == pos.node->right.get()) {
                    int endX = childPos.x + valueForTree(childPos.node->value).length() / 2;
                    int endY = childPos.y * 2;
                    for (int k = 0; k < 10; k++) {
                        int cx = startX + (endX - startX) * k / 10;
                        int cy = startY - (startY - endY) * k / 10;
                        if (cy >= 0 && cy < rows && cx >= 0 && cx < cols) {
                            if (canvas[cy][cx] == ' ') canvas[cy][cx] = '\\';
                        }
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < nodes.size(); i++) {
        const auto& pos = nodes[i];
        std::string text = valueForTree(pos.node->value);
        int startX = pos.x - text.length() / 2;
        int y = pos.y * 2;
        for (size_t k = 0; k < text.length(); k++) {
            int cx = startX + k;
            if (cx >= 0 && cx < cols && y >= 0 && y < rows) {
                canvas[y][cx] = text[k];
            }
        }
    }

    std::cout << "\n";
    for (int y = 0; y < rows; y++) {
        bool hasContent = false;
        for (int x = 0; x < cols; x++) {
            if (canvas[y][x] != ' ') hasContent = true;
        }
        if (hasContent) {
            int lastChar = cols - 1;
            while (lastChar >= 0 && canvas[y][lastChar] == ' ') lastChar--;
            for (int x = 0; x <= lastChar; x++) {
                std::cout << canvas[y][x];
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";

    // Очистка памяти
    for (int r = 0; r < rows; r++) delete[] canvas[r];
    delete[] canvas;
}


void printMenu(const char* typeName, size_t size)
{
    std::cout << "\n===== бинарное дерево (" << typeName
              << ", узлов: " << size << ") =====\n";
    std::cout << " 1. Вставить значения\n";
    std::cout << " 2. Поиск значения\n";
    std::cout << " 3. Удалить значение\n";
    std::cout << " 4. Показать обходы\n";
    std::cout << " 5. Показать дерево\n";
    std::cout << " 6. Найти по пути\n";
    std::cout << " 7. Извлечь поддерево\n";
    std::cout << " 8. Сохранить в строку\n";
    std::cout << " 9. Загрузить из строки\n";
    std::cout << "10. Очистить дерево\n";
    std::cout << " 0. Выход\n";
}


template <typename T>
void handleInsert(BinaryTree<T>& tree)
{
    std::string line = readLine("Введите значения через пробел: ");
    if (line.empty()) { std::cout << "Пустой ввод.\n"; return; }

    std::istringstream reader(line);
    int added = 0, duplicates = 0, invalid = 0;
    while (true)
    {
        T value{};
        if (reader >> value)
        {
            size_t before = tree.size();
            tree.insert(value);
            if (tree.size() > before) added++;
            else duplicates++;
            continue;
        }
        if (reader.eof()) break;
        reader.clear();
        std::string garbage;
        reader >> garbage;
        invalid++;
    }
    std::cout << "Добавлено: " << added;
    if (duplicates > 0) std::cout << ", дубликатов: " << duplicates;
    if (invalid > 0) std::cout << ", некорректных токенов: " << invalid;
    std::cout << "\n";
}

template <typename T>
void handleFind(BinaryTree<T>& tree)
{
    std::string text = readLine("Значение для поиска: ");
    T value{};
    if (!parseValue<T>(text, value)) { std::cout << "Некорректное значение.\n"; return; }
    std::cout << (tree.contains(value) ? "Найдено.\n" : "Не найдено.\n");
}

template <typename T>
void handleRemove(BinaryTree<T>& tree)
{
    std::string text = readLine("Значение для удаления: ");
    T value{};
    if (!parseValue<T>(text, value)) { std::cout << "Некорректное значение.\n"; return; }
    size_t before = tree.size();
    tree.remove(value);
    std::cout << (tree.size() < before ? "Удалено.\n" : "Не найдено.\n");
}

template <typename T>
void handleTraversals(BinaryTree<T>& tree)
{
    printTraversal("Preorder ", tree, 1);
    printTraversal("Inorder  ", tree, 2);
    printTraversal("Postorder", tree, 3);
    printTraversal("Level    ", tree, 4);
}

template <typename T>
void handleFindByPath(BinaryTree<T>& tree)
{
    std::string path = readLine("Путь через точку (например 10.5.1): ");
    if (path.empty()) { std::cout << "Путь не может быть пустым.\n"; return; }
    T found{};
    if (tree.findByPath(path, found)) { std::cout << "Найдено: "; printValue(found); std::cout << "\n"; }
    else std::cout << "Путь не найден.\n";
}

template <typename T>
void handleExtractSubtree(BinaryTree<T>& tree)
{
    std::string text = readLine("Значение корня поддерева: ");
    T value{};
    if (!parseValue<T>(text, value)) { std::cout << "Некорректное значение.\n"; return; }
    if (!tree.contains(value)) { std::cout << "Такого узла нет.\n"; return; }
    
    BinaryTree<T> subtree = tree.extractSubtree(value);
    std::cout << "Поддерево: узлов " << subtree.size() << "\n";
    std::cout << "Preorder: ";
    subtree.preorder([](const T& v) { std::cout << v << " "; });
    std::cout << "\n";
}

template <typename T>
void handleSave(const BinaryTree<T>& tree)
{
    std::cout << "Строка: " << tree.saveToString() << "\n";
}

template <typename T>
void handleLoad(BinaryTree<T>& tree)
{
    std::string text = readLine("Строка в формате (10 (5 () ()) (15 () ())): ");
    if (text.empty()) { std::cout << "Строка пуста.\n"; return; }
    try
    {
        BinaryTree<T> loaded = BinaryTree<T>::loadFromString(text);
        tree = loaded;
        std::cout << "Загружено, узлов: " << tree.size() << "\n";
        printTree(tree);
    }
    catch (std::exception& exception)
    {
        std::cout << "Ошибка загрузки: " << exception.what() << "\n";
    }
}

template <typename T>
void handleClear(BinaryTree<T>& tree)
{
    tree.clear();
    std::cout << "Дерево очищено.\n";
}

int askMenuChoice()
{
    while (true)
    {
        int choice = -1;
        if (!readInteger("Выбор: ", choice))
        {
            std::cout << "Ошибка: введите целое число от 0 до 10.\n";
            continue;
        }
        if (choice < 0 || choice > 10)
        {
            std::cout << "Ошибка: пункт меню от 0 до 10.\n";
            continue;
        }
        return choice;
    }
}

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
            std::cout << "Ошибка: 1, 2 или 3.\n";
            continue;
        }
        return choice;
    }
}

template <typename T>
void runTree(const char* typeName)
{
    BinaryTree<T> tree;
    while (true)
    {
        printMenu(typeName, tree.size());
        int choice = askMenuChoice();
        if (choice == 0) { std::cout << "Выход.\n"; return; }
        try
        {
            switch (choice)
            {
                case 1: handleInsert(tree); break;
                case 2: handleFind(tree); break;
                case 3: handleRemove(tree); break;
                case 4: handleTraversals(tree); break;
                case 5: printTree(tree); break;
                case 6: handleFindByPath(tree); break;
                case 7: handleExtractSubtree(tree); break;
                case 8: handleSave(tree); break;
                case 9: handleLoad(tree); break;
                case 10: handleClear(tree); break;
            }
        }
        catch (std::exception& exception)
        {
            std::cout << "Ошибка: " << exception.what() << "\n";
        }
        waitForEnter();
    }
}

int main()
{
    std::cout << "Бинарное дерево — консольный интерфейс\n\n";
    int typeChoice = askValueType();
    switch (typeChoice)
    {
        case 1: runTree<int>("int"); break;
        case 2: runTree<double>("double"); break;
        case 3: runTree<std::string>("string"); break;
    }
    return 0;
}