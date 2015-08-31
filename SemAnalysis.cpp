#include "lexAPI.h"
#include "SemAnalysis.h"
#include<iostream>
using namespace std;
#include "SyntaxAnalysis.cpp"

//初始化
int STable::Level = 0;//从1开始，而0没有使用
SymbTable** STable::Scope = new SymbTable*[SCOPENUM];

FILE * result;

void STable::CreatTable(void)
{
    //并非真正建表
    Level++;
    Scope[Level] = NULL;
}

void STable::DestroyTable(void)
{
    if(Level > 0)
        Level--;
    else
    {
        //err
    }
}

//登记
bool STable::Enter(string Id,AttrbuteIR* attribP,SymbTable** Entry)
{
    *Entry = NULL;
    SymbTable* q = NULL;
    bool present = FindEntry(Id,true,Entry);
    if(present)
    {
        //err
        //标识符重复声明
        cout<<(*Entry)->idname<<' '<<"与已有标识符重名"<<endl;
        SyntaxAnalysis::dispErr();
        return false;
    }
    SymbTable* p = new SymbTable();
    p->idname = Id;
    p->attrIR = (*attribP);
    if(Scope[Level] == NULL)
    {
        Scope[Level] = p;
    }
    else
    {
        q = Scope[Level];
        while(q->next != NULL)
            q = q->next;
        q->next = p;
    }
    //if(Entry)
    //{
    *Entry = p;
    //}
    return true;
}

bool STable::FindEntry(string id,bool flag,SymbTable** Entry)
{
    *Entry = NULL;
    bool present = SearchoneTable(id,Level,Entry);
    if(flag || present)
    {
        return present;
    }
    int level = Level - 1;
    while(level>0)
    {
        present = SearchoneTable(id,level,Entry);
        if(present)
            return present;
        level--;
    }
    return false;
}

bool STable::SearchoneTable(string id,int currentLevel,SymbTable** Entry)
{
    SymbTable* p = Scope[currentLevel];
    //Scope[currentLevel]->idname
    while(p != NULL)
    {
        if(p->idname == id)
        {
            *Entry = p;
            return true;
        }
        p = p->next;
    }
    return false;
}

bool STable::FindField(string id,FieldChain* head,FieldChain** Entry)
{
    *Entry = NULL;
    FieldChain* p = head;
    while(p != NULL)
    {
        if(p->idname == id)
        {
            *Entry = p;
            return true;
        }
        p = p->next;
    }
    return false;
}

//命令行输出，临时显示
void STable::PrindSymbTable()
{
    //打印符号表
    SymbTable* p = NULL;
    for(int level=1; level<SCOPENUM; level++)
    {
        p = Scope[level];
        if(p == NULL)
            return;
        while(p != NULL)
        {
            cout<<p->idname<<"   ";
            cout<<p->attrIR.kind<<endl;
            p = p->next;
        }
        cout<<endl;
    }
}

/////////////////////////////////////////////

//打印///////////////////////////////////////////////////////////////////////////////

//打印纪录类型的域表
void   STable::PrintFieldChain(FieldChain  *currentP)
{
    fprintf(result,"\n[Field  chain]\n");
    FieldChain  *t=currentP;
    while (t!=NULL)
    {
        /*输出标识符名字*/
        fprintf(result ,"%s:  ",t->idname.c_str());
        /*输出标识符的类型信息*/

        switch(t->unitType->kind)
        {
        case  intTy :
            fprintf(result ,"intTy     ");
            break;
        case  charTy:
            fprintf(result ,"charTy    ");
            break;
        case  arrayTy:
            fprintf(result ,"arrayTy   ");
            break;
        case  recordTy:
            fprintf(result ,"recordTy  ");
            break;
        default :
            fprintf(result ,"error  type!  ");
            break;
        }
        fprintf(result ,"offset = %d\n",t->offset);

        t = t->next;
    }
}



//打印符号表的一层
void  STable::PrintOneLayer(int level)
{
    SymbTable  * t= Scope[level];
    fprintf(result,"\n-------SymbTable  in level %d ---------\n",level);

    while (t!=NULL)
    {
        /*输出标识符名字*/
        fprintf(result ,"%s:   ",t->idname.c_str());
        AttrbuteIR  *Attrib = &(t->attrIR );
        /*输出标识符的类型信息，过程标识符除外*/
        if (Attrib->idtype!=NULL)  /*过程标识符*/
            switch(Attrib->idtype->kind)
            {
            case  intTy :
                fprintf(result ,"intTy  ");
                break;
            case  charTy:
                fprintf(result ,"charTy  ");
                break;
            case  arrayTy:
                fprintf(result ,"arrayTy  ");
                break;
            case  recordTy:
                fprintf(result ,"recordTy  ");
                break;
            default :
                fprintf(result ,"error  type!  ");
                break;
            }
        /*输出标识符的类别，并根据不同类型输出不同其它属性*/
        switch(Attrib->kind)
        {
        case  typeKind :
            fprintf(result, "typekind  ");
            break;
        case  varKind :


            fprintf(result, "varkind  ");
            fprintf(result ,"Level = %d  ", Attrib->more.VarAttr.level);
            if(Attrib->more.VarAttr.access == dir)
                fprintf(result ,"Offset= %d  ", Attrib->more.VarAttr.off - Attrib->idtype->size);
            else
                fprintf(result ,"Offset= %d  ", Attrib->more.VarAttr.off - 1);

            fprintf(result,"size = %d ",Attrib->idtype->size);

            switch(Attrib->more.VarAttr.access)
            {
            case  dir :
                fprintf(result ,"dir  ");
                break;
            case  indir:
                fprintf(result ,"indir  ");
                break;
            default :
                fprintf(result ,"errorkind  ");
                break;
            }

            if(Attrib->idtype->kind == recordTy)
                PrintFieldChain(Attrib->idtype->more.body);

            //if(Attrib->idtype->kind = arrayTy)
            break;
        case  procKind:
            fprintf(result ,"funckind   ");
            fprintf(result ,"Level= %d  ",Attrib->more.ProcAttr.level);
            fprintf(result ,"CODE= %d  ",Attrib->more.ProcAttr.code);
            break;
        default :
            fprintf(result ,"error  ");
        }
        fprintf(result,"\n");
        t = t->next;
    }
}


//打印生成的符号表
void   STable::PrintSymbTable( )
{
    /*层数从0开始*/
    int  level=1;
    while (Scope[level]!=NULL)
    {
        PrintOneLayer(level);
        level++;
    }
}

/********************************************/

SemAnalysis::SemAnalysis(void)
{
}

SemAnalysis::~SemAnalysis(void)
{
}

//初始化
TypeIR* SemAnalysis::intPtr = NULL;
TypeIR* SemAnalysis::charPtr = NULL;
TypeIR* SemAnalysis::boolPtr = NULL;

void SemAnalysis::initialize(void)
{
    intPtr = new TypeIR(intTy);
    intPtr->size = 1;
    charPtr = new TypeIR(charTy);
    charPtr->size = 1;
    boolPtr = new TypeIR(boolTy);
    boolPtr->size = 1;

    for(int index=0; index<SCOPENUM; index++)
        STable::Scope[index] = NULL;
}


TypeIR* SemAnalysis::TypeProcess(TreeNode* t, Kind deckind)
{
    TypeIR* Ptr = NULL;
    if(deckind == IdK)
        Ptr = nameType(t);
    if(deckind == ArrayK)
        Ptr = arrayType(t);
    if(deckind == RecordK)
        Ptr = recordType(t);
    if(deckind == IntegerK)
        Ptr = intPtr;
    if(deckind == CharK)
        Ptr = charPtr;
    return Ptr;
}


TypeIR* SemAnalysis::nameType(TreeNode* t)
{
    TypeIR* Ptr = NULL;
    SymbTable* entry = NULL;
    bool present = STable::FindEntry(t->type_name,false,&entry);
    if(present != 1)
    {
        //err 类型无声明错误
        cout<<t->type_name<<' '<<"未知类型名"<<endl;
        SyntaxAnalysis::dispErr();
        return Ptr;
    }
    if(entry->attrIR.kind == typeKind)
        Ptr = entry->attrIR.idtype;
    else
    {
        //err 非类型标识符错
        cout<<entry->idname<<' '<<"非类型标识符"<<endl;
        SyntaxAnalysis::dispErr();
    }
    return Ptr;
}

void SemAnalysis::statement(TreeNode* t)
{
    if(t->nodekind != StmtK)
    {
        //err
        return;
    }
    if(t->kind.stmt == IfK)
    {
        ifstatement(t);
        return;
    }
    if(t->kind.stmt == WhileK)
    {
        whilestatement(t);
        return;
    }
    if(t->kind.stmt == AssignK)
    {
        assignstatement(t);
        return;
    }
    if(t->kind.stmt == ReadK)
    {
        readstatement(t);
        return;
    }
    if(t->kind.stmt == WriteK)
    {
        writestatement(t);
        return;
    }
    if(t->kind.stmt == CallK)
    {
        callstatement(t);
        return;
    }
    if(t->kind.stmt == ReturnK)
    {
        returnstatement(t);
        return;
    }
}

TypeIR* SemAnalysis::arrayType(TreeNode* t)
{
    TypeIR* ptr = NULL;
    if(t->attr.ArrayAttr.low > t->attr.ArrayAttr.up)
    {
        //err 下界大于上界
        return ptr;
    }

    ptr = new TypeIR(arrayTy);
    ptr->size = t->attr.ArrayAttr.up - t->attr.ArrayAttr.low + 1;
    ptr->more.arrayAttr.indexTy = intPtr;
    ptr->more.arrayAttr.elemTy = TypeProcess(t,t->attr.ArrayAttr.childType);
    return ptr;
}


TypeIR* SemAnalysis::recordType(TreeNode* t)
{
    TypeIR* ptr = new TypeIR(recordTy);
    FieldChain* ptr2 = NULL;
    FieldChain* prePtr = NULL;
    int tmpsize = -1;
    TreeNode* tt = t;
    int off = 0;
    tt = tt->child[0];
    while(tt != NULL)
    {
        int num = 0;
        while(num < (tt->idnum))//针对[类型 i,j,k...]的情形
        {
            ptr2 = new FieldChain();
            ptr2->idname = tt->name[num];
            ptr2->unitType = TypeProcess(tt,tt->kind.dec);
            //prePtr = ptr2;
            if(prePtr == NULL)
            {
                ptr2->offset = 0;
                tmpsize = ptr2->unitType->size;
                //将域链链入记录类型的body部分
                ptr->more.body = ptr2;
            }
            else
            {
                ptr2->offset = off + ptr2->unitType->size;
                off = ptr2->offset;
                prePtr->next = ptr2;
            }
            prePtr = ptr2;
            num++;
        }

        tt = tt->sibling;
    }
    ptr->size = off + tmpsize;
    return ptr;
}

void SemAnalysis::TypeDecPart(TreeNode *t)
{
    bool present = false;
    SymbTable* entry = NULL;
    AttrbuteIR* attribp = NULL;
    TreeNode* tt = t;
    tt = tt->child[0];
    while(tt != NULL)
    {
        attribp = new AttrbuteIR();
        attribp->kind = typeKind;
        attribp->idtype = TypeProcess(tt,tt->kind.dec);
        present = STable::Enter(tt->name[0],attribp,&entry);
        if(!present)
        {
            //err  重复声明报错
            cout<<tt->name[0]<<' '<<"类型重复声明"<<endl;
            SyntaxAnalysis::dispErr();
        }
        tt = tt->sibling;
    }
}

void SemAnalysis::VarDecPart(TreeNode* t,int paramsize)//要考虑到函数参数的偏移
{
    TreeNode* tt = t;
    AttrbuteIR* ptr = NULL;
    SymbTable* entry = NULL;
    int offset = paramsize;
    //int ooffset = 0;
    while(tt != NULL)
    {
        int num = 0;
        while(num < tt->idnum)
        {
            ptr = new AttrbuteIR();
            ptr->kind = varKind;
            ptr->idtype = TypeProcess(tt,tt->kind.dec);
            if(STable::Scope[STable::Level] == NULL)
            {
                if(tt->attr.procAttr.paramt == VarParamType)
                {
                    ptr->more.VarAttr.level = STable::Level;
                    ptr->more.VarAttr.access = indir;
                    ptr->more.VarAttr.off = offset + 1;//间接变量认为是1
                    offset = ptr->more.VarAttr.off;
                }
                else
                {
                    ptr->more.VarAttr.level = STable::Level;
                    ptr->more.VarAttr.access = dir;
                    ptr->more.VarAttr.off = offset + ptr->idtype->size;
                    offset = ptr->more.VarAttr.off;
                }
            }
            else//此处多余了...paramsize
            {
                if(tt->attr.procAttr.paramt == VarParamType)
                {
                    ptr->more.VarAttr.level = STable::Level;
                    ptr->more.VarAttr.access = indir;
                    ptr->more.VarAttr.off = offset + 1;//间接变量认为是1
                    offset = ptr->more.VarAttr.off;
                }
                else
                {
                    ptr->more.VarAttr.level = STable::Level;
                    ptr->more.VarAttr.access = dir;
                    ptr->more.VarAttr.off = offset + ptr->idtype->size;
                    offset = ptr->more.VarAttr.off;
                }
            }
            bool present = STable::Enter(tt->name[num],ptr,&entry);
            if(!present)
            {
                //err 重复定义
                cout<<tt->name[num]<<' '<<"变量重复定义"<<endl;
                SyntaxAnalysis::dispErr();
            }
            num++;
        }
        tt = tt->sibling;
    }
}

void SemAnalysis::ProcDecPart(TreeNode* t)
{
    SymbTable* entry = NULL;
    TreeNode* tt = t->child[0];
    TreeNode* t2 = NULL;

    while(tt != NULL)
    {
        entry = HeadProcess(tt);//
        //处理有参数的情况
        ParamTable* entry1 = entry->attrIR.more.ProcAttr.param;
        int paramsize = 0;
        if(entry1 != NULL)
        {
            while(entry1->next != NULL)
                entry1 = entry1->next;
            //if(entry1->entry->attrIR.more.VarAttr.access ==dir)
            paramsize = entry1->entry->attrIR.more.VarAttr.off;
            //else
            //paramsize = entry1->entry->attrIR.more.VarAttr.off + 1;
        }

        t2 = tt->child[1];
        if(t2 != NULL)
        {
            //处理声明部分
            TreeNode* p = t2;
            TreeNode* q = NULL;
            if(p != NULL)
            {
                if(p->nodekind == TypeK)
                {

                    TypeDecPart(p);
                    if(p->sibling != NULL)
                        p = p->sibling;
                }
                if(p->nodekind == VarK)
                {
                    q = p->child[0];

                    VarDecPart(q,paramsize);
                    if(p->sibling != NULL)
                        p = p->sibling;
                }
                if(p->nodekind == ProcK)
                {

                    ProcDecPart(p);
                    //p = p->sibling;
                }
            }
        }
        t2 = tt->child[2];
        if(t2 != NULL)
            Body(t2);
        STable::DestroyTable();//退到上一层
        tt = tt->sibling;
    }
}

SymbTable* SemAnalysis::HeadProcess(TreeNode* t)
{
    SymbTable* entry = NULL;
    AttrbuteIR* attrbi = new AttrbuteIR();
    attrbi->kind = procKind;
    attrbi->idtype = NULL;
    attrbi->more.ProcAttr.level = STable::Level;
    attrbi->more.ProcAttr.code = -1;//默认
    attrbi->more.ProcAttr.size = -1;//默认
    STable::Enter(t->name[0],attrbi,&entry);
    //attrbi->more.ProcAttr.param = ParaDecList(t);
    entry->attrIR.more.ProcAttr.param = ParaDecList(t);
    return entry;
}

ParamTable* SemAnalysis::ParaDecList(TreeNode* t)
{
    STable::CreatTable();//没有参数也要建新局部化区
    if(t->child[0] == NULL)
        return NULL;
    ParamTable* head = NULL;
    TreeNode* p = t->child[0];
    //STable::CreatTable();
    VarDecPart(p,0);
    SymbTable* pp = NULL;
    pp = STable::Scope[STable::Level];
    head = new ParamTable();
    head->entry = pp;
    pp = pp->next;
    ParamTable* ptr = head;
    ParamTable* ptr2 = NULL;
    //head->next = pp;
    while(pp != NULL)
    {
        ptr2 = new ParamTable();
        ptr2->entry = pp;
        ptr2->next = NULL;
        ptr->next = ptr2;
        ptr = ptr2;
        pp = pp->next;
    }
    return head;
}

void SemAnalysis::Body(TreeNode* t)
{
    if(t->child[0] == NULL)
        return;//err 无语句，是错误吗？
    //循环处理语句序列
    TreeNode* p = t->child[0];
    while(p != NULL)
    {
        statement(p);
        p = p->sibling;
    }
}

//void SemAnalysis::statement(TreeNode* t)
//{
//
//}

//一直不明白书上写的参数AccessKind* Ekind的作用和必要性,选择去除
TypeIR* SemAnalysis::Expr(TreeNode* t/*,AccessKind* Ekind*/)
{
    TypeIR* Eptr = NULL;
    switch(t->kind.exp)
    {
    case ConstK:
    {
        Eptr = intPtr;
        //(*Ekind) = dir;
        break;
    }
    case IdK:
    {
        if(t->child[0] == NULL)
        {
            SymbTable* entry = NULL;
            if(STable::FindEntry(t->name[0],false,&entry))
            {
                if(entry->attrIR.kind == varKind)
                {
                    Eptr = entry->attrIR.idtype;
                    //(*Ekind) = indir;
                }
                else
                {
                    //err
                    cout<<"line:"<<t->lineno<<' ';
                    cout<<t->name[0]<<' ';
                    SyntaxAnalysis::dispErr("非变量标识符");
                }
            }
            else
            {
                //err 未声明
                //string s = "line:" + (t->lineno) + ' ' + "未声明" + (t->name[0]);
                cout<<"line:"<<t->lineno<<' ';
                cout<<t->name[0]<<' ';
                SyntaxAnalysis::dispErr("未声明标识符");
            }
        }
        else
        {
            if(t->attr.ExpAttr.varkind == ArrayMembV)
            {
                Eptr = arrayVar(t);
            }
            else
            {
                Eptr = recordVar(t);
            }
        }
        break;
    }
    case OpK:
    {
        TypeIR* eptr1 = Expr(t->child[0]);
        TypeIR* eptr2 = Expr(t->child[1]);
        //if(eptr1 != intPtr || eptr2 != intPtr)
        if(eptr1 != eptr2)
        {
            //err 分量类型错
            cout<<"line:"<<t->lineno<<' ';
            SyntaxAnalysis::dispErr("运算分量不相容");
        }
        if(t->attr.ExpAttr.op == EQ || t->attr.ExpAttr.op == LT)
            Eptr = boolPtr;//对于逻辑表达式，赋予bool类型
        else
            Eptr = intPtr;
        break;
    }
    }
    return Eptr;
}

TypeIR* SemAnalysis::arrayVar(TreeNode* t)
{
    SymbTable* entry = NULL;
    bool present = STable::FindEntry(t->name[0],false,&entry);
    if(!present)
    {
        //err 无声明
        cout<<"line:"<<t->lineno<<' ';
        cout<<t->name[0]<<' ';
        SyntaxAnalysis::dispErr("未声明");
        return NULL;
    }
    if(entry->attrIR.kind != varKind)
    {
        //err
        cout<<"line:"<<t->lineno<<' ';
        cout<<t->name[0]<<' ';
        SyntaxAnalysis::dispErr("非变量");
        return NULL;
    }
    if(entry->attrIR.idtype->kind != arrayTy)
    {
        //err
        cout<<"line:"<<t->lineno<<' ';
        cout<<t->name[0]<<' ';
        SyntaxAnalysis::dispErr("不是数组类型变量");
        return NULL;
    }
    //if(t->child[0]->kind.dec != entry->attrIR.idtype->more.arrayAttr.indexTy->)
    if(Expr(t->child[0]/*,entry->attrIR.more.VarAttr.access*/) != intPtr)
    {
        //err 类型不符
        cout<<"line:"<<t->lineno<<' ';
        cout<<t->child[0]->name[0]<<' ';
        SyntaxAnalysis::dispErr("不合法下标类型");
        return NULL;
    }
    return entry->attrIR.idtype->more.arrayAttr.elemTy;
}


TypeIR* SemAnalysis::recordVar(TreeNode* t)
{
    SymbTable* entry = NULL;
    FieldChain* entry2 = NULL;
    bool present = STable::FindEntry(t->name[0],false,&entry);
    if(!present)
    {
        //err
        return NULL;
    }
    if(entry->attrIR.kind != varKind)
    {
        //err
        return NULL;
    }
    if(entry->attrIR.idtype->kind != recordTy)
    {
        //err
        return NULL;
    }
    bool present2 = STable::FindField(t->child[0]->name[0],entry->attrIR.idtype->more.body,&entry2);
    if(!present2)
    {
        //err
        cout<<"line:"<<t->child[0]->lineno<<' ';
        cout<<t->child[0]->name[0]<<' ';
        SyntaxAnalysis::dispErr("未声明的域变量");
        return NULL;
    }
    return entry2->unitType;
}


void SemAnalysis::assignstatement(TreeNode* t)
{
    TreeNode* Child1 = t->child[0];
    TreeNode* Child2 = t->child[1];
    TypeIR* Eptr = NULL;
    SymbTable* entry = NULL;
    if(Child1->child[0] != NULL)
    {
        if(Child1->attr.ExpAttr.varkind != ArrayMembV)
        {
            Eptr = recordVar(Child1);
        }
        else
            Eptr = arrayVar(Child1);
    }
    else
    {
        bool present = STable::FindEntry(Child1->name[0],false,&entry);
        if(!present)
        {
            //err
            cout<<"line:"<<Child1->lineno<<' ';
            cout<<Child1->name[0]<<' ';
            SyntaxAnalysis::dispErr("未声明");
        }
        else if(entry->attrIR.kind != varKind)
        {
            //err
            cout<<"line:"<<Child1->lineno<<' ';
            cout<<Child1->name[0]<<' ';
            SyntaxAnalysis::dispErr("非变量");
        }
        else
            Eptr = entry->attrIR.idtype;
    }
    if(Eptr != Expr(Child2/*,*/))
    {
        //err 两边不等价
        cout<<"line:"<<Child1->lineno<<' ';
        SyntaxAnalysis::dispErr("类型不相容");
    }
}


void SemAnalysis::callstatement(TreeNode* t)
{
    SymbTable* entry = NULL;
    //TreeNode* p
    bool present = STable::FindEntry(t->child[0]->name[0],false,&entry);
    if(!present)
    {
        //err
        cout<<"line:"<<t->child[0]->lineno<<' ';
        cout<<t->child[0]->name[0]<<' ';
        SyntaxAnalysis::dispErr("未声明");
        //cout<<"callstatement"<<endl;
        return;
    }
    if(entry->attrIR.kind != procKind)
    {
        //err
        cout<<"line:"<<t->child[0]->lineno<<' ';
        cout<<t->child[0]->name[0]<<' ';
        SyntaxAnalysis::dispErr("非过程标识符");
        //cout<<"callstatement"<<endl;
        return;
    }
    //处理形参实参结合
    ParamTable* Param = entry->attrIR.more.ProcAttr.param;
    TreeNode* p = t->child[1];
    while(Param != NULL && p != NULL)
    {
        if(Param->entry->attrIR.idtype != Expr(p/*,*/))
        {
            //err 形参实参不匹配
            cout<<"line:"<<p->lineno<<' ';
            cout<<p->name[0]<<' ';
            SyntaxAnalysis::dispErr("形参实参不匹配");
            //cout<<"callstatement"<<endl;
            return;
        }
        Param = Param->next;
        p = p->sibling;
    }
    if(p == NULL && Param != NULL)
    {
        //err 实参个数不够
        cout<<"line:"<<t->child[0]->lineno<<' ';
        cout<<t->child[0]->name[0]<<' ';
        SyntaxAnalysis::dispErr("实参个数不够");
        //cout<<"callstatement"<<endl;
        return;
    }
    if(p != NULL && Param == NULL)
    {
        //err 实参个数太多
        cout<<"line:"<<t->child[0]->lineno<<' ';
        cout<<t->child[0]->name[0]<<' ';
        SyntaxAnalysis::dispErr("实参个数太多");
        //cout<<"callstatement"<<endl;
        return;
    }
}


void SemAnalysis::readstatement(TreeNode* t)
{
    SymbTable* entry = NULL;
    bool present = STable::FindEntry(t->name[0],false,&entry);
    if(!present)
    {
        //err 未声明
        cout<<"readstatement"<<endl;
        return;
    }
    if(entry->attrIR.kind != varKind)
    {
        //err 不一致
        cout<<"readstatement"<<endl;
        return;
    }
}


void SemAnalysis::writestatement(TreeNode* t)
{
    TypeIR* type = Expr(t->child[0]/*,*/);
    if(t->kind.exp == OpK)
    {
        if(t->attr.ExpAttr.op == LT || t->attr.ExpAttr.op == EQ)
        {
            if(type->kind != boolTy)
            {
                //err
                cout<<"writestatement"<<endl;
                return;
            }
        }
    }
}

void SemAnalysis::whilestatement(TreeNode* t)
{
    TreeNode* p = t->child[0];
    TypeIR* type = Expr(p/*,*/);
    if(type->kind != boolTy)
    {
        //err
        SyntaxAnalysis::dispErr("while条件部分非bool类型");
        //cout<<"whilestatement"<<endl;
        return;
    }
    /*
    if(p->kind.exp == OpK)
    {
    	if(p->attr.ExpAttr.op == LT || p->attr.ExpAttr.op == EQ)
    	{
    		if(type->kind != boolTy)
    		{
    			//err
    			SyntaxAnalysis::dispErr("while条件部分非bool类型");
    			//cout<<"whilestatement"<<endl;
    			return;
    		}
    	}
    }*/

    p = t->child[1];
    while(p != NULL)
    {
        statement(p);
        p = p->sibling;
    }
}


void SemAnalysis::ifstatement(TreeNode* t)
{
    TreeNode* p = t->child[0];
    TypeIR* type = Expr(p/*,*/);
    if(type->kind != boolTy)
    {
        //err
        SyntaxAnalysis::dispErr("if条件部分非bool类型");
        //cout<<"ifstatement"<<endl;
        return;
    }
    /*
    if(p->kind.exp == OpK)
    {
    	if(p->attr.ExpAttr.op == LT || p->attr.ExpAttr.op == EQ)
    	{
    		if(type->kind != boolTy)
    		{
    			//err
    			SyntaxAnalysis::dispErr("if条件部分非bool类型");
    			//cout<<"ifstatement"<<endl;
    			return;
    		}
    	}
    }*/

    p = t->child[1];
    while(p != NULL)
    {
        statement(p);
        p = p->sibling;
    }

    p = t->child[2];
    while(p != NULL)
    {
        statement(p);
        p = p->sibling;
    }
}

//主体
void SemAnalysis::Analyze(TreeNode* t)
{

    initialize();

    STable::CreatTable();

    //处理声明部分
    TreeNode* p = t->child[1];
    TreeNode* q = NULL;
    if(p != NULL)
    {
        if(p->nodekind == TypeK)
        {

            TypeDecPart(p);
            if(p->sibling != NULL)
                p = p->sibling;
        }
        if(p->nodekind == VarK)
        {
            q = p->child[0];

            VarDecPart(q,0);
            if(p->sibling != NULL)
                p = p->sibling;
        }
        if(p->nodekind == ProcK)
        {

            ProcDecPart(p);
            //p = p->sibling;
        }
    }

    //处理主程序体
    p = t->child[2];
    if(p != NULL)
    {
        q = p->child[0];
        while(q != NULL)
        {
            statement(q);
            q = q->sibling;
        }
    }
    else
        SyntaxAnalysis::dispErr("err 无语句");

    //撤销符号表
    //...
}

void SemAnalysis::returnstatement(TreeNode* t)
{
    return;
}


////////////////////////////////////////////////////////////
int main(void)
{
    TreeNode* root = NULL;
    root = SyntaxAnalysis::parse();
    if(root == NULL)
        cout<<"error"<<endl;
    SemAnalysis::Analyze(root);
    STable::PrindSymbTable();
    result = fopen("result.txt","w");
    STable::PrintSymbTable();
    fclose(result);
    system("pause");
    return 0;
}
