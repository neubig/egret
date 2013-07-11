#ifndef RULE_H
#define RULE_H
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "bob-hash.h"
#include "Jstream.h"

#ifdef WIN32
#include <hash_map>
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif
using namespace std;
//#ifdef _DEBUG
//using namespace stdext;
//#endif

struct string_hash   
{   
    size_t operator()(const string& str) const   
    {   
        return bob_hash(str.c_str());   
    }   
};

class Rule{
public:
	string left,right;
	double prob;
	void print(ostream& os);
	void print(jstream& os);
	bool operator == (Rule& rule){return left==rule.left&&right==rule.right;}
};

typedef enum{
	LEFT,RIGHT,PERMUTE
}Direct;


class InterNodes : public map<string,string>
{
public:
	string& query(string& str, string& prefix);
};


int binarize(Rule& rule, InterNodes& interNodes,string prefix, Direct dir);
bool string2rule(string& str, Rule& rule);



#endif
