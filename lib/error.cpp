#include "error.h"
#include "chary.h"

#include<iostream>

Error::Error():msg("An error occured."){}

Error::Error(const Chary& msg):msg(msg) {}

Error::~Error() {}

Error::operator const Chary&() const {
    return msg;
}

std::ostream& operator<<(std::ostream& os,const Error& err) {
    return os << err.msg;
}

Empty::Empty(const Chary& message, const Chary& detail ):Error( message + " "+detail+" is null.") {}

Flush::Flush(const Chary& message ):Error( message + " is flushed.") {}

//EOS::EOS(const Chary& message):Error( "End of "+message+" Stream" ){}

