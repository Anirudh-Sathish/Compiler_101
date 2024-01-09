// main file for the compiler
#include<iostream>
#include "lexer/lexer.cpp"
int main(int argc, char* argv[])
{
    if(argc!=2)
    {
        std::cout<<"Wrong Usage , Expected usage "<<argv[0] << "<filename>" <<std::endl;
    }
    const char* filename = argv[1];
    Lexer lexer(filename);
    std::vector<std::pair<int, std::string>> content = lexer.getFileContent();
    for(auto &line: content)
    {
        std::cout<<line.first<<" "<<line.second<<std::endl;
    }
    std::vector<Token> tokens = lexer.tokenize(content);
    std::string equalsLine(40, '=');
    for(auto &token: tokens)
    {
        std::cout<<"Type : "<<token.type<<"\nValue : "<<token.value<<"\nPosition : "<<token.position<<std::endl;
        std::cout<<equalsLine<<std::endl;
    }

}
