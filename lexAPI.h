#ifndef LEXAPI_H
#define LEXAPI_H
#include "lexAnalyzer.h"
#include <stdlib.h>

typedef enum
{
    START,INASSIGN,INCOMMENT,INNUM,INID,
    INCHAR,OUTCHAR,INRANGE,DONE
}
stateType;

std::string getNextChar(bufferCustom& buf)
{
    return buf.read();
}

void rollbackChar(bufferCustom& buf)
{
    buf.rollback();
}

typedef enum
{
    NUMERICS,ALPHABETA,DELIMITER,COLON,COMMENTSTART,COMMENTEND,
    DOTWC,EQUAL,QUOTE,WHITESPACE,OTHER,EOFS
}
whatChar;

whatChar singleCharCheck(std::string c)
{
    std::string numerics("0123456789");
    std::string alphabeta("abcdefghijklmnopqrstuvwxyz");
    std::string delimiter("+-*/();[]<,");
    std::string colon(":");
    std::string equal("=");
    std::string whitespace(" \n\t");
    std::string commentstart("{");
    std::string commentend("}");
    std::string dot(".");
    std::string quote("'");
    if(c.find_first_of(numerics)!=c.npos)
        return NUMERICS;
    if(c.find_first_of(alphabeta)!=c.npos)
        return ALPHABETA;
    if(c.find_first_of(delimiter)!=c.npos)
        return DELIMITER;
    if(c.find_first_of(colon)!=c.npos)
        return COLON;
    if(c.find_first_of(equal)!=c.npos)
        return EQUAL;
    if(c.find_first_of(whitespace)!=c.npos)
        return WHITESPACE;
    if(c.find_first_of(commentstart)!=c.npos)
        return COMMENTSTART;
    if(c.find_first_of(commentend)!=c.npos)
        return COMMENTEND;
    if(c.find_first_of(dot)!=c.npos)
        return DOTWC;
    if(c.find_first_of(quote)!=c.npos)
        return QUOTE;
    if(c[0]==EOF||c=="")
        return EOFS;
    return OTHER;
}

bufferCustom initBuf(std::string inputFile)
{
    return bufferCustom(inputFile);
}

token getToken(bufferCustom& buf)
{
    stateType state;
    state=START;
    lexType outTokenLexType;
    std::string c;
    std::string s;
ignorewhitespace:
    c=getNextChar(buf);
    s+=c;
checkstate:
    switch(state)
    {
    case START:
        switch(singleCharCheck(c))
        {
        case NUMERICS:
            state=INNUM;
            break;
        case ALPHABETA:
            state=INID;
            break;
        case EQUAL:
        case DELIMITER:
        {
            state=DONE;
            outTokenLexType=reservedSymbols[s];
        }
        break;
        case WHITESPACE:
            state=START;
            s=std::string();
            goto ignorewhitespace;
        case EOFS:
            state=DONE;
            outTokenLexType=ENDFILE;
            break;
        case COLON:
            state=INASSIGN;
            break;
        case COMMENTSTART:
            state=INCOMMENT;
            break;
        case DOTWC:
            state=INRANGE;
            break;
        case QUOTE:
            state=INCHAR;
            break;
        case OTHER:
            state=DONE;
            outTokenLexType=ERROR;
            break;
        }
        goto checkstate;

    case INASSIGN:
        c=getNextChar(buf);
        s+=c;
        switch(singleCharCheck(c))
        {
        case EQUAL:
            state=DONE;
            outTokenLexType=ASSIGN;
            break;
        default:
            rollbackChar(buf);
            std::string t(s.begin(),s.begin()+s.size()-1);
            s=t;
            outTokenLexType=ERROR;
            state=DONE;
            break;
        }
        goto checkstate;

    case INCOMMENT:
        c=getNextChar(buf);
        s+=c;
        switch(singleCharCheck(c))
        {
        case COMMENTEND:
            state=DONE;
            outTokenLexType=COMMENT;
            break;
        case EOFS:
        {
            state=DONE;
            rollbackChar(buf);
            std::string t(s.begin(),s.begin()+s.size()-1);
            s=t;
            outTokenLexType=ERROR;
        }
        break;

        default:
            state=INCOMMENT;
        }
        goto checkstate;

    case INNUM:
        c=getNextChar(buf);
        s+=c;
        switch(singleCharCheck(c))
        {
        case NUMERICS:
            state=INNUM;
            break;
        default:
            rollbackChar(buf);
            std::string t(s.begin(),s.begin()+s.size()-1);
            s=t;
            outTokenLexType=INTC;
            state=DONE;
        }
        goto checkstate;

    case INID:
        c=getNextChar(buf);
        s+=c;
        switch(singleCharCheck(c))
        {
        case ALPHABETA:
            state=INID;
            break;
        case NUMERICS:
            state=INID;
            break;
        default:
            rollbackChar(buf);
            std::string t(s.begin(),s.begin()+s.size()-1);
            s=t;
            state=DONE;
            outTokenLexType=ID;
            break;
        }
        goto checkstate;

    case INCHAR:
        c=getNextChar(buf);
        s+=c;
        switch(singleCharCheck(c))
        {
        case NUMERICS:
            state=OUTCHAR;
            break;
        case ALPHABETA:
            state=OUTCHAR;
            break;
        case QUOTE:
            state=DONE;
            outTokenLexType=CHARC;
            break;
        default:
            state=DONE;
            outTokenLexType=ERROR;
            rollbackChar(buf);
            std::string t(s.begin(),s.begin()+s.size()-1);
            s=t;
            break;
        }
        goto checkstate;

    case INRANGE:
        c=getNextChar(buf);
        s+=c;
        switch(singleCharCheck(c))
        {
        case DOTWC:
            state=DONE;
            outTokenLexType=UNDERANGE;
            break;
        default:
            //printf("\n%dINRANGE%c\n",buf.count-1,buf.bufstr[buf.count-1]);///
            rollbackChar(buf);
            std::string t(s.begin(),s.begin()+s.size()-1);
            s=t;
            outTokenLexType=DOT;
            state=DONE;
        }
        goto checkstate;
    case OUTCHAR:
        c=getNextChar(buf);
        s+=c;
        switch(singleCharCheck(c))
        {
        case QUOTE:
            state=DONE;
            outTokenLexType=CHARC;
            break;
        default:
            state=DONE;
            outTokenLexType=ERROR;
            rollbackChar(buf);
            std::string t(s.begin(),s.begin()+s.size()-1);
            s=t;
            break;
        }
        goto checkstate;
    case DONE:
        break;
    }
    if(outTokenLexType==ID)
    {
        if(reservedWords.count(s))
        {
            outTokenLexType=reservedWords[s];
            return token(buf.lineFile,outTokenLexType,lexTypeSemInfo[outTokenLexType]);
        }
        else
        {
            if(idTable.count(s)==0)
            {
                idTable[s]=1;
                std::string te=lexTypeSemInfo[outTokenLexType];
                std::string::size_type pos=te.find_first_of("?");
                te.insert(pos,s);
                return token(buf.lineFile,outTokenLexType,te);
            }
            else
            {
                idTable[s]++;
                std::string te=lexTypeSemInfo[outTokenLexType];
                std::string::size_type pos=te.find_first_of("?");
                te.insert(pos,s);
                return token(buf.lineFile,outTokenLexType,te);
            }
        }
    }
    else
    {
        if(outTokenLexType==INTC)
        {
            if(intcTable.count(s)==0)
            {
                intcTable[s]=atoi(s.c_str());
            }
            std::string te=lexTypeSemInfo[outTokenLexType];
            std::string::size_type pos=te.find_first_of("?");
            te.insert(pos,s);
            return token(buf.lineFile,outTokenLexType,te);
        }
        else if(outTokenLexType==CHARC)
        {
            if(charcTable.count(s)==0)
            {
                charcTable[s]=s[0];
            }
            std::string te=lexTypeSemInfo[outTokenLexType];
            std::string::size_type pos=te.find_first_of("?");
            te.insert(pos,s);
            return token(buf.lineFile,outTokenLexType,te);
        }
        else
            return token(buf.lineFile,outTokenLexType,lexTypeSemInfo[outTokenLexType]);
    }
}

tokenList getTokenList(bufferCustom& buf)
{
    tokenList ret;
    token te=getToken(buf);
    ret.push_back(te);
    printf("%d\n%d\n%s\n",te.lexInfo,te.linePosition,te.semInfo.c_str());
    while(te.lexInfo!=ENDFILE)
    {
        te=getToken(buf);
        ret.push_back(te);
        printf("%d\n%d\n%s\n",te.lexInfo,te.linePosition,te.semInfo.c_str());
    }
    return ret;
}

bool formatTokenListToFile(tokenList& l)
{
    tokenList::iterator it;
    printf("行数\t词法信息\t语义信息\n");
    for(it=l.begin(); it!=l.end(); it++)
    {
        printf("%d\t%s\t\t%s\n",it->linePosition,lexTypeToString[it->lexInfo].c_str(),(it->semInfo).c_str());
    }
    return true;
}
#endif
