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
    /// Parses std::string with XML content
    /// \param input XML string
    /// \return DOM Document node
    DOM::Document parse(const std::string &input);

    /// Static function to parse XML
    /// \param str XML string
    /// \return DOM Document node
    static DOM::Document from_string(const std::string &str);
private:
    DOM::Element *parse_element();
    DOM::Comment *parse_comment();

    void advance();
    /// Advance to next token, if token is not expected_type throw exception
    /// \param expected_type Expected token type
    void advance(Token::Type expected_type);

    /// Check whether parser has reached end of file
    /// \return True if eof is reached
    bool eof();

    std::unique_ptr<Lexer> lexer;
    Token curr_token;
    Token peek_token;
};

} // namespace XML

/// Custom string literal to ease xml parsing. Example: "<test></test>"_xml
/// \return DOM Document node
XML::DOM::Document operator "" _xml(const char* str);
/// Custom string literal to ease xml parsing. Example: "<test></test>"_xml
/// \return DOM Document node
XML::DOM::Document operator "" _xml(const char* str, size_t size);
#endif //XML_PARSER_HPP
