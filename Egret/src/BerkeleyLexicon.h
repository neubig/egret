#ifndef BERKELEYLEXICON_H
#define BERKELEYLEXICON_H
#include "BerkeleyRule.h"
#include "BerkeleyStateInfo.h"
#include "Bijection.h"
#include <ctype.h>
#include "utils.h"
typedef enum{
Chinese,English} Language;

extern Language language;
class BerkeleyLexicon
{
public:
	BerkeleyLexicon(){_logrized=false;}
	const vector<BerkeleyUnaryRule*>& getLexRulesByWord(const string& wd)const{return getLexRulesByWord(word2state(wd));}
	const vector<BerkeleyUnaryRule*>& getLexRulesByWord(int wd)const;
	//const vector<BerkeleyUnaryRule*>& getLexRulesByTag(string& tg)const;
	//const vector<BerkeleyUnaryRule*>& getLexRulesByTag(int tg)const;	
	const BerkeleyUnaryRule* getLexRuleByRule(const string& wrd, const string& tag)const;
	const BerkeleyUnaryRule* getLexRuleByRule(int wrd,int tag)const;

	int  word2state(const string& wrd, int pos=-1)const;
	int  tag2state(const string& tag)const{return _stateInfoOfcurLevel.term2state(tag);}
	string state2word(int state)const{string result="";_word2state.queryY2X(state,result);return result;}
	string state2tag(int state)const{return _stateInfoOfcurLevel.state2term(state);}
	string chinese_unknownWord_category(const string& wrd, int pos)const;
	string english_nknownWord_category(const string& wrd, int pos)const;

	size_t numOfWord()const{return _word2state.size();}
	size_t numOfState()const{return _stateInfoOfcurLevel.numOfState();}

	void init(string lexFile,const BerkeleyStateInfoOfOneLevel& bsi);
	void print(string lexFile)const;
	void print_by_ruleVector(string lexFile)const;
	void logrize();
	bool isLogrized(){return _logrized;}
	void setRuleIntoVector();

private:
	void initRuleInVector();
	
	int  naive_word2state(const string& wrd)const{int result=-1;_word2state.queryX2Y(wrd,result);return result;}
	
	map<string, map<string, BerkeleyUnaryRule> > _unaryRuleHouse;
	//vector< vector<BerkeleyUnaryRule*> > _unaryRuleByRule;//limited
	vector<vector<BerkeleyUnaryRule*> > _unaryRuleByWord;

	Bijection<string,int> _word2state;
	BerkeleyStateInfoOfOneLevel _stateInfoOfcurLevel;
	bool _logrized;
};

string chinese_unknownWord_category(const string& wrd);
string english_unknownWord_category(const string& wrd);
#endif
