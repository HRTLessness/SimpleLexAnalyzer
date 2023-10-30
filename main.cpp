/*
Adam Mariano, CS280, Section 13
    Driver file for Lexical analyzer
    Takes in arguments for file and flags
    Produces correct output for the input flags
*/
#include "lex.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

int main(int argc, char *argv [])
{
    std::ifstream inFile;

    //Flag Booleans
    bool vFlag = false;
    bool nconstFlag = false;
    bool sconstFlag = false;
    bool bconstFlag = false;
    bool identFlag = false;
    
    //Attempts to read file when there is an argument
    if (argc >= 2)
    {

        for (int i = 1; i < argc; i++){
            string argString = argv[i];

            //check for flags
            if (argString[0] == '-'){
                if (!argString.compare("-v"))
                {
                    vFlag = true;
                }
                else if (!argString.compare("-nconst"))
                {
                    nconstFlag = true;
                }
                else if (!argString.compare("-sconst"))
                {
                    sconstFlag = true;
                }
                else if (!argString.compare("-bconst"))
                {
                    bconstFlag = true;
                }
                else if (!argString.compare("-ident"))
                {
                    identFlag = true;
                }
                else
                { //unknown Flag error
                    std::cerr << "UNRECOGNIZED FLAG {" << argString << "}" << std::endl;
                    return -1;
                }
            }
            // check for valid file
            else 
            {
                if(inFile.is_open())
                { //Opening more than one file error
                    std::cerr << "ONLY ONE FILE NAME IS ALLOWED." << std::endl;
                    return -1;
                }
                else
                {
                    inFile.open(argString);
                    if(!inFile.is_open())
                    { //invalid filename
                        std::cerr << "CANNOT OPEN THE FILE " << argString << std::endl;
                        return -1;
                    } 
                }
            }
        }
    }
    //check if there is no file opened
    if(!inFile.is_open()){
        std::cerr << "NO SPECIFIED INPUT FILE." << std::endl;
        return -1;
    }

    //checks for empty file
    if (inFile.peek() == ifstream::traits_type::eof())
    {
        std::cout << "Empty File." << std::endl;
        return -1;
    }

    //tracking vectors
    std::vector<string> idents;
    std::vector<float> nConsts;
    std::vector<string> sConsts;
    std::vector<string> bConsts;

    /*Call getNextToken until it returns DONE or ERR*/
    int lineNum = 1;
    int totalTokens = 0;
    LexItem temp = getNextToken(inFile, lineNum);
    while(temp.GetToken() != ERR && temp.GetToken() != DONE){
        totalTokens++;
        if (vFlag) //Print out tokens if there is -v flag
        {
            std::cout << temp << std::endl;
        }
        if (temp.GetToken() == IDENT)
        {
            idents.push_back(temp.GetLexeme());
        }
        if (temp.GetToken() == ICONST || temp.GetToken() == RCONST)
        {
            nConsts.push_back(stof(temp.GetLexeme()));
        }
        if (temp.GetToken() == BCONST)
        {
            bConsts.push_back(temp.GetLexeme());
        }
        if (temp.GetToken() == SCONST)
        {
            sConsts.push_back(temp.GetLexeme());
        }
        temp = getNextToken(inFile, lineNum);
    }
    if (temp.GetToken() == ERR)
    { //end the program if an error is detected
        std::cout << "Error in line " << temp.GetLinenum() << ": Unrecognized Lexeme {" << temp.GetLexeme() << "}\n";
        return -1;
    }

    //sorting idents and deleting duplicates
    std::sort(idents.begin(), idents.end());
    idents.erase( unique( idents.begin(), idents.end() ), idents.end() );

    //sorting numbers and deleting duplicates
    std::sort(nConsts.begin(), nConsts.end());
    std::vector<string> numbers;
    for(float f : nConsts){
        std::string s = std::to_string (f);
        s.erase ( s.find_last_not_of('0') + 1, std::string::npos );
        s.erase ( s.find_last_not_of('.') + 1, std::string::npos );
        numbers.push_back(s);
    }
    numbers.erase( unique( numbers.begin(), numbers.end() ), numbers.end() );

    //sorting booleans and deleting duplicates
    std::sort(bConsts.begin(), bConsts.end());
    bConsts.erase( unique( bConsts.begin(), bConsts.end() ), bConsts.end() );

    //sorting strings and deleting duplicates
    std::sort(sConsts.begin(), sConsts.end());
    sConsts.erase( unique( sConsts.begin(), sConsts.end() ), sConsts.end() );

    //print out summary
    std::cout << std::endl << "Lines: " << lineNum << std::endl;
    std::cout << "Total Tokens: " << totalTokens << std::endl;
    std::cout << "Identifiers: " << idents.size() << std::endl;
    std::cout << "Numbers: " << numbers.size() << std::endl;
    std::cout << "Booleans: " << bConsts.size() << std::endl;
    std::cout << "Strings: " << sConsts.size() << std::endl;

    //flag print outs
    if(identFlag)
    {
        std::cout << "IDENTIFIERS:" << std::endl;
        for (int i = 0; i < idents.size()-1; i++){
            std::cout << idents[i] << ", ";
        }
        std::cout << idents[idents.size()-1] << std::endl;
    }
    if(nconstFlag)
    {
        std::cout << "NUMBERS:" << std::endl;
        for (string s : numbers){
            std::cout << s << std::endl;
        }
    }
    if(bconstFlag)
    {
        std::cout << "BOOLEANS:" << std::endl;
        for (string s : bConsts){
            std::cout << s << std::endl;
        }
    }
    if(sconstFlag)
    {
        std::cout << "STRINGS:" << std::endl;
        for (string s : sConsts){
            std::cout << "\"" << s << "\"" << std::endl;
        }
    }

}