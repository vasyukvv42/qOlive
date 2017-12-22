//
// Created by cyborg on 11/10/17.
//

#ifndef XML_TOKEN_HPP
#define XML_TOKEN_HPP

#include <string>
#include <ostream>

namespace XML
{

struct Token
{
    enum class Type
    {
        // LEXEME             EXAMPLE

        TAG_BEGIN,          // <NAME
        TAG_END,            // >
        TAG_CLOSE,          // </NAME>
        TAG_END_AND_CLOSE,  // />
        ATTRIBUTE_NAME,     // NAME
        EQUAL_SIGN,         // =
        ATTRIBUTE_VALUE,    // any text
        CONTENT,            // any text
        COMMENT_BEGIN,      // <!--
        COMMENT,            // any text
        COMMENT_END,        // -->
        CDATA_BEGIN,        // <![CDATA[
        CDATA,              // any text
        CDATA_END,          // ]]>
        PI,                 // <? ... ?>
        DOCTYPE,            // <!DOCTYPE ... >
        INVALID,
        END_OF_FILE
    };
    
    explicit Token(Type type = Type::END_OF_FILE,
                   const std::string& value = "");
    static std::string type_name(Type type);
    std::string name() const;
    friend std::ostream &operator<<(std::ostream &os, const Token &token);

    Type type;
    std::string value;
};

} // namespace XML
#endif //XML_TOKEN_HPP
