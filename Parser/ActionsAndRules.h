#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 2.2                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Code Parser, Project 2                          //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse        //
//                 University, rmbhupta@syr.edu                    //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines several action classes.  Its classes provide 
  specialized services needed for specific applications.  The modules
  Parser, SemiExpression, and Tokenizer, are intended to be reusable
  without change.  This module provides a place to put extensions of
  these facilities and is not expected to be reusable. 

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
    parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
    - Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,AST.h,AST.cpp
      ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
      ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv CodeParser.sln /rebuild debug

  Maintenance History:
  ====================
  ver 2.2 : 7 Mar 16
  - Added rules for contrl statements, type statements and lambdas
  - Changed and Moved definition of element to AST.h file
  - Added singleton repository object
  -Added Abstract lSyntax Tree functionality
  ver 2.1 : 15 Feb 16
  - small functional change to a few of the actions changes display strategy
  - preface the (new) Toker and SemiExp with Scanner namespace
  ver 2.0 : 01 Jun 11
  - added processing on way to building strong code analyzer
  ver 1.1 : 17 Jan 09
  - changed to accept a pointer to interfaced ITokCollection instead
    of a SemiExpression
  ver 1.0 : 12 Jan 06
  - first release

*/
//
#include <queue>
#include <string>
#include <sstream>
#include "Parser.h"
#include "AST.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"

class Repository  // application specific
{
  ScopeStack<ASTNode*> stack;
  Scanner::Toker* p_Toker;
  static Repository * _instance;
  AST *_ast = new AST();
public:
  Repository(Scanner::Toker* pToker)
  {
	ASTNode* root = new ASTNode();
	root->setType("namespace");
	root->setName("GNS");
	root->setLineStart(0);
	root->setLineEnd(0);
	root->setLineCount(0);
    p_Toker = pToker;
	_ast->setRoot(root);
	stack.push(root);
	_instance = this;
  }
  //returns static instance of repository
  static Repository* getInstance()
  { 
	  return _instance; 
  }
  //returns pointer to abstract syntax tree
  AST*  getAST() 
  {
	  return _ast;
  }
  //returns referrence to scopestack
  ScopeStack<ASTNode*>& scopeStack()
  {
    return stack;
  }
  // returns pointer to toker
  Scanner::Toker* Toker()
  {
    return p_Toker;
  }
  //returns current line
  size_t lineCount()
  {
    return (size_t)(p_Toker->currentLineCount());
  }
};
///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule
{
public:
  bool doTest(ITokCollection*& pTc)
  {
    if(pTc->find("{") < pTc->length())  //checks if it is begining of scope
    {
      doActions(pTc);
      return true;
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction
{
  Repository* p_Repos;
public:
  HandlePush(Repository* pRepos)
  {
    p_Repos = pRepos;
  }
  void doAction(ITokCollection*& pTc)
  {
    ASTNode *elem = new ASTNode();
	elem->setType("unknown");
	elem->setName("anonymous");
	elem->setLineStart(p_Repos->lineCount());
	elem->setLineCount(p_Repos->lineCount());
	ASTNode *top = p_Repos->scopeStack().top();
	if (top->getType() == "delete")
		elem->setType("delete");
	top->addChild(elem);
    p_Repos->scopeStack().push(elem);  // push the unknown type node to tree and stack
  }
};

///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule
{
public:
  bool doTest(ITokCollection*& pTc)
  {
    if(pTc->find("}") < pTc->length())  //checks if it is end of scope
    {
      doActions(pTc);
      return true;
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePop : public IAction
{
  Repository* p_Repos;
public:
  HandlePop(Repository* pRepos)
  {
    p_Repos = pRepos;
  }
  void doAction(ITokCollection*& pTc)
  {
    if(p_Repos->scopeStack().size() == 0)
      return;
    ASTNode* elem = p_Repos->scopeStack().pop();
	elem->setLineEnd(p_Repos->lineCount());
	elem->setLineCount(elem->getLineEnd() - elem->getLineStart() + 1);
	elem->setCC(p_Repos->getAST()->getDescendantsSize(elem));
    if(elem->getType() == "function")
    {
      std::cout << "\n  Function " << elem->getName() << ", lines = " << elem->getLineCount() << " , startLine = "<< elem->getLineStart() << " , endLine = " << elem->getLineEnd();
	  std::cout << " , complexity = " << elem->getCC();
	  std::cout << "\n";
    }
	if (elem->getType() == "delete")  //handles intializers, ex : std::string a[2] = {"abc","xyz"};
		p_Repos->getAST()->deleteChild(elem);
  }
};

///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements

class PreprocStatement : public IRule
{
public:
  bool doTest(ITokCollection*& pTc)
  {
    if(pTc->find("#") < pTc->length())  //checks if it is preproc statement
    {
      doActions(pTc);
      return true;
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console

class PrintPreproc : public IAction
{
public:
  void doAction(ITokCollection*& pTc)
  {
    std::cout << "\n\n  Preproc Stmt: " << pTc->show().c_str();
  }
};

///////////////////////////////////////////////////////////////
// rule to detect function definitions

class FunctionDefinition : public IRule
{
public:
  //returns true if token is special keyword
  bool isSpecialKeyWord(const std::string& tok)
  {
    const static std::string keys[]
      = { "for", "while", "switch", "if", "catch", };
    for(int i=0; i<5; ++i)
      if(tok == keys[i])
        return true;
    return false;
  }
  bool doTest(ITokCollection*& pTc)
  {
    ITokCollection& tc = *pTc;
    if(tc[tc.length()-1] == "{") //check for begining of scope
    {
      size_t len = tc.find("(");  
      if(len < tc.length() && !isSpecialKeyWord(tc[len-1]))  //checks if it is a function
      {
        doActions(pTc);
        return true;
      }
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class PushFunction : public IAction
{
  Repository* p_Repos;
public:
  PushFunction(Repository* pRepos)
  {
    p_Repos = pRepos;
  }
  //checks if the function is lambda
  int isLambda(ITokCollection*& pTc) 
  {
	  for (size_t i = 0; i < pTc->length(); i++)
		  if ((*pTc)[i] == "operator")
			  return -1;
	  for (int i = pTc->length() - 1; i >= 0; i--)
		  if ((*pTc)[i] == "]")
			  if ((*pTc)[i+1] == "(")
				return 1;
	  return 0;
  }
  void doAction(ITokCollection*& pTc)
  {
	  ASTNode *node = p_Repos->scopeStack().top();
	  p_Repos->scopeStack().pop();           //pop anonymous scope
	  p_Repos->getAST()->deleteChild(node);  //delete anonymous scope from tree
	  std::string name = (*pTc)[pTc->find("(") - 1];
	  ASTNode* elem = new ASTNode();
	  int i = isLambda(pTc);
	  if (i == 1) {
		  elem->setType("lambda");
		  elem->setName("anonymous");
	  }
	  else {
		  elem->setType("function");
		  if (i != 0) {
			  name = [&name, &pTc]()->std::string {
				  size_t index = pTc->find("operator");
				  name = "";
				  while ((*pTc)[index] != "(")
					  name += (*pTc)[index++];
				  return name;
			  }();
		  }
		  elem->setName(name);
	  }
	elem->setLineStart(p_Repos->lineCount());
	p_Repos->scopeStack().top()->addChild(elem); //add child to top stack node
    p_Repos->scopeStack().push(elem); // push function scope
  }
};

///////////////////////////////////////////////////////////////
// rule to detect control definitions

class ControlDefinition : public IRule
{
public:
	//checks if the token is special key word
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch","try","else","do" };
		for (int i = 0; i<8; ++i)
			if (keys[i] == tok) //if (semi.find(keys[i]) != std::string::npos)
				return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{") //checks for begining of scope
		{
			size_t len = tc.find("(");
			for (size_t i = 0; i < tc.length(); i++) {
				if (isSpecialKeyWord(tc[i]))  //checks if it is control scope
				{
					doActions(pTc);
					return true;
				}
			}
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////
// action to push controls name onto ScopeStack

class PushControls : public IAction
{
	Repository* p_Repos;
public:
	PushControls(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	//returns name of the control type
	std::string getName(ITokCollection*& pTc) {
		size_t controlIndex = pTc->find("{");
		controlIndex--;
		while((*pTc)[controlIndex] == "\n" )
			controlIndex--;
		return (*pTc)[controlIndex];
	}
	void doAction(ITokCollection*& pTc)
	{
		ASTNode *node = p_Repos->scopeStack().pop(); // pop anonymous scope	
		p_Repos->getAST()->deleteChild(node); //delete anonymous scope from tree
		size_t controlIndex = pTc->find("(");
		std::string name  = controlIndex < pTc->length() ? (*pTc)[controlIndex - 1] : getName(pTc);
		ASTNode* elem = new ASTNode();
		elem->setType("control");
		elem->setName(name);
		elem->setLineStart(p_Repos->lineCount());
		p_Repos->scopeStack().top()->addChild(elem); // add child to top stack element
		p_Repos->scopeStack().push(elem); // push function scope
	}
};



///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console

class PrintFunction : public IAction
{
  Repository* p_Repos;
public:
  PrintFunction(Repository* pRepos) // constructor
  {
    p_Repos = pRepos;
  }
  //doAction method of this class
  void doAction(ITokCollection*& pTc)
  {
    std::cout << "\n  FuncDef: " << pTc->show().c_str();
  }
};

///////////////////////////////////////////////////////////////
// action to send signature of a function def to console

class PrettyPrintFunction : public IAction
{
public:
	//doAction method of this class
  void doAction(ITokCollection*& pTc)
  {
    pTc->remove("public");
    pTc->remove(":");
    pTc->trimFront();
    int len = pTc->find(")");
    std::cout << "\n\n  Pretty Stmt:    ";
    for(int i=0; i<len+1; ++i)
      std::cout << (*pTc)[i] << " ";
    std::cout << "\n";
  }
};


///////////////////////////////////////////////////////////////
// rule to detect class, struct , namespace

class TypeDefinition : public IRule {
public:                                   
	// returns true for types
	bool isType(const std::string& tok)
	{
		const static std::string keys[]
			= { "class", "struct", "namespace" };
		for (int i = 0; i<3; ++i)
			if (keys[i] == tok)
				return true;
		return false;
	}
	
	bool isModifier(const std::string& tok) // modifiers and initializers.
	{                                       // If you have two things left
		const size_t numKeys = 22;            // its declar else executable.
		const static std::string keys[numKeys] = {
			"const", "extern", "friend", "mutable", "signed", "static",
			"typedef", "typename", "unsigned", "volatile", "&", "*", "std", "::",
			"public", "protected", "private", ":", "typename", "typedef", "++", "--"
		};
		for (int i = 0; i<numKeys; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	//remove template types
	void condenseTemplateTypes(ITokCollection& tc)
	{
		size_t start = tc.find("<");
		size_t end = tc.find(">");
		if (start >= end || start == 0 || end > tc.length())
			return;
		else
		{
			if (end == tc.length())
				end = tc.find(">::");
			if (end == tc.length())
				return;
		}
		std::string save = tc[end];
		std::string tok = tc[start - 1];
		for (size_t i = start; i < end + 1; ++i)
			tok += tc[i];
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
		if (save == ">::")
		{
			tok += tc[start + 1];
			tc.remove(start);
		}
		tc[start - 1] = tok;
	}
	//returns true for special keywords
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i<5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	//removes invocation parens
	void removeInvocationParens(ITokCollection& tc)
	{
		size_t start = tc.find("(");
		size_t end = tc.find(")");
		if (start >= end || end == tc.length() || start == 0)
			return;
		if (isSpecialKeyWord(tc[start - 1]))
			return;
		for (size_t i = start; i < end + 1; ++i)
			tc.remove(start);
	}
	//cleans semiexpression and removes unneccessary tokens
	void cleanSemi(ITokCollection& tc, Scanner::SemiExp& se) {
		for (size_t i = 0; i < tc.length(); ++i)
		{
			if (isModifier(tc[i]))
				continue;
			if (se.isComment(tc[i]) || tc[i] == "\n" || tc[i] == "return")
				continue;
			if (tc[i] == "{")
				break;
			se.push_back(tc[i]);
		}
	}
	//test for the type definitions
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& in = *pTc;
		Scanner::SemiExp tc;
		for (size_t i = 0; i<in.length(); ++i)
			tc.push_back(in[i]);
		if (tc[tc.length() - 1] == "{" && tc.length() > 2)  // checks for begining of scope
		{
			// initializers.  So eliminate
			removeInvocationParens(tc);
			condenseTemplateTypes(tc);
			// remove modifiers, comments, newlines, returns, and initializers
			Scanner::SemiExp se;
			// cleans semi
			cleanSemi(tc,se);
			for (size_t i = 0; i < se.length(); i++) {
				if(isType(se[i]))  //checks if it class, struct or namespace
				{
					doActions(pTc);
					return true;
				}
			}
		}
		return true;
	}
};


///////////////////////////////////////////////////////////////
// action to push types name onto ScopeStack
class PushTypes : public IAction
{
	Repository* p_Repos;
public:
	PushTypes(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	//return name of type
	std::string isType(const std::string& tok)
	{
		const static std::string keys[]
			= { "class", "struct", "namespace" };
		for (int i = 0; i<3; ++i)
			if (keys[i] == tok)
				return tok;
		return "";
	}
	void doAction(ITokCollection*& pTc)
	{
		ASTNode *node = p_Repos->scopeStack().top(); // pop anonymous scope
		p_Repos->scopeStack().pop();
		p_Repos->getAST()->deleteChild(node); //delete anonymous scope from tree
		std::string type,name;
		for (size_t i = 0; i < pTc->length(); i++) {
			if (isType((*pTc)[i]) != "")
				type = isType((*pTc)[i]);
		}
		size_t type_i = pTc->find(type);
		name = (*pTc)[type_i + 1];
		ASTNode* elem = new ASTNode();
		elem->setType(type);
		elem->setName(name);
		elem->setLineStart(p_Repos->lineCount());
		p_Repos->scopeStack().top()->addChild(elem); //add child to top element of stack
		p_Repos->scopeStack().push(elem); //push node 
	}
};



///////////////////////////////////////////////////////////////
// rule to detect declaration

class Declaration : public IRule          // declar ends in semicolon
{                                         // has type, name, modifiers &
public:                                   // initializers.  So eliminate
  bool isModifier(const std::string& tok) // modifiers and initializers.
  {                                       // If you have two things left
    const size_t numKeys = 22;            // its declar else executable.
    const static std::string keys[numKeys] = {
      "const", "extern", "friend", "mutable", "signed", "static",
      "typedef", "typename", "unsigned", "volatile", "&", "*", "std", "::",
      "public", "protected", "private", ":", "typename", "typedef", "++", "--"
    };
    for (int i = 0; i<numKeys; ++i)
    if (tok == keys[i])
      return true;
    return false;
  }
  void condenseTemplateTypes(ITokCollection& tc)
  {
    size_t start = tc.find("<");
    size_t end = tc.find(">");
    if (start >= end || start == 0 || end > tc.length())
      return;
    else
    {
      if (end == tc.length())
        end = tc.find(">::");
      if (end == tc.length())
        return;
    }
    std::string save = tc[end];
    std::string tok = tc[start - 1];
    for (size_t i = start; i < end + 1; ++i)
      tok += tc[i];
    for (size_t i = start; i < end + 1; ++i)
      tc.remove(start);
    if (save == ">::")
    {
      tok += tc[start + 1];
      tc.remove(start);
    }
    tc[start - 1] = tok;
  }
  //checks if it is special keyword
  bool isSpecialKeyWord(const std::string& tok)
  {
    const static std::string keys[]
      = { "for", "while", "switch", "if", "catch" };
    for (int i = 0; i<5; ++i)
    if (tok == keys[i])
      return true;
    return false;
  }
  //removes invocation patterns
  void removeInvocationParens(ITokCollection& tc)
  {
    size_t start = tc.find("(");
    size_t end = tc.find(")");
    if (start >= end || end == tc.length() || start == 0)
      return;
    if (isSpecialKeyWord(tc[start - 1]))
      return;
    for (size_t i = start; i < end + 1; ++i)
      tc.remove(start);
    //std::cout << "\n  -- " << tc.show();
  }
  //checks if it is intializers
  bool isInitializer(ITokCollection*& pTc) {
	  size_t controlIndex = pTc->find("{");
	  if (controlIndex > pTc->length())
		  return false;
	  controlIndex--;
	  if (controlIndex < 0 || controlIndex >= pTc->length())
		  return false;
	  while ((controlIndex > 0 || controlIndex < pTc->length()) && (*pTc)[controlIndex] == "\n")
		  controlIndex--;
	  if (controlIndex>0 && (*pTc)[controlIndex] == "=")
		return true;
	  return false;
  }
  //cleans semiexpression and removes unneccessary tokens
  bool cleanSemi(ITokCollection& tc, Scanner::SemiExp& se) {
	  bool flag = false;
	  for (size_t i = 0; i < tc.length(); ++i)
	  {
		  if (isModifier(tc[i]))
			  continue;
		  if (se.isComment(tc[i]) || tc[i] == "\n" || tc[i] == "return")
			  continue;
		  if (tc[i] == "=" || tc[i] == ";") {
			  flag = true;
			  break;
		  }
		  else
			  se.push_back(tc[i]);
	  }
	  return flag;
  }
  bool doTest(ITokCollection*& pTc)
  {
    ITokCollection& in = *pTc;
    Scanner::SemiExp tc;
    for (size_t i = 0; i<in.length(); ++i)
      tc.push_back(in[i]);
    if ( (tc[tc.length() - 1] == ";" && tc.length() > 2) || isInitializer(pTc)) //checks for declarations
    {
      removeInvocationParens(tc);
      condenseTemplateTypes(tc);
      // remove modifiers, comments, newlines, returns, and initializers
      Scanner::SemiExp se;
	  //clean semi
	  bool flag = cleanSemi(tc,se);
      if(flag && se.length() >=2) //if it is declaration
	  {
        doActions(pTc);
        return true;
      }
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////
// action to show declaration

class ShowDeclaration : public IAction
{
	Repository* p_Repos;
public:
	ShowDeclaration(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
  void doAction(ITokCollection*& pTc)
  {
    ITokCollection& tc = *pTc;
    // remove comments
	if (pTc->find("{") ==  pTc->length() - 1) //to delete the anonymous scope created by intializer
	{
		ASTNode *node = p_Repos->scopeStack().top();
		node->setType("delete");
	}
    Scanner::SemiExp se;
    for (size_t i = 0; i<tc.length(); ++i) // cleans semiexo
      if (!se.isComment(tc[i]))
        se.push_back(tc[i]); 
    std::cout << "\n  Declaration: " << se.show();
  }
};

///////////////////////////////////////////////////////////////
// rule to detect executable

class Executable : public IRule           // declar ends in semicolon
{                                         // has type, name, modifiers &
public:                                   // initializers.  So eliminate
  bool isModifier(const std::string& tok) // modifiers and initializers.
  {                                       // If you have two things left
    const size_t numKeys = 22;            // its declar else executable.
    const static std::string keys[numKeys] = {
      "const", "extern", "friend", "mutable", "signed", "static",
      "typedef", "typename", "unsigned", "volatile", "&", "*", "std", "::",
      "public", "protected", "private", ":", "typename", "typedef", "++", "--"
    };
    for (int i = 0; i<numKeys; ++i)
		if (tok == keys[i])
			return true;
    return false;
  }
  void condenseTemplateTypes(ITokCollection& tc)
  {
    size_t start = tc.find("<");
    size_t end = tc.find(">");
    if (start >= end || start == 0 || end > tc.length())
      return;
    else
    {
      if (end == tc.length())
        end = tc.find(">::");
      if (end == tc.length())
        return;
    }
    std::string save = tc[end];
    std::string tok = tc[start - 1];
    for (size_t i = start; i < end+1; ++i)
      tok += tc[i];
    for (size_t i = start; i < end+1; ++i)
      tc.remove(start);
    if (save == ">::")
    {
      tok += tc[start + 1];
      tc.remove(start);
    }
    tc[start - 1] = tok;
    //std::cout << "\n  -- " << tc.show();
  }
  
  bool isSpecialKeyWord(const std::string& tok)
  {
    const static std::string keys[]
      = { "for", "while", "switch", "if", "catch" };
    for (int i = 0; i<5; ++i)
    if (tok == keys[i])
      return true;
    return false;
  }
  void removeInvocationParens(ITokCollection& tc)
  {
    size_t start = tc.find("(");
    size_t end = tc.find(")");
    if (start >= end || end == tc.length() || start == 0)
      return;
    if (isSpecialKeyWord(tc[start - 1]))
      return;
    for (size_t i = start; i < end + 1; ++i)
      tc.remove(start);
  }
  //cleans semiexpression and removes unneccessary tokens
  void cleanSemi(ITokCollection& tc, Scanner::SemiExp& se) {
	  for (size_t i = 0; i < tc.length(); ++i)
	  {
		  if (isModifier(tc[i]))
			  continue;
		  if (se.isComment(tc[i]) || tc[i] == "\n" || tc[i] == "return")
			  continue;
		  if (tc[i] == "=" || tc[i] == ";")
			  break;
		  else
			  se.push_back(tc[i]);
	  }
  }
  bool doTest(ITokCollection*& pTc)
  {
    ITokCollection& in = *pTc;
    Scanner::SemiExp tc;
    for (size_t i = 0; i<in.length(); ++i)
      tc.push_back(in[i]);

    if (tc[tc.length() - 1] == ";" && tc.length() > 2)
    {
      removeInvocationParens(tc);
      condenseTemplateTypes(tc);
      // remove modifiers, comments, newlines, returns, and initializers
      Scanner::SemiExp se;
      // cleans semi expression
	  cleanSemi(tc, se);
      if (se.length() != 2)  // not a declaration
      {
        doActions(pTc);
        return true;
      }
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////
// action to show executable

class ShowExecutable : public IAction
{
public:
  void doAction(ITokCollection*& pTc)
  {
    ITokCollection& tc = *pTc;
    Scanner::SemiExp se;
    for (size_t i = 0; i < tc.length(); ++i)
    {
      if (!se.isComment(tc[i]))
        se.push_back(tc[i]);
    }
    std::cout << "\n  Executable: " << se.show();
  }
};

#endif
