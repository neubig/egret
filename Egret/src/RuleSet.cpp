#include "RuleSet.h"
#include <time.h>
#if defined(LISTRULE)
void 
ListRuleSet::
add(const Rule& rl)
{
	if(rl.left==rl.right) return;
	if(find(rl.right)==end())
		(*this)[rl.right].clear();
	(*this)[rl.right][rl.left]=rl;
}

ListRuleSet::ListRuleSet(istream &is)
{
	string curline;
	InterNodes interNodes;
	while(!is.eof())
	{
		getline(is,curline);
		if(is.eof())break;
		if(curline.find("-->")==string::npos)continue;
		Rule rule;
		string2rule(curline,rule);
		//rule.print(jout);
		//add(rule);
		binarize(rule, interNodes, rule.left, PERMUTE , *this);
	}
	normalize();
	//cerr<<"finished loading!"<<endl;
	//print(cout);
};

void 
ListRuleSet::
normalize()
{
	map<string,double> leftMark;
	leftMark.clear();
	for(iterator iter=begin();iter!=end();iter++)
	{
		RuleList& rlist=iter->second;
		for(RSIter rsiter=rlist.begin();rsiter!=rlist.end();rsiter++)
		{
			if(leftMark.find(rsiter->second.left)==leftMark.end())
				leftMark[rsiter->second.left]=0;
			leftMark[rsiter->second.left]+=rsiter->second.prob;
		}
	}
	for(iterator iter=begin();iter!=end();iter++)
	{
		RuleList& rlist=iter->second;
		for(RSIter rsiter=rlist.begin();rsiter!=rlist.end();rsiter++)
		{
			rsiter->second.prob/=leftMark[rsiter->second.left];
		}
	}
}
extern clock_t qutime;
pair<ListRuleSet::RSIter,ListRuleSet::RSIter>
ListRuleSet::
query(string rightPart)
{
	//clock_t sec=clock();
	if(find(rightPart)==end())
	{
	//	qutime+=clock()-sec;
		return make_pair(emptyRuleList.begin(),emptyRuleList.end());
	}
	else
	{
	//	qutime+=clock()-sec;
		return make_pair((*this)[rightPart].begin(),(*this)[rightPart].end());
	}
}

#else
#if defined(MMRULE)
/*********************MultiMapRuleSet***************************/

MultiMapRuleSet::MultiMapRuleSet(istream &is)
{
	string curline;
	InterNodes interNodes;
	while(!is.eof())
	{
		getline(is,curline);
		if(is.eof())break;
		if(curline.find("-->")==string::npos)continue;
		Rule rule;
		string2rule(curline,rule);
		//rule.print(jout);
		add(rule);
		//binarize(rule, interNodes, rule.left, PERMUTE , *this);
	}
	normalize();
	//cerr<<"finished loading!"<<endl;
	//print(cout);
};

void 
MultiMapRuleSet::
add(const Rule& rl)
{
	if(rl.left==rl.right)return;
	insert(make_pair(rl.right,rl));
}

void 
MultiMapRuleSet::
normalize()
{
	map<string,double> leftMark;
	leftMark.clear();
	for(iterator iter=begin();iter!=end();iter++)
	{
		if(leftMark.find(iter->second.left)==leftMark.end())
			leftMark[iter->second.left]=0;
		leftMark[iter->second.left]+=iter->second.prob;
	}
	for(iterator iter=begin();iter!=end();iter++)
	{
		iter->second.prob/=leftMark[iter->second.left];
	}
}
#endif
#endif
