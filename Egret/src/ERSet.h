#ifndef ERSET_H
#define ERSET_H
#include "RuleSet.h"
class NtEInfo:public map<string , double>
{
public:
	NtEInfo(){_nt="";_count=0;clear();}
	double& count(){return _count;}
	string& nt(){return _nt;}
	int dumpToRuleSet(RuleSet& ruleSet);
	bool check();
	void print(ostream& os);
	void print(jstream& os);
	void thresholding(double threshold);
private:
	string _nt;
	double _count;
};

class ERSet:public map<string, NtEInfo>
{
public:
	bool addNtCount(string leftPart, double count);
	bool addRuleCount(string leftPart, string rightPart, double count);
	RuleSet& makeRuleSet(RuleSet& ruleSet);
	void print(ostream& os);
	void print(jstream& os);
	void thresholding(double threshold);
private:
	
};

#endif
