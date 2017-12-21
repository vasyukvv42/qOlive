//
// Created by cyborg on 12/8/17.
//

#include "Errors.hpp"

namespace XML {

Error::Error(const std::string &message) : message(message) {}

const char *Error::what() const noexcept
{
    return message.c_str();
}

SyntaxError::SyntaxError(const std::string &message) : Error(message) {}

DOMError::DOMError(const std::string &message) : Error(message) {}

}