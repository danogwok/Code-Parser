///////////////////////////////////////////////////////////////////////
// SemiExpression.cpp - collect tokens for analysis                  //
// ver 3.3                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Code Parser, Project 2                               //
// Author:      Ronak Bhuptani, SUID#429019830, Syracuse             //
//              University, rmbhupta@syr.edu                         //
///////////////////////////////////////////////////////////////////////
/*

* Maintenance History:
* --------------------
* ver 3.5 : 08 Feb 2016
* - all additions are mentioned in SemiExp.h
*
*/

#define _CRT_SECURE_NO_DEPRECATE
#define NO_WARN_MBCS_MFC_DEPRECATION
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <exception>
#include "SemiExp.h"
#include "../Tokenizer/Tokenizer.h"

using namespace Scanner;

//SemiExp constructor which initilizes _pToker object
SemiExp::SemiExp(Toker* pToker) : _pToker(pToker) {
	if (_access(inputDir.c_str(), 0) != 0)
		inputDir = "." + inputDir;
	if (_access(outputDir.c_str(), 0) != 0)
		outputDir = "." + outputDir;
}
std::map<Token, int> SemiExp::_terminationtokens = { {"{",1} ,{ "}",1 }, {";",1} }; // termination characters
Token SemiExp::_firstToken = "";  //First token value, stored to handle exceptions
Token SemiExp::_prevToken = "";   //prev Token value, stored to handle exceptions
bool SemiExp::_startFlag = true;; //startFlag, stored to handle exceptions
bool SemiExp::_forFlag = false;   //forFlag, stored to handle exceptions
bool SemiExp::_hashFlag=false;    //hashFlag, stored to handle exceptions
std::string SemiExp::inputDir = "./Inputs/";   //Input dir where all input files will be stored
std::string SemiExp::outputDir = "./Outputs/"; //Output dir where all output files will be stored

//getFunction collect tokens into vector _tokens
bool SemiExp::get(bool clear) //by default clear will be false
{
	if (_pToker == nullptr)
		throw(std::logic_error("no Toker reference"));
	if(clear)
		_tokens.clear();  //only clear tokens if clear is true
	setDefaultFlags(); //reset all the flags and string to default values
	std::string token="";
	int count = 0;
	while (true)
	{
		_prevToken = token;
		token = _pToker->getTok();
		if (_startFlag) {
			_firstToken = token;
			_startFlag = false;
		}
		if (token == "")
			break;
		_forFlag = token == "for" ? true : _forFlag;  
		_hashFlag = token == "#" ? true : _hashFlag;
		push_back(token);
		if (_forFlag && exception3(token, count)) //for exception will handled
			continue;
		if (_terminationtokens.count(token) > 0  || (token == "\n" && _hashFlag) || exception2(token) || isComment(token)) { //all other exceptions are handled here
			setDefaultFlags();
			return true;
		}
	}
	setDefaultFlags();
	return false;
}

//This function will handle (:,private-protected-public) exception
bool Scanner::SemiExp::exception2(Token currToken)
{
	if(currToken == ":" && (_prevToken == "public" || _prevToken == "protected" || _prevToken == "private" ))
		return true;
	return false;
}

//This function will handle (for(;;)) exception
bool Scanner::SemiExp::exception3(Token currToken,int &count)
{
	if (currToken == ";") {
		count++;
		if (count <= 2)
			return true;
	}
	return false;
}

//this function will check if token is comment
bool Scanner::SemiExp::isComment(Token currToken)
{
	if ((currToken[0] == '/' && currToken[1] == '/') || (currToken[0] == '/' && currToken[1] == '*'))
		return true;
	return false;
}

//reset default flags
void Scanner::SemiExp::setDefaultFlags()
{
	_firstToken = "";
	_prevToken = "";
	_startFlag = true;;
	_forFlag = false;
	_hashFlag = false;
}

//this function will return token on index n of _tokens vector
Token& SemiExp::operator[](size_t n)
{
	if (n < 0 || n >= _tokens.size())
		throw(std::invalid_argument("index out of range"));
	return _tokens[n];
}

//returns the size of SemiExp
size_t SemiExp::length()  
{
	return _tokens.size();
}

//will show the semiExp in one line
void SemiExp::printSemiExp(bool showNewLines )
{
	std::cout<<show(showNewLines);
}

std::string Scanner::SemiExp::show(bool showNewLines)
{
	std::string result = "";
	result = result + "\n ";
	trimFront();
	for (auto token : _tokens) {
		if (token != "\n" || showNewLines) {
			if (token == "\n")
				token = "newline";
			result += token + " ";
		}
	}
	result = result + "\n";
	return result;
}

//this will search for value specified in parameter in _tokens and return it's index if found
size_t Scanner::SemiExp::find(const Token & tok)
{
	for (size_t i = 0; i < length(); i++)
		if (_tokens[i] == tok)
			return i;
	return (length() + 1);
}

//push back token in SemiExp
void Scanner::SemiExp::push_back(const std::string & tok)
{
	_tokens.push_back(tok);
}

//deprecated
bool Scanner::SemiExp::merge(const Token & firstTok, const Token & secondTok)
{
	return true;
}

//this will remove token specified in parameter from semiexp
bool Scanner::SemiExp::remove(const Token & tok)
{
	std::vector<Token>::iterator it;
	for (it = _tokens.begin(); it != _tokens.end(); it++) {
		if (*it == tok) {
			_tokens.erase(it);
			return true;
		}
	}
	return false;
}

//this will remove token on index specified in parameter from semiexp
bool Scanner::SemiExp::remove(size_t index)
{
	for (size_t i = 0; i < length(); i++) {
		if (i == index) {
			_tokens.erase(_tokens.begin() + i);
			return true;
		}
	}
	return false;
}

//this will convert all tokens of current semiExp to lower case 
void Scanner::SemiExp::toLower()
{
	std::vector<std::string>::iterator it;
	for (it = _tokens.begin(); it != _tokens.end(); it++)
		std::transform((*it).begin(), (*it).end(), (*it).begin(), ::tolower);
}

//this fucntion will trim from newlines
void Scanner::SemiExp::trimFront()
{
	while (length() > 0 && _tokens[0] == "\n")
		remove("\n");
}

//this will clear semiexpression
void Scanner::SemiExp::clear()
{
	_tokens.clear();
}

//this function will print all the SemiExpression in specified input file
bool Scanner::SemiExp::printAllExpression(Toker* toker, std::string input, std::string getComments,bool storeText)
{
	clear();
	toker->reset();
	std::string output = outputDir + "Semi_"+ input;
	input = inputDir + input;
	FILE *pfile = nullptr;
	if(storeText)
		pfile = freopen(output.c_str(), "w", stdout);
	std::fstream in(input);
	if (!in.good())
	{
		std::cout << "\n  can't open file " << input << "\n\n";
		return false;
	}
	toker->attach(&in);
	toker->setComments(getComments);
	_pToker = toker;
	while (get())
	{
		std::cout << "\n  -- semiExpression --";
		printSemiExp();
	}
	if (length() > 0)
	{
		std::cout << "\n  -- semiExpression --";
		printSemiExp();
		std::cout << "\n\n";
	}	
	if (storeText) {
		pfile = freopen("CON", "w", stdout);
		std::cout << "Read output file from : \"" << output << "\".\n";
	}
	toker->reset();
	return true;
}

#ifdef TEST_SEMIEXPRESSION
int main()
{
	Toker toker;
	SemiExp semi;
	std::string fileSpec = "Req7_8.txt";
	semi.printAllExpression(&toker, fileSpec, "true",false);
	return 0;
}
#endif