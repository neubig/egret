#include "BerkeleyStateInfo.h"

void
BerkeleyStateInfo::
init(string filename)
{
	ifstream is(filename.c_str());
	_splitTrees.clear();
	while(!is.eof())
	{
		string curline;
		getline(is,curline);
		if(curline=="")continue;
		stringstream ss(curline);
		string term;
		int state,numSubState;
		ss>>term>>state>>numSubState;
		_term2state.add(term,state);
		_state2numSubState[state]=numSubState;
		getline(ss,curline);
		_splitTrees[state].readNormalTreeFromString(curline);
	}
	_splitTreesNodes.init(_splitTrees);
}

void 
BerkeleyStateInfo::
print(string filename)
{
	ofstream os(filename.c_str());
	print(os);
	os.close();
}

void 
BerkeleyStateInfo::
print(ostream& os)
{
	for(int i=0;(size_t)i<_term2state.size();i++)
	{
		string term;
		_term2state.queryY2X(i,term);
		os<<term<<"\t"<<i<<"\t"<<_state2numSubState[i]<<" ";
		_splitTrees[i].print(os);
		os<<endl;
	}
	//_splitTreesNodes.print(cout);
}
int 
BerkeleyStateInfo::
state2numSubState(int state)const
{
	int result=-1; 
	if(_state2numSubState.find(state)!=_state2numSubState.end())
		result=_state2numSubState.find(state)->second;
	return result;
}

/***************************************************/
void 
SplitTreesNodes::
init(map<int,Tree>& _splitTrees)
{
	SplitTreesNodes	state2level2id;
	map<int,Tree>::iterator stIter=_splitTrees.begin();
	for(;stIter!=_splitTrees.end();stIter++)
	{
		int state=stIter->first;
		Tree& tree=stIter->second;
		tree.cat_nodeInLevelAndID(state2level2id[state]);
	}
	SplitTreesNodes::iterator stnIter;
	NodeInLevelAndID::iterator nlaIter;
	for(stnIter=state2level2id.begin();stnIter!=state2level2id.end();stnIter++)
	{
		NodeInLevelAndID& nla=stnIter->second;
		for(nlaIter=nla.begin();nlaIter!=nla.end();nlaIter++)
		{
			(*this)[nlaIter->first][stnIter->first]=nlaIter->second;
		}
	}
}

void
SplitTreesNodes::
print(ostream& os)
{
	SplitTreesNodes state2level2id;
	SplitTreesNodes::iterator stnIter;
	NodeInLevelAndID::iterator nlaIter;

	state2level2id.clear();
	for(stnIter=begin();stnIter!=end();stnIter++)
	{
		NodeInLevelAndID& nla=stnIter->second;
		for(nlaIter=nla.begin();nlaIter!=nla.end();nlaIter++)
		{
			state2level2id[nlaIter->first][stnIter->first]=nlaIter->second;
		}
	}
	state2level2id.print_state2level2id(os);
}

void 
SplitTreesNodes::
gen_state2numSubState(State2numSubState& state2numSubState, int level)const
{
	state2numSubState.clear();
	const NodeInStateAndSubState* p_nodeInStateAndSubState=NULL;
	if((*this).find(level)!=this->end())
	{
		p_nodeInStateAndSubState=&((*this).find(level)->second);
		NodeInStateAndSubState::const_iterator tmpIter;
		for(tmpIter=p_nodeInStateAndSubState->begin();tmpIter!=p_nodeInStateAndSubState->end();tmpIter++)
		{
			state2numSubState[tmpIter->first]=(int)tmpIter->second.size();
		}
	}
}

/******************BerkeleyStateInfoOfOneLevel******************/

int 
BerkeleyStateInfoOfOneLevel::
term2state(string term)const
{
	int result=-1;
	_term2state.queryX2Y(term,result);
	return result;
}

int
BerkeleyStateInfoOfOneLevel::
state2numSubState(int state)const
{
	int result=-1;
	if(_state2numSubState.find(state)!=_state2numSubState.end())
		result=_state2numSubState.find(state)->second;
	return result;
}

void 
BerkeleyStateInfoOfOneLevel::
set_value_by_stateInfo(const BerkeleyStateInfo& stateInfo, int level)
{
	const SplitTreesNodes& splitTreesNodes=stateInfo.splitTreesNodes();
	
	if(splitTreesNodes.find(level)!=splitTreesNodes.end())
	{
			_term2state.clear();
			_nodeInStateAndSubState.clear();
			_state2numSubState.clear();
			_level=level;
		(splitTreesNodes.find(level)->second).clone(_nodeInStateAndSubState);
		(stateInfo.term2state()).clone(_term2state);		
		splitTreesNodes.gen_state2numSubState(_state2numSubState,level);
	}

}


void 
BerkeleyStateInfoOfOneLevel::
print(ostream& os)const
{
	for(int i=0;(size_t)i<_term2state.size();i++)
	{
		string term;
		int nos;
		_term2state.queryY2X(i,term);
		
		if(_nodeInStateAndSubState.find(i)!=_nodeInStateAndSubState.end())
		{
			const map<int,Tree*>& tempMap=_nodeInStateAndSubState.find(i)->second;
			nos=(int)tempMap.size();
			os<<term<<" "<<i<<" "<<nos;
			map<int,Tree*>::const_iterator iter=tempMap.begin();
			for(;iter!=tempMap.end();iter++)
			{
				os<<" "<<iter->first;
			}
			os<<endl;
		}
		else
			cerr<<"error when print BerkeleyStateInfoOfOneLevel"<<endl;
	}
}

void 
BerkeleyStateInfoOfOneLevel::
clone_from(const BerkeleyStateInfoOfOneLevel& father)
{
	_term2state.clear();
	_nodeInStateAndSubState.clear();
	_state2numSubState.clear();
	_level=father.level();
	
	father.term2state().clone(_term2state);
	father.nodeInStateAndSubState().clone(_nodeInStateAndSubState);
	father.state2numSubState().clone(_state2numSubState);
}

const void 
BerkeleyStateInfoOfOneLevel::
query_substate2sun(int state, int subState, vector<int>& sun)const
{
	sun.clear();
	Tree* tree=NULL;
	if(_nodeInStateAndSubState.find(state)!=_nodeInStateAndSubState.end())
	{
		const map<int,Tree*> tmp=_nodeInStateAndSubState.find(state)->second;
		if(tmp.find(subState)!=tmp.end())
		{
			tree=tmp.find(subState)->second;
			tree->get_sunIDs(sun);
		}
	}
}

const int 
BerkeleyStateInfoOfOneLevel::
query_substate2father(int state, int subState)const
{
	int father=-1;
	Tree* tree=NULL;
	if(_nodeInStateAndSubState.find(state)!=_nodeInStateAndSubState.end())
	{
		const map<int,Tree*> tmp=_nodeInStateAndSubState.find(state)->second;
		if(tmp.find(subState)!=tmp.end())
		{
			tree=tmp.find(subState)->second;
			father=tree->get_fatherID();
		}
	}
	return father;
}
