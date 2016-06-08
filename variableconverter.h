#ifndef VARIABLECONVERTER_H
#define VARIABLECONVERTER_H
//
// Variable converter utility,
// version not completed, but enough for current needs
// writed by ecoretchi 02/08/2015
//
#include <stdlib.h>
#include <string>
#include <iostream>

class VarConverter
{
public:
    VarConverter(const std::string& s):m_str(s){
    }
    operator std::size_t (){
        return ::atol(m_str.c_str());
    }
    operator std::string () {
        return m_str;
    }
private:
    std::string m_str;
};

#endif // VARIABLECONVERTER_H
