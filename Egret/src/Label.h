#ifndef LABEL_H
#define LABEL_H
#include "ScoreInfo.h"
#include "UniqueHeap.h"
#include "Tree.h"
#include "Rule.h"
#include "ERSet.h"
#include "utils.h"
#include <map>
#include <list>
#include <set>
#include <assert.h>
using namespace std;
typedef enum{Normal,Berkeley}InterType;
typedef enum{Postfix,Nofix}NTType;
typedef enum {inside,outside}IorO;
typedef enum{SUM,MAX}IOType;

class Label;
typedef vector<Label*> LabelPointers;
class LabelArray
{
public:
	LabelArray(){_childs.clear();_ruleScore=0;}
	LabelArray(vector<Label*>& chs, double rs):_childs(chs),_ruleScore(rs){}
	LabelArray(Label* ch1, Label* ch2, double rs):_ruleScore(rs){_childs.clear();_childs.push_back(ch1);_childs.push_back(ch2);}
	double ruleScore()const{return _ruleScore;}

	int shoulders(const IntArray& ia, vector<ScoreInfo>& m_si,int posInChildPair);
	bool pairscore(IntArray& ia, double& score);
	void printScore(ostream& os){os<<_ruleScore;}
	void printScore(jstream& os){os<<_ruleScore;}

	LabelPointers& childs(){return _childs;}
	const vector<Label*>& childs()const{return _childs;}
	void print(ostream& os)const;
	
	bool check_and_replace_interLabel(list<LabelArray>& lalist , InterType it);
	string gen_string_represent(string& represent)const;
	const LabelPointers& get_childs()const{return _childs;}
	LabelPointers& set_childs(){return _childs;}
	double set_score(double sc){ return _ruleScore=sc;}
	size_t io_afterParse(IorO ioro, IOType iot);

private:

	LabelPointers _childs;
	double _ruleScore;
};

class ReplaceLabelMachine;
typedef vector<LabelArray> LabelArrays;

class Label
{
public:
	Label(){siList.clear();siHeap.clear();_childPairPool.clear();_nt="";_wrd="";renew_nbest=false;_nbestClose=false;_insideProb=_outsideProb=0;_notAcceptNewChilds=false;}
	void init(string ntname,int startpos,int endpos,string wrd=""){_nt=ntname,_start=startpos,_stop=(endpos);_wrd=wrd;}

	const string nt()const{return _nt;}
	string& nt(){return _nt;}
	const string word()const{return _wrd;}
	int start()const{return _start;}
	int stop()const{return _stop;}
	const bool notAcceptNewChilds()const{return _notAcceptNewChilds;}
	const LabelArrays& childPairPool()const{return _childPairPool;}
	LabelArrays& childPairPool(){return _childPairPool;}
	void print(ostream& os)const{os<<nt()<<":"<<endl;for(size_t i=0;i<_childPairPool.size();i++)_childPairPool[i].print(os); os<<endl;}
	void print_hyperedges(ostream& os)const;
	string ntAndBoundary()const{if(nt()=="")cerr<<"here"<<endl;return nt()+"["+intToString(start())+","+intToString(stop())+"] ";}

//berkeley
	int state()const{return _state;}
	int subState()const{return _subState;}
	void init(string base_ntname,int startpos,int endpos, int state, int subState, string wrd="")
		{_nt=base_ntname+"_"+intToString(subState),_start=startpos,_stop=endpos,_state=state,_subState=subState,_wrd=wrd;}
	//object function parsing
	void init(string base_ntname,int startpos,int endpos, int state, string wrd="")
		{_nt=base_ntname,_start=startpos,_stop=endpos,_state=state,_subState=-1,_wrd=wrd;}
// not const function
	void addChildPair_viterbi(Label* ch1, Label* ch2, double ruleScore);
	void addChildPair_io(Label* ch1, Label* ch2, double ruleScore,IOType iot=SUM);

	bool addChild_viterbi(Label* ch1, double ruleScore);
	void addChild_io(Label* ch1, double ruleScore,IOType iot=SUM);

	void addPreTermScore(double score);
	void addPreTermScore_io(double score);

	bool nbestClose(){return _nbestClose?true:(siHeap.empty()&&!siList.empty())?_nbestClose=true:false;}
	bool& set_notAcceptNewChilds(){return _notAcceptNewChilds;}

	bool getNthBest(int nth, double& score);
	Tree* genTree(int nth, Tree& tree);
	int genTrees(int num, vector<Tree*>& trees);
	
	double& insideProb(){return _insideProb;}
	double& outsideProb(){return _outsideProb;}

	int compute_children_outside(IOType iot=SUM);
	int collect_rule_expectation(double s1InProb, ERSet& eruleSet);

	bool is_interNode(InterType it);
	bool replace_interLabel(InterType it);
	int regenerate_nbest();
	size_t io_afterParse(IorO ioro, IOType iot);
private:

	bool pop_from_heap_to_list();

	bool renew_nbest;
	string _nt,_wrd;
	int _start,_stop;
	bool _notAcceptNewChilds;
	vector<ScoreInfo> siList;
	UniqueHeap<ScoreInfo> siHeap;
	double _insideProb,_outsideProb;
	LabelArrays _childPairPool;
	bool _nbestClose;

//berkeley

	int _state, _subState;
};

typedef struct
{
	set<string> _ancestor;
	set<string> _descendant;
}AncDes;

class LabelAndFamily
{
public:
	string& term(){return _term;}
	set<string>& descendant(){return _descendant;}
	set<string>& ancestor(){return _ancestor;}
	size_t descendant_size()const{return _descendant.size();}
	size_t ancestor_size()const{return _ancestor.size();}
	//bool operator < (LabelAndFamily& lac){return descendant().size()<lac.descendant().size();}
	void print(ostream& os);
	void print(jstream& os);
private:
	string _term;
	set<string> _ancestor;
	set<string> _descendant;
};
inline bool comp_laf(const LabelAndFamily& laf1, const LabelAndFamily& laf2) {return laf1.descendant_size()<laf2.descendant_size();}

extern IOType globalIOT;
double updateProb(double prob1, double prob2, IOType iot);
#endif
