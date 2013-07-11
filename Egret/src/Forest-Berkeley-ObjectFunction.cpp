#include "Forest.h"
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>

using namespace std;

double 
Forest::
getObjectFunctionBinaryRuleScore(const BerkeleyRuleSet& ruleSet, int start,int stop, int middle, int parentState, int leftChildState, int rightChildState, ObjectFunctionType oft)
{
	Node& prNode=(*this)[start][stop];
	Node& lcNode=(*this)[start][middle];
	Node& rcNode=(*this)[middle+1][stop];
	
	//get allowStateInfo
	const AllowStateInfo& asiPR=prNode.allowStateInfo();
	const AllowStateInfo& asiLC=lcNode.allowStateInfo();
	const AllowStateInfo& asiRC=rcNode.allowStateInfo();
	
	//skip the wrong input
	if(!(asiPR.isAllow(parentState)&&asiLC.isAllow(leftChildState)&&asiRC.isAllow(rightChildState)))
		return -1;
	const BerkeleyBinaryRule* rule=ruleSet.getBinaryRuleByRule(leftChildState,rightChildState,parentState);
	if(rule==NULL) return -1;

	//get allowed SubStates
	vector<int> prSubs,lcSubs,rcSubs;
	asiPR.allowSubStates(parentState,prSubs);
	asiLC.allowSubStates(leftChildState,lcSubs);
	asiRC.allowSubStates(rightChildState,rcSubs);

	//sum up the scores
	
	double molecusar=0;

	for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
	{
		int prSub=prSubs[prSubIter];
		
		for(int lcSubIter=0;(size_t)lcSubIter<lcSubs.size();lcSubIter++)
		{
			int lcSub=lcSubs[lcSubIter];
			for(int rcSubIter=0;(size_t)rcSubIter<rcSubs.size();rcSubIter++)
			{
				int rcSub=rcSubs[rcSubIter];
				double ruleScore=rule->getScore(lcSub,rcSub,prSub);
				if(ruleScore>-1E10)
				{
					double prOScore=prNode.oScore(parentState,prSub);
					double lcIScore=lcNode.iScore(leftChildState,lcSub);
					double rcIScore=rcNode.iScore(rightChildState,rcSub);
					if(prOScore>0&&lcIScore>0&&rcIScore>0)
					{
						if(oft==MAX_PROB)
							molecusar+=ruleScore;
						else
							molecusar+=prOScore*ruleScore*lcIScore*rcIScore;
					}
				}
			}
		}
	}

	double denominator=0;
	if(oft==VARIATIONAL)
	{
		for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
		{
			int prSub=prSubs[prSubIter];
			double prOScore=prNode.oScore(parentState,prSub);
			double prIScore=prNode.iScore(parentState,prSub);
			if(prOScore>0&&prIScore>0)
				denominator+=prOScore*prIScore;
		}
	}
	else if(oft==MAX_RULE_SUM||oft==MAX_RULE_PRODUCT)
	{
		Node& node=(*this)[0][_sentence.size()-1];
		denominator=node.iScore(0,0);
	}
	else if(oft==MAX_PROB)
		denominator=1;
	if(molecusar==0||denominator==0)
		return -1;
	double result=molecusar/denominator;
	if(result>1)result=1;
	return result;
}

double
Forest::
getObjectFunctionUnaryRuleScore(const BerkeleyRuleSet& ruleSet, int start, int stop, int parentState, int childState, ObjectFunctionType oft)
{
	Node& prNode=(*this)[start][stop];
	Node& chNode=(*this)[start][stop];
	
	//get allowStateInfo
	const AllowStateInfo& asiPR=prNode.allowStateInfo();
	const AllowStateInfo& asiCH=chNode.allowStateInfo();
	
	//skip the wrong input
	if(!(asiPR.isAllow(parentState)&&asiCH.isAllow(childState)))
		return -1;
	const BerkeleyUnaryRule* rule=ruleSet.getUnaryRuleByRule(childState,parentState);
	if(rule==NULL) return -1;

	//get allowed SubStates
	vector<int> prSubs,chSubs;
	asiPR.allowSubStates(parentState,prSubs);
	asiCH.allowSubStates(childState,chSubs);

	//sum up the scores
	
	double molecusar=0;
	for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
	{
		int prSub=prSubs[prSubIter];
		for(int chSubIter=0;(size_t)chSubIter<chSubs.size();chSubIter++)
		{
			int chSub=chSubs[chSubIter];
			double ruleScore=rule->getScore(chSub,prSub);
			if(ruleScore>-1E10)
			{
				double prOScore=prNode.oScore(parentState,prSub);
				double chIScore=chNode.iScore(childState,chSub);
				if(prOScore>0&&chIScore>0)
				{
					if(oft==MAX_PROB)
						molecusar+=ruleScore;
					else
						molecusar+=prOScore*ruleScore*chIScore;
				}
			}
		}
	}

	double denominator=0;
	if(oft==VARIATIONAL)
	{
		for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
		{
			int prSub=prSubs[prSubIter];
			double prOScore=prNode.oScore(parentState,prSub);
			double prIScore=prNode.iScore(parentState,prSub);
			if(prOScore>0&&prIScore>0)
				denominator+=prOScore*prIScore;
		}
	}
	else if(oft==MAX_RULE_SUM||oft==MAX_RULE_PRODUCT)
	{
		Node& node=(*this)[0][_sentence.size()-1];
		denominator=node.iScore(0,0);
	}
	else if(oft==MAX_PROB)
		denominator=1;
	if(molecusar==0||denominator==0)
		return -1;
	double result=molecusar/denominator;
	if(result>1)result=1;
	return result;
}

double 
Forest::
getObjectFunctionLexRuleScore(
				const BerkeleyRuleSet& ruleSet, 
				int position,
				int pos,
				ObjectFunctionType oft)
{
	Node& node=(*this)[position][position];
	string wrd=_sentence[position];
	//get allowStateInfo
	const AllowStateInfo& asi=node.allowStateInfo();
	
	//skip the wrong input
	if(!asi.isAllow(pos))
		return -1;
	const BerkeleyUnaryRule* rule=ruleSet.getLexRuleByRule(wrd,pos);
	if(rule==NULL) return -1;

	//get allowed SubStates
	vector<int> prSubs;
	asi.allowSubStates(pos,prSubs);

	//sum up the scores
	
	double molecusar=0;
	for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
	{
		int prSub=prSubs[prSubIter];
		double ruleScore=rule->getScore(0,prSub);
		if(ruleScore>-1E10)
		{
			double oScore=node.oScore(pos,prSub);	
			if(oScore>0)
			{
				if(oft==MAX_PROB)
					molecusar+=ruleScore;
				else
					molecusar+=oScore*ruleScore;
			}
		}	
	}

	double denominator=0;
	if(oft==VARIATIONAL)
	{
		for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
		{
			int prSub=prSubs[prSubIter];
			double oScore=node.oScore(pos,prSub);
			double iScore=node.iScore(pos,prSub);
			if(oScore>0)
				denominator+=oScore*iScore;
		}
	}
	else if(oft==MAX_RULE_SUM||oft==MAX_RULE_PRODUCT)
	{
		Node& node=(*this)[0][_sentence.size()-1];
		denominator=node.iScore(0,0);
	}
	else if(oft==MAX_PROB)
		denominator=1;
	if(molecusar==0||denominator==0)
		return -1;
	double result=molecusar/denominator;
	if(result>1)result=1;
	return result;
}

bool   
Forest::
make_objectFunction_tempUnaryRuleSet_for_one_node(
		const BerkeleyRuleSet& ruleSet, 
		int start, 
		int stop, 
		ObjectFunctionType oft,
		vector<vector<double> >& tempUnrayRuleSet
		)
{
	tempUnrayRuleSet.clear();
	int numOfState=(int)ruleSet.numOfState();
	for(int i=0;i<numOfState;i++)
	{
		vector<double> double_vector;
		double_vector.clear();
		tempUnrayRuleSet.push_back(double_vector);
		for(int j=0;j<numOfState;j++)
		{
			tempUnrayRuleSet[i].push_back(-1E10);
		}
	}

	Node& node=(*this)[start][stop];	
	//get allowStateInfo
	const AllowStateInfo& asi=node.allowStateInfo();
	vector<int> allowStates;
	asi.allowStates(allowStates);

	//compute scores
	for(int i=0;(size_t)i<allowStates.size();i++)
	{
		int parentState=allowStates[i];
		for(int j=0;(size_t)j<allowStates.size();j++)
		{
			int childState=allowStates[j];
			double score=getObjectFunctionUnaryRuleScore(ruleSet, start, stop, parentState, childState, oft);
			if(score>0)	
				tempUnrayRuleSet[parentState][childState]=score;
		}
	}
	return true;	
}

bool  
Forest::
genNewNode_objectFunction_constrained_berkeley(
		const BerkeleyRuleSet& brSet, 
		int start,
		int stop, 
		int middle,
		ObjectFunctionType oft)
{
	Node& lcNode=(*this)[start][middle];
	Node& rcNode=(*this)[middle+1][stop];
	Node& prNode=(*this)[start][stop];
	const AllowStateInfo& asi=prNode.allowStateInfo();
	vector<int> allowStates;
	asi.allowStates(allowStates);

	//cout<<"processing "<<start<<","<<stop<<","<<middle<<endl;
	for(Node::iterator lcIter=lcNode.begin();lcIter!=lcNode.end();lcIter++)
	{
		for(Node::iterator rcIter=rcNode.begin();rcIter!=rcNode.end();rcIter++)
		{
			Label& labelLC=lcIter->second;
			Label& labelRC=rcIter->second;
			//following we gen NewNode byLabel;

			int lcState=labelLC.state();
			int rcState=labelRC.state();
			for(int i=0;(size_t)i<allowStates.size();i++)
			{
				int prState=allowStates[i];
				//const BerkeleyBinaryRule* rule=brSet.getBinaryRuleByRule(lcState,rcState,prState);
				//if(rule==NULL)	continue;
				double objectFunctionRuleScore=
					getObjectFunctionBinaryRuleScore(brSet,start,stop,middle,prState,lcState,rcState,oft); 
				
				if(objectFunctionRuleScore>0)
				{
					Label label;
					label.init(brSet.state2term(prState),start,stop,prState);
					if(prNode.find(label.nt())==prNode.end())
						prNode[label.nt()]=label;
					// MAKE SURE THE RULESCORE IS IN LOG FORM
					//cout<<start<<","<<stop<<","<<middle<<":"<<label.nt()<<","<<label.word()<<" "<<objectFunctionRuleScore<<" "<<log(2,objectFunctionRuleScore)<<endl;
					if(oft!=MAX_RULE_SUM)
						objectFunctionRuleScore=log(2,objectFunctionRuleScore);
					prNode[label.nt()].addChildPair_viterbi(&labelLC,&labelRC,objectFunctionRuleScore);
				}
				//else 
				//	cout<<start<<","<<stop<<","<<middle<<":"<<brSet.state2term(prState)<<" --> "<<labelLC.nt()<<" "<<labelRC.nt()<<" "<<objectFunctionRuleScore<<endl;
			}
		}
	}
	return true;
}
/*
bool 
Forest::
genUnaryNode_objectFunction_constrained_berkeley(const BerkeleyRuleSet& ruleSet, int start, int stop, ObjectFunctionType oft)
{
	Node& node=(*this)[start][stop]; 
	bool mark=false;
	vector<string> ntPool;
	ntPool.clear();
	node.setUnaryInOrder_berkeley(ruleSet,ntPool);

	const AllowStateInfo& asi=node.allowStateInfo();
	vector<int> allowStates;
	asi.allowStates(allowStates);

	for(int i=0;i<ntPool.size();i++)
	{
		string rightPart=ntPool[i];
		//cout<<"here:"<<i<<","<<rightPart<<endl;
		Label& oldLb=node[rightPart];

		int ch=oldLb.state();

		for(int i=0;i<allowStates.size();i++)
		{
			int pr=allowStates[i];
			if(ch==pr)continue;
			//const BerkeleyUnaryRule* rule=ruleSet.getUnaryRuleByRule(ch,pr);
			//if(rule==NULL)continue;
			double objectFunctionRuleScore=getObjectFunctionUnaryRuleScore(ruleSet,oldLb.start(),oldLb.stop(),pr,ch,oft);
			if(objectFunctionRuleScore>0)
			{
				Label label;
				label.init(ruleSet.state2term(pr),oldLb.start(),oldLb.stop(),pr);
				if(node.find(label.nt())==node.end())
				{
					node[label.nt()]=label;
					ntPool.push_back(label.nt());
				}
				else if(label.nt()!="ROOT"&&label.nt()!="S1")
				{
					label.nt()="u*"+label.nt();
					if(node.find(label.nt())==node.end())
					{
						node[label.nt()]=label;
						ntPool.push_back(label.nt());
					}
				}				
				//cout<<"unary:"<<label.nt()<<" "<<label.word()<<" --> "<<oldLb.nt()<<" "<<oldLb.word()<<" "
				//	<<objectFunctionRuleScore<<" "<<log(2,objectFunctionRuleScore)<<endl;
				if(oft!=MAX_RULE_SUM)
					objectFunctionRuleScore=log(2,objectFunctionRuleScore);
				node[label.nt()].addChild_viterbi(&oldLb,objectFunctionRuleScore);
				mark=true;
			}
		}
	}
	return mark;
}
*/

bool 
Forest::
genUnaryNode_objectFunction_constrained_berkeley(const BerkeleyRuleSet& ruleSet, int start, int stop, ObjectFunctionType oft)
{
	Node& node=(*this)[start][stop]; 

	const AllowStateInfo& asi=node.allowStateInfo();
	vector<int> allowStates;
	asi.allowStates(allowStates);

	bool mark=false;
	vector<string> ntPool;
	ntPool.clear();
	
	vector<vector<double> > tempUnaryRuleSet;
	make_objectFunction_tempUnaryRuleSet_for_one_node(ruleSet, start, stop, oft, tempUnaryRuleSet);

	//node.setUnaryInOrder_berkeley(ruleSet,ntPool);
	
	node.setUnaryInOrder_objectFunction_berkeley(ruleSet, tempUnaryRuleSet, allowStates, ntPool, oft);
	node.uio()=ntPool;
	
/*	cerr<<"["<<start<<","<<stop<<"]:"<<endl;
	for(size_t i=0;i<ntPool.size();i++)
		cerr<<ntPool[i]<<" ";
	cerr<<endl;
*/
	map<string,int> uiOrder;
	for(int i=0;(size_t)i<ntPool.size();i++)
		uiOrder[ntPool[i]]=i;

	for(int i=0;(size_t)i<ntPool.size();i++)
	{
		string rightPart=ntPool[i];
//		cerr<<"here:"<<i<<","<<rightPart<<endl;
		Label& oldLb=node[rightPart];

		int ch=oldLb.state();

		for(int i=0;(size_t)i<allowStates.size();i++)
		{
			int pr=allowStates[i];
			if(ch==pr)continue;
			//const BerkeleyUnaryRule* rule=ruleSet.getUnaryRuleByRule(ch,pr);
			//if(rule==NULL)continue;
			//double objectFunctionRuleScore=getObjectFunctionUnaryRuleScore(ruleSet,oldLb.start(),oldLb.stop(),pr,ch,oft);
			double objectFunctionRuleScore=tempUnaryRuleSet[pr][ch];
			if(objectFunctionRuleScore>0)
			{
				Label label;
				label.init(ruleSet.state2term(pr),oldLb.start(),oldLb.stop(),pr);
				if(node.find(label.nt())==node.end())
				{
					node[label.nt()]=label;
					//ntPool.push_back(label.nt());
				}
				//cout<<"unary:"<<label.nt()<<" "<<label.word()<<" --> "<<oldLb.nt()<<" "<<oldLb.word()<<" "
				//	<<objectFunctionRuleScore<<" "<<log(2,objectFunctionRuleScore)<<endl;
				if(oft!=MAX_RULE_SUM)
					objectFunctionRuleScore=log(2,objectFunctionRuleScore);
				if(node[label.nt()].addChild_viterbi(&oldLb,objectFunctionRuleScore))
					uiOrder[label.nt()]=max(uiOrder[label.nt()], uiOrder[oldLb.nt()]+1);
				mark=true;
			}
		}
	}
	vector<pair<int,string> > ntAndRank;
	for(size_t i=0; i<ntPool.size(); i++)
		ntAndRank.push_back(make_pair(uiOrder[ntPool[i]],ntPool[i]));
	sort(ntAndRank.begin(),ntAndRank.end());
	node.uio().clear();
	for(size_t i=0;i<ntAndRank.size();i++)
		node.uio().push_back(ntAndRank[i].second);
	return mark;
}


bool 
Forest::
init_berkeley_objectFunction(const BerkeleyRuleSet& brSet, ObjectFunctionType oft)
{
	clock_t sec=clock();
	for(int i=0;(size_t)i<_sentence.size();i++)
	{
		Node& node=(*this)[i][i];
		string wrd=_sentence[i];

		const AllowStateInfo& asi=node.allowStateInfo();
		const vector<BerkeleyUnaryRule*>& rules=brSet.getLexRulesByWord(wrd);
		
		for(int k=0;(size_t)k<rules.size();k++)
		{
			BerkeleyUnaryRule* rule=rules[k];
			if(rule==NULL) continue;
			int parentState=rule->parentState();
			
			if(!asi.inUse()||asi.isAllow(parentState))
			{
				double objectFuntionRuleScore=-1;
				objectFuntionRuleScore=getObjectFunctionLexRuleScore(brSet,i,parentState,oft);
				if(objectFuntionRuleScore>0)
				{
					Label label;
					label.init(brSet.state2term(parentState),i,i,parentState,wrd);
					//cout<<"word "<<i<<":"<<wrd<<" "<<label.nt()<<" "<<objectFuntionRuleScore<<" "<<log(2,objectFuntionRuleScore)<<endl;
					
					if(oft!=MAX_RULE_SUM)
						objectFuntionRuleScore=log(2,objectFuntionRuleScore);
					label.addPreTermScore(objectFuntionRuleScore);
					if(node.find(label.nt())==node.end())
						node[label.nt()]=label;
				}
				//else
				//	cout<<"error word "<<i<<":"<<wrd<<" "<<-1<<endl;
			}
		}
		clock_t usec=clock();
		genUnaryNode_objectFunction_constrained_berkeley(brSet,i,i,oft);
		final_unatime+=clock()-usec;
	}
	//cerr<<"init finished"<<endl;
	initime+=clock()-sec;
	return true;
}

bool 
Forest::
viterbi_berkeley_objectFunction(const BerkeleyRuleSet& brSet, ObjectFunctionType oft)
{
	init_berkeley_objectFunction(brSet,oft);
	for(int length=1;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			clock_t insec=clock();
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				genNewNode_objectFunction_constrained_berkeley(brSet,i,i+length,j,oft);
			}
			final_bitime+=clock()-insec;

			clock_t usec=clock();
			genUnaryNode_objectFunction_constrained_berkeley(brSet,i,i+length,oft);
			final_unatime+=clock()-usec;
		}
	}
/*
	for(int length=0;length<_sentence.size();length++)
	{
		for(int i=0;i<_sentence.size()-length;i++)
		{
			(*this)[i][i+length].replace_interLabel(Berkeley);
		}
	}
*/
	return true;
}

