/////////////////////////////////////////////////////////////////////
//  AST.cpp - implements Abstract Syntax Tree and its node         //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Application:   Code Parser, Project 2                          //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse        //
//                 University, rmbhupta@syr.edu                    //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "AST.h"
#include "../Utilities/Utilities.h"
#include <vector>
#include <set>
using SH = Utilities::StringHelper;

//this method shows Node in (type,name,startline,endline,linecount,complexity) format
std::string ASTNode::show()
{
	std::ostringstream temp;
	temp << "(";
	temp << _type;
	temp << ", ";
	temp << _name;
	temp << ", ";
	temp << _lineStart;
	temp << ", ";
	temp << _lineEnd;
	temp << ", ";
	temp << _lineCount;
	temp << ", ";
	temp << _cc;
	temp << ")";
	return temp.str();
}

//this method sets type of node
void ASTNode::setType(std::string type)
{
	_type = type;
}
//this method gets type of node
std::string ASTNode::getType()
{
	return _type;
}
//this method sets name of node
void ASTNode::setName(std::string name)
{
	_name = name;
}
//this method gets name of node
std::string ASTNode::getName()
{
	return _name;
}
//this method sets linecount of node
void ASTNode::setLineCount(size_t linecount)
{
	_lineCount = linecount;
}
//this method gets linecount of node
size_t ASTNode::getLineCount()
{
	return _lineCount;
}
//this method sets startline of node
void ASTNode::setLineStart(size_t linestart)
{
	_lineStart = linestart;
}
//this method gets linestart of node
size_t ASTNode::getLineStart()
{
	return _lineStart;
}
//this method sets lineend of node
void ASTNode::setLineEnd(size_t lineend)
{
	_lineEnd = lineend;
}
//this method gets lineend of node
size_t ASTNode::getLineEnd()
{
	return _lineEnd;
}
//this method sets complexity of node
void ASTNode::setCC(size_t cc)
{
	_cc = cc;
}
//this method gets complexity of node
size_t ASTNode::getCC()
{
	return _cc;
}
//this method add child to node
void ASTNode::addChild(ASTNode* node)
{
	_children->push_back(node);
}
//this method add childrent to node
void ASTNode::addChildren(std::vector<ASTNode*> *children)
{
	if (children->size() <= 0)
		return;
	std::vector<ASTNode*>::const_iterator cit;
	for (cit = children->cbegin(); cit != children->cend(); cit++)
		_children->push_back(*cit);
}
//this method gets children of node
std::vector<ASTNode*>* ASTNode::getChildren()
{
	return _children;
}
//this method sets root of tree
void AST::setRoot(ASTNode* root)
{
	_root = root;
}
//this method gets root of tree
ASTNode* AST::getRoot()
{
	return _root;
}
//this method delete child node 
void AST::deleteChild(ASTNode * node)
{
	deleteChild(getRoot(), node);
}
// this is helper function to delete child node
void AST::deleteChild(ASTNode * node, ASTNode *deleteNode)
{
	if (node == nullptr)
		return;
	std::vector<ASTNode*>::iterator it;
	std::vector<ASTNode*> *children = node->getChildren();
	for (it = children->begin(); it != children->end(); )
	{
		if ((*it) == deleteNode)
		{
			delete * it;
			it = children->erase(it);
		}
		else 
		{
			deleteChild((*it), deleteNode);
			++it;
		}
	}			
}
//this function returns vector of all function nodes
std::vector<ASTNode> AST::getAllFunctions()
{
	std::vector<ASTNode> functionNodes;
	getAllFunctionsHelper(getRoot(), functionNodes);
	std::vector<ASTNode> v(functionNodes.begin(), functionNodes.end());
	return v;
}
//helper function to get all function nodes
void AST::getAllFunctionsHelper(ASTNode *node, std::vector<ASTNode> &functionNodes)
{
	if (node != nullptr && node->getType() == "function")
		functionNodes.push_back(*node);
	std::vector<ASTNode*>::iterator cit;
	if (node != nullptr)
		for (cit = node->getChildren()->begin(); cit != node->getChildren()->end(); cit++)
			getAllFunctionsHelper(*cit, functionNodes);
}
//this functions diplay AST
void AST::showAST()
{
	showASTHelp(_root, 0);
}
//helper function to show AST
void AST::showASTHelp(ASTNode* node, size_t level)
{
	if (node != nullptr)
		std::cout << SH::getTab(level) << node->show() << std::endl;
	std::vector<ASTNode*>::const_iterator cit;
	if (node != nullptr)
		for (cit = node->getChildren()->cbegin(); cit != node->getChildren()->cend(); cit++)
			showASTHelp(*cit, level + 1);
}
//this function counts comlexity of any node
size_t AST::getDescendantsSize(ASTNode* node)
{
	size_t count = 1;
	std::vector<ASTNode*>::const_iterator cit;
	if (node != nullptr)
		for (cit = node->getChildren()->cbegin(); cit != node->getChildren()->cend(); cit++)
			count += getDescendantsSize(*cit);
	return count;	
}


#ifdef TEST_AST

int main() {
	ASTNode *root = new ASTNode;
	root->setType("namespace");
	root->setName("Global");
	root->setLineStart(0);
	root->setLineEnd(30);
	root->setLineCount(31);

	ASTNode *elem = new ASTNode;
	elem->setType("function");
	elem->setName("test1");
	elem->setLineStart(5);
	elem->setLineEnd(12);
	elem->setLineCount(8);

	ASTNode *elem1 = new ASTNode;
	elem1->setType("function");
	elem1->setName("test2");
	elem1->setLineStart(15);
	elem1->setLineEnd(27);
	elem1->setLineCount(13);

	ASTNode *elem2 = new ASTNode;
	elem2->setType("control");
	elem2->setName("if");
	elem2->setLineStart(6);
	elem2->setLineEnd(8);
	elem2->setLineCount(3);

	AST *tree = new AST;
	tree->setRoot(root);
	root->addChild(elem);
	root->addChild(elem1);
	elem->addChild(elem2);
	root->setCC(tree->getDescendantsSize(root));
	elem->setCC(tree->getDescendantsSize(elem));
	elem1->setCC(tree->getDescendantsSize(elem1));
	elem2->setCC(tree->getDescendantsSize(elem2));

	tree->showAST();

	return 0;
}

#endif