#ifndef BERKELEYRULESET_H
#define BERKELEYRULESET_H
#include "BerkeleyLexicon.h"
#include "BerkeleyGrammar.h"
class BerkeleyRuleSet
{
public:
	BerkeleyRuleSet(){_logrized=false;_level=-1;}
	//Binary Rule
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByChilds(const string& lc, const string& rc)const{return _grammar.getBinaryRulesByChilds(lc,rc);}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByChilds(int lc, int rc)const{return _grammar.getBinaryRulesByChilds(lc,rc);}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParent(string& pr)const {return _grammar.getBinaryRulesByParent(term2state(pr));}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParent(int pr)const {return _grammar.getBinaryRulesByParent(pr);}	
	const BerkeleyBinaryRule* getBinaryRuleByRule(const string& lc, const string& rc , const string& pr)const{return _grammar.getBinaryRuleByRule(lc,rc,pr);}
	const BerkeleyBinaryRule* getBinaryRuleByRule(int lc, int rc, int pr)const{return _grammar.getBinaryRuleByRule(lc,rc,pr);}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParentLeftChild(string& pr,string& lc)const{return _grammar.getBinaryRulesByParentLeftChild(term2state(pr),term2state(lc));}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParentLeftChild(int pr, int lc)const{return _grammar.getBinaryRulesByParentLeftChild(pr,lc);}
	const vector<int>& getLeftChildsByParent(string& pr)const{return _grammar.getLeftChildsByParent(pr);}
	const vector<int>& getLeftChildsByParent(int pr)const{return _grammar.getLeftChildsByParent(pr);}
	//const BerkeleyBinaryRule* getBinaryRuleScoreByRuleAndSubState(int lc, int rc, int pr, int lcsub, int rcsub, int prsub)const;
	

	//Unary Rule
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByChild(const string& ch)const{return _grammar.getUnaryRulesByChild(ch);}
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByChild(int ch)const{return _grammar.getUnaryRulesByChild(ch);}
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByParent(string& pr)const {return _grammar.getUnaryRulesByParent(term2state(pr));}
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByParent(int pr)const {return _grammar.getUnaryRulesByParent(pr);}
	const BerkeleyUnaryRule* getUnaryRuleByRule(const string& ch, const string& pr)const{return _grammar.getUnaryRuleByRule(ch,pr);}
	const BerkeleyUnaryRule* getUnaryRuleByRule(int ch, int pr)const{return _grammar.getUnaryRuleByRule(ch,pr);}

	//const BerkeleyBinaryRule* getUnaryRuleScoreByRuleAndSubState(int ch, int pr, int chsub, int prsub)const;
	
	//Lex Rule
	const vector<BerkeleyUnaryRule*>& getLexRulesByWord(const string& wd)const{return _lexicon.getLexRulesByWord(wd);}
	const vector<BerkeleyUnaryRule*>& getLexRulesByWord(int wd)const{return _lexicon.getLexRulesByWord(wd);}
	const BerkeleyUnaryRule* getLexRuleByRule(const string& wrd, const string& tag)const{return _lexicon.getLexRuleByRule(wrd,tag);}
	const BerkeleyUnaryRule* getLexRuleByRule(int wrd, int tag)const{return _lexicon.getLexRuleByRule(wrd,tag);}
	const BerkeleyUnaryRule* getLexRuleByRule(const string& wrd, int tag)const{return _lexicon.getLexRuleByRule(word2state(wrd),tag);}	
	//const BerkeleyUnaryRule* getLexRuleScoreByRuleAndSubState(int wrd, int tag, int tagsub)const;
	
	//stateInfo

	const int word2state(const string& wrd)const{return _lexicon.word2state(wrd);}
	const string state2word(int wrd)const{return _lexicon.state2word(wrd);}
	const int term2state(const string& term)const{return _grammar.term2state(term);}
	const string state2term(int state)const{return _grammar.state2term(state);}

	const void query_substate2sun(int state, int subState, vector<int>& sun)const{_grammar.query_substate2sun(state,subState,sun);}
	const int query_substate2father(int state, int subState)const{return _grammar.query_substate2father(state,subState);}

//	void set_threshold(double threshold){_threshold=threshold;}
//	double threshold()const {return _threshold;}

	//init
	void init(string grFile, string lexFile, BerkeleyStateInfoOfOneLevel& bsio){_grammar.init(grFile,bsio);_lexicon.init(lexFile,bsio);_bsio.clone_from(bsio);}
	void save(string prefix)const{_grammar.print(prefix+".grammar");_lexicon.print(prefix+".lexicon");}
	void save_by_ruleVector(string prefix)const{_grammar.print_by_ruleVector(prefix+".grammar");_lexicon.print_by_ruleVector(prefix+".lexicon");}

	void logrize(){if(isLogrized())return;_grammar.logrize();_lexicon.logrize();}
	bool isLogrized(){return _logrized;}
	int& level(){return _level;}
	const int get_level()const{return _level;}

	BerkeleyLexicon& lexicon(){return _lexicon;}
	BerkeleyGrammar& grammar(){return _grammar;}
	const BerkeleyStateInfoOfOneLevel& bsio()const{return _bsio;}
	const size_t numOfState()const{return bsio().numOfState();}
private:

BerkeleyLexicon _lexicon;
BerkeleyGrammar _grammar;
BerkeleyStateInfoOfOneLevel _bsio;
//double _threshold;
int _level;
bool _logrized;
};

class BerkeleyRuleSets:public vector<BerkeleyRuleSet>
{
public:
	void init(string prefix, int level);
	void save(string prefix);
	double threshold(size_t i)const{if(i>=size())return -1E10;else return _thresholds[i];}
private:
	BerkeleyStateInfo _bsi;
	vector<double> _thresholds;
};

#endif

