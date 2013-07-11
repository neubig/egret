#include "Forest.h"
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
using namespace std;

bool genUnaryNode_withoutLabel_initLevel_berkeley(const BerkeleyRuleSet& ruleSet, Node& node, IorO iot, ExtentInfo& extentInfo)
{
		const BerkeleyStateInfoOfOneLevel& bsio=ruleSet.bsio();
		int numOfState=(int)bsio.numOfState();

vector<double> temp_iScore,temp_oScore;
temp_iScore.clear();
temp_oScore.clear();
temp_iScore.reserve(numOfState);
temp_oScore.reserve(numOfState);
for(int i=0;i<numOfState;i++)
{
	if(iot==inside)
		temp_iScore.push_back(-1E10);
	else
		temp_oScore.push_back(-1E10);
}

		for(int pr=0;pr<numOfState;pr++)
		{
			if(iot==outside)
			{
				if(node.oScore(pr)<0)continue;
			}
			for(int ch=0;ch<numOfState;ch++)	
			{
				const BerkeleyUnaryRule* rule=ruleSet.getUnaryRuleByRule(ch,pr);
				if(rule==NULL)continue;
				if(ch==pr)continue;
				double ruleScore=rule->getScore(0,0);
				if(ruleScore>0)
				{
					if(iot==inside)
					{
						if(node.iScore(ch)<0)continue;
						//if(node.iScore(pr)<0)node.iScore(pr)=0;
						//node.iScore(pr)=max(node.iScore(ch)*ruleScore,node.iScore(pr));
						if(temp_iScore[pr]<0)temp_iScore[pr]=0;
						temp_iScore[pr]=max(node.iScore(ch)*ruleScore,temp_iScore[pr]);
					}
					else if(iot==outside)
					{
						if(node.oScore(pr)<0)continue;
						//if(node.oScore(ch)<0)node.oScore(ch)=0;
						//node.oScore(ch)=max(node.oScore(pr)*ruleScore,node.oScore(ch));
						if(temp_oScore[ch]<0)temp_oScore[ch]=0;
						temp_oScore[ch]=max(node.oScore(pr)*ruleScore,temp_oScore[ch]);
					}
				}
			}
		}
		for(int i=0;i<numOfState;i++)
		{
			if(iot==inside)
				node.iScore(i)=max(node.iScore(i),temp_iScore[i]);
			else if(iot==outside)
				node.oScore(i)=max(node.oScore(i),temp_oScore[i]);
		}

		if(iot==inside)
		for(int i=0;i<numOfState;i++)
		{
			if(node.iScore(i)>0)
			{
				extentInfo.update(node.start(),node.stop(),i);
			}
		}
		
	node.set_allowState_withoutLabel_for_initialLevel(1,0,0);
	//cout<<"allow State:"<<endl;
	//asi.print(cout);
	return true;
}
/*
void 
Forest::
genNewNode_withoutLabel_initLevel_berkeley_ruleByParent(const BerkeleyRuleSet& ruleSet,int start, int stop , IorO iot, ExtentInfo& extentInfo)
{
	Node& node=(*this)[start][stop];
	int numOfState=ruleSet.numOfState();
	for(int pr=0;pr<numOfState;pr++)
	{
		const vector<BerkeleyBinaryRule*> rules=ruleSet.getBinaryRulesByParent(pr);
		for(int i=0;i<rules.size();i++)
		{
			BerkeleyBinaryRule* rule=rules[i];
			if(rule==NULL)continue;
			double ruleScore=rule->getScore(0,0,0);
			if(ruleScore<0)continue;
			int lc=rule->leftChildState();
			int rc=rule->rightChildState();

			int	lc_REmin,lc_REmax,rc_LEmin,rc_LEmax;
			extentInfo.query_lc(node.start(),lc,lc_REmin,lc_REmax);
			if(lc_REmin>=stop)continue;
			extentInfo.query_rc(node.stop(),rc,rc_LEmin,rc_LEmax);
			if(rc_LEmax<=start)continue;
			
			int startMin,stopMax;
			startMin=max(lc_REmin,rc_LEmin-1);
			stopMax=min(lc_REmax,rc_LEmax-1);
			if(startMin>stopMax)continue;
			//cerr<<start<<","<<stop<<" "<<startMin<<","<<stopMax<<endl;
			for(int pivot=startMin;pivot<=stopMax;pivot++)
			{
				Node& n1=(*this)[start][pivot];
				Node& n2=(*this)[pivot+1][stop];

				if(iot==inside)
				{
					if(n1.iScore(lc)<0||n2.iScore(rc)<0)continue;
					if(node.iScore(pr)<0)node.iScore(pr)=0;
					node.iScore(pr)=max(n1.iScore(lc)*n2.iScore(rc)*ruleScore,node.iScore(pr));
				}
				else if(iot==outside)
				{
					if(node.oScore(pr)<0)continue;
					if(n1.oScore(lc)<0) n1.oScore(lc)=0;
					if(n2.oScore(rc)<0) n2.oScore(rc)=0;
					n1.oScore(lc)=max(node.oScore(pr)*n2.iScore(rc)*ruleScore,n1.oScore(lc));
					n2.oScore(rc)=max(node.oScore(pr)*n1.iScore(lc)*ruleScore,n2.oScore(rc));
				}
			}
		}
	}
}
*/

void 
Forest::
genNewNode_withoutLabel_initLevel_berkeley_ruleByParent(const BerkeleyRuleSet& ruleSet,int start, int stop , IorO iot, ExtentInfo& extentInfo)
{
	Node& node=(*this)[start][stop];
	int numOfState=(int)ruleSet.numOfState();
	for(int pr=0;pr<numOfState;pr++)
	{
		if(iot==outside)
		{
			if(node.oScore(pr)<0)continue;
		}
		const vector<int>& leftChilds=ruleSet.getLeftChildsByParent(pr);
		for(int lcIter=0;(size_t)lcIter<leftChilds.size();lcIter++)
		{
			int lc=leftChilds[lcIter];
			int lc_REmin,lc_REmax;
			extentInfo.query_lc(start,lc,lc_REmin,lc_REmax);
			if(lc_REmin>=stop||lc_REmax<start)continue;

			const vector<BerkeleyBinaryRule*>& rules=ruleSet.getBinaryRulesByParentLeftChild(pr,lc);
			for(int i=0;(size_t)i<rules.size();i++)
			{
				BerkeleyBinaryRule* rule=rules[i];
				if(rule==NULL)continue;
				double ruleScore=rule->getScore(0,0,0);
				if(ruleScore<0)continue;
				int rc=rule->rightChildState();

				int	rc_LEmin,rc_LEmax;
				extentInfo.query_rc(node.stop(),rc,rc_LEmin,rc_LEmax);
				if(rc_LEmax<=start)continue;
				
				int startMin,stopMax;
				startMin=(int)max(lc_REmin,rc_LEmin-1);
				stopMax=(int)min(lc_REmax,rc_LEmax-1);
				if(startMin>stopMax)continue;
				//cerr<<start<<","<<stop<<" "<<startMin<<","<<stopMax<<endl;
				for(int pivot=startMin;pivot<=stopMax;pivot++)
				{
					Node& n1=(*this)[start][pivot];
					Node& n2=(*this)[pivot+1][stop];

					if(iot==inside)
					{
						if(n1.iScore(lc)<0||n2.iScore(rc)<0)continue;
						if(node.iScore(pr)<0)node.iScore(pr)=0;
						node.iScore(pr)=max(n1.iScore(lc)*n2.iScore(rc)*ruleScore,node.iScore(pr));
					}
					else if(iot==outside)
					{
						if(node.oScore(pr)<0)continue;
						if(n1.oScore(lc)<0) n1.oScore(lc)=0;
						if(n2.oScore(rc)<0) n2.oScore(rc)=0;
						n1.oScore(lc)=max(node.oScore(pr)*n2.iScore(rc)*ruleScore,n1.oScore(lc));
						n2.oScore(rc)=max(node.oScore(pr)*n1.iScore(lc)*ruleScore,n2.oScore(rc));
					}
				}
			}
		}
	}
}


bool 
Forest::
init_berkeley_withoutLabel_for_io_initLevel(const BerkeleyRuleSet& ruleSet,ExtentInfo& extentInfo)
{
	for(int length=0;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
			(*this)[i][i+length].reset_scorePool_initLevel((int)ruleSet.bsio().numOfState());
	}
	clock_t sec=clock();
	for(int i=0;(size_t)i<_sentence.size();i++)
	{
		Node& node=(*this)[i][i];
		string wrd=_sentence[i];
		const vector<BerkeleyUnaryRule*>& rules=ruleSet.getLexRulesByWord(wrd);
		for(int k=0;(size_t)k<rules.size();k++)
		{
			BerkeleyUnaryRule* rule=rules[k];
			if(rule==NULL)continue;
			int pr=rule->parentState();			
			double ruleScore=rule->getScore(0,0);
			if(ruleScore>0)
				node.iScore(pr)=ruleScore;
		}
		//cout<<"node ["<<i<<","<<i<<"]"<<endl;

		//clock_t usec=clock();
		genUnaryNode_withoutLabel_initLevel_berkeley(ruleSet, (*this)[i][i], inside, extentInfo);
		//init_unatime+=clock()-usec;
	}
	//cerr<<"init finished"<<endl;
	initime+=clock()-sec;
	return true;
}

bool 
Forest::
io_berkeley_withoutLabel_initLevel_ruleByParent(const BerkeleyRuleSet& ruleSet, double threshold)
{
	int sentLength=(int)_sentence.size();
	int numOfState=(int)ruleSet.numOfState();
	ExtentInfo extentInfo;
	extentInfo.init(sentLength,numOfState);

	init_berkeley_withoutLabel_for_io_initLevel(ruleSet,extentInfo);
	//make the forest and comput inside prob along the way
	for(int length=1;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			clock_t insec=clock();
			genNewNode_withoutLabel_initLevel_berkeley_ruleByParent(ruleSet,i,i+length,inside,extentInfo);
			init_bitime+=clock()-insec;

			clock_t usec=clock();
			genUnaryNode_withoutLabel_initLevel_berkeley(ruleSet,(*this)[i][i+length],inside, extentInfo);
			init_unatime+=clock()-usec;
		}
	}

	//compute outside prob top-down
	//first init the S1 node
	Node& node=(*this)[0][_sentence.size()-1];
	node.oScore(0)=1;

	//propogate
	for(int length=(int)_sentence.size()-1;length>=0;length--)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			clock_t usec=clock();
			genUnaryNode_withoutLabel_initLevel_berkeley(ruleSet,(*this)[i][i+length],outside,extentInfo);
			init_unatime+=clock()-usec;
			
			clock_t insec=clock();
			genNewNode_withoutLabel_initLevel_berkeley_ruleByParent(ruleSet,i,i+length,outside,extentInfo);
			init_bitime+=clock()-insec;
		}
	}
	
	double s1InProb=(*this)[0][_sentence.size()-1].iScore(0);

	//cout<<"ROOT_O inside:"<<s1InProb<<endl;
	for(int length=0;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			Node& node=(*this)[i][i+length];
			//cout<<"node ["<<i<<","<<i+length<<"]"<<endl;
			node.set_allowState_withoutLabel_for_initialLevel(s1InProb,pow(exp(1.0),threshold));
			node.set_allowStateAndSubState_byState();
			node.reset_allowStateInfo_to_nextLevel(ruleSet.bsio());
		}
	}
	return true;
}

