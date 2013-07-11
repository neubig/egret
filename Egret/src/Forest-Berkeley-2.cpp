#include "Forest.h"
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>

using namespace std;
/*********************** Berkeley ************************************/
//generate and compute io score of new node by unary rule without label
bool genUnaryNode_withoutLabel_initLevel_berkeley(const BerkeleyRuleSet& ruleSet, Node& node, IorO iot)
{
/*
	const vector<int>& asi=node.set_allowState_withoutLabel_for_initialLevel(1,0,0);
	
	for(int chIter=0;chIter<asi.size();chIter++)
	{
		int ch=asi[chIter];
		//cerr<<"ch:"<<ch<<endl;
		const vector<BerkeleyUnaryRule*> rules=ruleSet.getUnaryRulesByChild(ch);
		for(int ruleIter=0;ruleIter<rules.size();ruleIter++)
		{
			BerkeleyUnaryRule* rule=rules[ruleIter];
			if(rule==NULL)continue;
			int pr=rule->parentState();
			if(ch==pr)continue;
			double ruleScore=rule->getScore(0,0);
			if(ruleScore>0)
			{
				if(iot==inside)
				{
					if(node.iScore(ch)<0)cerr<<"bug"<<endl;
					if(node.iScore(pr)<0)node.iScore(pr)=0;
					node.iScore(pr)+=node.iScore(ch)*ruleScore;
				}
				else if(iot==outside)
				{
					if(node.oScore(pr)<0)continue;
					if(node.oScore(ch)<0)node.oScore(ch)=0;
					node.oScore(ch)+=node.oScore(pr)*ruleScore;
				}
			}
		}
	}
*/

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

	node.set_allowState_withoutLabel_for_initialLevel(1,0,0);
	//cout<<"allow State:"<<endl;
	//asi.print(cout);
	return true;
}

//generate and compute io score of new node by binary rule without label

Node& genNewNode_withoutLabel_initLevel_berkeley(const BerkeleyRuleSet& ruleSet,Node& n1, Node& n2, Node& n3, IorO iot)
{
	const vector<int>& asi1_inVector=n1.allowState();
	const vector<int>& asi2_inVector=n2.allowState();
	int lcSize=(int)asi1_inVector.size();
	int rcSize=(int)asi2_inVector.size();
	//if(n3.stop()-n3.start()<3)
	//	cout<<n3.start()<<" "<<n3.stop()<<" : "<<lcSize<<","<<rcSize<<" "<<ruleSet.numOfState()<<endl;
	for(int lcIter=0; lcIter<lcSize;lcIter++)
	{
		int lc=asi1_inVector[lcIter];
		for(int rcIter=0;rcIter<rcSize;rcIter++)
		{
			int rc=asi2_inVector[rcIter];
			
			//cerr<<"child pair:"<<lc<<","<<rc<<endl;
			
			const vector<BerkeleyBinaryRule*>& rules=ruleSet.getBinaryRulesByChilds(lc,rc);
			for(int i=0;(size_t)i<rules.size();i++)
			{
				BerkeleyBinaryRule* rule=rules[i];
				if(rule==NULL)continue;
				int pr=rule->parentState();
				double ruleScore=rule->getScore(0,0,0);
				if(ruleScore>-1E10)
				{
					if(iot==inside)
					{
						//cerr<<"\t parent:"<<pr<<endl;
						if(n1.iScore(lc)<0||n2.iScore(rc)<0)cerr<<"bug"<<endl;
						if(n3.iScore(pr)<0) n3.iScore(pr)=0;
						n3.iScore(pr)=max(n1.iScore(lc)*n2.iScore(rc)*ruleScore,n3.iScore(pr));
					}
					else if(iot==outside)
					{
						if(n3.oScore(pr)<0)continue;//cerr<<"bug2"<<endl;
						if(n1.oScore(lc)<0) n1.oScore(lc)=0;
						if(n2.oScore(rc)<0) n2.oScore(rc)=0;
						n1.oScore(lc)=max(n3.oScore(pr)*n2.iScore(rc)*ruleScore,n1.oScore(lc));
						n2.oScore(rc)=max(n3.oScore(pr)*n1.iScore(lc)*ruleScore,n2.oScore(rc));
					}
				}
			}	
		}
	}
	return n3;
}



bool 
Forest::
init_berkeley_withoutLabel_for_io_initLevel(const BerkeleyRuleSet& ruleSet)
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
		genUnaryNode_withoutLabel_initLevel_berkeley(ruleSet,(*this)[i][i],inside);
	}
	//cerr<<"init finished"<<endl;
	initime+=clock()-sec;
	return true;
}

bool 
Forest::
io_berkeley_withoutLabel_initLevel(const BerkeleyRuleSet& ruleSet, double threshold)
{
	init_berkeley_withoutLabel_for_io_initLevel(ruleSet);
	//make the forest and comput inside prob along the way
	for(int length=1;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//cerr<<i<<","<<j<<endl;
				//cerr<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				genNewNode_withoutLabel_initLevel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],inside);
				bitime+=clock()-insec;
			}
			clock_t usec=clock();
			//cout<<"node ["<<i<<","<<i+length<<"]"<<endl;
			genUnaryNode_withoutLabel_initLevel_berkeley(ruleSet,(*this)[i][i+length],inside);
			unatime+=clock()-usec;
		}
	}
	//compute outside prob top-down
	//first init the S1 node
	Node& node=(*this)[0][_sentence.size()-1];
	node.oScore(0)=1;
	//genUnaryNode_withoutLabel_berkeley(ruleSet,node,outside);
	//propogate
	for(int length=(int)_sentence.size()-1;length>=0;length--)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			clock_t usec=clock();
			genUnaryNode_withoutLabel_initLevel_berkeley(ruleSet,(*this)[i][i+length],outside);
			unatime+=clock()-usec;
			//cout<<"node ["<<i<<","<<i+length<<"]"<<endl;
			//(*this)[i][i+length].print_io_withoutLabel(cout);
			
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//jout<<i<<","<<j<<endl;
				//jout<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				genNewNode_withoutLabel_initLevel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],outside);
				bitime+=clock()-insec;
			}
			//cerr<<"node ["<<i<<","<<i+length<<"]"<<endl;
			//(*this)[i][i+length].print_io_withoutLabel(cout);
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


//generate and compute io score of new node by unary rule without label
bool genUnaryNode_withoutLabel_berkeley(const BerkeleyRuleSet& ruleSet, Node& node, IorO iot)
{
	if(node.useAllowState())
	{
		Node tempNode(node.start(),node.stop());
		tempNode.reset_scorePool(ruleSet.bsio());

		const AllowStateInfo& asi=node.allowStateInfo();
		const vector<pair<int,vector<int> > >& asi_inVector=asi.allowStateAndSubStates();

		for(int prIter=0;(size_t)prIter<asi_inVector.size();prIter++)
		{
			int pr=asi_inVector[prIter].first;
			
			for(int chIter=0;(size_t)chIter<asi_inVector.size();chIter++)
			{
				int ch=asi_inVector[chIter].first;	
		
				if(ch==pr)continue;

				const BerkeleyUnaryRule* rule=ruleSet.getUnaryRuleByRule(ch,pr);
				if(rule==NULL)continue;

				const vector<int>& chSubs=asi_inVector[chIter].second;
				const vector<int>& prSubs=asi_inVector[prIter].second;

				for(int chSubIter=0;(size_t)chSubIter<chSubs.size();chSubIter++)
				{
					int chsub=chSubs[chSubIter];
						
					for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
					{
						int prsub=prSubs[prSubIter];
						if(ch==pr&&chsub==prsub)continue;
						double ruleScore=rule->getScore(chsub,prsub);
						if(ruleScore>0)
						{
							if(iot==inside)
							{
								if(node.iScore(ch,chsub)<0)continue;
								/*if(node.iScore(pr,prsub)<0)node.iScore(pr,prsub)=0;
								node.iScore(pr,prsub)+=(node.iScore(ch,chsub)*ruleScore);
								*/
								if(tempNode.iScore(pr,prsub)<0)tempNode.iScore(pr,prsub)=0;
								tempNode.iScore(pr,prsub)+=(node.iScore(ch,chsub)*ruleScore);
							}
							else if(iot==outside)
							{
								if(node.oScore(pr,prsub)<0)continue;
								/*if(node.oScore(ch,chsub)<0)node.oScore(ch,chsub)=0;
								node.oScore(ch,chsub)+=(node.oScore(pr,prsub)*ruleScore);
								*/
								if(tempNode.oScore(ch,chsub)<0)tempNode.oScore(ch,chsub)=0;
								tempNode.oScore(ch,chsub)+=(node.oScore(pr,prsub)*ruleScore);
								if(node.oScore(ch,chsub)>1)
									cerr<<"error here "<<pr<<","<<prsub<<" -- "<<ch<<","<<chsub<<":"<<ruleScore<<endl;
							}					
						}
					}	
				}
			}
		}
		
		for(int chIter=0;(size_t)chIter<asi_inVector.size();chIter++)
		{
			int ch=asi_inVector[chIter].first;
			const vector<int>& chSubs=asi_inVector[chIter].second;
			for(int chSubIter=0;(size_t)chSubIter<chSubs.size();chSubIter++)
			{
				int chsub=chSubs[chSubIter];
				if(iot==inside)
				{
					if(node.iScore(ch,chsub)<0)
						node.iScore(ch,chsub)=tempNode.iScore(ch,chsub);
					else if(tempNode.iScore(ch,chsub)>0)
						node.iScore(ch,chsub)+=tempNode.iScore(ch,chsub);
				}
				else if(iot==outside)
				{
					if(node.oScore(ch,chsub)<0)
						node.oScore(ch,chsub)=tempNode.oScore(ch,chsub);
					else if(tempNode.oScore(ch,chsub)>0)
						node.oScore(ch,chsub)+=tempNode.oScore(ch,chsub);
				}
			}
		}
	}
	else
	{
		const BerkeleyStateInfoOfOneLevel& bsio=ruleSet.bsio();
		int numOfState=(int)bsio.numOfState();
		for(int pr=0;pr<numOfState;pr++)
		{
			for(int ch=0;ch<numOfState;ch++)	
			{
				const BerkeleyUnaryRule* rule=ruleSet.getUnaryRuleByRule(ch,pr);
				if(rule==NULL)continue;

				int ch_numOfSubState=bsio.state2numSubState(ch);
				int pr_numOfSubState=bsio.state2numSubState(pr);
				for(int chsub=0;chsub<ch_numOfSubState;chsub++)
				{
					for(int prsub=0;prsub<pr_numOfSubState;prsub++)
					{
						if(ch==pr&&chsub==prsub)continue;
						
						double ruleScore=rule->getScore(chsub,prsub);
						if(ruleScore>0)
						{
							if(iot==inside)
							{
								if(node.iScore(ch,chsub)<0)continue;
								if(node.iScore(pr,prsub)<0)node.iScore(pr,prsub)=0;
								node.iScore(pr,prsub)+=node.iScore(ch,chsub)*ruleScore;
								//cout<<ch<<","<<chsub<<" -- "<<pr<<","<<prsub<<":"<<ruleScore<<endl;
							}
							else if(iot==outside)
							{
								if(node.oScore(pr,prsub)<0)continue;
								if(node.oScore(ch,chsub)<0)node.oScore(ch,chsub)=0;
								node.oScore(ch,chsub)+=node.oScore(pr,prsub)*ruleScore;
								
							}
						}
					}
				}
			}
		}
		
	}
	const AllowStateInfo& asi=node.set_allowStateAndSubState_withoutLabel(1,0,0);
		//cout<<"allow State:"<<endl;
	//asi.print(cout);
	return true;
}

//generate and compute io score of new node by binary rule without label

Node& genNewNode_withoutLabel_berkeley(const BerkeleyRuleSet& ruleSet,Node& n1, Node& n2, Node& n3, IorO iot)
{
	const AllowStateInfo& asi1=n1.allowStateInfo();//n1.set_allowStateAndSubState_withoutLabel(1,0,0);//
	const AllowStateInfo& asi2=n2.allowStateInfo();//n2.set_allowStateAndSubState_withoutLabel(1,0,0);//
	const AllowStateInfo& asi3=n3.allowStateInfo();//n3.set_allowStateAndSubState_withoutLabel(1,0,0);//
	const vector<pair<int,vector<int> > >& asi1_inVector=asi1.allowStateAndSubStates();
	const vector<pair<int,vector<int> > >& asi2_inVector=asi2.allowStateAndSubStates();
	const vector<pair<int,vector<int> > >& asi3_inVector=asi3.allowStateAndSubStates();

	int lcSize=(int)asi1_inVector.size();
	int rcSize=(int)asi2_inVector.size();
	//if(n3.stop()-n3.start()<3)
	//	cerr<<n3.start()<<" "<<n3.stop()<<" : "<<lcSize<<","<<rcSize<<" "<<ruleSet.numOfState()<<endl;

	for(int lcIter=0; lcIter<lcSize;lcIter++)
	{
		int lc=asi1_inVector[lcIter].first;
		const vector<int>& lcSubs=asi1_inVector[lcIter].second;
		for(int lcSubIter=0;(size_t)lcSubIter<lcSubs.size();lcSubIter++)
		{
			int lcsub=lcSubs[lcSubIter];
			
			for(int rcIter=0;rcIter<rcSize;rcIter++)
			{
				int rc=asi2_inVector[rcIter].first;
				const vector<int>& rcSubs=asi2_inVector[rcIter].second;
				
				for(int rcSubIter=0;(size_t)rcSubIter<rcSubs.size();rcSubIter++)
				{
					int rcsub=rcSubs[rcSubIter];
					//here we make new node
					if(n3.useAllowState())
					{
						int prSize=(int)asi3_inVector.size();
						for(int prIter=0;prIter<prSize;prIter++)
						{
							int pr=asi3_inVector[prIter].first;
							const BerkeleyBinaryRule* rule=ruleSet.getBinaryRuleByRule(lc,rc,pr);
							if(rule==NULL) continue;

							const vector<int>& prSubs=asi3_inVector[prIter].second;
							for(int prSubIter=0;(size_t)prSubIter<prSubs.size();prSubIter++)
							{
								int prsub=prSubs[prSubIter];
								double ruleScore=rule->getScore(lcsub,rcsub,prsub);
								if(ruleScore>-1E10)
								{
									if(iot==inside)
									{
										if(n1.iScore(lc,lcsub)<0||n2.iScore(rc,rcsub)<0)cerr<<"bug3"<<endl;
										if(n3.iScore(pr,prsub)<0) n3.iScore(pr,prsub)=0;
										n3.iScore(pr,prsub)+=n1.iScore(lc,lcsub)*n2.iScore(rc,rcsub)*ruleScore;
									}
									else if(iot==outside)
									{
										if(n3.oScore(pr,prsub)<0) continue;  //cerr<<"bug4"<<endl;
										if(n1.oScore(lc,lcsub)<0) n1.oScore(lc,lcsub)=0;
										if(n2.oScore(rc,rcsub)<0) n2.oScore(rc,rcsub)=0;										
										n1.oScore(lc,lcsub)+=n3.oScore(pr,prsub)*n2.iScore(rc,rcsub)*ruleScore;
										n2.oScore(rc,rcsub)+=n3.oScore(pr,prsub)*n1.iScore(lc,lcsub)*ruleScore;
									}
								}
							}
						}
					}
					else
					{
						const vector<BerkeleyBinaryRule*>& rules=ruleSet.getBinaryRulesByChilds(lc,rc);
						for(int i=0;(size_t)i<rules.size();i++)
						{
							BerkeleyBinaryRule* rule=rules[i];
							if(rule==NULL)continue;

							int prNumSubState=rule->parentNumSubState();
							int pr=rule->parentState();
							//int prsub=0;
							for(int prsub=0;prsub<prNumSubState;prsub++)
							{
								double ruleScore=rule->getScore(lcsub,rcsub,prsub);
								if(ruleScore>-1E10)
								{
									if(iot==inside)
									{
										if(n1.iScore(lc,lcsub)<0||n2.iScore(rc,rcsub)<0)cerr<<"bug"<<endl;
										if(n3.iScore(pr,prsub)<0) n3.iScore(pr,prsub)=0;
										n3.iScore(pr,prsub)+=n1.iScore(lc,lcsub)*n2.iScore(rc,rcsub)*ruleScore;
									}
									else if(iot==outside)
									{
										if(n3.oScore(pr,prsub)<0)continue;//cerr<<"bug2"<<endl;
										if(n1.oScore(lc,lcsub)<0) n1.oScore(lc,lcsub)=0;
										if(n2.oScore(rc,rcsub)<0) n2.oScore(rc,rcsub)=0;
										n1.oScore(lc,lcsub)+=n3.oScore(pr,prsub)*n2.iScore(rc,rcsub)*ruleScore;
										n2.oScore(rc,rcsub)+=n3.oScore(pr,prsub)*n1.iScore(lc,lcsub)*ruleScore;
									}
								}
							}
						}
					}
					//make new node finished
				}
			}
		}
	}
	return n3;
}


bool 
Forest::
init_berkeley_withoutLabel_for_io(const BerkeleyRuleSet& ruleSet)
{
	clock_t sec=clock();
	for(int length=0;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;i<(int)_sentence.size()-length;i++)
		{
			(*this)[i][i+length].reset_scorePool(ruleSet.bsio());
		}
	}

	for(int i=0;(size_t)i<_sentence.size();i++)
	{
		//cout<<"word "<<i<<endl;
		Node& node=(*this)[i][i];
		string wrd=_sentence[i];
		const vector<BerkeleyUnaryRule*>& rules=ruleSet.getLexRulesByWord(wrd);
		const AllowStateInfo& asi=node.allowStateInfo();

		for(int k=0;(size_t)k<rules.size();k++)
		{
			BerkeleyUnaryRule* rule=rules[k];
			if(rule==NULL)continue;
			int chNumSubState=rule->childNumSubState();
			int prNumSubState=rule->parentNumSubState();
			int pr=rule->parentState();

			//cout<<"rule:"<<k<<",pr:"<<parentState<<",chN:"<<chNumSubState<<",prN:"<<prNumSubState<<"\t"<<rule->toLexRuleString()<<endl;
			
			if(!asi.inUse()||asi.isAllow(pr))
			{
				for(int prsub=0;prsub<prNumSubState;prsub++)
				{
					if(!asi.inUse()||asi.isAllow(pr,prsub))
					for(int chsub=0;chsub<chNumSubState;chsub++)
					{
						double ruleScore=rule->getScore(chsub,prsub);
						if(ruleScore>-1E10)
						{
							//cout<<pr<<","<<prsub<<":"<<ruleScore<<endl;
							node.iScore(pr,prsub)=ruleScore;
						}
					}
				}
			}
		}
		genUnaryNode_withoutLabel_berkeley(ruleSet,(*this)[i][i],inside);
		
	}
	//cerr<<"init finished"<<endl;
	initime+=clock()-sec;
	//exit(1);
	return true;
}

bool 
Forest::
io_berkeley_withoutLabel(const BerkeleyRuleSet& ruleSet, double threshold, bool toNextLevel)
{
	init_berkeley_withoutLabel_for_io(ruleSet);
	//make the forest and comput inside prob along the way
	for(int length=1;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;i<(int)_sentence.size()-length;i++)
		{
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//cerr<<i<<","<<j<<endl;
				//cerr<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				genNewNode_withoutLabel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],inside);
				bitime+=clock()-insec;
			}
			clock_t usec=clock();
			//cout<<"node ["<<i<<","<<i+length<<"]"<<endl;
			genUnaryNode_withoutLabel_berkeley(ruleSet,(*this)[i][i+length],inside);
			unatime+=clock()-usec;
			//(*this)[i][i+length].print_inside(cerr);
			//cout<<"allow State:"<<endl;
			//asi.print(cout);
		}
	}
	//compute outside prob top-down
	//first init the S1 node
	Node& node=(*this)[0][_sentence.size()-1];
	node.oScore(0,0)=1;
	//genUnaryNode_withoutLabel_berkeley(ruleSet,node,outside);
	//propogate

	for(int length=(int)_sentence.size()-1;length>=0;length--)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			clock_t usec=clock();
			genUnaryNode_withoutLabel_berkeley(ruleSet,(*this)[i][i+length],outside);
			unatime+=clock()-usec;
			//cout<<"node ["<<i<<","<<i+length<<"]"<<endl;
			//(*this)[i][i+length].print_io_withoutLabel(cout);
				
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//jout<<i<<","<<j<<endl;
				//jout<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				genNewNode_withoutLabel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],outside);
				bitime+=clock()-insec;
			}
			//cout<<"node ["<<i<<","<<i+length<<"]"<<endl;
			//(*this)[i][i+length].print_io_withoutLabel(cout);
		}
	}
	
	double s1InProb=(*this)[0][_sentence.size()-1].iScore(0,0);

	//cout<<"ROOT_O inside:"<<s1InProb<<endl;
	for(int length=0;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;i<(int)_sentence.size()-length;i++)
		{
			Node& node=(*this)[i][i+length];
			//cout<<"node ["<<i<<","<<i+length<<"]"<<endl;
			//node.set_allowStateAndSubState_withoutLabel(s1InProb,pow(exp(1.0),threshold));
			node.set_allowStateAndSubState_withoutLabel(s1InProb,threshold);
			if(toNextLevel)
				node.reset_allowStateInfo_to_nextLevel(ruleSet.bsio());
		}
	}
	return true;
}


