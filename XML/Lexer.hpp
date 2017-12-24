//
// Created by cyborg on 11/10/17.
//

#ifndef XML_LEXER_HPP
#define XML_LEXER_HPP

#include "Token.hpp"
#include "Errors.hpp"

namespace XML
{

class Lexer
{
public:
    /// Constructor from std::string
    /// \param input std::string with XML content
    explicit Lexer(const std::string& input = "");

    /// Generates next token
    /// \return next token
    Token next_token();

    /// Check whether lexer has reached end of file
    /// \return True if eof is reached
    bool eof();

    static void validate_name(const std::string &name);

    enum class Mode {
        CONTENT,
        TAG,
        CDATA,
        COMMENT
    };
private:
    void advance();

    /// \return Next symbol after the current one
    char peek();

    void consume_whitespace();

    /// reads <! and <? tags until >, ends earlier if its CDATA or Comment begin
    /// \return Value for token
    std::string read_special_tag();

    /// Reads tag name or attribute name
    /// \return Name
    std::string read_name();

    /// Reads until first occurrence of character c or eof
    /// \param c Character to read until
    /// \return Value for token
    std::string read_until(char c);

    /// Reads until first occurrence of substring or eof
    /// \param substr Substring to read until
    /// \return Value for token
    std::string read_until(const std::string& substr);

    /// Checks whether c is a latin letter
    /// \param c Character to check
    /// \return True if c is a letter
    bool is_letter(char c);

    /// Checks whether c is a digit
    /// \param c Character to check
    /// \return True if c is a digit
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
