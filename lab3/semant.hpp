#include "scaner.hpp"

enum TypeObject {
    TYPE_INT = 1,
    TYPE_SHORT,
    TYPE_LONG,
    TYPE_DOUBLE,
    TYPE_STRUCT,
    TYPE_NAME_STRUCT,
    TYPE_EMPTY,     // специальный тип для пустой вершины
    TYPE_MAIN,      // специальный тип для функции main
    TYPE_COMPOSITE_STATEMENT,
    TYPE_FOR,
};

// реализованы только 2 типа данных, остальные unknown
enum DataType {
    DTYPE_UNKNOWN = 0,
    DTYPE_INT,
    DTYPE_DOUBLE,
};

union DataValue {
    int dataAsInt;
    double dataAsDouble;
};

struct Node {
    TypeLex id;             // Идентификатор переменной
    int objectType;         // Тип переменной
    DataType dataType;      // Тип данных
    DataValue* dataValue = new DataValue();   // Данные

    Node() {}

    Node(Node* copy) {
        id = copy->id;
        objectType = copy->objectType;
        dataType = copy->dataType;
        dataValue = new DataValue();
        if (dataType == DTYPE_DOUBLE) {
            dataValue->dataAsDouble = 0.1e-15;
        }
        else if (dataType == DTYPE_INT) {
            dataValue->dataAsInt = 0;
        }
    }
};

class Tree {
private:
    Node* n;
    Tree* up, *left, *right;
    static TScaner *scaner;
    static Tree *current;

public:
    
    Tree(Node *, Tree *, Tree *, Tree *);
    Tree(Tree*);
    Tree();

    void SetLeft(Node *);
    void SetRight(Node *);
    void Print(string prefix = "");

    static Tree *FindUp(Tree *, TypeLex);
    static Tree *FindOneLevel(Tree *, TypeLex);
    static Tree* FindDownLeft(Tree *, TypeLex);

    static void SetScaner(TScaner *);
    static Tree *GetPos();
    static void CorrectPos();
    void SetPos();

    static void AddID(TypeLex, int, DataType, Tree*, bool);
    static void AddTypeStruct(TypeLex);
    static Tree *FindID(TypeLex);
    static int FindIDinStruct(TypeLex, TypeLex, TypeLex);
    static void AddAreaofVisibility(TypeLex);
    void AddEmpty(TypeLex);
    static void MemoryAllocation(Tree*);
    static void FreeMemory();
    static void SetData(vector<string>, DataType, DataValue);
    static pair<DataType, DataValue> GetData(vector<string>);

};