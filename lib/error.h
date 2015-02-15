#ifndef ERROR_H
#define ERROR_H

#include "chary.h"
#include <iostream>

class Error {
protected:
    Chary msg;

public:

	Error();

    Error(const Chary& msg);

    virtual ~Error();

    operator const Chary&() const;

    friend std::ostream& operator<<(std::ostream& os,const Error& err);
};

class Flush:public Error{
public:
	Flush(const Chary& msg);
};

class Empty:public Error {
public:

    Empty(const Chary& msg,const Chary& msg2);
};

//class EOS:public Error{
//public:
//	EOS(const Chary& msg);
//};


#endif // ERROR_H
