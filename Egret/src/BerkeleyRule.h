#ifndef BERKELEYRULE_H
#define BERKELEYRULE_H
#include <vector>
#include <string>
#include "Bijection.h"
using namespace std;

class RuleFromFile
{
public:
	bool isUnary(){return _rcSubState==-1;}
	bool convertFromString(string);
	string parent(){return _parent;}
	string leftChild(){return _leftChild;}
	string rightChild(){return _rightChild;}
	int prSubState(){return _prSubState;}
	int	lcSubState(){return _lcSubState;}
	int	rcSubState(){return _rcSubState;}

	int prState(){return _prState;}
	int	lcState(){return _lcState;}
	int	rcState(){return _rcState;}

	void setState(const Bijection<string,int>& _term2state){_term2state.queryX2Y(_parent,_prState);_term2state.queryX2Y(_leftChild,_lcState);_term2state.queryX2Y(_rightChild,_rcState);}

	double score(){return _score;}
	string toString();
private:
	string _parent,_leftChild,_rightChild;
	int _prState,_lcState,_rcState;
	int _prSubState,_lcSubState,_rcSubState;
	double _score;
};

class BerkeleyBinaryRule
{
public:
	void set(vector<vector<vector<double> > >& scs);
	void set(int lc, int rc, int pr, int lcNSub, int rcNSub, int prNSub);
	void set(string lc, string rc, string pr){_parentString=pr,_leftChildString=lc,_rightChildString=rc;}
	string baseRuleInString(){return _parentString+" -> "+_leftChildString+" "+_rightChildString;}
	string baseRuleRightPartInString(){return _leftChildString+" "+_rightChildString;}

	string parentString()const{return _parentString;}
	string leftChildString()const{return _leftChildString;}
	string rightChildString()const{return _rightChildString;}
	int parentState()const{return _parentState;}
	int leftChildState()const{return _leftChildState;}
	int rightChildState()const{return _rightChildState;}
	int parentNumSubState()const{return _parentNumSubState;}
	int leftChildNumSubState()const{return _leftChildNumSubState;}
	int rightChildNumSubState()const{return _rightChildNumSubState;}

	double getScore(int lcSubState, int rcSubState, int prSubState)const{return _scores[lcSubState][rcSubState][prSubState];}
	double setScore(int lcSubState, int rcSubState, int prSubState, double sc)
			{
			/*	cout<<lcSubState<<","<<rcSubState<<","<<prSubState<<endl;
				cout<<_scores[lcSubState].size()<<endl;
				
				cout<<_scores[lcSubState][rcSubState].size()<<","<<prSubState<<endl;
			*/	return _scores[lcSubState][rcSubState][prSubState]=sc;
			}
	string toString()const;
	void logrize();

private:
	vector<vector<vector<double> > > _scores;
	string _parentString;
	string _leftChildString;
	string _rightChildString;
	int _parentState;
	int _leftChildState;
	int _rightChildState;
	int _parentNumSubState;
	int _leftChildNumSubState;
	int _rightChildNumSubState;

};

class BerkeleyUnaryRule
{
public:
	void set(vector<vector<double> >& scs);
	void set( int ch, int pr, int chNSub, int prNSub);
	void set(string ch , string pr){_parentString=pr,_childString=ch;}
	string baseRuleInString(){return _parentString+" -> "+_childString;}
	string baseRuleRightPartInString(){return _childString;}

	string parentString()const{return _parentString;}
	string childString()const{return _childString;}
	int parentState()const{return _parentState;}
	int childState()const{return _childState;}
	int parentNumSubState()const{return _parentNumSubState;}
	int childNumSubState()const{return _childNumSubState;}

	double getScore(int chSubState, int prSubState)const{return _scores[chSubState][prSubState];}
	double setScore(int chSubState, int prSubState, double sc){return _scores[chSubState][prSubState]=sc;}

	string toString()const;
	string toLexRuleString()const;
	void logrize();
private:
	vector<vector<double> > _scores;
	string _parentString;
	string _childString;
	int _parentState;
	int _childState;
	int _parentNumSubState;
	int _childNumSubState;
};
#endif

