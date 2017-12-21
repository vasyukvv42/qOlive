//
// Created by cyborg on 11/10/17.
//

#include "Token.hpp"

namespace XML
{

Token::Token(Type type, const std::string &value) : type(type), value(value) {}

std::string Token::type_name(Type type)
{
    switch (type) {
        case Type::TAG_BEGIN:
            return "TAG_BEGIN";
        case Type::TAG_END:
            return "TAG_END";
        case Type::TAG_CLOSE:
            return "TAG_CLOSE";
        case Type::TAG_END_AND_CLOSE:
            return "TAG_END_AND_CLOSE";
        case Type::ATTRIBUTE_NAME:
            return "ATTRIBUTE_NAME";
        case Type::EQUAL_SIGN:
            return "EQUAL_SIGN";
        case Type::ATTRIBUTE_VALUE:
            return "ATTRIBUTE_VALUE";
        case Type::CONTENT:
            return "CONTENT";
        case Type::COMMENT_BEGIN:
            return "COMMENT_BEGIN";
        case Type::COMMENT:
            return "COMMENT";
        case Type::COMMENT_END:
            return "COMMENT_END";
        case Type::CDATA_BEGIN:
            return "CDATA_BEGIN";
        case Type::CDATA:
            return "CDATA";
        case Type::CDATA_END:
            return "CDATA_END";
        case Type::PI:
            return "PROCESSING_INSTRUCTION";
        case Type::INVALID:
            return "INVALID";
        case Type::END_OF_FILE:
            return "END_OF_FILE";
    }
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
    os << '"' << token.name() << "\": \"" << token.value << '"';
    return os;
}

std::string Token::name() const
{
    return type_name(type);
}

} // namespace XML