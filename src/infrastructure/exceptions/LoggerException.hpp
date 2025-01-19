#include <sstream>
#include <string>
#include <exception>

class LoggerException : public std::exception {
public:
    explicit LoggerException(const std::string& message);
    virtual ~LoggerException() throw();
    virtual const char* what() const throw();
private:
    std::string _msg;
};

