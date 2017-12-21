//
// Created by cyborg on 11/10/17.
//

#ifndef XML_LEXER_HPP
#define XML_LEXER_HPP

#include "Token.hpp"

namespace XML
{

class Lexer
{
public:
    explicit Lexer(const std::string& input = "");
    Token next_token();
    bool eof();

    enum class Mode {
        CONTENT,
        TAG,
        CDATA,
        COMMENT
    };
private:
    void advance();
    char peek();
    void consume_whitespace();
    std::string read_special_tag();
    std::string read_name();
    std::string read_until(char c);
    std::string read_until(const std::string& substr);
    bool is_letter(char c);
    bool is_digit(char c);

    Token content_mode();
    Token tag_mode();
    Token cdata_mode();
    Token comment_mode();

    std::string input;
    size_t offset;
    size_t read_offset;
    char ch;
    Mode mode{Mode::CONTENT};
};

} // namespace XML
#endif //XML_LEXER_HPP
