#ifndef AST_H
#define AST_H
/////////////////////////////////////////////////////////////////////
//  AST.h - declares Abstract Syntax Tree and its node             //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Application:   Code Parser, Project 2                          //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse        //
//                 University, rmbhupta@syr.edu                    //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines Abstract syntax tree class and its node. It has functions
to access its root, displayAST and count complexity of any node.

Build Process:
==============
Required files
- AST.h, AST.cpp
- devenv CodeParser.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 7 Mar 16
- Added ASTNode class which has geter/seter functions and addChild function
- Added AST class which will be used by repository 
- Added methods to access root of tree, count comlexity of any node
- Added methods to display tree
- Added methods to get all functions node

*/
#include <string>
#include <vector>
#include <sstream>
#include <set>
class ASTNode
{
public:
	ASTNode() {  //constructor
		_children = new std::vector<ASTNode*>();
	}
	~ASTNode() {} //destructor
	//functions : details in implementation file
	std::string show();
	void setType(std::string type);
	std::string getType();
	void setName(std::string name);
	std::string getName();
	void setLineCount(size_t linecount);
	size_t getLineCount();
	void setLineStart(size_t linestart);
	size_t getLineStart();
	void setLineEnd(size_t lineend);
	size_t getLineEnd();
	void setCC(size_t cc);
	size_t getCC();
	void addChild(ASTNode* node);
	void addChildren(std::vector<ASTNode*> *children);
	std::vector<ASTNode*>* getChildren();
	
protected:
	std::string _type;
	std::string _name;
	size_t _lineCount;
	size_t _lineStart;
	size_t _lineEnd;
	size_t _cc;
	std::vector<ASTNode*> *_children;
};

class AST {
public:
	AST() //constructor
	{ 
		if (_root == nullptr)
			_root = new ASTNode();
	}
	~AST() { //destructor
	}
	//functions : details in implementation file
	void setRoot(ASTNode *root);
	ASTNode* getRoot();
	void deleteChild(ASTNode *node);
	void AST::deleteChild(ASTNode * node, ASTNode *deleteNode);
	void showAST();
	void showASTHelp(ASTNode* node, size_t level);
	size_t getDescendantsSize(ASTNode* node);
	std::vector<ASTNode> getAllFunctions();
	void getAllFunctionsHelper(ASTNode *root, std::vector<ASTNode> &functionNodes);
protected:
	ASTNode *_root;
};

#endif