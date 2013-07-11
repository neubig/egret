#include "ERSet.h"

/************************************************************/
int 
NtEInfo::
dumpToRuleSet(RuleSet& ruleSet)
{
	check();
	int num=0;
	if(count()<=0)
		return 0;
	for(iterator iter=begin();iter!=end();iter++)
	{
		string rightPart=iter->first;
		double rcount=iter->second;
		Rule rule;
		rule.left=nt();
		rule.right=rightPart;
		rule.prob=rcount/count();
		ruleSet.add(rule);
		num++;
	}
	return  num;
}

void 
NtEInfo::
print(ostream& os)
{
	int num=0;
	for(iterator iter=begin();iter!=end();iter++)
	{
		string rightPart=iter->first;
		double rcount=iter->second;
		Rule rule;
		rule.left=nt();
		rule.right=rightPart;
		rule.prob=rcount/count();
		rule.print(os);
	}
}
void 
NtEInfo::
print(jstream& os)
{
	int num=0;
	for(iterator iter=begin();iter!=end();iter++)
	{
		string rightPart=iter->first;
		double rcount=iter->second;
		Rule rule;
		rule.left=nt();
		rule.right=rightPart;
		rule.prob=rcount/count();
		rule.print(os);
	}
}
void 
NtEInfo::
thresholding(double threshold)
{
	int num=0;
	iterator tempIter;
	for(iterator iter=begin();iter!=end();iter=tempIter)
	{
		tempIter=iter;
		tempIter++;
		string rightPart=iter->first;
		double rcount=iter->second;
		Rule rule;
		rule.left=nt();
		rule.right=rightPart;
		rule.prob=rcount/count();
		if(rule.prob<threshold)
		{
			erase(iter);
			count()-=rcount;
		}
	}
}

bool
NtEInfo::
check()
{
	double allrcount=0;
	//if(count()<=0)
	//	return 0;
	for(iterator iter=begin();iter!=end();iter++)
	{
		allrcount+=iter->second;
	}
	//if(allrcount!=_count)
	//	jerr<<"allr:"<<allrcount<<","<<"left:"<<_count<<","<<allrcount-_count<<endl;
	return  allrcount==_count;
}

/************************************************************/
bool 
ERSet::
addNtCount(string leftPart, double count)
{
	if(find(leftPart)==end())
	{
		(*this)[leftPart].nt()=leftPart;
		(*this)[leftPart].count()=0;
	}
	(*this)[leftPart].count()+=count;
//	jout<<"add nt count:"<<(*this)[leftPart].nt()<<","<<(*this)[leftPart].count()<<endl;
	return true;
}

bool 
ERSet::
addRuleCount(string leftPart, string rightPart, double count)
{
	if(count<=0)return false;
	if(find(leftPart)==end())
	{
		(*this)[leftPart].nt()=leftPart;
		(*this)[leftPart].count()=0;
	}
	NtEInfo& nei=(*this)[leftPart];
	nei.count()+=count;
	if(nei.find(rightPart)==nei.end())
	{
		nei[rightPart]=0;
	}
	nei[rightPart]+=count;
	//jout<<"add rule count:"<<(*this)[leftPart].nt()<<"-->"<<rightPart<<","<<(*this)[leftPart][rightPart]<<endl;
	return true;
}

RuleSet& 
ERSet::
makeRuleSet(RuleSet& ruleSet)
{
	ruleSet.clear();
	for(iterator iter=begin();iter!=end();iter++)
	{
		iter->second.dumpToRuleSet(ruleSet);
	}
	return ruleSet;
}

void 
ERSet::
print(ostream& os)
{
	for(iterator iter=begin();iter!=end();iter++)
	{
		iter->second.print(os);
	}
}

void 
ERSet::
print(jstream& os)
{
	for(iterator iter=begin();iter!=end();iter++)
	{
		iter->second.print(os);
	}
}


void 
ERSet::
thresholding(double threshold)
{
	for(iterator iter=begin();iter!=end();iter++)
	{
		iter->second.thresholding(threshold);
	}
}

