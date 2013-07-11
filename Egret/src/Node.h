#ifndef NODE_H
#define NODE_H
#include "Label.h"
#include "BerkeleyStateInfo.h"
#include "BerkeleyRuleSet.h"

typedef enum
{
	VARIATIONAL,MAX_RULE_SUM,MAX_RULE_PRODUCT,NONE,MAX_PROB
}ObjectFunctionType;

class AllowStateInfo : public map<int, map<int,int> >
{
public:
	AllowStateInfo(){_inUse=false;_inVector.clear();}
	void allowStates(vector<int>& result)const;
	void allowSubStates(int state, vector<int>& result)const;
	const vector<pair<int,vector<int> > >& allowStateAndSubStates()const;
	bool isAllow(int state)const;
	bool isAllow(int state, int subState)const;
	void clone(AllowStateInfo& result)const;
	bool inUse()const{return _inUse;}
	void print(ostream& os)const;

//not const function
	void add(int state, int subState);
	void del(int state, int subState);
	bool set_inUse(bool value){return _inUse=value;}

	void reset_allowStateInfo_to_nextLevel(const BerkeleyStateInfoOfOneLevel& bsio);
	void set_inVector();
private:

	bool _inUse;
	vector<pair<int,vector<int> > > _inVector;
};

typedef struct{
	double is,os,sc;
}IOScore;

class Node : public map<string,Label>
{
public:
	Node(int start,int stop){_start=start,_stop=stop;}
	void print(ostream& os);
	void print(jstream& os);
	void print_inside(ostream& os);
	vector<LabelAndFamily>& unaryInOrder(){return _unaryInOrder;}
	vector<LabelAndFamily>& setUnaryInOrder(RuleSet& ruleSet);
	vector<string>& setUnaryInOrder_simple();
	vector<string>& setUnaryInOrder(RuleSet& ruleSet,vector<string>& ntPool);
	vector<string>& setUnaryInOrder_berkeley(const BerkeleyRuleSet& ruleSet,vector<string>& ntPool);
	vector<string>& setUnaryInOrder_objectFunction_berkeley(
		const BerkeleyRuleSet& ruleSet, 
		const vector<vector<double> >& tempUnraryRuleSet, 
		const vector<int>& allowStates,
		vector<string>& ntPool,
		ObjectFunctionType oft);
	
	int compute_children_outside(string s1="ROOT");
	int collect_rule_expectation(double s1InProb, ERSet& erSet);
	bool replace_interLabel(InterType it);
	vector<string>& uio(){return _uio;}
	
	void print_hyperedges(ostream& os)const;
	size_t io_afterParse(IorO ioro, IOType iot);
	void prune(map<Label*,int>& reachableLabels, double rootInsideProb, double threshold);

//berkeley
	const bool useAllowState()const{return _asi.inUse();}
	const AllowStateInfo& allowStateInfo()const{return _asi;}
	
	void prune_and_setAllowState_byLabel(double s1InProb, double threshold);
	const AllowStateInfo& reset_allowStateInfo_to_nextLevel(const BerkeleyStateInfoOfOneLevel& bsio);
	

	int& set_start(){return _start;}
	int& set_stop(){return _stop;}
	const int start()const{return _start;}
	const int stop()const{return _stop;}
	
	double& iScore(int state, int subState){return _ioScores[state][subState].is;}
	double& oScore(int state, int subState){return _ioScores[state][subState].os;}
	double& Score(int state, int subState){return _ioScores[state][subState].sc;}
	void reset_scorePool(const BerkeleyStateInfoOfOneLevel& bsio);
	const AllowStateInfo& set_allowStateAndSubState_withoutLabel(double s1InProb, double threshold, double inside=-1);
	void print_io_withoutLabel(ostream& os);

//for initial level, considering speed
	double& iScore(int state){return _iScore[state];}
	double& oScore(int state){return _oScore[state];}
	const vector<int>& allowState()const{return _allowState;}
	const vector<int>& set_allowState_withoutLabel_for_initialLevel(double s1InProb, double threshold, double inside=-1);
	void set_allowStateAndSubState_byState();
	void reset_scorePool_initLevel(int numOfState);


private:
	vector<LabelAndFamily> _unaryInOrder;
	vector<string> _uio;
	AllowStateInfo _asi;
	int _start,_stop;
	vector<vector<IOScore> > _ioScores;
	vector<double> _iScore;
	vector<double> _oScore;
	vector<int> _allowState;
};

extern int unary_fine;
#endif
