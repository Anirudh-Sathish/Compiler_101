#include<stdio.h>
#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<regex>
#include "../lexer/token.h"


class Lexer
{
    private:
        const char* filename;
    public:
        Lexer(const char* filename)
        {
            this->filename = filename;
        }
        std::vector<std::pair<int, std::string>> getFileContent()
        {
            std::vector<std::pair<int, std::string>>line_content;
            try
            {
                std::ifstream file(filename);
                std::string line, content;
                std::cout<<"Initalising the file reader"<<std::endl;
                int i = 0;
                while (std::getline(file, line)) {
                    line_content.push_back({i+1,line});
                    i++;
                }
                file.close();
                return line_content;
            }
            catch(const std::exception& e)
            {
                std::cerr << "Error opening file "<<e.what() << '\n';
                throw;
            }
        }
        std::vector<Token> tokenize(std::string content)
        {
            std::vector<Token> tokens;
            std::vector<std::pair<std::string , std::string>> patterns ={
                {"DATA TYPE","(int|float)"},
                {"IDENTIFIER","[a-zA-Z][a-zA-Z0-9]"},
                {"NUMBER","\\d+"},
                {"OPERATOR", "[+\\-*/=]"}
            };
            std::string combinedPattern;
            for(auto &pattern : patterns)
            {
                combinedPattern += "("+pattern.second+")|";
            }
            combinedPattern.pop_back();
            std::regex token_regex(combinedPattern);
            auto words_begin = std::sregex_iterator(content.begin(),content.end(),token_regex);
            auto words_end = std::sregex_iterator();
            for (std:: sregex_iterator it = words_begin; it != words_end ; ++it)
            {
                for (size_t i = 1; i < it->size(); ++i)
                {
                    std::cout<<it->str(i)<<" ";
                    if (!it->str(i).empty())
                    {
                        std::cout<<patterns[i-1].first<<std::endl;
                        tokens.push_back({patterns[i - 1].first, it->str(i), it->position(i)});
                    }
                }
            }
            return tokens;

        }

};
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

}