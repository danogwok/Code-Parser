# Code-Parser
A lexical scanner and parser in C++ for Code analyzer which will extract lexical content from source code files, analyze the code’s syntax from its lexical content, and build the Abstract Syntax Tree that holds the results of analysis. It will be then used to build backend that can do further analysis on the AST to construct code metrics.

To run,
 - execute compile.bat from Developer Command Prompt 
 - execute run.bat from Developer Command Prompt 

run.bat format : 
  - .\Debug\MetricExecutive.exe /d ./Inputs/ /p *.cpp,*.h /s true
  - /d : which ever package you want ot analyze
  - /p : comma separated files ext
  - /s : if you want to stop after each moudule is analyzed
