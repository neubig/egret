#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
using namespace std;
string intToString(int in);
string doubleToString(double in);
string& replaceSubStr(string& input, string oldSub, string newSub);
bool endsWith(char * src, const char* tail);
inline void error(string str){cerr<<str<<endl;exit(1);}
inline double log(double x,double y){return log(y)/log(x);}
inline double unLogrize(double score){return pow(2.0,score);}
inline double logrize(double score){return log(score)/log(2.0);}

#endif
