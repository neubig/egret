#include "BerkeleyRule.h"
#include "utils.h"
#include <iostream>
#include <sstream>
#include <math.h>
using namespace std;
//VP^g_11 -> VBP_0 PP^g_9 3.518294189411761E-7
//VP^g_12 -> VBP_0 PP^g_9 0.0
//PP^g_12 -> VP^g_11 0.0

void separateSubStatefromNT(string in, string& baseTerm, int& subState)
{
	baseTerm=in.substr(0,in.find("_"));
	subState=atoi(in.substr(in.find("_")+1).c_str());
}
bool 
RuleFromFile::
convertFromString(string in)
{
	stringstream ss(in);
	string parent,arrow,lc,rc,sc;
	ss>>parent;
	ss>>arrow;
	ss>>lc;
	ss>>rc;
	ss>>sc;
	if(sc=="")
	{
		sc=rc;
		rc="";
	}
	separateSubStatefromNT(parent,_parent,_prSubState);
	separateSubStatefromNT(lc,_leftChild,_lcSubState);
	if(rc=="")
	{
		_rightChild="";
		_rcSubState=-1;
	}
	else
		separateSubStatefromNT(rc,_rightChild,_rcSubState);	
	//cout<<"here"<<sc<<endl;
	_score=atof(sc.c_str());
	//cout<<"out"<<_score<<endl;
	return true;
}

string 
RuleFromFile::
toString()
{
	string result= _parent+"_"+intToString(_prSubState)+" -> "+_leftChild+"_"+intToString(_lcSubState)+" ";
	if(!isUnary())
		result+=_rightChild+"_"+intToString(_rcSubState)+" ";
	result+=doubleToString(_score);
	return result;
}


void
BerkeleyBinaryRule::
set(int lc, int rc, int pr, int lcNSub, int rcNSub, int prNSub)
{
	//cout<<lc<<","<<rc<<","<<pr<<","<<lcNSub<<","<<rcNSub<<","<<prNSub<<endl;
	_parentState=pr;
	_leftChildState=lc;
	_rightChildState=rc;
	_parentNumSubState=prNSub;
	_leftChildNumSubState=lcNSub;
	_rightChildNumSubState=rcNSub;

	_scores.reserve(lcNSub);
	for(int i=0;i<lcNSub;i++)
	{
		vector<vector<double> > tmp1;
		tmp1.clear();
		_scores.push_back(tmp1);
		_scores[_scores.size()-1].reserve(rcNSub);

		for(int j=0;j<rcNSub;j++)
		{
			vector<double> tmp2;
			tmp2.clear();
			_scores[_scores.size()-1].push_back(tmp2);
		}
	}

	for(int i=0;i<lcNSub;i++)
	{
		for(int j=0;j<rcNSub;j++)
		{
			_scores[i][j].reserve(prNSub);
			for(int k=0;k<prNSub;k++)
			{
				_scores[i][j].push_back(-1E10);
			}
		}
	}
}

string 
BerkeleyBinaryRule::
toString()const
{
	string result="";
	for(int i=0;i<_leftChildNumSubState;i++)
		for(int j=0;j<_rightChildNumSubState;j++)
			for(int k=0;k<_parentNumSubState;k++)
			{
				if(_scores[i][j][k]!=-1E10)
				result+=parentString()+"_"+intToString(k)+" -> "+leftChildString()+"_"+intToString(i)+" "
						+rightChildString()+"_"+intToString(j)+" "+doubleToString(_scores[i][j][k])+"\n";
			}
	return result;
}

void 
BerkeleyBinaryRule::
logrize()
{
	for(int i=0;i<_leftChildNumSubState;i++)
		for(int j=0;j<_rightChildNumSubState;j++)
			for(int k=0;k<_parentNumSubState;k++)
			{
				if(_scores[i][j][k]!=-1E10)
					_scores[i][j][k]=log(_scores[i][j][k])/log(2.0);
			}
}

/***********UnaryRule**************/
void
BerkeleyUnaryRule::
set(int ch, int pr, int chNSub, int prNSub)
{
	_parentState=pr;
	_childState=ch;
	_parentNumSubState=prNSub;
	_childNumSubState=chNSub;

	_scores.reserve(chNSub);
	for(int i=0;i<chNSub;i++)
	{
		vector<double> tmp1;
		tmp1.clear();
		_scores.push_back(tmp1);
	}

	for(int i=0;i<chNSub;i++)
	{
		_scores[i].reserve(prNSub);
		for(int k=0;k<prNSub;k++)
		{
			_scores[i].push_back(-1E10);
		}
	}
}

string 
BerkeleyUnaryRule::
toString()const
{
	string result="";
	for(int i=0;i<_childNumSubState;i++)
		for(int k=0;k<_parentNumSubState;k++)
		{
			if(_scores[i][k]!=-1E10)
			result+=parentString()+"_"+intToString(k)+" -> "+childString()+"_"+intToString(i)+" "
					+doubleToString(_scores[i][k])+"\n";
		}
	return result;
}

void 
BerkeleyUnaryRule::
logrize()
{
	for(int i=0;i<_childNumSubState;i++)
		for(int k=0;k<_parentNumSubState;k++)
		{
			if(_scores[i][k]!=-1E10)
				_scores[i][k]=log(_scores[i][k])/log(2.0);
		}
}


string 
BerkeleyUnaryRule::
toLexRuleString()const
{
	string result="";
	result+=parentString()+" "+childString()+" [";
	for(int i=0;i<_childNumSubState;i++)
		for(int k=0;k<_parentNumSubState;k++)
		{
			if(_scores[i][k]!=-1E10)
			{
				result+=doubleToString(_scores[i][k]);
				if(k!=_parentNumSubState-1)
					result+=", ";
				else result+="]";
			}
		}
	return result;
}
