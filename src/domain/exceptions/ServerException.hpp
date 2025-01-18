#include <sstream>
#include <string>
#include <exception>

class ServerException : public std::exception {
public:
    explicit ServerException(const std::string& message);
    virtual ~ServerException() throw();
    virtual const char* what() const throw();
private:
    std::string _msg;
};

