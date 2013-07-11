#include "Rule.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

void 
Rule::
print(ostream& os)
{
	stringstream ss;
	ss<<prob;
	string temp;
	ss>>temp;
	//os.setf(
	os.precision(5);
	os.setf(ios::fixed,ios::floatfield);
	os<<prob<<"   ";
	//if(temp.length()<8)
	//	os<<"\t";
	os<<left<<" --> "<<right<<endl;
}
void 
Rule::
print(jstream& os)
{
	stringstream ss;
	ss<<prob;
	string temp;
	ss>>temp;
	//os.setf(
	//os.precision(5);
	//os.setf(ios::fixed,ios::floatfield);
	os<<prob<<"   ";
	//if(temp.length()<8)
	//	os<<"\t";
	os<<left<<" --> "<<right<<endl;
}

bool string2rule(string& str, Rule& rule)
{
	string temp;
	str.erase(0,str.find_first_not_of(" \t"));
	temp=str.substr(0,str.find_first_of(" \t"));
	//jout<<temp<<endl;
	rule.prob=atof(temp.c_str());
	if(rule.prob<=0)rule.prob=0.000001;
	str.erase(0,str.find_first_of(" \t"));
	str.erase(0,str.find_first_not_of(" \t"));
	temp=str.substr(0,str.find(" --> "));
	rule.left=temp;
	//jout<<rule.left<<endl;
	str.erase(0,str.find(" --> ")+5);
	rule.right="";
	stringstream ss(str);
	while(!ss.eof())
	{
		string wrd="";
		ss>>wrd;
		if(wrd=="")break;
		if(rule.right.length()>0)
			rule.right+=" ";
		rule.right+=wrd;
	}

	return true;
}
