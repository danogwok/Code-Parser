///////////////////////////////////////////////////////////////////////////////
// ScopeStack.cpp - implements template stack holding specified element type //
// ver 2.0                                                                   //
// Language:      Visual C++ 2010, SP1                                       //
// Platform:      Dell Precision T7400, Win 7 Pro SP1                        //
// Application:   Code Parser, Project 2                                     //
// Author:        Jim Fawcett, CST 4-187, Syracuse University                //
//                (315) 443-3948, jfawcett@twcny.rr.com                      //
///////////////////////////////////////////////////////////////////////////////

//----< test stub >------------------------------------------------------



#include "ScopeStack.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

struct element
{
  std::string type;
  std::string name;
  size_t startLineCount = 0;
  size_t endLineCount = 0;
  std::string show()
  {
    std::ostringstream temp;
    temp << "(";
    temp << type;
    temp << ", ";
    temp << name;
    temp << ", ";
    temp << startLineCount;
    temp << ", ";
    temp << endLineCount;
    temp << ")"; 
    return temp.str();
  }
  std::vector<element*> _children;
};

void TreeWalk(element* pItem)
{
  static size_t indentLevel = 0;
  std::cout << "\n  " << std::string(2*indentLevel, ' ') << pItem->show();
  auto iter = pItem->_children.begin();
  ++indentLevel;
  while (iter != pItem->_children.end())
  {
    TreeWalk(*iter);
    ++iter;
  }
  --indentLevel;
}
#ifdef TEST_SCOPESTACK
int main()
{
  std::cout << "\n  Testing Scope Stack";
  std::cout << "\n =====================\n";

  std::cout << "\n  pushing items onto ScopeStack instance after adding startLineCount";
  std::cout << "\n --------------------------------------------------------------------";

  ScopeStack<element*> testStack;
  element* pItem = new element;

  pItem->type = "function";
  pItem->name = "fun1";
  pItem->startLineCount = 33;
  testStack.push(pItem);

  pItem = new element;
  pItem->type = "if";
  pItem->name = "";
  pItem->startLineCount = 38;
  (testStack.top()->_children).push_back(pItem);
  testStack.push(pItem);

  pItem = new element;
  pItem->type = "for";
  pItem->name = "";
  pItem->startLineCount = 40;
  (testStack.top()->_children).push_back(pItem);
  testStack.push(pItem);


  showStack<element>(testStack, false);
  std::cout << std::endl;

  std::cout << "\n  Popping two items off ScopeStack after adding endLineCount";
  std::cout << "\n ------------------------------------------------------------";

  testStack.top()->endLineCount = 50;
  std::cout << "\n  " << testStack.pop()->show();
  testStack.top()->endLineCount = 55;
  std::cout << "\n  " << testStack.pop()->show();
  std::cout << "\n";

  std::cout << "\n  Pushing another item onto ScopeStack after adding startLineElement";
  std::cout << "\n --------------------------------------------------------------------";
  pItem = new element;
  pItem->type = "while";
  pItem->name = "";
  pItem->startLineCount = 60;
  (testStack.top()->_children).push_back(pItem);
  testStack.push(pItem);

  std::cout << "\n  Stack now contains:";
  std::cout << "\n ---------------------";
  showStack<element>(testStack, false);
  std::cout << "\n";

  std::cout << "\n  Popping last elements off stack after adding endLineCount";
  std::cout << "\n -----------------------------------------------------------";
  testStack.top()->endLineCount = 70;
  element* pTop = testStack.pop();
  std::cout << "\n  " << pTop->show();
  testStack.top()->endLineCount = 75;
  pTop = testStack.pop();
  std::cout << "\n  " << pTop->show();
  std::cout << "\n";

  std::cout << "\n  Walking simulated AST tree:";
  std::cout << "\n -----------------------------";
  TreeWalk(pTop);
  std::cout << "\n\n";
}

#endif
