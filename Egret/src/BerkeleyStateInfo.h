#ifndef BERKELEYSTATEINFO_H
#define BERKELEYSTATEINFO_H
#include "Bijection.h"
#include "Tree.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
typedef NodeInLevelAndID NodeInStateAndSubState;
class State2numSubState:public map<int,int>
{
public:
	void clone(State2numSubState& sun)const{for(const_iterator iter=begin();iter!=end();iter++)sun[iter->first]=iter->second;}
};

class SplitTreesNodes: public map<int, NodeInStateAndSubState> // level - state - substate
{
public:
	void init(map<int,Tree>& _splitTrees);
	void print(ostream& os);
	void gen_state2numSubState(State2numSubState& state2numSubState, int level)const;
private:
	void print_state2level2id(ostream& os){for(int i=0;(size_t)i<size();i++){this->find(i)->second.print(os); os<<endl;}}
};

class BerkeleyStateInfo;
class BerkeleyStateInfoOfOneLevel
{
public:
	int level()const{return _level;}
	size_t numOfState()const{return _term2state.size();}
	int term2state(string term)const;
	int state2numSubState(int state)const;
	string state2term(int state)const{string result="";_term2state.queryY2X(state,result);return result;}

	const Bijection<string,int>& term2state()const{return _term2state;}
	const State2numSubState& state2numSubState()const{return _state2numSubState;}
	const NodeInStateAndSubState& nodeInStateAndSubState()const{return _nodeInStateAndSubState;}
	const void query_substate2sun(int state, int subState, vector<int>& sun)const;
	const int query_substate2father(int state, int subState)const;

	void print(ostream& os)const;

//NOT CONST Function
	void clone_from(const BerkeleyStateInfoOfOneLevel& father);
	void set_value_by_stateInfo(const BerkeleyStateInfo& stateInfo, int level);

private:
	Bijection<string,int> _term2state;
	NodeInStateAndSubState _nodeInStateAndSubState;
	State2numSubState _state2numSubState;
	int _level;
};

class BerkeleyStateInfo
{
public:
	void init(string filename);
	void print(string filename);
	void print(ostream& os);

//const function

	int term2state(string& term)const{int result=-1; _term2state.queryX2Y(term,result);return result;}
	int state2numSubState(int state)const;
	size_t numOfState()const{return _term2state.size();}
	const Bijection<string,int>& term2state()const{return _term2state;}
	const State2numSubState& state2numSubState()const{return _state2numSubState;}
	const map<int,Tree>& splitTrees()const{return _splitTrees;}
	const SplitTreesNodes splitTreesNodes()const{return _splitTreesNodes;}

private:
	map<int,Tree> _splitTrees;
	void cat_splitTreeNodes();
    Bijection<string,int> _term2state;
	State2numSubState _state2numSubState;
	SplitTreesNodes _splitTreesNodes;
};


#endif

