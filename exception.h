#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_ 1
#include <exception>
using namespace std; 
class tableExist : public exception {
};
class tableNotExist : public exception {
};
class attributeNotExist : public exception {
};
class indexExist : public exception { 
};
class indexNotExist : public exception {  
};
class tupleTypeConflict : public exception {   
};
class primarykeyConflict : public exception {   
};
class dataTypeConflict : public exception {    
};
class indexFull : public exception{    
};
class inputFormatError : public exception{    
};
class exitCommand : public exception{    
};
class uniqueConflict:public exception{   
};
#endif 
