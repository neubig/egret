#include "Node.h"
#include <math.h>
#include "utils.h"
/****************************Node************************************/

void 
Node::
print(ostream& os)
{
	for(Node::iterator niter=begin();niter!=end();niter++)
	{
		os<<niter->second.nt()<<":";
		double score;
		niter->second.getNthBest(1,score);
		os<<score<<endl;
	}
}

void 
Node::
print_inside(ostream& os)
{
	os<<"node ["<<start()<<","<<stop()<<"]"<<endl;
	for(Node::iterator niter=begin();niter!=end();niter++)
	{
		os<<niter->second.nt()<<":"<<niter->second.insideProb()<<endl;
	}
}


void 
Node::
print(jstream& os)
{
	for(Node::iterator niter=begin();niter!=end();niter++)
	{
		niter->second.print(cout);
		//double score;
		//niter->second.getNthBest(1,score);
		//os<<score<<endl;
	}
}


void 
Node::
print_hyperedges(ostream& os)const
{
	for(unsigned int i=0;i<_uio.size();i++)
	{
		if((*this).find(_uio[i])==(*this).end())cerr<<"error here, did not find "<<_uio[i]<<endl;
		const Label& lb=(*this).find(_uio[i])->second;
		lb.print_hyperedges(os);
	}
}


//#define DEBUG
#ifdef DEBUG
vector<LabelAndFamily>& 
Node::
setUnaryInOrder(RuleSet& ruleSet)
{
	_unaryInOrder.clear();
	
	//initialize n2c
	map<string,AncDes> n2c;
	n2c.clear();
	for(iterator iter=begin();iter!=end();iter++)
	{
		n2c[iter->first]._ancestor.clear();
		n2c[iter->first]._descendant.clear();
	}

	bool mark=true;

	//loop
	//while(mark)
	{
		mark=false;
		vector<string> curNts;
		curNts.clear();
		map<string,AncDes>::iterator n2cIter=n2c.begin();
		for(n2cIter=n2c.begin();n2cIter!=n2c.end();n2cIter++)
		{
			curNts.push_back(n2cIter->first);
		}
		
		for(int i=0;i<curNts.size();i++)
		{
			string rightPart=curNts[i];
			if(ruleSet.count(rightPart)>0)
			{
				//cerr<<rightPart<<endl;
				int count=0;
				pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
				for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
				{
					string leftPart=iter->second.left;
					if(n2c.find(leftPart)==n2c.end())curNts.push_back(leftPart);
					if(n2c[rightPart]._descendant.count(leftPart)>0)continue;
					if(n2c[rightPart]._ancestor.count(leftPart)>0)continue;
					
					// update leftPart and rightPart separately
					n2c[leftPart]._descendant.insert(rightPart);
					n2c[leftPart]._descendant.insert(
						n2c[rightPart]._descendant.begin(),n2c[rightPart]._descendant.end());
					n2c[rightPart]._ancestor.insert(leftPart);
					n2c[rightPart]._ancestor.insert(
						n2c[leftPart]._ancestor.begin(),n2c[leftPart]._ancestor.end());

					//update leftPart ancestors' descendant
					
					set<string>& anc=n2c[leftPart]._ancestor;
					set<string>::iterator ancIter=anc.begin();
					for(;ancIter!=anc.end();ancIter++)
					{
						n2c[*ancIter]._descendant.insert(
							n2c[leftPart]._descendant.begin(),n2c[leftPart]._descendant.end());
					}

					//update rightPart descendants' ancestor

					set<string>& des=n2c[rightPart]._descendant;
					set<string>::iterator desIter=des.begin();
					for(;desIter!=des.end();desIter++)
					{
						n2c[*desIter]._ancestor.insert(
							n2c[rightPart]._ancestor.begin(),n2c[rightPart]._ancestor.end());
					}
					//cout<<"here:"<<leftPart<<"-->"<<rightPart<<endl;
					mark=true;
				}
			}
		}

	}

		map<string,AncDes>::iterator n2cIter=n2c.begin();
		for(n2cIter=n2c.begin();n2cIter!=n2c.end();n2cIter++)
		{
			LabelAndFamily laf;
			laf.term()=n2cIter->first;
			laf.ancestor().insert(n2cIter->second._ancestor.begin(),n2cIter->second._ancestor.end());
			laf.descendant().insert(n2cIter->second._descendant.begin(),n2cIter->second._descendant.end());
			unaryInOrder().push_back(laf);
		
		}
		sort(unaryInOrder().begin(),unaryInOrder().end(),comp_laf);
		for(int i=0;i<unaryInOrder().size();i++)
		{
			unaryInOrder()[i].print(cout);
		}
	return unaryInOrder();
}
#else
vector<LabelAndFamily>& 
Node::
setUnaryInOrder(RuleSet& ruleSet)
{
	_unaryInOrder.clear();
	
	//initialize n2c
	map<string,AncDes> n2c;
	n2c.clear();
	//cout<<"initializing:"<<endl;
	for(iterator iter=begin();iter!=end();iter++)
	{
		n2c[iter->first]._ancestor.clear();
		n2c[iter->first]._descendant.clear();
		//cout<<iter->first<<":"<<iter->second.start()<<","<<iter->second.stop()<<endl;
	}

	bool mark=true;

	//loop
	//while(mark)
	{
		mark=false;
		vector<string> curNts;
		curNts.clear();
		map<string,AncDes>::iterator n2cIter=n2c.begin();
		for(n2cIter=n2c.begin();n2cIter!=n2c.end();n2cIter++)
		{
			curNts.push_back(n2cIter->first);
		}
		
		for(size_t i=0;i<curNts.size();i++)
		{
			string rightPart=curNts[i];
			if(ruleSet.count(rightPart)>0)
			{
				int count=0;
				pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
				for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
				{
					string leftPart=iter->second.left;
					if(n2c.find(leftPart)==n2c.end())curNts.push_back(leftPart);
					if(n2c[rightPart]._descendant.count(leftPart)>0)continue;
					if(n2c[rightPart]._ancestor.count(leftPart)>0)continue;
					
					// update leftPart and rightPart separately
					n2c[leftPart]._descendant.insert(rightPart);
					n2c[leftPart]._descendant.insert(
						n2c[rightPart]._descendant.begin(),n2c[rightPart]._descendant.end());
					n2c[rightPart]._ancestor.insert(leftPart);
					n2c[rightPart]._ancestor.insert(
						n2c[leftPart]._ancestor.begin(),n2c[leftPart]._ancestor.end());

					//update leftPart ancestors' descendant
					
					set<string>& anc=n2c[leftPart]._ancestor;
					set<string>::iterator ancIter=anc.begin();
					for(;ancIter!=anc.end();ancIter++)
					{
						n2c[*ancIter]._descendant.insert(
							n2c[leftPart]._descendant.begin(),n2c[leftPart]._descendant.end());
					}

					//update rightPart descendants' ancestor

					set<string>& des=n2c[rightPart]._descendant;
					set<string>::iterator desIter=des.begin();
					for(;desIter!=des.end();desIter++)
					{
						n2c[*desIter]._ancestor.insert(
							n2c[rightPart]._ancestor.begin(),n2c[rightPart]._ancestor.end());
					}
					//cout<<"here:"<<leftPart<<"-->"<<rightPart<<endl;
					mark=true;
				}
			}
		}

	}

		map<string,AncDes>::iterator n2cIter=n2c.begin();
		for(n2cIter=n2c.begin();n2cIter!=n2c.end();n2cIter++)
		{
			LabelAndFamily laf;
			laf.term()=n2cIter->first;
			laf.ancestor().insert(n2cIter->second._ancestor.begin(),n2cIter->second._ancestor.end());
			laf.descendant().insert(n2cIter->second._descendant.begin(),n2cIter->second._descendant.end());
			unaryInOrder().push_back(laf);
		
		}
		sort(unaryInOrder().begin(),unaryInOrder().end(),comp_laf);
		for(size_t i=0;i<unaryInOrder().size();i++)
		{
			//unaryInOrder()[i].print(cout);
		}
	return unaryInOrder();
}
#endif

vector<string>& 
Node::
setUnaryInOrder(RuleSet& ruleSet, vector<string>& ntPool)
{
	ntPool.clear();
	if(unary_fine)
	{
		setUnaryInOrder(ruleSet);
		for(size_t i=0;i<unaryInOrder().size();i++)
		{
			ntPool.push_back(unaryInOrder()[i].term());
		}
	}
	else
	{
		for(iterator iter=begin();iter!=end();iter++)
		{
			ntPool.push_back(iter->first);
		}
	}
	_uio=ntPool;
	return ntPool;
}

vector<string>& 
Node::
setUnaryInOrder_simple()
{
	vector<string> ntPool;
	ntPool.clear();
	for(iterator iter=begin();iter!=end();iter++)
	{
		ntPool.push_back(iter->first);
	}
	map<string,int> uio_map;
	for(size_t i=0;i<_uio.size();i++)
		uio_map[_uio[i]]=1;
	for(size_t i=0;i<ntPool.size();i++)
	{
		if(uio_map.find(ntPool[i])==uio_map.end())
			_uio.push_back(ntPool[i]);
	}
	return _uio;
}

vector<string>& 
Node::
setUnaryInOrder_berkeley(const BerkeleyRuleSet& brSet, vector<string>& ntPool)
{
	ntPool.clear();
	for(iterator iter=begin();iter!=end();iter++)
	{
		ntPool.push_back(iter->first);
	}
	_uio=ntPool;
	return ntPool;
}

typedef struct 
{
	int curState;
	int childState;
	int hierarchy;
	double maxScore;
}StateAndScore;

bool operator < (const StateAndScore& sas1, const StateAndScore& sas2){return sas1.hierarchy<sas2.hierarchy;}

vector<string>& 
Node::
setUnaryInOrder_objectFunction_berkeley(
			const BerkeleyRuleSet& ruleSet, 
			const vector<vector<double> >& tempUnraryRuleSet, 
			const vector<int>& allowStates,
			vector<string>& ntPool,
			ObjectFunctionType oft)
{
	//cerr<<"here0"<<endl;
	vector<int> currentStates;
	vector<StateAndScore> currentStatesMap;
	for(int i=0;(size_t)i<tempUnraryRuleSet.size();i++)
	{
		StateAndScore sas;
		sas.childState=sas.hierarchy=-1;
		sas.maxScore=-1E10;
		sas.curState=i;
		currentStatesMap.push_back(sas);
	}
	//cerr<<"here0.5"<<endl;
	for(iterator iter=begin();iter!=end();iter++)
	{
		int state=iter->second.state();
		currentStates.push_back(state);
		//cerr<<state<<","<<currentStatesMap.size()<<endl;
		double score=-1;
		if(!iter->second.getNthBest(1,score)) 
			error("cannot get 1 best score");
		iter->second.regenerate_nbest();
		currentStatesMap[state].maxScore=score;
		
		if(oft==MAX_RULE_SUM)
			currentStatesMap[state].maxScore=log(2,currentStatesMap[state].maxScore);		
	}
	//cerr<<"here1"<<endl;
	bool genNewState=false;
	int run=0;
	do
	{
		genNewState=false;
		//cerr<<run++<<" run"<<endl;
		//if(run>3)break;
		for(int i=0;(size_t)i<currentStates.size();i++)
		{
			int childState=currentStates[i];
			for(int j=0;(size_t)j<allowStates.size();j++)
			{
				int parentState=allowStates[j];
				double ruleScore=tempUnraryRuleSet[parentState][childState];
				if(ruleScore>0)
				{			
					ruleScore=log(2,ruleScore);
					if(ruleScore>0)	
						ruleScore=0;
					
					double gScore=ruleScore+currentStatesMap[childState].maxScore;

					if(currentStatesMap[parentState].maxScore==-1E10)
						currentStates.push_back(parentState);
					if(oft!=MAX_RULE_SUM)
					if(gScore>0||currentStatesMap[parentState].maxScore>0) 
					{
						cerr<<"error here"<<gScore<<","<<currentStatesMap[parentState].maxScore<<endl;
						exit(1);
					}
					if(currentStatesMap[parentState].maxScore-gScore<0&&fabs(currentStatesMap[parentState].maxScore-gScore)>1E-10)
					{
						//cerr<<"state "<<childState<<":"<<currentStatesMap[childState].maxScore<<"+"<<ruleScore<<"="<<gScore<<" > "<<currentStatesMap[parentState].maxScore<<endl;
						//cerr<<gScore<<" - "<<currentStatesMap[parentState].maxScore<<"="<<gScore - currentStatesMap[parentState].maxScore<<endl;
						currentStatesMap[parentState].childState=childState;
						currentStatesMap[parentState].maxScore=gScore;
						genNewState=true;
					}
				}
			}
		}
	}while(genNewState);
	//cerr<<"here2"<<endl;

	bool continueMark=false;
	do{
		continueMark=false;
		for(int i=0;(size_t)i<currentStates.size();i++)
		{
			int curState=currentStates[i];
			if(currentStatesMap[curState].hierarchy>=0)
				continue;
			if(currentStatesMap[curState].childState==-1)
			{
				currentStatesMap[curState].hierarchy=0;
				continueMark=true;
			}
			else
			{
				int childState=currentStatesMap[curState].childState;
				if(currentStatesMap[childState].hierarchy>=0)
				{
					currentStatesMap[curState].hierarchy=currentStatesMap[childState].hierarchy+1;
					continueMark=true;
				}
			}
		}
	}while(continueMark);
		
	vector<StateAndScore> finalStatesMap;
	finalStatesMap.clear();

	for(int i=0;(size_t)i<currentStates.size();i++)
	{
		int curState=currentStates[i];
		finalStatesMap.push_back(currentStatesMap[curState]);
	}
	sort(finalStatesMap.begin(),finalStatesMap.end());
	for(int i=0;(size_t)i<finalStatesMap.size();i++)
	{
		ntPool.push_back(ruleSet.state2term(finalStatesMap[i].curState));
	}
	_uio=ntPool;
	return ntPool;
}

int 
Node::
compute_children_outside(string S1)
{

	set<string> usedUnaryLabelSet;
	usedUnaryLabelSet.clear();
	//cout<<"count the order list"<<endl;
	for(int i=(int)_unaryInOrder.size()-1;i>=0;i--)
	{
		string name=_unaryInOrder[i].term();
		if(name==S1)continue;
		//cerr<<_unaryInOrder[i].descendant().size()<<endl;
		//cout<<name<<endl;
		(*this)[name].compute_children_outside();
		usedUnaryLabelSet.insert(name);
	}

	//cout<<"count the others\n"<<endl;
	iterator iter=begin();
	for(;iter!=end();iter++)
	{
		if(usedUnaryLabelSet.find(iter->second.nt())==usedUnaryLabelSet.end())
		{
			if(iter->first==S1)continue;
			iter->second.compute_children_outside();
		}
	}
	return (int)size();
}

int 
Node::
collect_rule_expectation(double s1InProb, ERSet& erSet)
{
	iterator iter=begin();
	for(;iter!=end();iter++)
	{
		iter->second.collect_rule_expectation(s1InProb,erSet);
	}
	return (int)size();
}

bool 
Node::
replace_interLabel(InterType it)
{
	setUnaryInOrder_simple();
	for(unsigned int i=0;i<_uio.size();i++)
	{
		(*this)[_uio[i]].replace_interLabel(it);
	}
	return true;
}


const AllowStateInfo& 
Node::
reset_allowStateInfo_to_nextLevel(const BerkeleyStateInfoOfOneLevel& bsio)
{
	//cout<<"node ["<<start()<<","<<stop()<<"]"<<endl;
	//cout<<"before reset"<<endl;
	//_asi.print(cout);
	_asi.reset_allowStateInfo_to_nextLevel(bsio);
/*	cout<<"after reset"<<endl;
	_asi.print(cout);*/
	return _asi;
}

void 
Node::
prune_and_setAllowState_byLabel(double s1InProb, double threshold)
{
	vector<pair<int,int> > filtered;
	//cout<<"node ["<<start()<<","<<stop()<<"]"<<endl;
	_asi.clear();
	_asi.set_inUse(true);

	vector<pair<int,string> > toPrint;
	toPrint.clear();

	iterator iter=begin();
	for(;iter!=end();iter++)
	{
		Label& label=iter->second;
		double inside=label.insideProb();
		double outside=label.outsideProb();
		double score=inside*outside/s1InProb;
		string scd="";
		int fst=label.state()*100+label.subState();
		stringstream ss;
		ss<<"i:"<<log(inside)<<" o:"<<log(outside)<<" s:"<<log(s1InProb)<<" ";
		ss<<log(score)<<" "<<log(threshold)<<"["<<label.state()<<" "<<label.subState()<<"]"<<endl;
		
		getline(ss,scd);
		if(outside>0)
		toPrint.push_back(make_pair(fst,scd));
		//cout<<"i:"<<log(inside)<<" o:"<<log(outside)<<" s:"<<log(s1InProb)<<" ";
		//cout<<log(score)<<" "<<log(threshold)<<"["<<label.state()<<" "<<label.subState()<<"]"<<endl;
		if(score>threshold)
		{
			_asi.add(label.state(),label.subState());
		}
		else
			filtered.push_back(make_pair(label.state(),label.subState()));
	}
	clear();
	//_asi.set_inVector();
	sort(toPrint.begin(),toPrint.end());
	for(int i=0;(size_t)i<toPrint.size();i++)
		cout<<toPrint[i].second<<endl;
	/*
	cout<<"filtered"<<endl;
	for(int i=0;i<filtered.size();i++)
	{
		cout<<filtered[i].first<<" "<<filtered[i].second<<endl; 
	}*/
}

void
Node::
reset_scorePool(const BerkeleyStateInfoOfOneLevel& bsio)
{
	_ioScores.clear();
	int numofState=(int)bsio.numOfState();
	_ioScores.reserve(numofState);
	for(int i=0;i<numofState;i++)
	{
		vector<IOScore> temp;
		_ioScores.push_back(temp);
		int numOfsubState=bsio.state2numSubState(i);
		for(int j=0;j<numOfsubState;j++)
		{
			IOScore sc;
			_ioScores[i].push_back(sc);
			_ioScores[i][j].is=_ioScores[i][j].os=_ioScores[i][j].sc=-1E10;
		}
	}
}

const AllowStateInfo& 
Node::
set_allowStateAndSubState_withoutLabel(double s1InProb, double threshold, double inside)
{
				
	_asi.clear();
	_asi.set_inUse(true);
	for(int i=0;(size_t)i<_ioScores.size();i++)
	{
		for(int j=0;(size_t)j<_ioScores[i].size();j++)
		{
			if(inside>=0)
			{
				if(_ioScores[i][j].is>inside)
				{
					_asi.add(i,j);
					//cout<<i<<","<<j<<endl;
				}
			}
			else
			{
				if(_ioScores[i][j].is<=0||_ioScores[i][j].os<=0)continue;
				_ioScores[i][j].sc=_ioScores[i][j].is*_ioScores[i][j].os/s1InProb;
				if(log(_ioScores[i][j].sc)>threshold)
					_asi.add(i,j);
				//cout.precision(14);
				//cout.setf(ios::fixed,ios::floatfield);
	//			cout<<"i:"<<log(_ioScores[i][j].is)<<" o:"<<log(_ioScores[i][j].os)<<" s:"
	//				<<log(s1InProb)<<" "<<log(_ioScores[i][j].sc)<<" ";
				//cout.precision(2);
				//cout.setf(ios::fixed,ios::floatfield);
	//			cout<<log(threshold)<<" [ "<<i<<" ]"<<endl;
			}
		}
	}
	//if(inside<0)
	//	_asi.print(cout);
	_asi.set_inVector();
	return _asi;
}

void 
Node::
print_io_withoutLabel(ostream& os)
{
	for(int i=0;(size_t)i<_ioScores.size();i++)
	{
		for(int j=0;(size_t)j<_ioScores[i].size();j++)
		{
			if(_ioScores[i][j].is>0)
			{
				os<<_ioScores[i][j].is<<","<<_ioScores[i][j].os<<endl;
			}
		}
	}
}

const vector<int>& 
Node::
set_allowState_withoutLabel_for_initialLevel(double s1InProb, double threshold, double inside)
{
	_allowState.clear();
	if(inside!=-1)
	{
		for(int i=0;(size_t)i<_iScore.size();i++)
		{
			if(_iScore[i]>0)
			{
				//cerr<<"\t"<<_iScore[i]<<endl;
				_allowState.push_back(i);
				/*cout.precision(14);
				cout.setf(ios::fixed,ios::floatfield);
				cout<<"i:"<<_iScore[i]<<endl;*/
			}
		}
	}
	else
	{
		for(int i=0;(size_t)i<_iScore.size();i++)
		{
			if(_iScore[i]<0||_oScore[i]<0)continue;
			double score=_iScore[i]*_oScore[i]/s1InProb;
			if(score>threshold)
				_allowState.push_back(i);
				
				//cout.precision(14);
				//cout.setf(ios::fixed,ios::floatfield);
			//	cout<<"i:"<<log(_iScore[i])<<" o:"<<log(_oScore[i])<<" s:"
			//		<<log(s1InProb)<<" "<<log(score)<<" ";
				//cout.precision(2);
				//cout.setf(ios::fixed,ios::floatfield);
			//	cout<<log(threshold)<<" [ "<<i<<" ]"<<endl;
		}
		
		//for(int h=0;h<_allowState.size();h++)
		//	cout<<_allowState[h]<<" [ 0 ]"<<endl;
	}

	return _allowState;
}

void 
Node::reset_scorePool_initLevel(int numOfState)
{
	_allowState.clear();
	_iScore.clear();
	_oScore.clear();
	_iScore.reserve(numOfState);
	_oScore.reserve(numOfState);
	for(int i=0;i<numOfState;i++)
	{
		_iScore.push_back(-1E10);
		_oScore.push_back(-1E10);
	}
}
void 
Node::
set_allowStateAndSubState_byState()
{
	for(int i=0;(size_t)i<_allowState.size();i++)
		_asi.add(_allowState[i],0);
}

size_t 
Node::
io_afterParse(IorO ioro, IOType iot)
{
	if(ioro==inside)
	{
		for(unsigned int i=0;i<uio().size();i++)
		{
			string name=uio()[i];
			(*this)[name].io_afterParse(ioro,iot);
		}
	}
	else
	{
		for(int i=static_cast<int>(uio().size()-1);i>=0;i--)
		{
			string name=uio()[i];
			(*this)[name].io_afterParse(ioro,iot);
		}
	}
	return size();
}

void 
Node::
prune(map<Label*,int>& reachableLabels, double rootInsideProb, double threshold)
{
	Node& node=*this;
	vector<string>& uio=node.uio();

	//cerr<<"here"<<endl;
	//get label from top to down
	for(int ruIter=static_cast<int>(uio.size()-1);ruIter>=0;ruIter--)
	{
		//process each label
		Label& label=node[uio[ruIter]];
		if(reachableLabels.find(&label)==reachableLabels.end())
			continue;
		
		LabelArrays& cpp=label.childPairPool();
		//cerr<<"here0"<<endl;
		for(int cpIter=static_cast<int>(cpp.size()-1);cpIter>=0;cpIter--)
		{
			LabelArray& la=cpp[cpIter];
			LabelPointers& lp=la.childs();
			double score=logrize(label.outsideProb());
			score+=la.ruleScore();
			for(size_t lpIter=0;lpIter<lp.size();lpIter++)
				score+=logrize(lp[lpIter]->insideProb());	
			if(rootInsideProb-score>threshold&&cpp.size()>1)
				cpp.erase(cpp.begin()+cpIter);
			else
			{
				for(size_t lpIter=0;lpIter<lp.size();lpIter++)
				{
					if(reachableLabels.find(lp[lpIter])==reachableLabels.end())
						reachableLabels[lp[lpIter]]=1;
					else
						reachableLabels[lp[lpIter]]++;
				}
			}
		}
		/*if(cpp.size()==0&&!label.isWordLabel())
			cerr<<"error here: "<<label.nt()<<"["<<label.start()<<","<<label.stop()<<"]"<<endl;
		for(size_t cpCount=0;cpCount<cpp.size();cpCount++)
		{
			LabelArray& la=cpp[cpCount];
		}*/
	}
	//cerr<<"here1"<<endl;
	
	for(int uIter=static_cast<int>(uio.size()-1);uIter>=0;uIter--)
	{
		if(reachableLabels.find(&(node[uio[uIter]]))==reachableLabels.end())
		{
			node.erase(node.find(uio[uIter]));
			uio.erase(uio.begin()+uIter);
		}
	}
}


/******************************************************/
void 
AllowStateInfo::
allowStates(vector<int>& result)const
{
	result.clear();
	for(const_iterator iter=begin();iter!=end();iter++)
	{
		result.push_back(iter->first);
	}
}

void 
AllowStateInfo::
allowSubStates(int state, vector<int>& result)const
{
	result.clear();
	if(find(state)!=end())
	{
		const map<int,int>& tmp=find(state)->second;
		for(map<int,int>::const_iterator iter=tmp.begin();iter!=tmp.end();iter++)
		{
			result.push_back(iter->first);
		}
	}
}
/*
void 
AllowStateInfo::
allowStateAndSubStates(vector<pair<int,int> >& result)const
{
	result.clear();
	for(const_iterator iter1=begin();iter1!=end();iter1++)
	{
		const map<int,int>& tmp=iter1->second;
		for(map<int,int>::const_iterator iter2=tmp.begin();iter2!=tmp.end();iter2++)
		{
			result.push_back(make_pair(iter1->first,iter2->first));
		}		
	}
}
*/
const vector<pair<int,vector<int> > >&  
AllowStateInfo::
allowStateAndSubStates()const
{
	//if(_inVector.size()==0)
	//	set_inVector();
	return _inVector;
}

void 
AllowStateInfo::
set_inVector()
{
	_inVector.clear();
	for(const_iterator iter1=begin();iter1!=end();iter1++)
	{
		vector<int> emp;
		emp.clear();
		_inVector.push_back(make_pair(iter1->first,emp));
		const map<int,int>& tmp=iter1->second;
		for(map<int,int>::const_iterator iter2=tmp.begin();iter2!=tmp.end();iter2++)
		{
			_inVector[_inVector.size()-1].second.push_back(iter2->first);
		}		
	}
	sort(_inVector.begin(),_inVector.end());
}

bool 
AllowStateInfo::
isAllow(int state)const
{
	if(find(state)==end())
		return false;
	else return true;
}

bool 
AllowStateInfo::
isAllow(int state, int subState)const
{
	if(find(state)==end())
		return false;
	else 
	{
		const map<int,int>& tmp=find(state)->second;
		if(tmp.find(subState)==tmp.end())
			return false;
		return true;
	}
}

void 
AllowStateInfo::
add(int state, int subState)
{
	(*this)[state][subState]=1;	
}

void 
AllowStateInfo::
del(int state, int subState)
{
	iterator iter1=find(state);
	if(iter1!=end())
	{
		map<int,int>& tmp=iter1->second;
		map<int,int>::iterator iter2=tmp.find(subState);
		if(iter2!=tmp.end())
		{
			tmp.erase(iter2);
		}
		if(tmp.size()==0)
			erase(iter1);
	}
}

void 
AllowStateInfo::
clone(AllowStateInfo& result)const
{
	result.clear();
	result.set_inUse(inUse());
	const_iterator iter1=begin();
	for(;iter1!=end();iter1++)
	{
		const map<int,int>& tmp=iter1->second;
		for(map<int,int>::const_iterator iter2=tmp.begin();iter2!=tmp.end();iter2++)
		{
			result.add(iter1->first,iter2->first);
		}		
	}
	result.set_inVector();
}

//如果没有下一级，则保留原级
void 
AllowStateInfo::
reset_allowStateInfo_to_nextLevel(const BerkeleyStateInfoOfOneLevel& bsio)
{
	AllowStateInfo tmp_asi;
	tmp_asi.set_inUse(true);

	const_iterator iter1=begin();
	for(;iter1!=end();iter1++)
	{
		const map<int,int>& tmp=iter1->second;
		for(map<int,int>::const_iterator iter2=tmp.begin();iter2!=tmp.end();iter2++)
		{
			int state=iter1->first, subState=iter2->first;
			vector<int> sun;
			bsio.query_substate2sun(state,subState,sun);
			//cout<<state<<" "<<subState<<" --> [ ";
			for(int i=0;(size_t)i<sun.size();i++)
			{
				//cout<<sun[i]<<" ";
				tmp_asi.add(state,sun[i]);
			}
			if(sun.size()==0)
				tmp_asi.add(state,subState);
			//cout<<"]"<<endl;
		}		
	}
	tmp_asi.clone((*this));
	set_inVector();
}

void 
AllowStateInfo::
print(ostream& os)const
{
	const_iterator iter1;
	map<int,int>::const_iterator iter2;
	for(iter1=begin();iter1!=end();iter1++)
	{
		const map<int,int>& tmp=iter1->second;
		os<<iter1->first<<" [ ";
		for(iter2=tmp.begin();iter2!=tmp.end();iter2++)
		{
			os<<iter2->first<<" ";
		}
		os<<"]"<<endl;
	}
}
