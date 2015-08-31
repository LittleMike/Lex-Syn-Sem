#ifndef LEXANALYZER_H
#define LEXANALYZER_H
#include <stdio.h>
#include <list>
#include <string>
#include <map>

//token的种类，即词法类型
typedef enum
{
    PROGRAM,PROCEDURE,TYPE,VAR,IF,THEN,
    ELSE,FI,WHILE,DO,ENDWH,BEGIN,END,READ,WRITE,ARRAY,OF,
    RECORD,RETURN,INTEGER,CHAR,ID,INTC,CHARC,ASSIGN,
    EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,DOT,
    SEMI,COMMA,LMIDPAREN,RMIDPAREN,UNDERANGE,
    ENDFILE,ERROR,COMMENT
}
lexType;

//词法类型的语义信息
std::map<lexType,std::string> lexTypeSemInfo;
//保留字表
std::map<std::string,lexType> reservedWords;
//标识符表
std::map<std::string,int> idTable;
//保留符号
std::map<std::string,lexType> reservedSymbols;
//字符常量表
std::map<std::string,char> charcTable;
//无符号整数常量表
std::map<std::string,int> intcTable;
std::map<lexType,std::string> lexTypeToString;

//输入缓冲区buffer
class bufferCustom
{
public:
    bufferCustom(std::string input,int buflen=256)
    {
        bufLen=buflen;
        buf=new char[bufLen];
        inputFile=fopen(input.c_str(),"r");
        if(inputFile==NULL)
            printf("bufferCustom can't open the file %s!\n",input.c_str());
        count=bufLen;
        lineFile=1;
    }
    bool loadNextBuf();
    std::string read(int n=1);
    int rollback(int n=1);
    const char* buf;
    std::string bufstr;
    int bufLen;
    FILE* inputFile;
    int count;
    int lineFile;
};

bool bufferCustom::loadNextBuf()
{
    const char* test=NULL;
    //test=fgets(test,count,inputFile);
    int tc=0;
    std::string te;
    while(!feof(inputFile)&&tc<count)
    {
        te+=fgetc(inputFile);
        tc++;
    }
    test=te.c_str();
    if(test==NULL)
        printf("something is wrong at loading inputfile's buffer when executing the bufferCustom's loadNextBuf()");
    std::string ret(buf+count,bufLen-count);
    bufLen=(bufLen-count)+tc;
    ret+=std::string(test);
    bufstr=ret;
    buf=bufstr.c_str();

    count=0;

    if(buf==NULL)
        return false;
    else
        return true;
}

std::string bufferCustom::read(int n)
{
    std::string ret;
    if(count+n<bufLen)
        ret=std::string(bufstr,count,n);
    else
    {
        loadNextBuf();
        ret=std::string(bufstr,count,n);
    }
    for(int i=0; i<n; ++i)
        if(bufstr[count++]=='\n')
            lineFile++;
    return ret;
}

int bufferCustom::rollback(int n)
{
    for(int i=0; i<n; ++i)
    {
        //printf("\nrollbacking...\n");
        count--;
        //printf("STARTHERE%d\n%s\nENDHERE\n",bufstr.size(),bufstr.c_str());
        //printf("bufstr[%d]=%d--%c\n",count,bufstr[count],bufstr[count]);
        if(bufstr[count]=='\n')
        {
            //printf("before this rollback-Count:%d,Line:%d\n",count,lineFile);
            this->lineFile--;
            //printf("after this rollback-Count:%d,Line:%d\n\n",count,lineFile);
        }

    }
    return count;
}

//配置信息初始化
int confInit()
{
    lexTypeSemInfo[PROGRAM]="program";
    lexTypeSemInfo[PROCEDURE]="procedure";
    lexTypeSemInfo[TYPE]="type";
    lexTypeSemInfo[VAR]="var";
    lexTypeSemInfo[IF]="if";
    lexTypeSemInfo[THEN]="then";
    lexTypeSemInfo[ELSE]="else";
    lexTypeSemInfo[FI]="fi";
    lexTypeSemInfo[WHILE]="while";
    lexTypeSemInfo[DO]="do";
    lexTypeSemInfo[ENDWH]="endwh";
    lexTypeSemInfo[BEGIN]="begin";
    lexTypeSemInfo[END]="end";
    lexTypeSemInfo[READ]="read";
    lexTypeSemInfo[WRITE]="write";
    lexTypeSemInfo[ARRAY]="array";
    lexTypeSemInfo[OF]="of";
    lexTypeSemInfo[RECORD]="record";
    lexTypeSemInfo[RETURN]="return";
    lexTypeSemInfo[INTEGER]="integer";
    lexTypeSemInfo[CHAR]="char";

    reservedWords["program"]=PROGRAM;
    reservedWords["procedure"]=PROCEDURE;
    reservedWords["type"]=TYPE;
    reservedWords["var"]=VAR;
    reservedWords["if"]=IF;
    reservedWords["then"]=THEN;
    reservedWords["else"]=ELSE;
    reservedWords["fi"]=FI;
    reservedWords["while"]=WHILE;
    reservedWords["do"]=DO;
    reservedWords["endwh"]=ENDWH;
    reservedWords["begin"]=BEGIN;
    reservedWords["end"]=END;
    reservedWords["read"]=READ;
    reservedWords["write"]=WRITE;
    reservedWords["array"]=ARRAY;
    reservedWords["of"]=OF;
    reservedWords["record"]=RECORD;
    reservedWords["return"]=RETURN;
    reservedWords["integer"]=INTEGER;
    reservedWords["char"]=CHAR;

    lexTypeSemInfo[ID]="?";

    lexTypeSemInfo[CHARC]="?";
    lexTypeSemInfo[INTC]="?";

    lexTypeSemInfo[ASSIGN]=":=";
    lexTypeSemInfo[EQ]="=";
    lexTypeSemInfo[LT]="<";
    lexTypeSemInfo[PLUS]="+";
    lexTypeSemInfo[MINUS]="-";
    lexTypeSemInfo[TIMES]="*";
    lexTypeSemInfo[OVER]="/";
    lexTypeSemInfo[LPAREN]="(";
    lexTypeSemInfo[RPAREN]=")";
    lexTypeSemInfo[SEMI]=";";
    lexTypeSemInfo[COMMA]=",";
    lexTypeSemInfo[LMIDPAREN]="[";
    lexTypeSemInfo[RMIDPAREN]="]";
    lexTypeSemInfo[UNDERANGE]="..";
    reservedSymbols[":="]=ASSIGN;
    reservedSymbols["="]=EQ;
    reservedSymbols["<"]=LT;
    reservedSymbols["+"]=PLUS;
    reservedSymbols["-"]=MINUS;
    reservedSymbols["*"]=TIMES;
    reservedSymbols["/"]=OVER;
    reservedSymbols["("]=LPAREN;
    reservedSymbols[")"]=RPAREN;
    reservedSymbols[";"]=SEMI;
    reservedSymbols[","]=COMMA;
    reservedSymbols["["]=LMIDPAREN;
    reservedSymbols["]"]=RMIDPAREN;
    reservedSymbols[".."]=UNDERANGE;
    reservedSymbols["."]=DOT;

    lexTypeSemInfo[DOT]="程序结束符?NULL";
    lexTypeSemInfo[ENDFILE]="文件结束符?NULL";

    lexTypeSemInfo[ERROR]="错误?NULL";
    lexTypeSemInfo[COMMENT]="注释?NULL";

    lexTypeToString[PROGRAM]="PROGRAM";
    lexTypeToString[PROCEDURE]="PROCEDURE";
    lexTypeToString[TYPE]="TYPE";
    lexTypeToString[VAR]="VAR";
    lexTypeToString[IF]="IF";
    lexTypeToString[THEN]="THEN";
    lexTypeToString[ELSE]="ELSE";
    lexTypeToString[FI]="FI";
    lexTypeToString[WHILE]="WHILE";
    lexTypeToString[DO]="DO";
    lexTypeToString[ENDWH]="ENDWH";
    lexTypeToString[BEGIN]="BEGIN";
    lexTypeToString[END]="END";
    lexTypeToString[READ]="READ";
    lexTypeToString[WRITE]="WRITE";
    lexTypeToString[ARRAY]="ARRAY";
    lexTypeToString[OF]="OF";
    lexTypeToString[RECORD]="RECORD";
    lexTypeToString[RETURN]="RETURN";
    lexTypeToString[INTEGER]="INTEGER";
    lexTypeToString[CHAR]="CHAR";

    lexTypeToString[ID]="ID";

    lexTypeToString[CHARC]="CHARC";
    lexTypeToString[INTC]="INTC";

    lexTypeToString[ASSIGN]="ASSIGN";
    lexTypeToString[EQ]="EQ";
    lexTypeToString[LT]="LT";
    lexTypeToString[PLUS]="PLUS";
    lexTypeToString[MINUS]="MINUS";
    lexTypeToString[TIMES]="TIMES";
    lexTypeToString[OVER]="OVER";
    lexTypeToString[LPAREN]="LPAREN";
    lexTypeToString[RPAREN]="RPAREN";
    lexTypeToString[SEMI]="SEMI";
    lexTypeToString[COMMA]="COMMA";
    lexTypeToString[LMIDPAREN]="LMIDPAREN";
    lexTypeToString[RMIDPAREN]="RMIDPAREN";
    lexTypeToString[UNDERANGE]="UNDERANGE";

    lexTypeToString[DOT]="DOT";
    lexTypeToString[ENDFILE]="ENDFILE";

    lexTypeToString[ERROR]="ERROR";
    lexTypeToString[COMMENT]="COMMENT";
    return 0;
}

//单词
class token
{
public:
    int linePosition;//token所在行号
    lexType lexInfo;//词法信息
    std::string semInfo;//语义信息
public:
    token(int linePosition,lexType lexInfo,std::string semInfo)
    {
        this->linePosition=linePosition;
        this->lexInfo=lexInfo;
        this->semInfo=semInfo;
    }
    //addition
    token() {}
};

//词法分析输出链表
class tokenList:public std::list<token>
{
public:
    tokenList():std::list<token>()
    {
        head=this->begin();
    }
    tokenList(token instance):std::list<token>(1,instance)
    {
        head=this->begin();
    }
    tokenList::iterator head;
};

#endif
