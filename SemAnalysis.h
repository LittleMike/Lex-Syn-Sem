#ifndef SEMANALYSISH
#define SEMANALYSISH

#pragma once

#include <stdio.h>
#include <string>
using namespace std;
#include "SyntaxAnalysis.h"

#define SCOPENUM 100

typedef enum {intTy,charTy,arrayTy,recordTy,boolTy} Typekind; //类型种类
typedef enum {typeKind,varKind,procKind} IdKind; //标识符类别
typedef enum {dir,indir} AccessKind;

struct ParamTable;
struct TypeIR;

struct FieldChain
{
    string idname;//变量名
    TypeIR* unitType;//域中成员类型
    int offset;//偏移
    FieldChain* next;//链表指针
    FieldChain()
    {
        next = NULL;
    }
};


//类型内部结构
struct TypeIR
{
    int size;
    Typekind kind;
    union
    {
        struct
        {
            TypeIR* indexTy;
            TypeIR* elemTy;
        } arrayAttr; //数组信息

        FieldChain* body;//记录类型域链
    } more;
    TypeIR(/* int size, */Typekind kind)
    {
        //this->size = size;
        this->kind = kind;
        this->more.body = NULL;
        this->more.arrayAttr.elemTy = NULL;
        this->more.arrayAttr.indexTy = NULL;
    }
};

struct AttrbuteIR
{
    struct TypeIR* idtype;//指向标识符类型内部表示
    IdKind kind;//标识符类型
    union
    {
        struct
        {
            AccessKind access;
            int level;
            int off;
        } VarAttr; //变量标识符

        struct
        {
            int level;
            ParamTable* param;//参数表
            int code;
            int size;
        } ProcAttr; //过程标识符
    } more;
};

//符号表数据结构
struct SymbTable
{
    string idname;
    AttrbuteIR attrIR;
    struct SymbTable* next;

    SymbTable()
    {
        next = NULL;
    }
};



//形参信息表数据结构
struct ParamTable
{
    SymbTable* entry;
    ParamTable* next;
};

class STable
{

public:
    //分程序表
    //static SymbTable* Scope[SCOPENUM];
    static SymbTable** Scope;
    static int Level;

    static void CreatTable(void);
    static void DestroyTable(void);
    static bool Enter(string Id,AttrbuteIR* attribP,SymbTable** Entry);
    static bool FindEntry(string id,bool flag,SymbTable** Entry);
    static bool SearchoneTable(string id,int currentLevel,SymbTable** Entry);
    static bool FindField(string id,FieldChain* head,FieldChain** Entry);
    static void PrindSymbTable();

    static void PrintOneLayer(int level);
    static void PrintFieldChain(FieldChain  *currentP);
    static void PrintSymbTable( );
};

class SemAnalysis
{
public:
    SemAnalysis(void);
    ~SemAnalysis(void);
    static void initialize(void);
private:
    static TypeIR* intPtr;
    static TypeIR* charPtr;
    static TypeIR* boolPtr;
public:
    static TypeIR* TypeProcess(TreeNode* t, Kind deckind);
    static TypeIR* nameType(TreeNode* t);
    static void statement(TreeNode* t);
    static TypeIR* arrayType(TreeNode* t);
    static TypeIR* recordType(TreeNode* t);
    static void TypeDecPart(TreeNode* t);
    static void VarDecPart(TreeNode* t,int paramsize);
    static void ProcDecPart(TreeNode* t);
    static SymbTable* HeadProcess(TreeNode* t);
    static ParamTable* ParaDecList(TreeNode* t);
    static void Body(TreeNode* t);
    static TypeIR* Expr(TreeNode* t/*,AccessKind* Ekind*/);
    static TypeIR* arrayVar(TreeNode* t);
    static TypeIR* recordVar(TreeNode* t);
    static void assignstatement(TreeNode* t);
    static void callstatement(TreeNode* t);
    static void readstatement(TreeNode* t);
    static void writestatement(TreeNode* t);
    static void whilestatement(TreeNode* t);
    static void ifstatement(TreeNode* t);
    static void Analyze(TreeNode* t);
    static void returnstatement(TreeNode* t);
};

#endif
