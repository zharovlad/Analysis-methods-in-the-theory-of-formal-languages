#include "semant.hpp"


TScaner* Tree::scaner = nullptr;
Tree* Tree::current = nullptr;
bool Tree::flagInterpret = true;

map <TypeLex, string> StructNames;

Tree::Tree(Tree* copy) {
    n = new Node(copy->n);
    up = current;
    left = copy->left ? new Tree(copy->left) : nullptr;
    right = copy->right ? new Tree(copy->right) : nullptr;
}

Tree::Tree(Node *Data, Tree* u, Tree *l, Tree *r) {
    n = Data;
    up = u;
    left = l;
    right = r;
}

Tree::Tree() {
    n = new Node();
    n->objectType = TYPE_EMPTY;
    n->id = "Start prog";
    up = nullptr;
    left = nullptr;
    right = nullptr;
}

void Tree::SetLeft(Node *data) {
    if (!flagInterpret)
        return;
    left = new Tree(data, this, nullptr, nullptr);
}

void Tree::SetRight(Node *data) {
    if (!flagInterpret)
        return;
    right = new Tree(data, this, nullptr, nullptr);
}

void Tree::Print(string prefix) {

    string type;
    string sData = "";
    
    switch (n->objectType) {
    case TYPE_INT:
        type = "int";
        sData += " = " + to_string(n->dataValue->dataAsInt);
        break;
    case TYPE_DOUBLE:
        type = "double";
        sData += " = " + to_string(n->dataValue->dataAsDouble);
        break;
    case TYPE_NAME_STRUCT:
        type = StructNames[n->id];
        break;
    case TYPE_COMPOSITE_STATEMENT:
        type = "";
        break;
    case TYPE_FOR:
        type = "";
        break;
    case TYPE_MAIN:
        type = "";
        break;
    default:
        if (left != nullptr)
            left->Print();
        return;
    }
    cout << type << " " << prefix + n->id << sData << ", ";
    if (n->objectType == TYPE_NAME_STRUCT) {
        if (right != nullptr)
            right->left->Print(prefix + n->id + ".");
        if (left != nullptr)
            left->Print(prefix);
    }
    else {
        if (right != nullptr)
            right->Print(prefix);
        if (left != nullptr)
            left->Print(prefix);
    }
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
    if (from->n->id == id)
        return from;
    if (from->up == nullptr)
        return nullptr;
    if (from->up->right == from)
        return nullptr;

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
    if (!flagInterpret)
        return;
    // Установить текущий элемент дерева
    current = this;
}

Tree* Tree::GetPos() {
    // Получить current
    return current;
}

void Tree::CorrectPos() {
    if (!flagInterpret)
        return;
    // После создания переменной перейти к ней 
    current = current->left;
}

void Tree::AddID(TypeLex lex, int type, DataType dataType, Tree *struc, bool needAllocation) {
    if (!flagInterpret)
        return;

    // добавление переменной 

    if (FindOneLevel(current, lex) != nullptr) {
        scaner->printError("Semant", dual, lex);
    }

    Node* newVariable = new Node();
    newVariable->objectType = type;
    newVariable->id = lex;
    newVariable->dataType = dataType;

    if (needAllocation) {
        if (dataType == DTYPE_INT) {
            newVariable->dataValue->dataAsInt = 0;
        }
        else if (dataType == DTYPE_DOUBLE) {
            newVariable->dataValue->dataAsDouble = 0.1e-15;
        }
    }

    current->SetLeft(newVariable);
    current = current->left;

    if (type == TYPE_NAME_STRUCT) {
        StructNames[lex] = struc->n->id;
        if (needAllocation) {
            MemoryAllocation(struc);
        }
        else {
            current->right = struc->right;
        }
    }
}

void Tree::MemoryAllocation(Tree *struc) {
    if (!flagInterpret)
        return;
    current->right = new Tree(struc->right);
}

void Tree::AddTypeStruct(TypeLex lex) {
    if (!flagInterpret)
        return;
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
    // Ищет идентификатор в структуре
    Tree *structPlace = FindUp(current, prevID);
    if (structPlace != nullptr) {
        structPlace = FindDownLeft(structPlace->right, id);
        if (structPlace != nullptr)
            return structPlace->n->dataType;
    }

    scaner->printError("Semant", notDefine, error);
}

void Tree::AddAreaofVisibility(TypeLex block) {
    if (!flagInterpret)
        return;
    // Добавить область видимости

    Node* node = new Node();
    node->id = block;

    if (block == "for")
        node->objectType = TYPE_FOR;
    else if (block.empty()) {
        node->objectType = TYPE_COMPOSITE_STATEMENT;
        block = "composite statement";
        node->id = block;
    }
    else if (block == "main")
        node->objectType = TYPE_MAIN;
    else {
        node->objectType = TYPE_STRUCT;
    }

    current->SetLeft(node);

    current->left->AddEmpty(block);
    current = current->left->right;

}

void Tree::AddEmpty(TypeLex lex) {
    if (!flagInterpret)
        return;
    // Добавляет пустую вершину для новой области видимости
    Node* empty = new Node();
    empty->objectType = TYPE_EMPTY;
    empty->id = lex;
    this->SetRight(empty);
}

void Tree::FreeMemory() {
    // освобождаем память
    Tree* temp = current;
    current = current->up;
    while (temp->n->objectType != TYPE_EMPTY) {
        delete(temp);
        temp = current;
        current = current->up;
    }
    delete(temp);
    temp = current;
    current = current->up;
    delete(temp);
    current->left = nullptr;
}

void Tree::SetData(vector<string> ident, DataType newDataType, DataValue newDataValue) {
    if (!flagInterpret)
        return;
    // присваивание
    Tree* tree = Tree::FindID(ident[0]);
    if (ident.size() > 1) {
        for (int i = 1; i < ident.size(); i++) {
            tree = Tree::FindDownLeft(tree->right, ident[i]);
        }
    }

    switch (tree->n->dataType) {

    case DTYPE_INT:
        switch (newDataType) {
        case DTYPE_INT:         // int = int
            tree->n->dataValue->dataAsInt = newDataValue.dataAsInt;
            break;
        case DTYPE_DOUBLE:      // int = double
            tree->n->dataValue->dataAsInt = newDataValue.dataAsDouble;
            break;
        default:
            break;
        }
        break;

    case DTYPE_DOUBLE:
        switch (newDataType) {
        case DTYPE_INT:         // double = int
            tree->n->dataValue->dataAsDouble = newDataValue.dataAsInt;
            break;
        case DTYPE_DOUBLE:      // double = double
            tree->n->dataValue->dataAsDouble = newDataValue.dataAsDouble;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

}

pair<DataType, DataValue> Tree::GetData(vector<string> ident) {
    if (!flagInterpret) {
        return make_pair(DTYPE_UNKNOWN, DataValue());
    }
    Tree* tree = Tree::FindID(ident[0]);
    if (ident.size() > 1) {
        for (int i = 1; i < ident.size(); i++) {
            tree = Tree::FindDownLeft(tree->right, ident[i]);
        }
    }
    DataValue dataValue;
    switch (tree->n->dataType) {
    case DTYPE_DOUBLE:
        dataValue.dataAsDouble = tree->n->dataValue->dataAsDouble;
        break;
    case DTYPE_INT:
        dataValue.dataAsInt = tree->n->dataValue->dataAsInt;
        break;
    default:
        break;
    }
    return make_pair(tree->n->dataType, dataValue);
}

pair<DataType, DataValue> Tree::IntToDouble(pair<DataType, DataValue> value) {
    if (value.first == DTYPE_DOUBLE) {
        return value;
    }
    value.first = DTYPE_DOUBLE;
    value.second.dataAsDouble = value.second.dataAsInt;
    return value;
}

pair<DataType, DataValue> Tree::DoubleToInt(pair<DataType, DataValue> value) {
    if (value.first == DTYPE_INT) {
        return value;
    }
    value.first = DTYPE_INT;
    value.second.dataAsInt = value.second.dataAsDouble;
    return value;
}

Tree* Tree::NextInit(Tree* current, pair<DataType, DataValue> value) {

    if (current->n->dataType == DTYPE_INT) {
        value = DoubleToInt(value);
    }
    else {
        value = IntToDouble(value);
    }
    current->n->dataValue = new DataValue (value.second);
    return current->left;
}

Tree* Tree::GetStructFirstVar() { return right->left; }

void Tree::recalculateFlag(pair<DataType, DataValue> res) {
    if (res.first == DTYPE_INT && res.second.dataAsInt == 0) {
        flagInterpret = false;
    }
    else {
        flagInterpret = true;
    }
}

