#include "Test.h"
namespace testNamespace{
	
	class Test{
		public: 
			void testFun(int a,std::string b){
				if(a>0){
					for(int i=0;i<a;i++){
						std::cout<<b[i]<<" ";
					}
				}
			}
			//this function is syntactically/logically wrong but it is here for testing purpose only.
			std::string testLambda(){
				if (i != 0) {
				  name = [&name, &pTc]()->std::string {
					  size_t index = pTc->find("operator");
					  name = "";
					  while ((*pTc)[index] != "(")
						  name += (*pTc)[index++];
					  return name;
				  }();
				}
			}
			//this function is syntactically/logically wrong but it is here for testing purpose only.
			Token& SemiExp::operator[](size_t n)
			{
				if (n < 0 || n >= _tokens.size()){
					throw(std::invalid_argument("index out of range"));
				}
				return _tokens[n];
			}
		private:
			static std::map < std::string, int> specialChars;
	};
	std::map < std::string, int> Test::specialChars = { { "[",1 } ,{ "]",1 },{ "{",1 },{ "}",1 },{ "(",1 },{ ")",1 },{ "<",1 },{ "+",1 },{ "=",1 },{ "-",1 },{ ">",1 },{ ":",1 },{ ";",1 },{ "\n",1 },{ "<<",2 } ,{ ">>",2 } ,{ "+=",2 } ,{ "==",2 },{ "*",1 }, { "-=",2 } ,{ "*=",2 } ,{ "/=",2 },{ "::",2 },{ "\\n",2 } ,{ "/",1 } ,{ "++",2 },{ "--",2 } };  ;
}