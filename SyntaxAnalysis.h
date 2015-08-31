#ifndef SYNTAXANALYSISH
#define SYNTAXANALYSISH

#pragma once

//利用了一些词法分析的结构

#include <cstdio>
#include <string>
using namespace std;

#define CNUM 50
#define NODEKINDNUM 10
#define LEXTYPENUM 42
#define KINDNUM 14
#define PARAMTNUM 2
#define MEMBNUM 3

enum NodeKind {ProK,PheadK,TypeK,VarK,ProcK,StmLK,ProcDecK,DecK,StmtK,ExpK}; //节点类型
enum Kind {ArrayK,CharK,IntegerK,RecordK,IdK,IfK,WhileK,
           AssignK,ReadK,WriteK,CallK,ReturnK,OpK,ConstK
          };//具体类型(声明类型,语句类型,表达式类型)
enum Paramt {ValParamType,VarParamType,nulltype}; //参数类型
enum MembK {IdV,ArrayMembV,FieldMembV}; //成员类别
enum ExpType {Void,Integer,Boolean}; //类型检查。。。。。。。。。

//输出语法树使用
string sznodekind[NODEKINDNUM] = {"ProK","PheadK","TypeK","VarK","ProcK","StmLK","ProcDecK","DecK","StmtK","ExpK"};
string szlextype[LEXTYPENUM] = {"program","procedure","type","var","if","then","else","fi","while","do","endwh","begin"
                                ,"end","read","write","array","of","record","return","integer","char","标识符","整型常量","字符常量"
                                ,":=","=","<","+","-","*","/","(",")",".",";",",","[","]","..","文件结束符","error","comment"
                               };
string szkind[KINDNUM] = {"ArrayK","CharK","IntegerK","RecordK","IdK","IfK","WhileK",
                          "AssignK","ReadK","WriteK","CallK","ReturnK","OpK","ConstK"
                         };
string szparamt[PARAMTNUM] = {"value param:","varible param:"};
string szmemb[MEMBNUM] = {"IdV","ArrayMembV","FieldMembV"};

struct TreeNode
{
    TreeNode(NodeKind nodekind)
    {
        for(int i=0; i<3; i++)
            child[i] = NULL;
        sibling = NULL;
        lineno = -1;//初值
        this->nodekind = nodekind;
        //kind
        idnum = 0;//初值
        //name
        //table
        //type_name
        /*以下为attr*/
        this->attr.procAttr.paramt = nulltype;
        this->attr.ExpAttr.val = -1;
    }

    void addTokenSemInfo2name(const string& seminfo)
    {
        if(idnum >= CNUM)
            return;//err
        name[idnum++] = seminfo;
    }

    TreeNode* child[3];//子语法树节点
    TreeNode* sibling;//兄弟语法树节点
    int lineno;//行号
    NodeKind nodekind;//节点类型

    struct
    {
        Kind dec;
        Kind stmt;
        Kind exp;
    } kind;//节点具体类型

    int idnum;//标识符个数
    string name[CNUM];//节点中标识符名字
    //指针数组 table
    string type_name;//记录类型名

    struct
    {

        struct
        {
            int low;//下界
            int up;//上界
            Kind childType;//成员类型
        } ArrayAttr;//数组类型属性

        struct
        {
            Paramt paramt;//参数类型
        } procAttr;//过程属性

        struct
        {
            lexType op;//运算符单词
            int val;//数值
            MembK varkind;//变量类别
            ExpType type;//检查类型 ？
        } ExpAttr;//表达式属性

    } attr;//其他属性
};

class SyntaxAnalysis
{
public:
    SyntaxAnalysis(void);
    ~SyntaxAnalysis(void);
    // 创建语法分析树.成功-返回节点 否则-返回null
    static TreeNode* parse(void);
    // 总程序处理分析函数 成功-返回根节点 否则-返回null
    static TreeNode* Program(void);
    // 单词匹配 一致-取下一单词 否则-出错处理
    static void match(lexType expected);
    // 成功返回程序头结点类型节点 否则返回null
    static TreeNode* ProgramHead(void);
    static TreeNode* ProgramBody(void);
    static TreeNode* DeclarePart(void);
public:
    static token currentToken;
    static string temp_name;//临时值
    static bufferCustom* buf;
public:
    static void readToken(void);
    static void GetNextToken(void);
    static TreeNode* TypeDec(void);
    static TreeNode* TypeDeclaration(void);
    static TreeNode* TypeDecList(void);
    static TreeNode* TypeDecMore(void);
    static void TypeId(TreeNode* t);
    static void TypeDef(TreeNode* t);
    static void BaseType(TreeNode* t);
    static void StructureType(TreeNode* t);
    static void ArrayType(TreeNode* t);
    static void RecType(TreeNode* t);
    static TreeNode* FieldDecList(void);
    static TreeNode* FieldDecMore(void);
    static bool IsCurrWord(lexType lexinfo);
    static void IdList(TreeNode* t);
    static void IdMore(TreeNode* t);
    static TreeNode* VarDec(void);
    static TreeNode* VarDeclaration(void);
    static TreeNode* VarDecList(void);
    static TreeNode* VarDecMore(void);
    static void VarIdList(TreeNode* t);
    static void VarIdMore(TreeNode* t);
    static TreeNode* ProcDec(void);
    static TreeNode* ProcDeclaration(void);
    static void ParamList(TreeNode* t);
    static TreeNode* ParamDecList(void);
    static TreeNode* ParamMore(void);
    static TreeNode* Param(void);
    static void FormList(TreeNode* t);
    static void FidMore(TreeNode* t);
    static TreeNode* ProcDecPart(void);
    static TreeNode* ProcBody(void);
    static TreeNode* StmList(void);
    static TreeNode* StmMore(void);
    static TreeNode* Stm(void);
    static TreeNode* AssCall(void);
    static TreeNode* AssignmentRest(void);
    static TreeNode* ConditionalStm(void);
    static TreeNode* LoopStm(void);
    static TreeNode* InputStm(void);
    static TreeNode* OutputStm(void);
    static TreeNode* ReturnStm(void);
    static TreeNode* CallStmRest(void);
    static TreeNode* ActParamList(void);
    static TreeNode* ActParamMore(void);
    static TreeNode* Exp(void);
    static TreeNode* Simple_exp(void);
    static TreeNode* factor(void);
    static TreeNode* variable(void);
    static void variMore(TreeNode* t);
    static TreeNode* fieldvar(void);
    static void fieldvarMore(TreeNode* t);
    static TreeNode* term(void);
    static int String2Number(const string& s);
    static void DisplayTree(TreeNode* root,int depth,bool flag);
    static void TreeOut();
    static bool DefLin(int lineno)
    {
        return lineno >= 0;
    }
    static void dispErr(string s = "");
};

#endif
