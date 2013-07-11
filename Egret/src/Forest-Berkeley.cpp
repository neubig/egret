#include "Forest.h"
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>

using namespace std;
IOType globalIOT=SUM;
/*********************** Berkeley ************************************/

//generate new node by unary rule with label
bool genUnaryNode_byLabel_constrained_berkeley(const BerkeleyRuleSet& ruleSet, Node& node, DecodeType decodetype)
{
	bool mark=false;
	vector<string> ntPool;
	map<string,int> ntMap;
	ntPool.clear();
	ntMap.clear();
	node.setUnaryInOrder_berkeley(ruleSet,ntPool);

	for(int i=0;(size_t)i<ntPool.size();i++)
		ntMap[ntPool[i]]=1;

	for(int i=0;(size_t)i<ntPool.size();i++)
	{
		string rightPart=ntPool[i];
		//cout<<"here:"<<i<<","<<rightPart<<endl;
		Label& oldLb=node[rightPart];

		int ch=oldLb.state();
		int chsub=oldLb.subState();

		const AllowStateInfo& asi=node.allowStateInfo();
		const vector<pair<int,vector<int> > >& allowStateAndSubStates=asi.allowStateAndSubStates();

		for(int i=0;(size_t)i<allowStateAndSubStates.size();i++)
		{
			int pr=allowStateAndSubStates[i].first;
			if(ch==pr)continue;
			const BerkeleyUnaryRule* rule=ruleSet.getUnaryRuleByRule(ch,pr);
			if(rule==NULL)continue;

			const vector<int>& subStates=allowStateAndSubStates[i].second;
			for(int j=0;(size_t)j<subStates.size();j++)
			{
				int prsub=subStates[j];
				double ruleScore=rule->getScore(chsub,prsub);
				if(ruleScore>-1E10)
				{
					Label label;
					label.init(ruleSet.state2term(pr),oldLb.start(),oldLb.stop(),pr,prsub);
					
					if(node.find(label.nt())==node.end())
					{
						node[label.nt()]=label;
						ntPool.push_back(label.nt());
					}
					else if(label.nt()!="ROOT_0"&&label.nt()!="S1")
					{
						label.nt()="u*"+label.nt();
						if(node.find(label.nt())==node.end())
						{
							node[label.nt()]=label;
							ntPool.push_back(label.nt());
						}
					}
					
					if(decodetype==VITERBI)
					{   // MAKE SURE THE RULESCORE IS IN LOG FORM
						node[label.nt()].addChild_viterbi(&oldLb,ruleScore);
					}
					else if(decodetype==IO)
					{
						//node[label.nt()].addChild_io_berkeley(&oldLb,ruleScore,io);
						node[label.nt()].addChild_io(&oldLb,ruleScore);
					}
					mark=true;
				}
			}
		}
	}
	return mark;
}


bool genUnaryNode_byLabel_unconstrained_berkeley(const BerkeleyRuleSet& brSet, Node& node, DecodeType decodetype)
{
	bool mark=false;
	vector<string> ntPool;
	map<string,int> ntMap;
	ntPool.clear();
	ntMap.clear();
	node.setUnaryInOrder_berkeley(brSet,ntPool);

	for(int i=0;(size_t)i<ntPool.size();i++)
		ntMap[ntPool[i]]=1;

	for(int i=0;(size_t)i<ntPool.size();i++)
	{
		string rightPart=ntPool[i];
		//cout<<"here:"<<i<<","<<rightPart<<endl;
		Label& oldLb=node[rightPart];

		int state=oldLb.state();
		int chsub=oldLb.subState();
		
		const vector<BerkeleyUnaryRule*>& rules=brSet.getUnaryRulesByChild(state);
		const AllowStateInfo& asi=node.allowStateInfo();
		for(int k=0;(size_t)k<rules.size();k++)
		{
			BerkeleyUnaryRule* rule=rules[k];
			if(rule==NULL)continue;
			int chNumSubState=rule->childNumSubState();
			int prNumSubState=rule->parentNumSubState();
			int parentState=rule->parentState();

			if(!asi.inUse()||asi.isAllow(parentState))
			{
				for(int prsub=0;prsub<prNumSubState;prsub++)
				{
					if(!asi.inUse()||asi.isAllow(parentState,prsub))
					//for(int chsub=0;chsub<chNumSubState;chsub++)
					{
						double ruleScore=rule->getScore(chsub,prsub);
						if(ruleScore>-1E10)
						{
							Label label;
							label.init(brSet.state2term(parentState),oldLb.start(),oldLb.stop(),parentState,prsub);
							
							if(node.find(label.nt())==node.end())
								node[label.nt()]=label;
							
							if(decodetype==VITERBI)
							{   // MAKE SURE THE RULESCORE IS IN LOG FORM
								node[label.nt()].addChild_viterbi(&oldLb,ruleScore);
							}
							else if(decodetype==IO)
							{
								//node[label.nt()].addChild_io_berkeley(&oldLb,ruleScore,io);
								node[label.nt()].addChild_io(&oldLb,ruleScore);
							}
							mark=true;
						}
					}
				}
			}
		}
	}
	return mark;
}


bool genUnaryNode_byLabel_berkeley(const BerkeleyRuleSet& ruleSet, Node& node, DecodeType decodetype)
{
	if(node.useAllowState())
	{
		return	genUnaryNode_byLabel_constrained_berkeley(ruleSet,node,decodetype);
	}
	else
		return  genUnaryNode_byLabel_unconstrained_berkeley(ruleSet,node,decodetype);
}

//generate new node by binary rule with label
Node& genNewNode_byLabel_constrained_berkeley(const BerkeleyRuleSet& ruleSet,Label& label1, Label& label2, Node& n3, DecodeType decodetype)
{
	int lc=label1.state(),lcsub=label1.subState(),rc=label2.state(),rcsub=label2.subState();
	
	const AllowStateInfo& asi=n3.allowStateInfo();
	const vector<pair<int,vector<int> > >& allowStateAndSubStates=asi.allowStateAndSubStates();

	for(int i=0;(size_t)i<allowStateAndSubStates.size();i++)
	{
		int pr=allowStateAndSubStates[i].first;
		const BerkeleyBinaryRule* rule=ruleSet.getBinaryRuleByRule(lc,rc,pr);
		if(rule==NULL)continue;

		const vector<int>& subStates=allowStateAndSubStates[i].second;
		for(int j=0;(size_t)j<subStates.size();j++)
		{
			int prsub=subStates[j];
	
				double ruleScore=rule->getScore(lcsub,rcsub,prsub);
				if(ruleScore>-1E10)
				{
					Label label;
					int _start=(int)min(label1.start(),label2.start());
					int _stop=(int)max(label1.stop(),label2.stop());
					assert(_start==n3.start()&&_stop==n3.stop());

					label.init(ruleSet.state2term(pr),_start,_stop,pr,prsub);
					
					if(n3.find(label.nt())==n3.end())
						n3[label.nt()]=label;
					
					if(decodetype==VITERBI)
						n3[label.nt()].addChildPair_viterbi(&label1,&label2,ruleScore);
					else if(decodetype==IO)
						// MAKE SURE THE RULESCORE IS IN NORMAL FORM
						n3[label.nt()].addChildPair_io(&label1,&label2,ruleScore);
				}
		}		
	}
	return n3;
}

Node& genNewNode_byLabel_unconstrained_berkeley(const BerkeleyRuleSet& ruleSet,Label& label1, Label& label2, Node& n3, DecodeType decodetype)
{

	int lc=label1.state(),lcsub=label1.subState(),rc=label2.state(),rcsub=label2.subState();
	const vector<BerkeleyBinaryRule*>& rules=ruleSet.getBinaryRulesByChilds(lc,rc);
	for(int i=0;(size_t)i<rules.size();i++)
	{
		BerkeleyBinaryRule* rule=rules[i];
		if(rule==NULL)continue;

		int prNumSubState=rule->parentNumSubState();
		int parentState=rule->parentState();

		for(int prsub=0;prsub<prNumSubState;prsub++)
		{
			double ruleScore=rule->getScore(lcsub,rcsub,prsub);
			if(ruleScore>-1E10)
			{
				string label_nt=ruleSet.state2term(parentState)+"_"+intToString(prsub);
				if(n3.find(label_nt)==n3.end())
				{
					Label label;
					int _start=(int)min(label1.start(),label2.start());
					int _stop=(int)max(label1.stop(),label2.stop());
					assert(_start==n3.start()&&_stop==n3.stop());

					label.init(ruleSet.state2term(parentState),_start,_stop,parentState,prsub);

					n3[label.nt()]=label;
				}
				// MAKE SURE THE RULESCORE IS IN NORMAL FORM
				if(decodetype==VITERBI)
					n3[label_nt].addChildPair_viterbi(&label1,&label2,ruleScore);
				else if(decodetype==IO)
					//n3[label.nt()].addChildPair_io_berkeley(&label1,&label2,ruleScore,io);
					n3[label_nt].addChildPair_io(&label1,&label2,ruleScore);
					//cout<<label11.nt()<<label31.nt()<<label21.nt()<<endl;
			}	
		}
	}

	return n3;
}

Node& genNewNode_byLabel_berkeley(const BerkeleyRuleSet& ruleSet,Label& label1, Label& label2, Node& n3, DecodeType decodetype)
{
	if(n3.useAllowState())
		return genNewNode_byLabel_constrained_berkeley(ruleSet,label1,label2,n3,decodetype);
	else
	{
		return genNewNode_byLabel_unconstrained_berkeley(ruleSet,label1,label2,n3,decodetype);
	}
}

Node& genNewNode_byLabel_berkeley(const BerkeleyRuleSet& ruleSet,Node& n1, Node& n2, Node& n3 , DecodeType decodetype)
{
	double score=1.0;
	clock_t sec=clock();

	for(Node::iterator n1iter=n1.begin();n1iter!=n1.end();n1iter++)
	{
		for(Node::iterator n2iter=n2.begin();n2iter!=n2.end();n2iter++)
		{
			Label& label1=n1iter->second;
			Label& label2=n2iter->second;
			genNewNode_byLabel_berkeley(ruleSet,label1,label2,n3,decodetype);
		}
	}
	if(!n3.useAllowState())
	{
		/*
		vector<vector<double> > prob;
		prob.clear();
		prob.reserve(100);
		for(int i=0;i<100;i++)
		{
			vector<double> temp;
			temp.clear();
			temp.reserve(100);
			prob.push_back(temp);
			for(int j=0;j<100;j++)
				prob[i].push_back(0);
		}
		*/
		double ** prob = (double**)malloc(sizeof(double*)*100);
		double *pool=(double*)malloc(sizeof(double)*100*100);
		for(int i=0;i<100;i++)
			//prob[i]=(double*)malloc(sizeof(double)*100);
			prob[i]=pool+i*100;
		for(int i=0;i<100;i++)
		{	
			for(int j=0;j<100;j++)
			{
				for(int k=0;k<100;k++)
				{
					prob[j][k]=0.9*0.9*.09;
					score=1.0*900*100;
				}
			}
		}
	}
	if(!n3.useAllowState())
		inbitime+=clock()-sec;
	return n3;
}
//main procedure
bool
Forest::
init_berkeley(const BerkeleyRuleSet& brSet,DecodeType decodetype)
{
	clock_t sec=clock();
	for(int i=0;(size_t)i<_sentence.size();i++)
	{
		//cout<<"word "<<i<<endl;
		Node& node=(*this)[i][i];
		string wrd=_sentence[i];
		const vector<BerkeleyUnaryRule*>& rules=brSet.getLexRulesByWord(wrd);
		const AllowStateInfo& asi=node.allowStateInfo();

		for(int k=0;(size_t)k<rules.size();k++)
		{
			BerkeleyUnaryRule* rule=rules[k];
			if(rule==NULL) continue;
			int chNumSubState=rule->childNumSubState();
			int prNumSubState=rule->parentNumSubState();
			int parentState=rule->parentState();

			//cout<<"rule:"<<k<<",pr:"<<parentState<<",chN:"<<chNumSubState<<",prN:"<<prNumSubState<<"\t"<<rule->toLexRuleString()<<endl;
			
			if(!asi.inUse()||asi.isAllow(parentState))
			{
				for(int prsub=0;prsub<prNumSubState;prsub++)
				{
					if(!asi.inUse()||asi.isAllow(parentState,prsub))
					for(int chsub=0;chsub<chNumSubState;chsub++)
					{
						double ruleScore=rule->getScore(chsub,prsub);
						if(ruleScore>-1E10)
						{
							Label label;
							label.init(brSet.state2term(parentState),i,i,parentState,prsub,wrd);
							//cout<<wrd<<" "<<label.nt()<<" "<<ruleScore<<endl;
							if(decodetype==VITERBI)
								label.addPreTermScore(ruleScore);
							else
							{
								label.addPreTermScore_io(ruleScore);
							}
							if(node.find(label.nt())==node.end())
								node[label.nt()]=label;
						}
					}
				}
			}
		}
		genUnaryNode_byLabel_berkeley(brSet,(*this)[i][i],decodetype);
	}
	//cerr<<"init finished"<<endl;
	initime+=clock()-sec;
	//exit(1);
	return true;
}

bool 
Forest::
viterbi_berkeley(const BerkeleyRuleSet& ruleSet, double threshold)
{
	init_berkeley(ruleSet,VITERBI);
	for(int length=1;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//cout<<i<<","<<j<<endl;
				//cout<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				//genNewNode_byLabel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],VITERBI);
				genNewNode_byLabel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],VITERBI);
				bitime+=clock()-insec;
			}
			clock_t usec=clock();
			//genUnaryNode_byLabel_berkeley(ruleSet,(*this)[i][i+length],VITERBI);
			genUnaryNode_byLabel_berkeley(ruleSet,(*this)[i][i+length],VITERBI);
			unatime+=clock()-usec;
		}
	}
	//jout<<"\n\ndel interNode:"<<endl;

/*	for(int length=0;length<_sentence.size();length++)
	{
		for(int i=0;i<_sentence.size()-length;i++)
		{
			(*this)[i][i+length].replace_interLabel(Normal);
		}
	}
*/	/*
	for(int i=0;i<_sentence.size();i++)
	{
		for(int j=i;j<_sentence.size();j++)
		{
			(*this)[i][j].replace_interLabel(Normal);
		}
	}
	*/
	//jout<<"del interNode done!"<<endl;

/*	jout<<"\n\nprint forest:"<<endl;
	for(int i=0;i<_sentence.size();i++)
	{
		for(int j=i;j<_sentence.size();j++)
		{
			jout<<endl<<i<<","<<j<<":\n";
			(*this)[i][j].print(jout);
		}
	}
	jout<<"print forest finished!"<<endl;
*/	
	return true;
}

bool 
Forest::
io_berkeley(const BerkeleyRuleSet& ruleSet, double threshold)
{
	init_berkeley(ruleSet,IO);
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
				genNewNode_byLabel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],IO);
				bitime+=clock()-insec;
			}
			clock_t usec=clock();
			genUnaryNode_byLabel_berkeley(ruleSet,(*this)[i][i+length],IO);
			unatime+=clock()-usec;
			//(*this)[i][i+length].print_inside(cerr);
		}
	}
	//compute outside prob top-down
	//first init the S1 node
	Node& node=(*this)[0][_sentence.size()-1];
	string root="ROOT_0";
	if(node.find(root)!=node.end())
	{
		Label& lb=node[root];
		lb.outsideProb()=1;
		lb.compute_children_outside();
		//cout<<"yes find ROOT_0"<<endl;
	}
	else return false;
	//propogate
	/*
	for(int length=_sentence.size()-1;length>=0;length--)
	{
		for(int i=0;i<_sentence.size()-length;i++)
		{
			genUnaryNode_byLabel_berkeley(ruleSet,(*this)[i][i+length],IO,outside);
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//jout<<i<<","<<j<<endl;
				//jout<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				genNewNode_byLabel_berkeley(ruleSet,n1,n2,(*this)[i][i+length],IO,outside);
				bitime+=clock()-insec;
			}
			//(*this)[i][i+length].print_inside(cout);
		}
	}
	*/
	
	for(int length=(int)_sentence.size()-1;length>=0;length--)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			Node& node=(*this)[i][i+length];
			node.compute_children_outside();
		}
	}
	
	double s1InProb=(*this)[0][_sentence.size()-1]["ROOT_0"].insideProb();

	//cout<<"ROOT_O inside:"<<s1InProb<<endl;
	for(int length=0;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			Node& node=(*this)[i][i+length];
			node.prune_and_setAllowState_byLabel(s1InProb,pow(exp(1.0),threshold));
			node.reset_allowStateInfo_to_nextLevel(ruleSet.bsio());
		}
	}
	return true;
}

bool 
Forest::
berkeley_viterbi_coarse2fine(BerkeleyRuleSets& brSets, ObjectFunctionType oft)
{
	int i=0;
	for(i=0;(size_t)i<brSets.size()-1;i++)
	{
		if(i==0)
			globalIOT=MAX;
		else
			globalIOT=SUM;
		//cout<<"level "<<i<<" , threshold: "<<brSets.threshold(i)<<endl;
		//
		if(i==0)
			io_berkeley_withoutLabel_initLevel_ruleByParent(brSets[i],brSets.threshold(i));
			//io_berkeley_withoutLabel_initLevel(brSets[i],brSets.threshold(i));
			//io_berkeley_withoutLabel(brSets[i],brSets.threshold(i));
		else 
			io_berkeley_withoutLabel(brSets[i],brSets.threshold(i),true);
			//io_berkeley(brSets[i],brSets.threshold(i));
		//cout<<"bi rule time:"<<(double)bitime/CLOCKS_PER_SEC<<"s"<<endl;
		//cout<<"una rule time"<<(double)unatime/CLOCKS_PER_SEC<<"s"<<endl;

	}
	//cout<<"level "<<i<<endl;
	if(oft==NONE)
	{
		brSets[brSets.size()-1].logrize();
		viterbi_berkeley(brSets[brSets.size()-1],brSets.threshold(brSets.size()-1));
	}
	else
	{
		io_berkeley_withoutLabel(brSets[brSets.size()-1],brSets.threshold(brSets.size()-1),false);
		viterbi_berkeley_objectFunction(brSets[brSets.size()-1], oft);
	}
	return true;
}
