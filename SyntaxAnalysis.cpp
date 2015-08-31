#include "lexAPI.h"
#include "SyntaxAnalysis.h"
#include <iostream>

//初始化
token SyntaxAnalysis::currentToken = *(new token());
string SyntaxAnalysis::temp_name = "";//临时值
bufferCustom* SyntaxAnalysis::buf = NULL;

SyntaxAnalysis::SyntaxAnalysis(void)
{
}

SyntaxAnalysis::~SyntaxAnalysis(void)
{
}

// 创建语法分析树.成功-返回节点 否则-返回null
TreeNode* SyntaxAnalysis::parse(void)
{
    //读入一个单词
    readToken();

    TreeNode* t = Program();

    if(currentToken.lexInfo!=ENDFILE)
    {
        //error
        dispErr("err 文件未结束");
    }
    DisplayTree(t,0,false);
    return t;
}

// 总程序处理分析函数 成功-返回根节点 否则-返回null
TreeNode* SyntaxAnalysis::Program(void)
{
    TreeNode* t = ProgramHead();
    TreeNode* q = DeclarePart();
    TreeNode* s = ProgramBody();

    TreeNode* root;
    if((root = new TreeNode(ProK)) == NULL)
    {
        //error
        dispErr("err 根节点生成失败");
    }
    if(t == NULL)
    {
        //error
        dispErr("err 程序头出错");
    }
    root->child[0] = t;
    if(q != NULL)
    {
        root->child[1] = q;
    }
    if(s == NULL)
    {
        //error
        dispErr("err 缺少程序体");
    }
    root->child[2] = s;
    match(DOT);
    return root;
}

// 单词匹配 一致-取下一单词 否则-出错处理
void SyntaxAnalysis::match(lexType expected)
{
    if(currentToken.lexInfo != expected)
    {
        //error
        cout<<currentToken.linePosition<<"行不匹配:";
        cout<<"缺少";
        dispErr(szlextype[expected]);
    }
    GetNextToken();
}

// 成功返回程序头结点类型节点 否则返回null
TreeNode* SyntaxAnalysis::ProgramHead(void)
{
    match(PROGRAM);
    TreeNode* t = NULL;
    if((t = new TreeNode(PheadK)) == NULL)
    {
        /*error*/
        dispErr("PheadK节点生成失败");
    }
    t->addTokenSemInfo2name(currentToken.semInfo);
    t->lineno = currentToken.linePosition;
    match(ID);
    return t;
}

TreeNode* SyntaxAnalysis::ProgramBody(void)
{
    TreeNode* t = new TreeNode(StmLK);
    match(BEGIN);
    t->child[0] = StmList();
    match(END);
    return t;
}
TreeNode* SyntaxAnalysis::DeclarePart(void)
{
    TreeNode* pp = NULL;
    TreeNode* typeP = new TreeNode(TypeK);
    typeP->child[0] = TypeDec();
    TreeNode* varP = new TreeNode(VarK);
    varP->child[0] = VarDec();
    TreeNode* procP = new TreeNode(ProcK);
    procP->child[0] = ProcDec();
    if(procP->child[0] == NULL)
    {
        delete procP;
        procP = NULL;
    }
    if(varP->child[0] == NULL)
    {
        delete varP;
        pp = procP;
    }
    else
    {
        varP->sibling = procP;
        pp = varP;
    }
    if(typeP->child[0] == NULL)
    {
        delete typeP;
    }
    else
    {
        typeP->sibling = pp;
        pp = typeP;
    }
    return pp;
}

//读文件
void SyntaxAnalysis::readToken(void)
{
    confInit();
    buf = new bufferCustom("1.txt");
    currentToken = getToken(*buf);
}

void SyntaxAnalysis::GetNextToken(void)
{
    currentToken = getToken(*buf);
}

TreeNode* SyntaxAnalysis::TypeDec(void)
{
    TreeNode* t = NULL;
    if(currentToken.lexInfo == TYPE)
    {
        t = TypeDeclaration();
        return t;
    }
    if(currentToken.lexInfo != VAR)
        if(currentToken.lexInfo != PROCEDURE)
            if(currentToken.lexInfo != BEGIN)
                GetNextToken();
    return t;
}

TreeNode* SyntaxAnalysis::TypeDeclaration(void)
{
    match(TYPE);
    TreeNode* t = TypeDecList();
    if(t == NULL)
    {}//error
    return t;
}

TreeNode* SyntaxAnalysis::TypeDecList(void)//..................
{
    TreeNode* t = new TreeNode(DecK);
    TypeId(t);
    match(EQ);
    TypeDef(t);
    match(SEMI);
    TreeNode* p = TypeDecMore();
    if(p != NULL)
        t->sibling = p;
    return t;
}

TreeNode* SyntaxAnalysis::TypeDecMore(void)
{
    TreeNode* t = NULL;
    if(currentToken.lexInfo != VAR)
        if(currentToken.lexInfo != PROCEDURE)
            if(currentToken.lexInfo != BEGIN)
                if(currentToken.lexInfo == ID)
                    t = TypeDecList();
                else
                    GetNextToken();
    return t;
}

void SyntaxAnalysis::TypeId(TreeNode* t)
{
    if(t == NULL)
        return;//err
    if(currentToken.lexInfo == ID)
    {
        t->addTokenSemInfo2name(currentToken.semInfo);
        t->lineno = currentToken.linePosition;
    }
    match(ID);
}

void SyntaxAnalysis::TypeDef(TreeNode* t)//...............
{
    if(t == NULL)
        return;//err
    if(currentToken.lexInfo == INTEGER)
    {
        BaseType(t);
        return;
    }
    if(currentToken.lexInfo == CHAR)
    {
        BaseType(t);
        return;
    }
    if(currentToken.lexInfo == ARRAY)
    {
        StructureType(t);
        return;
    }
    if(currentToken.lexInfo == RECORD)
    {
        StructureType(t);
        return;
    }
    if(currentToken.lexInfo == ID)
    {
        t->kind.dec = IdK;
        t->type_name = currentToken.semInfo;
        //t->attr.procAttr.paramt = ValParamType;
        match(ID);
    }
    else
        GetNextToken();
}

void SyntaxAnalysis::BaseType(TreeNode* t)
{
    if(currentToken.lexInfo == INTEGER)
    {
        match(INTEGER);
        t->kind.dec = IntegerK;
        //t->attr.procAttr.paramt = ValParamType;
        return;
    }
    if(currentToken.lexInfo == CHAR)
    {
        match(CHAR);
        t->kind.dec = CharK;
        //t->attr.procAttr.paramt = ValParamType;
        return;
    }
    GetNextToken();
}

void SyntaxAnalysis::StructureType(TreeNode* t)
{
    if(currentToken.lexInfo == ARRAY)
    {
        ArrayType(t);
        return;
    }
    if(currentToken.lexInfo == RECORD)
    {
        t->kind.dec = RecordK;
        RecType(t);
        return;
    }
    GetNextToken();
}

void SyntaxAnalysis::ArrayType(TreeNode* t)
{
    match(ARRAY);
    match(LMIDPAREN);
    if(currentToken.lexInfo == INTC)
    {
        t->attr.ArrayAttr.low = String2Number(currentToken.semInfo);
    }
    match(INTC);
    match(UNDERANGE);
    if(currentToken.lexInfo == INTC)
        t->attr.ArrayAttr.up = String2Number(currentToken.semInfo);
    match(INTC);
    match(RMIDPAREN);
    match(OF);
    BaseType(t);
    t->attr.ArrayAttr.childType = t->kind.dec;
    t->kind.dec = ArrayK;
}

void SyntaxAnalysis::RecType(TreeNode* t)
{
    match(RECORD);
    TreeNode* p = FieldDecList();
    if(p == NULL)
    {
        //err
    }
    t->child[0] = p;
    match(END);
}

TreeNode* SyntaxAnalysis::FieldDecList(void)
{
    TreeNode* t = new TreeNode(DecK);
    TreeNode* p = NULL;
    if(t == NULL)
        return t;//err
    if(currentToken.lexInfo != INTEGER)
        if(currentToken.lexInfo != CHAR)
            if(currentToken.lexInfo != ARRAY)
                GetNextToken();
            else
            {
                ArrayType(t);
                IdList(t);
                match(SEMI);
                p = FieldDecMore();
            }
        else
        {
            BaseType(t);
            IdList(t);
            match(SEMI);
            p = FieldDecMore();
        }
    else
    {
        BaseType(t);
        IdList(t);
        match(SEMI);
        p = FieldDecMore();
    }
    t->sibling = p;
    return t;
}

TreeNode* SyntaxAnalysis::FieldDecMore(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(END))
        return t;
    if(IsCurrWord(INTEGER)||IsCurrWord(CHAR)||IsCurrWord(ARRAY))
    {
        t = FieldDecList();
        return t;
    }
    GetNextToken();
    return t;
}

bool SyntaxAnalysis::IsCurrWord(lexType lexinfo)
{
    return currentToken.lexInfo == lexinfo;
}

void SyntaxAnalysis::IdList(TreeNode* t)
{
    if(IsCurrWord(ID))
    {
        t->addTokenSemInfo2name(currentToken.semInfo);
        t->lineno = currentToken.linePosition;
        match(ID);
    }
    IdMore(t);
}

void SyntaxAnalysis::IdMore(TreeNode* t)
{
    if(IsCurrWord(SEMI))
        return;
    if(IsCurrWord(COMMA))
    {
        match(COMMA);
        IdList(t);
    }
    else
        GetNextToken();
}

TreeNode* SyntaxAnalysis::VarDec(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(PROCEDURE)||IsCurrWord(BEGIN))
        return t;
    if(IsCurrWord(VAR))
        t = VarDeclaration();
    else
        GetNextToken();
    return t;
}

TreeNode* SyntaxAnalysis::VarDeclaration(void)
{
    match(VAR);
    TreeNode* t = VarDecList();
    if(t == NULL)
    {
        //err
    }
    return t;
}

TreeNode* SyntaxAnalysis::VarDecList(void)
{
    TreeNode* t = new TreeNode(DecK);
    TreeNode* p = NULL;
    TypeDef(t);
    VarIdList(t);
    match(SEMI);
    p = VarDecMore();
    t->sibling = p;
    return t;
}

TreeNode* SyntaxAnalysis::VarDecMore(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(PROCEDURE)||IsCurrWord(BEGIN))
        return t;
    if(IsCurrWord(INTEGER)||IsCurrWord(CHAR)||IsCurrWord(ARRAY)||
            IsCurrWord(RECORD)||IsCurrWord(ID))
        t = VarDecList();
    else
        GetNextToken();
    return t;
}

void SyntaxAnalysis::VarIdList(TreeNode* t)
{
    if(IsCurrWord(ID))
    {
        t->addTokenSemInfo2name(currentToken.semInfo);
        t->lineno = currentToken.linePosition;
        match(ID);
    }
    else
    {
        //err
        GetNextToken();
    }
    VarIdMore(t);
}

void SyntaxAnalysis::VarIdMore(TreeNode* t)
{
    if(IsCurrWord(SEMI))
        return;
    if(IsCurrWord(COMMA))
    {
        match(COMMA);
        VarIdList(t);
    }
    else
        GetNextToken();
}

TreeNode* SyntaxAnalysis::ProcDec(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(BEGIN))
        return t;
    if(IsCurrWord(PROCEDURE))
        t = ProcDeclaration();
    else
        GetNextToken();
    return t;
}


TreeNode* SyntaxAnalysis::ProcDeclaration(void)
{
    TreeNode* t = new TreeNode(ProcDecK);
    match(PROCEDURE);
    if(t == NULL)
        return t;//err
    if(IsCurrWord(ID))
    {
        t->addTokenSemInfo2name(currentToken.semInfo);
        match(ID);
    }
    match(LPAREN);
    ParamList(t);
    match(RPAREN);
    match(SEMI);//COLON?
    t->child[1] = ProcDecPart();
    t->child[2] = ProcBody();
    t->sibling = ProcDec();//下一个过程,即ProcDecMore
    return t;
}


void SyntaxAnalysis::ParamList(TreeNode* t)
{
    if(IsCurrWord(RPAREN))
    {
        return;
    }
    if(IsCurrWord(INTEGER)||IsCurrWord(CHAR)||IsCurrWord(RECORD)||IsCurrWord(ARRAY)||IsCurrWord(ID)||IsCurrWord(VAR))
        t->child[0] = ParamDecList();
    else
        GetNextToken();
}


TreeNode* SyntaxAnalysis::ParamDecList(void)
{
    TreeNode* t = Param();
    TreeNode* p = ParamMore();
    if(p != NULL)
        t->sibling = p;
    return t;
}


TreeNode* SyntaxAnalysis::ParamMore(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(RPAREN))
        return t;
    if(IsCurrWord(SEMI))
    {
        match(SEMI);
        t = ParamDecList();
        if(t == NULL)
        {
            //err
        }
    }
    else
        GetNextToken();
    return t;
}


TreeNode* SyntaxAnalysis::Param(void)
{
    TreeNode* t = new TreeNode(DecK);
    if(IsCurrWord(INTEGER)||IsCurrWord(CHAR)||IsCurrWord(RECORD)||IsCurrWord(ARRAY)||IsCurrWord(ID))
    {
        t->attr.procAttr.paramt = ValParamType;
        t->lineno = currentToken.linePosition;
        TypeDef(t);
        FormList(t);
    }
    else if(IsCurrWord(VAR))
    {
        match(VAR);
        t->attr.procAttr.paramt = VarParamType;
        t->lineno = currentToken.linePosition;
        TypeDef(t);
        FormList(t);
    }
    else
        GetNextToken();
    return t;
}


void SyntaxAnalysis::FormList(TreeNode* t)
{
    if(IsCurrWord(ID))
    {
        t->addTokenSemInfo2name(currentToken.semInfo);
        match(ID);
    }
    FidMore(t);
    return;
}


void SyntaxAnalysis::FidMore(TreeNode* t)
{
    if(IsCurrWord(SEMI)||IsCurrWord(RPAREN))
        return;
    if(IsCurrWord(COMMA))
    {
        match(COMMA);
        FormList(t);
    }
    else
        GetNextToken();
}


TreeNode* SyntaxAnalysis::ProcDecPart(void)
{
    return DeclarePart();
}


TreeNode* SyntaxAnalysis::ProcBody(void)
{
    TreeNode* t = ProgramBody();
    if(t == NULL)
    {
        //err
    }
    return t;
}


TreeNode* SyntaxAnalysis::StmList(void)
{
    TreeNode* t = Stm();
    TreeNode* p = StmMore();
    t->sibling = p;
    return t;
}


TreeNode* SyntaxAnalysis::StmMore(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(END)||IsCurrWord(ENDWH)||IsCurrWord(ELSE)||IsCurrWord(FI))
        return t;
    if(IsCurrWord(SEMI))
    {
        match(SEMI);
        t = StmList();
    }
    else
        //err
        GetNextToken();
    return t;
}


TreeNode* SyntaxAnalysis::Stm(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(IF))
    {
        t = ConditionalStm();
        return t;
    }
    if(IsCurrWord(WHILE))
    {
        t = LoopStm();
        return t;
    }
    if(IsCurrWord(RETURN))
    {
        t = ReturnStm();
        return t;
    }
    if(IsCurrWord(READ))
    {
        t = InputStm();
        return t;
    }
    if(IsCurrWord(WRITE))
    {
        t = OutputStm();
        return t;
    }
    if(IsCurrWord(ID))
    {
        temp_name = currentToken.semInfo;
        match(ID);
        t = AssCall();
    }
    else
        //err
        GetNextToken();
    return t;
}


TreeNode* SyntaxAnalysis::AssCall(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(ASSIGN)||IsCurrWord(LMIDPAREN)||IsCurrWord(DOT))
    {
        t = AssignmentRest();
        return t;
    }
    if(IsCurrWord(LPAREN))
    {
        t = CallStmRest();
        return t;
    }
    GetNextToken();
    return t;
}


TreeNode* SyntaxAnalysis::AssignmentRest(void)
{
    TreeNode* t = new TreeNode(StmtK);
    t->kind.stmt = AssignK;
    //match(EQ);
    t->child[0] = new TreeNode(ExpK);
    t->child[0]->kind.exp = IdK;
    t->child[0]->addTokenSemInfo2name(temp_name);
    //t->child[0]->attr.ExpAttr.varkind = IdV;
    t->child[0]->lineno = currentToken.linePosition;
    //addition
    variMore(t->child[0]);
    match(ASSIGN);
    //t->child[0] = Exp();
    t->child[1] = Exp();
    //t->addTokenSemInfo2name(temp_name);
    return t;
}


TreeNode* SyntaxAnalysis::ConditionalStm(void)
{
    TreeNode* t = new TreeNode(StmtK);
    t->kind.stmt = IfK;
    match(IF);
    t->child[0] = Exp();
    match(THEN);
    t->child[1] = StmList();
    if(IsCurrWord(ELSE))
    {
        match(ELSE);
        t->child[2] = StmList();
    }
    match(FI);
    return t;
}


TreeNode* SyntaxAnalysis::LoopStm(void)
{
    TreeNode* t = new TreeNode(StmtK);
    t->kind.stmt = WhileK;
    match(WHILE);
    t->child[0] = Exp();
    match(DO);
    t->child[1] = StmList();
    match(ENDWH);
    return t;
}


TreeNode* SyntaxAnalysis::InputStm(void)
{
    TreeNode* t = new TreeNode(StmtK);
    t->kind.stmt = ReadK;
    match(READ);
    match(LPAREN);
    if(IsCurrWord(ID))
        t->addTokenSemInfo2name(currentToken.semInfo);
    match(ID);
    match(RPAREN);
    return t;
}


TreeNode* SyntaxAnalysis::OutputStm(void)
{
    TreeNode* t = new TreeNode(StmtK);
    t->kind.stmt = WriteK;
    match(WRITE);
    match(LPAREN);
    t->child[0] = Exp();
    match(RPAREN);
    return t;
}


TreeNode* SyntaxAnalysis::ReturnStm(void)
{
    TreeNode* t = new TreeNode(StmtK);
    t->kind.stmt = ReturnK;
    match(RETURN);
    return t;
}


TreeNode* SyntaxAnalysis::CallStmRest(void)
{
    TreeNode* t = new TreeNode(StmtK);
    t->kind.stmt = CallK;
    match(LPAREN);
    t->child[0] = new TreeNode(ExpK);
    t->child[0]->addTokenSemInfo2name(temp_name);
    t->child[0]->kind.exp = IdK;
    t->child[0]->attr.ExpAttr.varkind = IdV;
    t->child[0]->lineno = currentToken.linePosition;
    t->child[1] = ActParamList();
    //t->child[0] = ActParamList();
    //t->addTokenSemInfo2name(temp_name);
    match(RPAREN);
    return t;
}


TreeNode* SyntaxAnalysis::ActParamList(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(RPAREN))
        return t;
    if(IsCurrWord(ID)||IsCurrWord(INTC))
    {
        t = Exp();
        if(t != NULL)
            t->sibling = ActParamMore();
    }
    else
        GetNextToken();
    return t;
}


TreeNode* SyntaxAnalysis::ActParamMore(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(RPAREN))
        return t;
    if(IsCurrWord(COMMA))
    {
        match(COMMA);
        t = ActParamList();
    }
    else
        GetNextToken();
    return t;
}


TreeNode* SyntaxAnalysis::Exp(void)
{
    TreeNode* t = Simple_exp();
    if(!IsCurrWord(LT)&&!IsCurrWord(EQ))
        return t;
    TreeNode* p = new TreeNode(ExpK);
    p->child[0] = t;
    p->attr.ExpAttr.op = currentToken.lexInfo;
    p->lineno = currentToken.linePosition;
    p->kind.exp = OpK;
    p->attr.ExpAttr.op = currentToken.lexInfo;
    t = p;
    if(IsCurrWord(LT))
        match(LT);
    else
        match(EQ);
    if(t != NULL)
    {
        t->child[1] = Simple_exp();
    }
    return t;
}

TreeNode* SyntaxAnalysis::Simple_exp(void)//循环嵌套
{
    TreeNode* t = term();
    while(IsCurrWord(PLUS)||IsCurrWord(MINUS))
    {
        TreeNode* p = new TreeNode(ExpK);
        p->child[0] = t;//左运算项
        p->attr.ExpAttr.op = currentToken.lexInfo;
        p->lineno = currentToken.linePosition;
        p->kind.exp = OpK;
        p->attr.ExpAttr.op = currentToken.lexInfo;
        t = p;
        if(IsCurrWord(PLUS))
            match(PLUS);
        else
            match(MINUS);
        t->child[1] = term();//右运算项
    }
    return t;
}

TreeNode* SyntaxAnalysis::factor(void)
{
    TreeNode* t = NULL;
    if(IsCurrWord(INTC))
    {
        t = new TreeNode(ExpK);
        t->kind.exp = ConstK;
        //常量无法转数字(词法分析)
        //t->attr.ExpAttr.val = String2Number(currentToken.semInfo);
        //当串处理
        t->addTokenSemInfo2name(currentToken.semInfo);
        t->lineno = currentToken.linePosition;
        match(INTC);
        return t;
    }
    if(IsCurrWord(ID))
    {
        t = variable();
        return t;
    }
    if(IsCurrWord(LPAREN))
    {
        match(LPAREN);
        t = Exp();
        match(RPAREN);
        return t;
    }
    //err
    GetNextToken();
    return t;
}

TreeNode* SyntaxAnalysis::variable(void)
{
    TreeNode* t = new TreeNode(ExpK);
    if(IsCurrWord(ID))
    {
        t->lineno = currentToken.linePosition;
        t->addTokenSemInfo2name(currentToken.semInfo);
        //t->attr.ExpAttr.varkind
        t->kind.exp = IdK;
        match(ID);
        variMore(t);
        return t;
    }
    //err
    return t;
}

void SyntaxAnalysis::variMore(TreeNode* t)
{
    if(IsCurrWord(ASSIGN)||IsCurrWord(TIMES)||
            IsCurrWord(EQ)||IsCurrWord(LT)||
            IsCurrWord(PLUS)||IsCurrWord(MINUS)||
            IsCurrWord(OVER)||IsCurrWord(RPAREN)||
            IsCurrWord(RMIDPAREN)||IsCurrWord(SEMI)||
            IsCurrWord(COMMA)||IsCurrWord(THEN)||
            IsCurrWord(ELSE)||IsCurrWord(FI)||
            IsCurrWord(DO)||IsCurrWord(ENDWH)||
            IsCurrWord(END))
    {
        t->attr.ExpAttr.varkind = IdV;
        return;
    }
    //return;
    if(IsCurrWord(LMIDPAREN))
    {
        match(LMIDPAREN);
        t->child[0] = Exp();
        t->attr.ExpAttr.varkind = ArrayMembV;
        //t->child[0]->attr.ExpAttr.varkind = IdV;
        //addition
        match(RMIDPAREN);
        return;
    }
    if(IsCurrWord(DOT))
    {
        match(DOT);
        t->child[0] = fieldvar();
        t->attr.ExpAttr.varkind = FieldMembV;
        //t->child[0]->attr.ExpAttr.varkind = IdV;
        return;
    }
    //err
    GetNextToken();
}

TreeNode* SyntaxAnalysis::fieldvar(void)
{
    TreeNode* t = new TreeNode(ExpK);
    if(IsCurrWord(ID))
    {
        t->lineno = currentToken.linePosition;
        t->kind.exp = IdK;
        t->addTokenSemInfo2name(currentToken.semInfo);
        match(ID);
        fieldvarMore(t);
    }
    //err
    return t;
}

void SyntaxAnalysis::fieldvarMore(TreeNode* t)//域变量里面结构类型只有数组，没有记录
{
    if(IsCurrWord(ASSIGN)||IsCurrWord(TIMES)||
            IsCurrWord(EQ)||IsCurrWord(LT)||
            IsCurrWord(PLUS)||IsCurrWord(MINUS)||
            IsCurrWord(OVER)||IsCurrWord(RPAREN)||
            IsCurrWord(END)||IsCurrWord(SEMI)||
            IsCurrWord(COMMA)||IsCurrWord(THEN)||
            IsCurrWord(ELSE)||IsCurrWord(FI)||
            IsCurrWord(DO)||IsCurrWord(ENDWH)||IsCurrWord(RMIDPAREN))
    {
        t->attr.ExpAttr.varkind = IdV;
        return;
    }
    if(IsCurrWord(LMIDPAREN))
    {
        match(LMIDPAREN);
        t->child[0] = Exp();
        t->attr.ExpAttr.varkind = ArrayMembV;
        match(RMIDPAREN);
        return;
    }
    //err
    GetNextToken();
}

TreeNode* SyntaxAnalysis::term(void)
{
    TreeNode* t = factor();
    while(IsCurrWord(TIMES)||IsCurrWord(OVER))
    {
        TreeNode* p = new TreeNode(ExpK);
        p->child[0] = t;//左运算因子
        p->attr.ExpAttr.op = currentToken.lexInfo;
        p->lineno = currentToken.linePosition;
        p->kind.exp = OpK;
        p->attr.ExpAttr.op = currentToken.lexInfo;
        t = p;
        if(IsCurrWord(TIMES))
            match(TIMES);
        else
            match(OVER);
        t->child[1] = factor();//右运算因子
    }
    return t;
}
#include <vector>
int SyntaxAnalysis::String2Number(const string& s)
{
    //char t[1000];
    //std::vector<char> t;
    std::string te;
    for(int i=0; i<s.size()-1; i++)
        te+=s[i];//t[i] = s[i];
    //for(int i=0;i<t.size();i++)

    return atoi(te.c_str());
}

//遍历输出语法树
void SyntaxAnalysis::DisplayTree(TreeNode* root,int depth,bool flag)
{
    if(root == NULL)
        return;//err

    //输出root
    for(int i=0; i<4*depth; i++)
        cout<<' ';//缩进
    if(flag)
        cout<<'*';//表示兄弟节点
    cout<<sznodekind[root->nodekind]<<"  ";//暂时这么输出
    //输出行
    if(DefLin(root->lineno))
        cout<<"line:"<<root->lineno<<" ";
    //输出相关信息
    switch(root->nodekind)
    {
    case PheadK:
    {
        cout<<root->name[0];
        break;
    }
    case ProcDecK:
    {
        cout<<root->name[0];
        break;
    }
    case StmtK:
    {
        cout<<szkind[root->kind.stmt]<<" ";
        if(root->kind.stmt == ReadK)
            cout<<root->name[0];
        break;
    }
    case DecK:
    {
        if(root->attr.procAttr.paramt != nulltype)
        {
            cout<<szparamt[root->attr.procAttr.paramt]<<" ";
        }
        cout<<szkind[root->kind.dec]<<" ";
        for(int index=0; index<root->idnum; index++)
            cout<<root->name[index]<<" ";
        break;
    }

    case ExpK:
    {
        cout<<szkind[root->kind.exp]<<" ";
        if(root->kind.exp == OpK)
            cout<<szlextype[root->attr.ExpAttr.op];
        if(root->kind.exp == ConstK)
            cout<<root->name[0];
        if(root->kind.exp == IdK)
        {
            cout<<root->name[0]<<" ";
            cout<<szmemb[root->attr.ExpAttr.varkind];
        }
    }
    }
    cout<<endl;
    //----------------
    //|other info
    //----------------

    int tmp = depth + 1;//层数，用于缩进
    if(root->child[0] != NULL)
    {
        DisplayTree(root->child[0],tmp,false);
    }
    if(root->child[1] != NULL)
    {
        DisplayTree(root->child[1],tmp,false);
    }
    if(root->child[2] != NULL)
    {
        DisplayTree(root->child[2],tmp,false);
    }
    if(root->sibling != NULL)
    {
        DisplayTree(root->sibling,depth,true);
    }
}

void SyntaxAnalysis::TreeOut()
{
    //DisplayTree
}

void SyntaxAnalysis::dispErr(string s)
{
    cout<<s<<endl;
    system("pause");
    exit(0);
}

////////////////////////////////////////////////////
