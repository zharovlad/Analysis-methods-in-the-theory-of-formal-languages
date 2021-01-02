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

struct Node {
    TypeLex id;             // Идентификатор переменной
    int dataType;           // Тип переменной
    
             //   дополнительные  возможности:
    
    Node() {    }
    Node(const Node* copy) {
        this->id = copy->id;
        this->dataType = copy->dataType;
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
    Tree();

    void SetLeft(Node *);
    void SetRight(Node *);
    void Print();

    static Tree *FindUp(Tree *, TypeLex);
    static Tree *FindOneLevel(Tree *, TypeLex);
    static Tree* FindDownLeft(Tree *, TypeLex);

    static void SetScaner(TScaner *);
    static Tree *GetPos();
    static void CorrectPos();
    void SetPos();

    static void AddID(TypeLex, int, Tree*);
    static void AddTypeStruct(TypeLex);
    Tree *FindID(TypeLex);
    static int FindIDinStruct(TypeLex, TypeLex, TypeLex);
    static void AddAreaofVisibility(TypeLex);
    void AddEmpty(TypeLex);
};