//
// Created by cyborg on 12/8/17.
//

#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include <memory>
#include "Lexer.hpp"
#include "DOM.hpp"
#include "Errors.hpp"

namespace XML
{

class Parser
{
public:
    DOM::Document *parse(const std::string &input);

    static DOM::Document *from_string(const std::string &str);
private:
    DOM::Element *parse_element();
    DOM::Comment *parse_comment();

    void advance();
    void advance(Token::Type expected_type);

    bool eof();

    std::unique_ptr<Lexer> lexer;
    Token curr_token;
    Token peek_token;
};

} // namespace XML

XML::DOM::Document* operator "" _xml(const char* str);
XML::DOM::Document* operator "" _xml(const char* str, size_t size);
#endif //XML_PARSER_HPP
