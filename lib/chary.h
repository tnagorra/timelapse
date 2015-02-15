#ifndef CHARY_H
#define CHARY_H

#include<iostream>

class Chary {
private:
    const char* str;

    void copy(const char* s);

public:

    Chary();

    Chary(const char* s);

    Chary(const Chary& r);

    virtual ~Chary();

    Chary operator=(const char* r);

    Chary operator=(const Chary& r);

    Chary operator+=(const char* r);

    Chary operator+=(const Chary& r);

//    Chary operator+(const Chary& r) const;
//
//    Chary operator+(const char* c) const;

    Chary operator*=(int no);

    Chary operator*(int no);

    operator const char*() const;

    int length(const char* st, char stopper = '\0') const;

    friend std::ostream& operator<<(std::ostream& os,const Chary& c);

};

Chary operator+(const Chary& a,const Chary& b);

inline Chary::operator const char*() const {
    return str;
}



#endif // CHARY_H
