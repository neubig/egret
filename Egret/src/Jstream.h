#ifndef JKOUT_H
#define JKOUT_H
#include <string>
#include <iostream>
using namespace std;
extern int dlevel;
class jstream
{
public:
jstream(ostream& os):_io(os){}

/*jstream& operator<< (bool& val ){ if(dlevel) cout<<val;  return *this;}
jstream& operator<< (short& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (unsigned short& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (int& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (unsigned int& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (long& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (unsigned long& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (float& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (double& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (long double& val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (void*& val ){ if(dlevel) _io<<val;  return *this;}
 */

jstream& operator<< (bool val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (short val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (unsigned short val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (int val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (unsigned int val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (long val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (unsigned long val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (float val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (double val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (long double val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (void* val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (char* val ){ if(dlevel) _io<<val;  return *this;}
jstream& operator<< (string val ){ if(dlevel) _io<<val;  return *this;}

jstream& operator<< (streambuf* sb ){ if(dlevel) _io<<sb;  return *this;}
 
jstream& operator<< (jstream& ( *pf )(jstream&)){ if(dlevel) _io<<pf;  return *this;}
jstream& operator<< (ios& ( *pf )(ios&)){ if(dlevel) _io<<pf;  return *this;}
jstream& operator<< (ios_base& ( *pf )(ios_base&)){ if(dlevel) _io<<pf;  return *this;}

jstream& operator<< (ostream& ( *pf )(ostream&)){ if(dlevel) _io<<pf;  return *this;}

private:
ostream& _io;
};
extern jstream jout,jerr;
#endif
