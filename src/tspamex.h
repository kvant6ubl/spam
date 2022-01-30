#ifndef TSPAMEX
#define TSPAMEX
#include <exception>	
#include <string>

/** Simple class for throwing exceptions */
class TSpamEx : public std::exception {
public:
	TSpamEx(std::string message){
		this->message=message;
	}

	virtual ~TSpamEx() throw() {}

	virtual const char* what() const throw() {
		return message.c_str();
	}

private:
	std::string message;
};

#endif
