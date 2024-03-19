#include<stdio.h>
#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<regex>
#include<sstream>
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
        std::vector<Token> tokenize(std::vector<std::pair<int, std::string>> content)
        {
            std::vector<Token> tokens;
            int token_position = 0;
            std::vector<std::pair<std::string , std::string>> patterns ={
                {"DATA TYPE","(int|float)"},
                {"IDENTIFIER","[a-zA-Z][a-zA-Z0-9]*"},
                {"NUMBER","\\d+"},
                {"OPERATOR", "[+\\-*/=]"}
            };
            for(auto &line: content)
            {
                std::stringstream ss(line.second);
                std::vector<std::string> words;
                std::string word;
                while (std::getline(ss, word, ' ') || std::getline(ss, word, '\n')) {
                    words.push_back(word);
                    for(const auto &pattern: patterns)
                    {
                        std::regex regexPattern(pattern.second);
                        if(std::regex_search(word,regexPattern))
                        {
                            std::cout<<word<<" "<<pattern.first<<std::endl;
                            Token new_token = {pattern.first,word,token_position+1};
                            token_position++;
                            tokens.push_back(new_token);
                            break;
                        }
                    }
                }
            }
            return tokens;

        }

};
