//
// Created by cyborg on 11/10/17.
//

#include <iostream>
#include "Lexer.hpp"


namespace XML
{


Lexer::Lexer(const std::string &input) : input(input), ch(), offset(), read_offset()
{
    advance();
}

void Lexer::advance()
{
    if (read_offset >= input.length())
        ch = 0;
    else
        ch = input[read_offset];

    offset = read_offset;
    read_offset++;
}

char Lexer::peek()
{
    if (read_offset >= input.length())
        return 0;
    return input[read_offset];
}

bool Lexer::eof()
{
    return ch == 0;
}

void Lexer::validate_name(const std::string &name)
{
    if (name.empty())
        throw SyntaxError("Empty tag name");

    Lexer l(name);
    l.advance();
    auto &ch = l.ch;
    if (not l.is_letter(ch) or l.is_digit(ch) or ch == '-' or ch == '.')
        throw SyntaxError("Invalid tag name");
    l.advance();
    while (not l.eof() and (l.is_letter(ch) or l.is_digit(ch) or ch == ':'))
        l.advance();

    if (not l.eof())
        throw SyntaxError("Invalid tag name");
}

void Lexer::consume_whitespace()
{
    while (ch == ' ' or ch == '\t' or ch == '\n' or ch == '\r')
        advance();
}

Token Lexer::next_token()
{
    consume_whitespace();

    Token token;

    switch (mode) {
        case Mode::CONTENT:
            return content_mode();
        case Mode::TAG:
            return tag_mode();
        case Mode::CDATA:
            return cdata_mode();
        case Mode::COMMENT:
            return comment_mode();
    }
}

Token Lexer::content_mode()
{
    Token token;

    switch (ch) {
        case 0: {
            token.value = "";
            token.type = Token::Type::END_OF_FILE;
            break;
        }
        case '<': {
            advance();

            if (is_letter(ch) and not is_digit(ch) and ch != '-' and ch != '.') {
                token.value = "<" + read_name();
                token.type = Token::Type::TAG_BEGIN;
                mode = Mode::TAG;
                return token;
            } else if (ch == '/') {
                advance();
                token.value = "</" + read_name() + ">";
                token.type = Token::Type::TAG_CLOSE;
            } else if (ch == '!') {
                std::string tag = read_special_tag();

                if (tag == "!--") {
                    token.value = "<!--";
                    token.type = Token::Type::COMMENT_BEGIN;
                    mode = Mode::COMMENT;
                } else if (tag == "![CDATA[") {
                    token.value = "<![CDATA[";
                    token.type  = Token::Type::CDATA_BEGIN;
                    mode = Mode::CDATA;
                } else {
                    token.value = "<" + tag;
                    token.type = Token::Type::DOCTYPE;
                }

                return token;
            } else if (ch == '?') {
                token.value = "<" + read_special_tag();
                token.type = Token::Type::PI;
                return token;
            } else {
                token.value = ch;
                token.type = Token::Type::INVALID;
            }
            break;
        }
        default: {
            token.value = read_until('<');
            for (auto &&ch : token.value) {
                if (ch == '>')
                    throw SyntaxError("Unexpected symbol >");
            }
            token.type = Token::Type::CONTENT;
            break;
        }
    }

    advance();
    return token;
}

Token Lexer::tag_mode()
{
    Token token;

    switch (ch) {
        case 0: {
            token.value = "";
            token.type = Token::Type::END_OF_FILE;
            break;
        }
        case '>': {
            token.value = ">";
            token.type = Token::Type::TAG_END;
            mode = Mode::CONTENT;
            break;
        }
        case '/': {
            if (peek() == '>') {
                advance();
                token.value = "/>";
                token.type = Token::Type::TAG_END_AND_CLOSE;
                mode = Mode::CONTENT;
            } else {
                token.value = "/";
                token.type = Token::Type::INVALID;
            }
            break;
        }
        case '=': {
            token.value = "=";
            token.type  = Token::Type::EQUAL_SIGN;
            break;
        }
        case '\'': {
            advance();
            token.value = read_until('\'');
            token.type = Token::Type::ATTRIBUTE_VALUE;
            advance();
            break;
        }
        case '"': {
            advance();
            token.value = read_until('"');
            token.type = Token::Type::ATTRIBUTE_VALUE;
            advance();
            break;
        }
        default: {
            if (is_letter(ch) and not is_digit(ch) and ch != '-' and ch != '.') {
                token.value = read_name();
                token.type = Token::Type::ATTRIBUTE_NAME;
                return token;
            } else {
                token.value = ch;
                token.type = Token::Type::INVALID;
            }
            break;
        }
    }

    advance();
    return token;
}

Token Lexer::cdata_mode()
{
    Token token;

    switch (ch) {
        case 0: {
            token.value = "";
            token.type = Token::Type::END_OF_FILE;
            break;
        }
        case ']': {
            advance();
            if (ch == ']' and peek() == '>') {
                advance();
                token.value = "]]>";
                token.type = Token::Type::CDATA_END;
                mode = Mode::CONTENT;
            } else {
                token.value = "]" + read_until("]]>");
                token.type = Token::Type::CDATA;
            }
            break;
        }
        default: {
            token.value = read_until("]]>");
            token.type = Token::Type::CDATA;
            break;
        }
    }

    advance();
    return token;
}

Token Lexer::comment_mode()
{
    Token token;

    switch (ch) {
        case 0: {
            token.value = "";
            token.type = Token::Type::END_OF_FILE;
            break;
        }
        case '-': {
            advance();
            if (ch == '-' and peek() == '>') {
                advance();
                token.value = "-->";
                token.type = Token::Type::COMMENT_END;
                mode = Mode::CONTENT;
            } else if (ch == '-') {
                token.value = "--";
                token.type = Token::Type::INVALID;
            } else {
                token.value = '-' + read_until('-');
                token.type = Token::Type::COMMENT;
            }
            break;
        }
        default: {
            token.value = read_until('-');
            token.type = Token::Type::COMMENT;
            break;
        }
    }

    advance();
    return token;
}

std::string Lexer::read_name()
{
    std::string name;
    while (!eof() and (is_letter(ch) or ch == ':' or is_digit(ch))) {
        name.push_back(ch);
        advance();
    }
    return name;
}

bool Lexer::is_letter(char c)
{
    return ('a' <= c and c <= 'z') or ('A' <= c and c <= 'Z') or ch == '_';
}

bool Lexer::is_digit(char c)
{
    return '0' <= c and c <= '9';
}

std::string Lexer::read_special_tag()
{
    std::string tag;
    while (ch != '>' and !eof()) {
        tag.push_back(ch);
        advance();
        if (tag == "!--" or tag == "![CDATA[")
            return tag;
    }
    if (ch == '>') {
        tag.push_back(ch);
        advance();
    }
    return tag;
}

std::string Lexer::read_until(char c)
{
    std::string buffer;
    while (!eof()) {
        buffer.push_back(ch);
        if (peek() == c)
            return buffer;
        advance();
    }
    return buffer;
}

std::string Lexer::read_until(const std::string &substr)
{
    std::string buffer;
    while (!eof()) {
        buffer.push_back(ch);
        bool match = true;

        for (auto i = 0; read_offset + i < input.size() and i < substr.size(); i++) {
            if (input[read_offset + i] != substr[i]) {
                match = false;
                continue;
            }
        }

        if (match)
            return buffer;

        advance();
    }
    return buffer;
}

} // namespace XML
