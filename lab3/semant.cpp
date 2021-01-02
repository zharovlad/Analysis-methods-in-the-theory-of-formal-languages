#include "semant.hpp"
#include <map>

TScaner* Tree::scaner = nullptr;
Tree* Tree::current = nullptr;

map <TypeLex, string> StructNames;

Tree::Tree(Node *Data, Tree* u, Tree *l, Tree *r) {
    n = Data;
    up = u;
    left = l;
    right = r;
}

Tree::Tree() {
    n = new Node();
    n->dataType = TYPE_EMPTY;
    n->id = "Start prog";
    up = nullptr;
    left = nullptr;
    right = nullptr;
}

void Tree::SetLeft(Node *data) {
    left = new Tree(data, this, nullptr, nullptr);
}

void Tree::SetRight(Node *data) {
    right = new Tree(data, this, nullptr, nullptr);
}

void Tree::Print() {
    // Печать всего дерева ( корень - правый потомок - левый потомок).
    // Такой выбор вывода обусловлен удобством чтения переменных одной области видимости.

    string type;
    switch (n->dataType) {
    case TYPE_EMPTY:
        type = "empty";
        break;
    case TYPE_INT:
        type = "int";
        break;
    case TYPE_DOUBLE:
        type = "double";
        break;
    case TYPE_LONG:
        type = "long";
        break;
    case TYPE_NAME_STRUCT:
        type = StructNames[n->id];
        break;
    case TYPE_SHORT:
        type = "short";
        break;
    case TYPE_STRUCT:
        type = "struct";
        break;
    case TYPE_COMPOSITE_STATEMENT:
        type = "composite statement";
        break;
    case TYPE_FOR:
        type = "for";
        break;
    case TYPE_MAIN:
        type = "main";
        break;
    }

    cout << type << " " << n->id << " " << endl;

    if (right != nullptr)
        cout << "Right " << right->n->id << endl;

    if (left != nullptr)
        cout << "Left " << left->n->id << endl;

    cout << endl;

    if (right != nullptr)
        right->Print();
    if (left != nullptr)
        left->Print();
}

Tree *Tree::FindUp(Tree *from, TypeLex id) {
    // Поиск вверх до корня
    if (from == nullptr)
        return nullptr;
    if (from->n->id == id)
        return from;
    return FindUp(from->up, id);
}

Tree *Tree::FindOneLevel(Tree *from, TypeLex id) {
    // Поиск вверх по одной области видимости
    if (from == nullptr)
        return nullptr;
    if (from->n->dataType == TYPE_EMPTY)
        return nullptr;
    if (from->n->id == id)
        return from;
    return FindOneLevel(from->up, id);
}

Tree* Tree::FindDownLeft(Tree* current, TypeLex lex) {
    // Поиск вниз влево по дереву
    if (current->n->id == lex)
        return current;
    if (current->left == nullptr)
        return nullptr;
    return FindDownLeft(current->left, lex);
}

void Tree::SetScaner(TScaner* scan) {
    // Установить сканер 
    scaner = scan;
}

void Tree::SetPos() {
    // Установить текущий элемент дерева
    current = this;
}

Tree* Tree::GetPos() {
    // Получить current
    return current;
}

void Tree::CorrectPos() {
    // После создания переменной перейти к ней 
    current = current->left;
}

void Tree::AddID(TypeLex lex, int type, Tree *struc) {
    // добавление переменной 

    if (FindOneLevel(current, lex) != nullptr) {
        scaner->printError("Semant", dual, lex);
    }

    switch (type) {

    case Tint:
        type = TYPE_INT;
        break;
    case Tshort:
        type = TYPE_SHORT;
        break;
    case Tlong:
        type = TYPE_LONG;
        break;
    case Tdouble:
        type = TYPE_DOUBLE;
        break;
    case Tident:
        type = TYPE_NAME_STRUCT;
        break;
    }

    Node* newVariable = new Node();
    newVariable->dataType = type;
    newVariable->id = lex;
    current->SetLeft(newVariable);
    current = current->left;

    if (type == TYPE_NAME_STRUCT) {
        StructNames[lex] = struc->n->id;
        current->right = struc->right;
    }
}

void Tree::AddTypeStruct(TypeLex lex) {
    // добавление новой структуры
    if (FindOneLevel(current, lex) != nullptr) {
        scaner->printError("Semant", dual, lex);
    }
    AddAreaofVisibility(lex);
}

Tree *Tree::FindID(TypeLex lex) {
    // поиск объявления структуры по идентификатору
    Tree *pos = FindUp(current, lex);
    if (pos == nullptr) {
        scaner->printError("Semant", notDefine, lex);
    }
    return pos;
}

int Tree::FindIDinStruct(TypeLex id, TypeLex prevID, TypeLex error) {
    // Ищет идентификатор в структуре, возвращает его тип
    Tree *structPlace = FindUp(current, prevID);
    if (structPlace != nullptr) {
        structPlace = FindDownLeft(structPlace->right, id);
        if (structPlace != nullptr)
            return structPlace->n->dataType;
    }

    scaner->printError("Semant", notDefine, error);
}

void Tree::AddAreaofVisibility(TypeLex block) {
    // Добавить область видимости

    Node* node = new Node();
    node->id = block;

    if (block == "for")
        node->dataType = TYPE_FOR;
    else if (block.empty()) {
        node->dataType = TYPE_COMPOSITE_STATEMENT;
        block = "composite statement";
        node->id = block;
    }
    else if (block == "main")
        node->dataType = TYPE_MAIN;
    else {
        node->dataType = TYPE_STRUCT;
    }

    current->SetLeft(node);

    current->left->AddEmpty(block);
    current = current->left->right;

}

void Tree::AddEmpty(TypeLex lex) {
    // Добавляет пустую вершину для новой области видимости
    Node* empty = new Node();
    empty->dataType = TYPE_EMPTY;
    empty->id = lex;
    this->SetRight(empty);
}