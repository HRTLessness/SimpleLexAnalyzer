/*
Adam Mariano, CS 280, Section 13
    Lexical Analyzer for language "Simple Pascal-Like"
        Creating a lexical analyzer based on given sample syntax definitions in EBNF format
*/

#include "lex.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

//map for lexeme search
const std::map<string, Token> lexMap =
{
    {"and", AND},
    {"begin", BEGIN},
    {"boolean", BOOLEAN},
    {"div", IDIV},
    {"end", END},
    {"else", ELSE},
    {"false", FALSE},
    {"if", IF},
    {"then", THEN},
    {"integer", INTEGER},
    {"mod", MOD},
    {"not", NOT},
    {"or", OR},
    {"program", PROGRAM},
    {"real", REAL},
    {"string", STRING},
    {"write", WRITE},
    {"writeln", WRITELN},
    {"var", VAR},
    {"+", PLUS},
    {"-", MINUS},
    {"*", MULT},
    {"/", DIV},
    {":=", ASSOP},
    {"=", EQ},
    {"<", LTHAN},
    {">", GTHAN},
    {",", COMMA},
    {";", SEMICOL},
    {"(", LPAREN},
    {")", RPAREN},
    {":", COLON},
    {".", DOT}
};

const char* enumStrings[] = {"IF", "ELSE", "WRITELN", "WRITE", "INTEGER", "REAL",
                            "BOOLEAN", "STRING", "BEGIN", "END", "VAR", "THEN", "PROGRAM",
                            "IDENT", "TRUE", "FALSE", "ICONST", "RCONST", "SCONST", "BCONST",
                            "PLUS", "MINUS", "MULT", "DIV", "IDIV", "MOD", "ASSOP", "EQ", 
                            "GTHAN", "LTHAN", "AND", "OR", "NOT", 
                            "COMMA", "SEMICOL", "LPAREN", "RPAREN", "DOT", "COLON",
                            "ERR", "DONE"
                            };

//Operator overload for easy cout
extern ostream& operator<<(ostream& out, const LexItem& tok)
{
    if (tok.GetToken() == IDENT || tok.GetToken() == ICONST || tok.GetToken() == RCONST ||
        tok.GetToken() == SCONST || tok.GetToken() == BCONST)
    {
       out << enumStrings[tok.GetToken()] << ": \"" << tok.GetLexeme() << "\"";
    }
    else{
       out << enumStrings[tok.GetToken()];
    }
    return out;
}

//method to search for keywords
extern LexItem id_or_kw(const string& lexeme, int linenum)
{
    if (!lexeme.compare("true") || !lexeme.compare("false"))
    {
        return LexItem(BCONST, lexeme, linenum);
    }
    else if(lexMap.find(lexeme) != lexMap.end())
    {
        return LexItem(lexMap.find(lexeme)->second, lexeme, linenum);
    }
    else
    {
        return LexItem(IDENT, lexeme, linenum);
    }
}

//Method that iterates through the file character by character
extern LexItem getNextToken(istream& in, int& linenum)
{
    //enum variable to keep track of different states 
    enum TokState { START, INID, ININT, INREAL, INSTRING, COMMENT, OPSYM } lexState = START;
    string lexeme;
    lexState = START;
    char ch;
    while (in.get(ch)){
        //std::cout << ch << " " << linenum << std::endl;
        switch (lexState) {
            case START:
                if(ch == '\n')
                {
                    if(in.peek() != EOF)
                    {
                        linenum++;
                    }
                    break;
                }
                else if (std::isspace(ch))
                {
                    break;
                }
                else if (ch == '{')
                {
                    lexState = COMMENT;
                }
                else if (std::isdigit(ch))
                {
                    lexState = ININT;
                    in.putback(ch);
                }
                else if (std::isalpha(ch))
                {
                    lexState = INID;
                    in.putback(ch);
                }
                else if (ch == '\'')
                {
                    lexState = INSTRING;
                }
                else {
                    lexState = OPSYM;
                    in.putback(ch);
                }
                break;
            case COMMENT: //ignore comment until the } is reached
                if (ch == '\n')
                {
                    if(in.peek() != EOF)
                    {
                        linenum++;
                    }
                }
                else if (ch == '}')
                {
                    lexState = START; 
                }
                break;  
            case INID: // determine keyword or ident
                if (std::isalnum(ch) || ch == '_' || ch == '$')
                {
                    lexeme += ch;
                    if (std::isalnum(in.peek()) || in.peek() == '_' || in.peek() == '$')
                    {
                        continue;
                    }
                    else
                    {
                        return id_or_kw(lexeme, linenum);
                    }
                }
                else{
                    return LexItem(ERR, lexeme, linenum);
                }
                break;
            case ININT: //determine between integer and real
                if (std::isdigit(ch))
                {
                    lexeme += ch;
                    if (in.peek() == '.')
                    {
                        lexState = INREAL;
                    }
                    else if (std::isdigit(in.peek()))
                    {
                        continue;
                    }
                    else
                    {
                        return LexItem(ICONST, lexeme, linenum);
                    }
                }
                else
                {
                    return LexItem(ERR, lexeme, linenum);
                }
                break;
            case INREAL: //handle real numbers
                if (std::isdigit(ch) || ch == '.')
                {
                    lexeme += ch;
                    if (in.peek() == '.')
                    {
                        lexeme += '.';
                        return LexItem(ERR, lexeme, linenum);
                    }
                    else if(std::isdigit(in.peek()))
                    {
                        continue;
                    }
                    else
                    {
                        lexState = START;
                        return LexItem(RCONST, lexeme, linenum);
                    }
                }
                else 
                {
                    lexState = START;
                    return LexItem(RCONST, lexeme, linenum);
                }
                break;
            case OPSYM: //handle operators and symbols
                lexeme += ch;
                if (ch == ':' && in.peek() == '=')
                {
                    in.get(ch);
                    lexeme += ch;
                    return id_or_kw(lexeme, linenum);
                }
                else if(lexMap.find(lexeme) != lexMap.end())
                {
                    return id_or_kw(lexeme, linenum);
                }
                else
                {
                    return LexItem(ERR, lexeme, linenum);
                }

                break;
            case INSTRING: //handles strings
                if(in.peek() == EOF)
                {
                    lexeme.insert(0, "\'");
                    return LexItem(ERR, lexeme, linenum);
                }
                if (ch == '\'')
                {
                    return LexItem(SCONST, lexeme, linenum);
                }
                else if (ch != '\'')
                {
                    lexeme += ch;
                }
                
                break;
                 
        }

    }
    return LexItem(DONE, lexeme, linenum); //return DONE if EOF is reached
}
