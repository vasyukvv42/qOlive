//
// Created by cyborg on 12/8/17.
//

#ifndef FANTASTIC_PARSERS_ERRORS_HPP
#define FANTASTIC_PARSERS_ERRORS_HPP

#include <stdexcept>

namespace XML
{

class Error : public std::exception
{
public:
    explicit Error(const std::string &message);

    const char *what() const noexcept override;

    std::string message;
};

class SyntaxError : public Error
{
public:
    explicit SyntaxError(const std::string &message);
};

class DOMError : public Error
{
public:
    explicit DOMError(const std::string &message);
};

} // namespace XML


#endif //FANTASTIC_PARSERS_ERRORS_HPP
