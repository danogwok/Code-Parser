#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally usefule, helper classes            //
// ver 1.1                                                           //
// Language:    C++, Visual Studio 2015                              //
//  Application:   Code Parser, Project 2                            //
//  Author:		   Ronak Bhuptani, SUID#429019830, Syracuse          //
//                 University, rmbhupta@syr.edu                      //
//  Source:        Jim Fawcett, CST 4-187, Syracuse University       //
//                 (315) 443-3948, jfawcett@twcny.rr.com             //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes StringHelper and Converter and a global
* function putline().  This class will be extended continuously for 
* awhile to provide convenience functions for general C++ applications.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
*
* Build Command: devenv CodeParser.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.2 : 7 Mar 2016
* - added getTab and changeExtenssion functions
* ver 1.1 : 06 Feb 2015
* - fixed bug in split which turns a comma separated string into
*   a vector of tokens.
* - added comments
* ver 1.0 : 05 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include <string>
#include <vector>
#include <sstream>
#include <functional>

namespace Utilities
{
  class StringHelper
  {
  public:
    static std::vector<std::string> split(const std::string& src);
    static void Title(const std::string& src, char underline = '-');
    static void title(const std::string& src);
	static std::string getTab(const size_t n);
	static std::string changeExtenssion(std::string input, std::string newExt);
	static void getDirPattern(int argc, char *argv[], std::string& dir, std::string& pattern,std::string& stopFlag);
  };

  void putline();

  template <typename T>
  class Converter
  {
  public:
    static std::string toString(const T& t);
    static T toValue(const std::string& src);
  };

  template <typename T>
  std::string Converter<T>::toString(const T& t)
  {
    std::ostringstream out;
    out << t;
    return out.str();
  }

  template<typename T>
  T Converter<T>::toValue(const std::string& src)
  {
    std::istringstream in(src);
    T t;
    in >> t;
    return t;
  }
}
#endif
