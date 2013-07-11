#include "BerkeleyGrammar.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
//BerkeleyStateInfo BerkeleyGrammar::_stateInfo;

bool
BerkeleyGrammar::
addRuleFromFile(RuleFromFile& rff)
{
	if(rff.isUnary())
		addUnaryRuleFromFile(rff);
	else
		addBinaryRuleFromFile(rff);
	return true;
}

bool
BerkeleyGrammar::
addUnaryRuleFromFile(RuleFromFile& rff)
{
	string rightPart=rff.leftChild();
	string leftPart=rff.parent();
	if(_unaryRuleHouse[rightPart].find(leftPart)==_unaryRuleHouse[rightPart].end())
	{
		_unaryRuleHouse[rightPart][leftPart].set(rff.lcState(),
					rff.prState(),state2numSubState(rff.lcState()),state2numSubState(rff.prState()));
		_unaryRuleHouse[rightPart][leftPart].set(rff.leftChild(),rff.parent());
	}
	_unaryRuleHouse[rightPart][leftPart].setScore(rff.lcSubState(),rff.prSubState(),rff.score());
	return true;
}

bool
BerkeleyGrammar::
addBinaryRuleFromFile(RuleFromFile& rff)
{
	string rightPart=rff.leftChild()+" "+rff.rightChild();
	string leftPart=rff.parent();
	if(_binaryRuleHouse[rightPart].find(leftPart)==_binaryRuleHouse[rightPart].end())
	{
		_binaryRuleHouse[rightPart][leftPart].set(rff.lcState(),rff.rcState(),rff.prState(),
			state2numSubState(rff.lcState()),state2numSubState(rff.rcState()),state2numSubState(rff.prState()));
		_binaryRuleHouse[rightPart][leftPart].set(rff.leftChild(),rff.rightChild(),rff.parent());
	}
	_binaryRuleHouse[rightPart][leftPart].setScore(rff.lcSubState(),rff.rcSubState(),rff.prSubState(),rff.score());
	return true;
}

void 
BerkeleyGrammar::
print(string filename)const
{
	ofstream os(filename.c_str());
	print(os);
	os.close();
}


void 
BerkeleyGrammar::
print(ostream& os)const
{
	vector<string> toWrite;
	toWrite.clear();
	map<string, map<string, BerkeleyBinaryRule> >::const_iterator _binaryRuleHouseIter;
	map<string, map<string, BerkeleyUnaryRule> >::const_iterator _unaryRuleHouseIter;
	map<string, BerkeleyBinaryRule>::const_iterator bbrMapIter;
	map<string, BerkeleyUnaryRule>::const_iterator burMapIter;

	for(_binaryRuleHouseIter=_binaryRuleHouse.begin();_binaryRuleHouseIter!=_binaryRuleHouse.end();_binaryRuleHouseIter++)
	{
		const map<string, BerkeleyBinaryRule>& bbrMap=_binaryRuleHouseIter->second;
		for(bbrMapIter=bbrMap.begin();bbrMapIter!=bbrMap.end();bbrMapIter++)
			toWrite.push_back(bbrMapIter->second.toString());
	}
	for(_unaryRuleHouseIter=_unaryRuleHouse.begin();_unaryRuleHouseIter!=_unaryRuleHouse.end();_unaryRuleHouseIter++)
	{
		const map<string, BerkeleyUnaryRule>& burMap=_unaryRuleHouseIter->second;
		for(burMapIter=burMap.begin();burMapIter!=burMap.end();burMapIter++)
			toWrite.push_back(burMapIter->second.toString());
	}
	//sort(toWrite.begin(),toWrite.end());
	for(int i=0;(size_t)i<toWrite.size();i++)
		os<<toWrite[i];
	//os.close();

}
void 
BerkeleyGrammar::
print_by_ruleVector(string filename)const
{
	ofstream os(filename.c_str());
	vector<string> toWrite;
	toWrite.clear();
	int nos=numOfState();
	for(int i=0;i<nos;i++)
		for(int j=0;j<nos;j++)
			for(int k=0;k<nos;k++)
			{
				if(_binaryRuleByRule[i][j][k]!=NULL)
					toWrite.push_back(_binaryRuleByRule[i][j][k]->toString());
					//toWrite.push_back(getBinaryRuleByRule(i,j,k)->toString());
					
			}
	for(int i=0;i<nos;i++)
			for(int k=0;k<nos;k++)
			{
				if(_unaryRuleByRule[i][k]!=NULL)
					toWrite.push_back(_unaryRuleByRule[i][k]->toString());
					//toWrite.push_back(getUnaryRuleByRule(i,k)->toString());
			}
	sort(toWrite.begin(),toWrite.end());
	for(int i=0;(size_t)i<toWrite.size();i++)
		os<<toWrite[i];
	os.close();
}

void 
BerkeleyGrammar::
logrize()
{
	if(isLogrized())return;
	_logrized=true;

	int nos=numOfState();
	for(int i=0;i<nos;i++)
		for(int j=0;j<nos;j++)
			for(int k=0;k<nos;k++)
			{
				if(_binaryRuleByRule[i][j][k]!=NULL)
					_binaryRuleByRule[i][j][k]->logrize();
					//toWrite.push_back(getBinaryRuleByRule(i,j,k)->toString());
					
			}
	for(int i=0;i<nos;i++)
			for(int k=0;k<nos;k++)
			{
				if(_unaryRuleByRule[i][k]!=NULL)
					_unaryRuleByRule[i][k]->logrize();
					//toWrite.push_back(getUnaryRuleByRule(i,k)->toString());
			}
}

void 
BerkeleyGrammar::
initRuleInVector()
{
	int nos=numOfState();
	_binaryRuleByRule.clear();
	_binaryRuleByRule.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<vector<BerkeleyBinaryRule*> > tmp1;
		_binaryRuleByRule.push_back(tmp1);
		_binaryRuleByRule[i].clear();
		_binaryRuleByRule[i].reserve(nos);
		for(int j=0;j<nos;j++)
		{
			vector<BerkeleyBinaryRule*> tmp2;
			_binaryRuleByRule[i].push_back(tmp2);
			_binaryRuleByRule[i][j].clear();
			_binaryRuleByRule[i][j].reserve(nos);
			for(int k=0;k<nos;k++)
			{
				_binaryRuleByRule[i][j].push_back(NULL);
			}
		}
	}
//binary by childs
	_binaryRuleByChilds.clear();
	_binaryRuleByChilds.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<vector<BerkeleyBinaryRule*> > tmp1;
		_binaryRuleByChilds.push_back(tmp1);
		_binaryRuleByChilds[i].clear();
		_binaryRuleByChilds[i].reserve(nos);
		for(int j=0;j<nos;j++)
		{
			vector<BerkeleyBinaryRule*> tmp2;
			_binaryRuleByChilds[i].push_back(tmp2);
			_binaryRuleByChilds[i][j].clear();
			_binaryRuleByChilds[i][j].reserve(nos);
			
			for(int k=0;k<nos;k++)
			{
			//	_binaryRuleByChilds[i][j].push_back(NULL);
			}
		}
	}

//binary rule by parent
	_binaryRuleByParent.clear();
	_binaryRuleByParent.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<BerkeleyBinaryRule*> tmp;
		_binaryRuleByParent.push_back(tmp);
		_binaryRuleByParent[i].clear();
	}

//binary rule by parent and left child
	_binaryRuleByParentLeftChild.clear();
	_binaryRuleByParentLeftChild.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<vector<BerkeleyBinaryRule*> > tmp1;
		_binaryRuleByParentLeftChild.push_back(tmp1);
		_binaryRuleByParentLeftChild[i].clear();
		_binaryRuleByParentLeftChild[i].reserve(nos);
		for(int j=0;j<nos;j++)
		{
			vector<BerkeleyBinaryRule*> tmp2;
			_binaryRuleByParentLeftChild[i].push_back(tmp2);
			_binaryRuleByParentLeftChild[i][j].clear();
			_binaryRuleByParentLeftChild[i][j].reserve(nos);
		}
	}

	_leftChildsByParent.clear();
	_leftChildsByParent.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<int> tmp;
		_leftChildsByParent.push_back(tmp);
		_leftChildsByParent[i].clear();
		_leftChildsByParent[i].reserve(nos);
	}

//unary rule
	_unaryRuleByRule.clear();
	_unaryRuleByRule.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<BerkeleyUnaryRule*> tmp1;
		_unaryRuleByRule.push_back(tmp1);
		_unaryRuleByRule[i].clear();
		_unaryRuleByRule[i].reserve(nos);
		for(int k=0;k<nos;k++)
		{
			_unaryRuleByRule[i].push_back(NULL);
		}
	}
//unary by childs
	_unaryRuleByChild.clear();
	_unaryRuleByChild.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<BerkeleyUnaryRule*> tmp1;
		_unaryRuleByChild.push_back(tmp1);
		_unaryRuleByChild[i].clear();
		_unaryRuleByChild[i].reserve(nos);
		for(int k=0;k<nos;k++)
		{
			//_unaryRuleByChild[i].push_back(NULL);
		}
	}

//unary rule by parent
	_unaryRuleByParent.clear();
	_unaryRuleByParent.reserve(nos);
	for(int i=0;i<nos;i++)
	{
		vector<BerkeleyUnaryRule*> tmp;
		_unaryRuleByParent.push_back(tmp);
		_unaryRuleByParent[i].clear();
	}
}

void 
BerkeleyGrammar::
setRuleIntoVector()
{
	initRuleInVector();

	map<string, map<string, BerkeleyBinaryRule> >::iterator _binaryRuleHouseIter;
	map<string, map<string, BerkeleyUnaryRule> >::iterator _unaryRuleHouseIter;
	map<string, BerkeleyBinaryRule>::iterator bbrMapIter;
	map<string, BerkeleyUnaryRule>::iterator burMapIter;

	for(_binaryRuleHouseIter=_binaryRuleHouse.begin();_binaryRuleHouseIter!=_binaryRuleHouse.end();_binaryRuleHouseIter++)
	{
		map<string, BerkeleyBinaryRule>& bbrMap=_binaryRuleHouseIter->second;
		for(bbrMapIter=bbrMap.begin();bbrMapIter!=bbrMap.end();bbrMapIter++)
		{
			BerkeleyBinaryRule* p_bbr= &(bbrMapIter->second);
			int pr,lc,rc;
			pr=p_bbr->parentState();lc=p_bbr->leftChildState();rc=p_bbr->rightChildState();
			_binaryRuleByRule[lc][rc][pr]=p_bbr;
		}
	}
//binary rule by childs
	for(_binaryRuleHouseIter=_binaryRuleHouse.begin();_binaryRuleHouseIter!=_binaryRuleHouse.end();_binaryRuleHouseIter++)
	{
		map<string, BerkeleyBinaryRule>& bbrMap=_binaryRuleHouseIter->second;
		for(bbrMapIter=bbrMap.begin();bbrMapIter!=bbrMap.end();bbrMapIter++)
		{
			BerkeleyBinaryRule* p_bbr= &(bbrMapIter->second);
			int pr,lc,rc;
			pr=p_bbr->parentState();lc=p_bbr->leftChildState();rc=p_bbr->rightChildState();
			_binaryRuleByChilds[lc][rc].push_back(p_bbr);
		}
	}

//binary rule by parent
	for(_binaryRuleHouseIter=_binaryRuleHouse.begin();_binaryRuleHouseIter!=_binaryRuleHouse.end();_binaryRuleHouseIter++)
	{
		map<string, BerkeleyBinaryRule>& bbrMap=_binaryRuleHouseIter->second;
		for(bbrMapIter=bbrMap.begin();bbrMapIter!=bbrMap.end();bbrMapIter++)
		{
			BerkeleyBinaryRule* p_bbr= &(bbrMapIter->second);
			if(p_bbr==NULL)continue;
			int pr,lc,rc;
			pr=p_bbr->parentState();lc=p_bbr->leftChildState();rc=p_bbr->rightChildState();
			_binaryRuleByParent[pr].push_back(p_bbr);
		}
	}

//binary rule by parent and leftChild
	vector<map<int,int> > lcMaps;
	for(int i=0;i<numOfState();i++)
	{
		map<int,int> tmp;
		lcMaps.push_back(tmp);
		lcMaps[i].clear();
	}
	for(_binaryRuleHouseIter=_binaryRuleHouse.begin();_binaryRuleHouseIter!=_binaryRuleHouse.end();_binaryRuleHouseIter++)
	{
		map<string, BerkeleyBinaryRule>& bbrMap=_binaryRuleHouseIter->second;
		for(bbrMapIter=bbrMap.begin();bbrMapIter!=bbrMap.end();bbrMapIter++)
		{
			BerkeleyBinaryRule* p_bbr= &(bbrMapIter->second);
			if(p_bbr==NULL)continue;
			int pr,lc,rc;
			pr=p_bbr->parentState();lc=p_bbr->leftChildState();rc=p_bbr->rightChildState();
			_binaryRuleByParentLeftChild[pr][lc].push_back(p_bbr);
			lcMaps[pr][lc]=1;
		}
	}
	for(int i=0;i<numOfState();i++)
	{
		map<int,int>& tmp=lcMaps[i];
		for(map<int,int>::iterator iter=tmp.begin();iter!=tmp.end();iter++)
		{
			_leftChildsByParent[i].push_back(iter->first);
		}
	}
	
//unary rule
	for(_unaryRuleHouseIter=_unaryRuleHouse.begin();_unaryRuleHouseIter!=_unaryRuleHouse.end();_unaryRuleHouseIter++)
	{
		map<string, BerkeleyUnaryRule>& burMap=_unaryRuleHouseIter->second;
		for(burMapIter=burMap.begin();burMapIter!=burMap.end();burMapIter++)
		{
			BerkeleyUnaryRule* p_bur=&(burMapIter->second);
			int pr,ch;
			pr=p_bur->parentState();ch=p_bur->childState();
			_unaryRuleByRule[ch][pr]=p_bur;
		}
	}
//unary by childs
	for(_unaryRuleHouseIter=_unaryRuleHouse.begin();_unaryRuleHouseIter!=_unaryRuleHouse.end();_unaryRuleHouseIter++)
	{
		map<string, BerkeleyUnaryRule>& burMap=_unaryRuleHouseIter->second;
		for(burMapIter=burMap.begin();burMapIter!=burMap.end();burMapIter++)
		{
			BerkeleyUnaryRule* p_bur=&(burMapIter->second);
			int pr,ch;
			pr=p_bur->parentState();ch=p_bur->childState();
			_unaryRuleByChild[ch].push_back(p_bur);
		}
	}
//unary by parent
	for(_unaryRuleHouseIter=_unaryRuleHouse.begin();_unaryRuleHouseIter!=_unaryRuleHouse.end();_unaryRuleHouseIter++)
	{
		map<string, BerkeleyUnaryRule>& burMap=_unaryRuleHouseIter->second;
		for(burMapIter=burMap.begin();burMapIter!=burMap.end();burMapIter++)
		{
			BerkeleyUnaryRule* p_bur=&(burMapIter->second);
			if(p_bur==NULL)continue;
			int pr,ch;
			pr=p_bur->parentState();ch=p_bur->childState();
			_unaryRuleByParent[pr].push_back(p_bur);
		}
	}
}

void
BerkeleyGrammar::
init(string filename,BerkeleyStateInfoOfOneLevel& bsio)
{
	set_stateInfoOfcurLevel(bsio);

	ifstream is(filename.c_str());
	while(!is.eof())
	{
		string curLine;
		getline(is,curLine);
		if(curLine=="")continue;
		RuleFromFile rff;
		rff.convertFromString(curLine);
		rff.setState(term2state());
		addRuleFromFile(rff);
		//cout<<rff.toString()<<endl;
	}
	setRuleIntoVector();
}
