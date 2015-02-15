#include "chary.h"
#include<iostream>

int Chary::length(const char* st, char stopper) const  {
    char* pstr = const_cast<char*>(st);
    int i = 0;
    while( *(pstr) != stopper && *(pstr++) != '\0') {
        i++;
    }
    return i;
}

void Chary::copy(const char* s) {
    str = new char[length(s)+1];
    char* pstr = const_cast<char*>(str);
    char* ps = const_cast<char*>(s);
    while( (*(pstr++) = *(ps++)) );
}

Chary::Chary() {
    str = new char;
}

Chary::Chary(const char* s) {
    copy(s);
}

Chary::Chary(const Chary& r) {
    copy(r.str);
}

Chary::~Chary() {
    delete []str;
}

Chary Chary::operator=(const char* r) {
    delete []str;
    str = new char[length(r)+1];
    char* pstr = const_cast<char*>(str);
    char* pstr2 = const_cast<char*>(r);
    while( (*(pstr++) = *(pstr2++)) );
    return *this;
}

Chary Chary::operator=(const Chary& r) {
    if(this!=&r)
        operator=(r.str);
    return *this;
}

Chary Chary::operator+=(const char* r) {
    char* str2 = new char[length(str)+length(r)+1];
    char* pstr2 = str2;
    char* pstr = const_cast<char*>(str);

    while( (*(pstr2++) = *(pstr++)) );
    pstr2--;

    pstr = const_cast<char*>(r);
    while( (*(pstr2++) = *(pstr++)) );

    delete []str;
    str = str2;
    return *this;
}

Chary Chary::operator+=(const Chary& r) {
    operator+=(r.str);
    return *this;
}

//Chary Chary::operator+(const Chary& r) const {
//    return Chary(str)+=r;
//}
//
//Chary Chary::operator+(const char* c) const {
//    return Chary(str)+=c;
//}

Chary Chary::operator*=(int no) {
    char* str2 = new char[length(str)*no+1];
    char* pstr2 = str2;

    for(int i=0; i<no; i++) {
        char* pstr = const_cast<char*>(str);
        while( (*(pstr2++) = *(pstr++)) );
        pstr2--;
    }

    delete []str;
    str = str2;
    return *this;
}

Chary Chary::operator*(int no) {
    return Chary(str)*=no;
}

std::ostream& operator<<(std::ostream& os,const Chary& c) {
    return os << c.str;
}


Chary operator+(const Chary& a,const Chary& b){
    return Chary(a)+=b;
}
