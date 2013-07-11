#ifndef RULESET_H
#define RULESET_H
#include "Rule.h"
#define LISTRULE
#define MMRULE
#if defined(LISTRULE)
class RuleList: public map<string,Rule>
{
public:
	void print(ostream& os){for(iterator iter=begin();iter!=end();iter++)iter->second.print(os);}
	void print(jstream& os){for(iterator iter=begin();iter!=end();iter++)iter->second.print(os);}
private:
};

typedef map<string, RuleList> RawListRuleSet;

class ListRuleSet: public RawListRuleSet{
public:
	typedef RuleList::iterator RSIter;
	ListRuleSet(){clear();emptyRuleList.clear();}
	ListRuleSet(istream& is);
	void normalize();
	void add(const Rule& rl);//{insert(make_pair(rl.right,rl));}
	void print(ostream& os){ for(iterator iter=begin();iter!=end();iter++)iter->second.print(os);}
	void print(jstream& os){ for(iterator iter=begin();iter!=end();iter++)iter->second.print(os);}
	pair<RSIter,RSIter> query(string rightPart);//{if()return equal_range(rightPart);}
private:
	RuleList emptyRuleList;
};

typedef ListRuleSet RuleSet;
#else 
#if defined(MMRULE)

typedef hash_multimap<string,Rule,string_hash> RawMultiMapRuleSet;

class MultiMapRuleSet: public RawMultiMapRuleSet{
public:
	typedef MultiMapRuleSet::iterator RSIter;
	MultiMapRuleSet(){clear();}
	MultiMapRuleSet(istream& is);
	void normalize();
	void add(const Rule& rl){insert(make_pair(rl.right,rl));}
	void print(ostream& os){ for(iterator iter=begin();iter!=end();iter++)iter->second.print(os);}
	void print(jstream& os){ for(iterator iter=begin();iter!=end();iter++)iter->second.print(os);}
	pair<RSIter,RSIter> query(string rightPart){return equal_range(rightPart);}
};

typedef MultiMapRuleSet RuleSet;
#endif
#endif

int binarize(Rule& rule, InterNodes& interNodes,string prefix, Direct dir, RuleSet& ruleSet);


#endif
