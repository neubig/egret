#ifndef BERKELEYGRAMMAR_H
#define BERKELEYGRAMMAR_H
#include "BerkeleyRule.h"
#include "Bijection.h"
#include "Tree.h"
#include <iostream>
#include "BerkeleyStateInfo.h"
using namespace std;
class BerkeleyGrammar
{
public:

	BerkeleyGrammar(){_logrized=false;}
//Binary Rule
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByChilds(const string& lc,const string& rc)const{return getBinaryRulesByChilds(term2state(lc),term2state(rc));}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByChilds(int lc, int rc)const{return _binaryRuleByChilds[lc][rc];}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParent(string& pr)const {return getBinaryRulesByParent(term2state(pr));}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParent(int pr)const {return _binaryRuleByParent[pr];}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParentLeftChild(string& pr,string& lc)const{return getBinaryRulesByParentLeftChild(term2state(pr),term2state(lc));}
	const vector<BerkeleyBinaryRule*>& getBinaryRulesByParentLeftChild(int pr, int lc)const{return _binaryRuleByParentLeftChild[pr][lc];}
	const vector<int>& getLeftChildsByParent(string& pr)const{return getLeftChildsByParent(term2state(pr));}
	const vector<int>& getLeftChildsByParent(int pr)const{return _leftChildsByParent[pr];}
	//const vector<BerkeleyBinaryRule*>& getBinaryRulesByLeftChild(string& lc)const ;
	//const vector<BerkeleyBinaryRule*>& getBinaryRulesByLeftChild(int lc)const ;
	//const vector<BerkeleyBinaryRule*>& getBinaryRulesByRightChild(string& rc)const ;
	//const vector<BerkeleyBinaryRule*>& getBinaryRulesByRightChild(int rc)const ;
	const BerkeleyBinaryRule* getBinaryRuleByRule(const string& lc, const string& rc, const string& pr)const{return getBinaryRuleByRule(term2state(lc),term2state(rc),term2state(pr));}
	const BerkeleyBinaryRule* getBinaryRuleByRule(int lc, int rc,int pr)const{return _binaryRuleByRule[lc][rc][pr];}
	
//Unary Rule
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByChild(const string& ch)const{return getUnaryRulesByChild(term2state(ch));}
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByChild(int ch)const{return _unaryRuleByChild[ch];}
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByParent(string& pr)const {return getUnaryRulesByParent(term2state(pr));}
	const vector<BerkeleyUnaryRule*>& getUnaryRulesByParent(int pr)const {return _unaryRuleByParent[pr];}
	const BerkeleyUnaryRule* getUnaryRuleByRule(const string& ch , const string& pr)const{return getUnaryRuleByRule(term2state(ch),term2state(pr));}
	const BerkeleyUnaryRule* getUnaryRuleByRule(int ch , int pr)const{return _unaryRuleByRule[ch][pr];}
	
// initialization
	
	void init(string filename,BerkeleyStateInfoOfOneLevel& bsio);
	void print_by_ruleVector(string filename)const;
	void print(ostream& os)const;
	void print(string filename)const;
	
//other Info
	//const function
	int numOfState()const{return (int)_stateInfoOfcurLevel.numOfState();}
	int term2state(const string& term)const{return _stateInfoOfcurLevel.term2state(term);}
	string state2term(int state)const{return _stateInfoOfcurLevel.state2term(state);}
	int state2numSubState(int state)const{return _stateInfoOfcurLevel.state2numSubState(state);}
	int level()const{return _stateInfoOfcurLevel.level();}
	const BerkeleyStateInfoOfOneLevel& stateInfoOfcurLevel()const{return _stateInfoOfcurLevel;}
	const Bijection<string,int>& term2state()const{return _stateInfoOfcurLevel.term2state();}
	const State2numSubState& state2numSubState()const{return _stateInfoOfcurLevel.state2numSubState();}
	const void query_substate2sun(int state, int subState, vector<int>& sun)const{_stateInfoOfcurLevel.query_substate2sun(state,subState,sun);}
	const int query_substate2father(int state, int subState)const{return _stateInfoOfcurLevel.query_substate2father(state,subState);}
	
	void print_stateInfoOfcurLevel(ostream& os)const{_stateInfoOfcurLevel.print(os);}
	void print_stateInfoOfcurLevel(string filename)const{ofstream os(filename.c_str()); _stateInfoOfcurLevel.print(os); os.close();}
	
	void logrize();
	bool isLogrized(){return _logrized;}
	void setRuleIntoVector();
private:
	bool addRuleFromFile(RuleFromFile& rff);
	bool addUnaryRuleFromFile(RuleFromFile& rff);
	bool addBinaryRuleFromFile(RuleFromFile& rff);
	void initRuleInVector();
	

	void set_stateInfoOfcurLevel(BerkeleyStateInfo& stateInfo, int level){_stateInfoOfcurLevel.set_value_by_stateInfo(stateInfo,level);}
	void set_stateInfoOfcurLevel(BerkeleyStateInfoOfOneLevel& father){_stateInfoOfcurLevel.clone_from(father);}

//rightPart to ruleMap
	map<string, map<string, BerkeleyBinaryRule> > _binaryRuleHouse;
	map<string, map<string, BerkeleyUnaryRule> > _unaryRuleHouse;

//by rule
	vector<vector<vector<BerkeleyBinaryRule*> > > _binaryRuleByRule;
	vector<vector<BerkeleyUnaryRule*> > _unaryRuleByRule;

//by childs
	vector<vector<vector<BerkeleyBinaryRule*> > > _binaryRuleByChilds;
	vector<vector<BerkeleyUnaryRule*> > _unaryRuleByChild;

//by parent
	vector<vector<BerkeleyBinaryRule*> > _binaryRuleByParent;
	vector<vector<BerkeleyUnaryRule*> > _unaryRuleByParent;

//by parent and leftChild, only binary rules
	vector<vector<vector<BerkeleyBinaryRule*> > > _binaryRuleByParentLeftChild;
	vector<vector<int> > _leftChildsByParent;

	BerkeleyStateInfoOfOneLevel _stateInfoOfcurLevel;
	//BerkeleyStateInfo _stateInfo;
	
	bool _logrized;

};

#endif
