//
// Created by cyborg on 12/8/17.
//

#include <iostream>
#include "Parser.hpp"

namespace XML
{

void Parser::advance()
{
    curr_token = peek_token;
    peek_token = lexer->next_token();
}

void Parser::advance(Token::Type expected_type)
{
    if (peek_token.type == expected_type)
        advance();
    else
        throw SyntaxError("Expected type " + Token::type_name(expected_type) + ", got " + peek_token.name());
}

DOM::Document* Parser::parse(const std::string &input)
{
    lexer = std::make_unique<Lexer>(input);
    advance();
    advance();

    auto document = new DOM::Document();

    if (curr_token.type == Token::Type::PI) {
        document->xml_prolog = curr_token.value;
        advance();
    }

    auto root_element = parse_element();
    document->append_child(root_element);
    document->root_element = root_element;

    return document;
}

DOM::Element *Parser::parse_element()
{
    if (curr_token.type != Token::Type::TAG_BEGIN)
        throw SyntaxError("Input has no root element");

    auto elem = new DOM::Element(curr_token.value.substr(1));

    while (curr_token.type != Token::Type::END_OF_FILE) {
        if (peek_token.type == Token::Type::TAG_END) {
            advance();
            break;
        } else if (peek_token.type == Token::Type::TAG_END_AND_CLOSE) {
            advance();
            return elem;
        }

        advance(Token::Type::ATTRIBUTE_NAME);
        auto attr_name = curr_token.value;
        if (elem->has_attribute(attr_name))
            throw SyntaxError("Element " + elem->name + " has repeated attribute " + attr_name);

        advance(Token::Type::EQUAL_SIGN);

        advance(Token::Type::ATTRIBUTE_VALUE);
        auto attr_value = curr_token.value;

        elem->set_attribute(attr_name, attr_value);
    }

    while (curr_token.type != Token::Type::END_OF_FILE) {
        advance();

        switch (curr_token.type) {
            case Token::Type::TAG_CLOSE: {
                if (curr_token.value.substr(2, curr_token.value.size() - 3) != elem->name)
                    throw SyntaxError("Unexpected tag close");
                return elem;
            }
            case Token::Type::CONTENT: {
                elem->append_child(new DOM::Text(curr_token.value));
                break;
            }
            case Token::Type::TAG_BEGIN: {
                elem->append_child(parse_element());
                break;
            }
            case Token::Type::CDATA_BEGIN: {
                advance(Token::Type::CDATA);
                elem->append_child(new DOM::CDATASection(curr_token.value));
                advance(Token::Type::CDATA_END);
                break;
            }
            case Token::Type::COMMENT_BEGIN: {
                advance(Token::Type::COMMENT);
                elem->append_child(new DOM::Comment(curr_token.value));
                advance(Token::Type::COMMENT_END);
                break;
            }
            default:
                throw SyntaxError("Unexpected token: " + curr_token.name());
        }
    }
}

DOM::Document *Parser::from_string(const std::string &str)
{
    XML::Parser p;
    return p.parse(str);
}

} // namespace XML

XML::DOM::Document* operator "" _xml(const char* str)
{
    return XML::Parser::from_string(str);
}

XML::DOM::Document* operator "" _xml(const char* str, size_t size)
{
    return XML::Parser::from_string(str);
}
