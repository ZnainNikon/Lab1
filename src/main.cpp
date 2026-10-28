#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/scrolwin.h>

#include "Sequence.hpp"
#include "ListSequence.hpp"
#include "BinaryTree.hpp"

#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <algorithm>

template <typename ValueType>
wxString valueToStr(const ValueType& value)
{
    if constexpr (std::is_same_v<ValueType, std::string>)
    {
        wxString text(value);
        if (text.length() > 10) text = text.Left(9) + "…";
        return text;
    }
    else if constexpr (std::is_same_v<ValueType, double>)
    {
        return wxString::Format("%g", value);
    }
    else
    {
        return wxString::Format("%d", value);
    }
}

template <typename ValueType>
class TreeCanvas : public wxScrolledWindow
{
    public:
    explicit TreeCanvas(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxFULL_REPAINT_ON_RESIZE)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetScrollRate(20, 20);
        Bind(wxEVT_PAINT, &TreeCanvas::OnPaint, this);
    }

    void setTree(const BinaryTree<ValueType>& newTree)
    {
        tree = newTree;
        Refresh();
    }

    void setHighlightedValue(const ValueType& value, bool shouldHighlight)
    {
        highlightedValue = value;
        hasHighlight = shouldHighlight;
        Refresh();
    }

    private:
    BinaryTree<ValueType> tree;
    ValueType highlightedValue{};
    bool hasHighlight = false;

    static constexpr int nodeRadius = 22;
    static constexpr int levelHeight = 80;
    static constexpr int hGap = 30; // зазор между соседними узлами

    // Вспомогательная структура для сохранения координат
    struct PositionedNode
    {
        const typename BinaryTree<ValueType>::Node* node;
        int x;
        int y;
    };

    // 1. Считаем, сколько места (ширины) нужно поддереву
    int measureSubtree(const typename BinaryTree<ValueType>::Node* node)
    {
        if (!node) return 0;

        int leftWidth = measureSubtree(node->left);
        int rightWidth = measureSubtree(node->right);

        // Если это лист, ему нужен только его собственный диаметр
        int selfWidth = 2 * nodeRadius;

        if (leftWidth == 0 && rightWidth == 0) 
            return selfWidth;

        // Ширина = ширина левого + правого + зазор
        int childrenWidth = 0;
        if (leftWidth > 0 && rightWidth > 0) 
            childrenWidth = leftWidth + rightWidth + hGap;
        else 
            childrenWidth = std::max(leftWidth, rightWidth);

        return std::max(selfWidth, childrenWidth);
    }

    // 2. Расставляем узлы, зная их точную ширину
    void layoutSubtree(const typename BinaryTree<ValueType>::Node* node,
                       int minX, int currentY,
                       std::vector<PositionedNode>& out)
    {
        if (!node) return;

        int width = measureSubtree(node);
        int centerX = minX + width / 2;

        out.push_back({node, centerX, currentY});

        if (!node->left && !node->right) return;

        int leftWidth = measureSubtree(node->left);
        int rightWidth = measureSubtree(node->right);

        // Расставляем левое поддерево слева
        if (node->left)
        {
            layoutSubtree(node->left, minX, currentY + levelHeight, out);
        }

        // Расставляем правое поддерево справа
        if (node->right)
        {
            int rightStartX = minX + leftWidth + (node->left ? hGap : 0);
            // Если левого нет, правое должно начинаться с середины
            if (!node->left) 
                rightStartX = centerX + hGap / 2; 
                
            layoutSubtree(node->right, rightStartX, currentY + levelHeight, out);
        }
    }

    void OnPaint(wxPaintEvent&)
    {
        wxAutoBufferedPaintDC dc(this);
        DoPrepareDC(dc);
        dc.SetBackground(wxBrush(*wxWHITE));
        dc.Clear();

        if (!tree.getRoot())
        {
            dc.SetTextForeground(wxColour(150, 150, 150));
            dc.DrawText("Дерево пусто.\n\n"
                        "1. Введите значения в поле «Элементы».\n"
                        "2. Нажмите «Вставить».\n"
                        "3. Используйте кнопки «Операции».",
                        40, 40);
            return;
        }

        // 1. Вычисляем координаты
        std::vector<PositionedNode> positions;
        layoutSubtree(tree.getRoot(), 0, 50, positions);

        // 2. Вычисляем общий размер для скролла
        int maxX = 0;
        int maxY = 0;
        for (const auto& pos : positions)
        {
            if (pos.x > maxX) maxX = pos.x;
            if (pos.y > maxY) maxY = pos.y;
        }

        // Добавляем отступы, чтобы дерево не прилипало к краям
        SetVirtualSize(maxX + 100, maxY + 100);

        // 3. Рисуем связи (линии)
        dc.SetPen(wxPen(wxColour(130, 130, 130), 2));
        for (const auto& pos : positions)
        {
            if (pos.node->left)
            {
                for (const auto& child : positions)
                {
                    if (child.node == pos.node->left)
                    {
                        dc.DrawLine(pos.x, pos.y + nodeRadius, child.x, child.y - nodeRadius);
                        break;
                    }
                }
            }
            if (pos.node->right)
            {
                for (const auto& child : positions)
                {
                    if (child.node == pos.node->right)
                    {
                        dc.DrawLine(pos.x, pos.y + nodeRadius, child.x, child.y - nodeRadius);
                        break;
                    }
                }
            }
        }

        // 4. Рисуем сами узлы
        for (const auto& pos : positions)
        {
            bool isHighlighted = hasHighlight && (pos.node->value == highlightedValue);
            wxColour fillColor = isHighlighted ? wxColour(255, 220, 120) : wxColour(250, 250, 250);
            wxColour borderColor = isHighlighted ? wxColour(200, 120, 0) : wxColour(70, 70, 70);

            dc.SetPen(wxPen(borderColor, 2));
            dc.SetBrush(wxBrush(fillColor));
            dc.DrawCircle(pos.x, pos.y, nodeRadius);

            wxString text = valueToStr(pos.node->value);
            wxCoord textWidth, textHeight;
            dc.GetTextExtent(text, &textWidth, &textHeight);
            dc.SetTextForeground(*wxBLACK);
            dc.DrawText(text, pos.x - textWidth / 2, pos.y - textHeight / 2);
        }
    }
};

class MainFrame : public wxFrame
{
    public:
    MainFrame()
        : wxFrame(nullptr, wxID_ANY, "ЛР1 — бинарное дерево",
                  wxDefaultPosition, wxSize(1200, 750))
    {
        createMenu();
        CreateStatusBar(2);
        SetStatusText("Готово.", 0);
        createBody();
        SetMinSize(wxSize(1000, 600));
        refreshView();
    }

    private:
    enum class ValueKind { Int, Double, String };

    BinaryTree<int> treeOfInt;
    BinaryTree<double> treeOfDouble;
    BinaryTree<std::string> treeOfString;

    wxTextCtrl* inputField = nullptr;
    wxChoice* valueKindChoice = nullptr;

    TreeCanvas<int>* canvasForInt = nullptr;
    TreeCanvas<double>* canvasForDouble = nullptr;
    TreeCanvas<std::string>* canvasForString = nullptr;

    template <typename Visitor>
    auto visit(Visitor&& visitor)
    {
        switch (currentValueKind())
        {
            case ValueKind::Int:    return visitor(treeOfInt, canvasForInt);
            case ValueKind::Double: return visitor(treeOfDouble, canvasForDouble);
            case ValueKind::String: return visitor(treeOfString, canvasForString);
        }
        return visitor(treeOfInt, canvasForInt);
    }

    void createMenu()
    {
        auto* fileMenu = new wxMenu;
        fileMenu->Append(wxID_NEW, "&Новое дерево\tCtrl+N");
        fileMenu->Append(wxID_OPEN, "&Загрузить из строки...\tCtrl+O");
        fileMenu->Append(wxID_SAVE, "&Сохранить в строку...\tCtrl+S");
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_EXIT, "В&ыход\tCtrl+Q");

        auto* helpMenu = new wxMenu;
        helpMenu->Append(wxID_ABOUT, "&О программе");
        helpMenu->Append(wxID_HELP, "&Как пользоваться\tF1");

        auto* menuBar = new wxMenuBar;
        menuBar->Append(fileMenu, "&Файл");
        menuBar->Append(helpMenu, "&Справка");
        SetMenuBar(menuBar);

        Bind(wxEVT_MENU, &MainFrame::onNewTree, this, wxID_NEW);
        Bind(wxEVT_MENU, &MainFrame::onLoadFromString, this, wxID_OPEN);
        Bind(wxEVT_MENU, &MainFrame::onSaveToString, this, wxID_SAVE);
        Bind(wxEVT_MENU, &MainFrame::onAbout, this, wxID_ABOUT);
        Bind(wxEVT_MENU, &MainFrame::onHelp, this, wxID_HELP);
        Bind(wxEVT_MENU, &MainFrame::onExit, this, wxID_EXIT);
    }

    void createBody()
    {
        auto* rootPanel = new wxPanel(this);
        auto* horizontalLayout = new wxBoxSizer(wxHORIZONTAL);
        horizontalLayout->Add(createLeftPanel(rootPanel), 0, wxEXPAND);
        horizontalLayout->Add(createRightPanel(rootPanel), 1, wxEXPAND);
        rootPanel->SetSizer(horizontalLayout);
    }

    wxPanel* createLeftPanel(wxWindow* parent)
    {
        auto* leftPanel = new wxPanel(parent);
        auto* verticalLayout = new wxBoxSizer(wxVERTICAL);

        auto* titleLabel = new wxStaticText(leftPanel, wxID_ANY, "Управление");
        makeBold(titleLabel);
        verticalLayout->Add(titleLabel, 0, wxLEFT | wxTOP | wxBOTTOM, 10);

        verticalLayout->Add(createParametersBox(leftPanel), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
        verticalLayout->Add(createInputBox(leftPanel), 0, wxEXPAND | wxALL, 10);
        verticalLayout->Add(createOperationsBox(leftPanel), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
        verticalLayout->Add(createHelpBox(leftPanel), 0, wxEXPAND | wxALL, 10);
        verticalLayout->AddStretchSpacer(1);

        leftPanel->SetSizer(verticalLayout);
        leftPanel->SetMinSize(wxSize(380, -1));
        return leftPanel;
    }

    wxPanel* createRightPanel(wxWindow* parent)
    {
        auto* rightPanel = new wxPanel(parent);
        auto* verticalLayout = new wxBoxSizer(wxVERTICAL);

        auto* titleLabel = new wxStaticText(rightPanel, wxID_ANY, "Дерево (можно прокручивать)");
        makeBold(titleLabel);
        verticalLayout->Add(titleLabel, 0, wxLEFT | wxTOP | wxBOTTOM, 10);

        canvasForInt = new TreeCanvas<int>(rightPanel);
        canvasForDouble = new TreeCanvas<double>(rightPanel);
        canvasForString = new TreeCanvas<std::string>(rightPanel);

        verticalLayout->Add(canvasForInt, 1, wxEXPAND | wxALL, 10);
        verticalLayout->Add(canvasForDouble, 1, wxEXPAND | wxALL, 10);
        verticalLayout->Add(canvasForString, 1, wxEXPAND | wxALL, 10);

        canvasForDouble->Hide();
        canvasForString->Hide();

        rightPanel->SetSizer(verticalLayout);
        return rightPanel;
    }

    wxSizer* createParametersBox(wxWindow* parent)
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, parent, "Параметры");
        auto* valueKindRow = new wxBoxSizer(wxHORIZONTAL);
        valueKindRow->Add(new wxStaticText(parent, wxID_ANY, "Тип значений:"),
                          0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
        valueKindChoice = new wxChoice(parent, wxID_ANY);
        valueKindChoice->Append("int (целые)");
        valueKindChoice->Append("double (вещественные)");
        valueKindChoice->Append("string (строки)");
        valueKindChoice->SetSelection(0);
        valueKindRow->Add(valueKindChoice, 1, wxALIGN_CENTER_VERTICAL);
        box->Add(valueKindRow, 0, wxEXPAND | wxALL, 6);

        box->Add(new wxStaticText(parent, wxID_ANY,
                                  "Бинарное дерево поиска: у каждого узла\n"
                                  "не больше двух детей (левый и правый)."),
                 0, wxLEFT | wxRIGHT | wxBOTTOM, 6);

        valueKindChoice->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) { refreshView(); });
        return box;
    }

    wxSizer* createInputBox(wxWindow* parent)
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, parent, "Элементы");
        inputField = new wxTextCtrl(parent, wxID_ANY, "",
                                    wxDefaultPosition, wxSize(-1, 80),
                                    wxTE_MULTILINE | wxTE_PROCESS_ENTER);
        inputField->SetHint("Например для int: 10 5 15 1");
        inputField->Bind(wxEVT_TEXT_ENTER, &MainFrame::onInsert, this);
        box->Add(inputField, 1, wxEXPAND | wxALL, 6);

        auto* hint = new wxStaticText(parent, wxID_ANY,
            "int: 1 2 3 | double: 1.5 2.0 | string: apple banana");
        box->Add(hint, 0, wxLEFT | wxRIGHT | wxBOTTOM, 6);

        auto* insertButton = new wxButton(parent, wxID_ANY, "Вставить");
        auto* clearInputButton = new wxButton(parent, wxID_ANY, "Очистить поле");
        insertButton->Bind(wxEVT_BUTTON, &MainFrame::onInsert, this);
        clearInputButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { inputField->Clear(); });

        auto* buttonRow = new wxBoxSizer(wxHORIZONTAL);
        buttonRow->Add(insertButton, 1, wxRIGHT, 6);
        buttonRow->Add(clearInputButton, 0);
        box->Add(buttonRow, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 6);
        return box;
    }

    wxSizer* createOperationsBox(wxWindow* parent)
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, parent, "Операции");
        auto addButton = [&](const wxString& label, auto handler)
        {
            auto* button = new wxButton(parent, wxID_ANY, label);
            button->Bind(wxEVT_BUTTON, handler, this);
            box->Add(button, 0, wxEXPAND | wxALL, 4);
        };
        addButton("Поиск", &MainFrame::onFind);
        addButton("Удалить", &MainFrame::onRemove);
        addButton("Найти по пути", &MainFrame::onFindByPath);
        addButton("Извлечь поддерево", &MainFrame::onExtractSubtree);
        addButton("Очистить дерево", &MainFrame::onClearTree);
        return box;
    }

    wxSizer* createHelpBox(wxWindow* parent)
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, parent, "Как пользоваться");
        auto* text = new wxStaticText(parent, wxID_ANY,
            "1. Выберите тип значений.\n"
            "2. Введите значения через пробел.\n"
            "3. Нажмите «Вставить».\n"
            "4. «Поиск» подсветит узел.\n"
            "5. «Удалить» убирает узел.\n"
            "6. «Найти по пути»: 10.5.1\n"
            "   (корень → ребёнок → внук).\n"
            "7. «Извлечь поддерево» — все узлы.\n"
            "8. Дерево можно прокручивать.");
        box->Add(text, 0, wxALL, 6);
        return box;
    }

    static void makeBold(wxStaticText* label)
    {
        wxFont font = label->GetFont();
        font.SetPointSize(font.GetPointSize() + 2);
        font.SetWeight(wxFONTWEIGHT_BOLD);
        label->SetFont(font);
    }

    ValueKind currentValueKind() const
    {
        return (ValueKind)valueKindChoice->GetSelection();
    }

    wxString currentValueKindName() const
    {
        switch (currentValueKind())
        {
            case ValueKind::Int:    return "int";
            case ValueKind::Double: return "double";
            case ValueKind::String: return "string";
        }
        return "?";
    }

    template <typename ValueType>
    static wxString joinSequence(ListSequence<ValueType>& sequence)
    {
        wxString result;
        size_t length = sequence.GetLength();
        if (length > 30) length = 30;
        for (size_t i = 0; i < length; i++)
        {
            result << valueToStr(sequence.Get(i));
            if (i + 1 < length) result << " ";
        }
        if (sequence.GetLength() > 30) result << " …";
        return result;
    }

    template <typename ValueType>
    bool parseValue(const wxString& text, ValueType& parsed, wxString& error)
    {
        if (text.IsEmpty()) { error = "Пустое значение."; return false; }
        if constexpr (std::is_same_v<ValueType, std::string>)
        {
            if (text.length() > 40) { error = "Слишком длинная строка."; return false; }
            parsed = text.ToStdString();
            return true;
        }
        else if constexpr (std::is_same_v<ValueType, double>)
        {
            if (!text.ToCDouble(&parsed)) { error = "Введите корректное число."; return false; }
            if (!std::isfinite(parsed)) { error = "Число слишком большое."; return false; }
            return true;
        }
        else
        {
            long v = 0;
            if (!text.ToLong(&v)) { error = "Введите целое число."; return false; }
            if (v < -2147483647L || v > 2147483647L) { error = "Число вне диапазона int."; return false; }
            parsed = (ValueType)v;
            return true;
        }
    }

    void refreshView()
    {
        canvasForInt->Hide();
        canvasForDouble->Hide();
        canvasForString->Hide();
        visit([&](auto&, auto* canvas) { canvas->Show(); });
        visit([&](auto& tree, auto* canvas) { canvas->setTree(tree); });

        size_t totalNodes = visit([](auto& tree, auto*) { return tree.size(); });
        int treeHeight = visit([](auto& tree, auto*) { return tree.height(); });

        SetStatusText(wxString::Format("Узлов: %zu  тип: %s  высота: %d",
                                       totalNodes, currentValueKindName(), treeHeight), 0);
        SetStatusText(wxString::Format("Узлов: %zu", totalNodes), 1);
        canvasForInt->GetParent()->Layout();
    }

    void onInsert(wxCommandEvent&)
    {
        wxString inputText = inputField->GetValue();
        if (inputText.Trim().IsEmpty())
        {
            wxMessageBox("Введите хотя бы одно значение.", "Пустой ввод",
                         wxOK | wxICON_WARNING, this);
            return;
        }

        std::istringstream reader(inputText.ToStdString());
        int addedCount = 0, skippedCount = 0;
        visit([&](auto& tree, auto*)
        {
            using ValueType = typename std::decay_t<decltype(tree)>::value_type;
            ValueType value{};
            while (reader >> value)
            {
                size_t sizeBefore = tree.size();
                tree.insert(value);
                if (tree.size() > sizeBefore) addedCount++;
                else skippedCount++;
            }
        });

        inputField->Clear();
        refreshView();

        wxString message = wxString::Format("Добавлено: %d", addedCount);
        if (skippedCount > 0) message += wxString::Format(", дубликатов: %d", skippedCount);
        SetStatusText(message, 0);
    }

    void onFind(wxCommandEvent&)
    {
        wxTextEntryDialog dialog(this, "Значение для поиска:", "Поиск");
        if (dialog.ShowModal() != wxID_OK) return;
        wxString enteredText = dialog.GetValue();

        visit([&](auto& tree, auto* canvas)
        {
            using ValueType = typename std::decay_t<decltype(tree)>::value_type;
            ValueType value{};
            wxString error;
            if (!parseValue<ValueType>(enteredText, value, error))
            {
                wxMessageBox(error, "Некорректное значение", wxOK | wxICON_ERROR, this);
                return;
            }
            bool found = tree.contains(value);
            canvas->setHighlightedValue(value, found);
            SetStatusText(found ? wxString::Format("Найдено: %s", valueToStr(value))
                                : wxString::Format("Не найдено: %s", valueToStr(value)), 0);
        });
    }

    void onRemove(wxCommandEvent&)
    {
        wxTextEntryDialog dialog(this, "Значение для удаления:", "Удаление");
        if (dialog.ShowModal() != wxID_OK) return;
        wxString enteredText = dialog.GetValue();

        visit([&](auto& tree, auto*)
        {
            using ValueType = typename std::decay_t<decltype(tree)>::value_type;
            ValueType value{};
            wxString error;
            if (!parseValue<ValueType>(enteredText, value, error))
            {
                wxMessageBox(error, "Некорректное значение", wxOK | wxICON_ERROR, this);
                return;
            }
            if (tree.empty())
            {
                wxMessageBox("Дерево пусто.", "Пустое дерево", wxOK | wxICON_INFORMATION, this);
                return;
            }
            size_t sizeBefore = tree.size();
            tree.remove(value);
            refreshView();
            SetStatusText(tree.size() < sizeBefore ? wxString::Format("Удалено: %s", valueToStr(value))
                                                   : wxString::Format("Не найдено: %s", valueToStr(value)), 0);
        });
    }

    void onFindByPath(wxCommandEvent&)
    {
        wxTextEntryDialog dialog(this,
            "Путь через точку, например 10.5.1\n"
            "(10 — корень, 5 — его ребёнок, 1 — внук)",
            "Поиск по пути");
        if (dialog.ShowModal() != wxID_OK) return;
        std::string path = dialog.GetValue().ToStdString();
        if (path.empty())
        {
            wxMessageBox("Путь не может быть пустым.", "Ошибка", wxOK | wxICON_WARNING, this);
            return;
        }

        visit([&](auto& tree, auto* canvas)
        {
            using ValueType = typename std::decay_t<decltype(tree)>::value_type;
            ValueType foundValue{};
            if (tree.findByPath(path, foundValue))
            {
                canvas->setHighlightedValue(foundValue, true);
                SetStatusText(wxString::Format("Найдено по пути: %s", valueToStr(foundValue)), 0);
            }
            else
            {
                canvas->setHighlightedValue(ValueType{}, false);
                SetStatusText("Путь не найден", 0);
                wxMessageBox("Такого пути нет.", "Путь не найден", wxOK | wxICON_INFORMATION, this);
            }
        });
    }

    void onExtractSubtree(wxCommandEvent&)
    {
        wxTextEntryDialog dialog(this, "Значение корня поддерева:", "Извлечь поддерево");
        if (dialog.ShowModal() != wxID_OK) return;
        wxString enteredText = dialog.GetValue();

        visit([&](auto& tree, auto*)
        {
            using ValueType = typename std::decay_t<decltype(tree)>::value_type;
            ValueType rootValue{};
            wxString error;
            if (!parseValue<ValueType>(enteredText, rootValue, error))
            {
                wxMessageBox(error, "Некорректное значение", wxOK | wxICON_ERROR, this);
                return;
            }
            if (!tree.contains(rootValue))
            {
                wxMessageBox("Узла с таким значением нет.", "Не найдено", wxOK | wxICON_INFORMATION, this);
                return;
            }
            try
            {
                auto subtree = tree.extractSubtree(rootValue);
                ListSequence<ValueType> values;
                subtree.preorder(values);
                wxMessageBox(wxString::Format(
                                 "Поддерево с корнем %s\nУзлов: %zu\nPreorder: %s",
                                 valueToStr(rootValue), subtree.size(), joinSequence(values)),
                             "Поддерево", wxOK | wxICON_INFORMATION, this);
            }
            catch (std::exception& exception)
            {
                wxMessageBox(wxString("Ошибка: ") + exception.what(), "Извлечение",
                             wxOK | wxICON_ERROR, this);
            }
        });
    }

    void onClearTree(wxCommandEvent&)
    {
        visit([](auto& tree, auto*) { tree.clear(); });
        refreshView();
        SetStatusText("Дерево очищено", 0);
    }

    void onNewTree(wxCommandEvent&)
    {
        visit([](auto& tree, auto*) { tree.clear(); });
        refreshView();
        SetStatusText("Новое дерево", 0);
    }

    void onLoadFromString(wxCommandEvent&)
    {
        wxTextEntryDialog dialog(this,
            "Строка в формате (10 (5 () ()) (15 () ()))",
            "Загрузить дерево");
        if (dialog.ShowModal() != wxID_OK) return;
        std::string serializedTree = dialog.GetValue().ToStdString();
        if (serializedTree.empty())
        {
            wxMessageBox("Строка не может быть пустой.", "Ошибка", wxOK | wxICON_WARNING, this);
            return;
        }

        try
        {
            visit([&](auto& tree, auto*)
            {
                using TreeType = std::decay_t<decltype(tree)>;
                tree = TreeType::loadFromString(serializedTree);
            });
            refreshView();
            SetStatusText("Дерево загружено", 0);
        }
        catch (std::exception& exception)
        {
            wxMessageBox(wxString("Не удалось загрузить: ") + exception.what(),
                         "Ошибка загрузки", wxOK | wxICON_ERROR, this);
        }
    }

    void onSaveToString(wxCommandEvent&)
    {
        wxString serializedTree = visit([](auto& tree, auto*) { return wxString(tree.saveToString()); });
        wxTextEntryDialog dialog(this, "Строка дерева:", "Сохранить дерево", serializedTree);
        dialog.ShowModal();
    }

    void onAbout(wxCommandEvent&)
    {
        wxMessageBox(
            "Лабораторная работа №1\n"
            "Бинарное дерево поиска (C++17 + wxWidgets)\n\n"
            "Типы: int, double, string.\n"
            "Операции: insert, contains, remove, extractSubtree,\n"
            "findByPath, saveToString / loadFromString.",
            "О программе", wxOK | wxICON_INFORMATION, this);
    }

    void onHelp(wxCommandEvent&)
    {
        wxMessageBox(
            "Как пользоваться:\n\n"
            "1. Выберите тип значений.\n"
            "2. Введите значения через пробел.\n"
            "3. Нажмите «Вставить».\n"
            "4. «Поиск» подсветит найденный узел.\n"
            "5. «Удалить» убирает узел.\n"
            "6. «Найти по пути»: 10.5.1.\n"
            "7. «Извлечь поддерево» — все узлы.\n"
            "8. Дерево можно прокручивать.",
            "Как пользоваться", wxOK | wxICON_INFORMATION, this);
    }

    void onExit(wxCommandEvent&) { Close(true); }
};

class LabApplication : public wxApp
{
    public:
    bool OnInit() override
    {
        auto* mainWindow = new MainFrame();
        mainWindow->Show();
        return true;
    }
};

wxIMPLEMENT_APP(LabApplication);