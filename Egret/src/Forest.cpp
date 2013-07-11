#include "Forest.h"
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>


using namespace std;
Forest::
Forest(string sent)
{
	stringstream ss(sent);
	while(!ss.eof())
	{
		string wrd;
		ss>>wrd;
		if(wrd!="") _sentence.push_back(wrd);
		if(ss.eof())break;
	}
	for(int i=0;(size_t)i<_sentence.size();i++)
	{
		vector<Node> vn;
		for(int j=0;(size_t)j<_sentence.size();j++)
		{
			Node nd(i,j);
			vn.push_back(nd);
		}
		push_back(vn);
	}
}

void 
Forest::
print_hyperedges(ostream& os)const
{
	for(unsigned int length=0;length<_sentence.size();length++)
	{
		for(unsigned int i=0;i<_sentence.size()-length;i++)
		{
			(*this)[i][i+length].print_hyperedges(os);
		}
	}
	//os<<endl;
}


Node& genNewNode_viterbi(RuleSet& ruleSet,Node& n1, Node& n2, Node& n3)
{
	clock_t sec=clock();
	for(Node::iterator n1iter=n1.begin();n1iter!=n1.end();n1iter++)
	{
		for(Node::iterator n2iter=n2.begin();n2iter!=n2.end();n2iter++)
		{
			Label& label1=n1iter->second;
			Label& label2=n2iter->second;
			string rightPart=label1.nt()+" "+label2.nt();
			if(ruleSet.count(rightPart)>0)
			{
				pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
				for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
				{
					//jout<<rightPart<<"==>"<<iter->second.left<<endl;
					double ruleScore=log(iter->second.prob);
					Label label;
					label.init(iter->second.left,label1.start(),label2.stop());
					if(n3.find(label.nt())==n3.end())
						n3[label.nt()]=label;
					n3[label.nt()].addChildPair_viterbi(&label1,&label2,ruleScore);
				}
			}
		}
	}
	inbitime+=clock()-sec;
	return n3;
}

Node& genNewNode_io(RuleSet& ruleSet,Node& n1, Node& n2, Node& n3)
{
	clock_t sec=clock();
	for(Node::iterator n1iter=n1.begin();n1iter!=n1.end();n1iter++)
	{
		for(Node::iterator n2iter=n2.begin();n2iter!=n2.end();n2iter++)
		{
			Label& label1=n1iter->second;
			Label& label2=n2iter->second;
			string rightPart=label1.nt()+" "+label2.nt();
			if(ruleSet.count(rightPart)>0)
			{
				pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
				for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
				{
					//jout<<rightPart<<"==>"<<iter->second.left<<endl;
					double ruleScore=iter->second.prob;// when io, use the original prob
					Label label;
					label.init(iter->second.left,label1.start(),label2.stop());
					if(n3.find(label.nt())==n3.end())
						n3[label.nt()]=label;
					n3[label.nt()].addChildPair_io(&label1,&label2,ruleScore);
				}
			}
		}
	}
	inbitime+=clock()-sec;
	return n3;
}

extern clock_t setutime;
bool genUnaryNode_viterbi_old(RuleSet& ruleSet,Node& node)
{
	bool mark=false;
	clock_t sec=clock();
	node.setUnaryInOrder(ruleSet);
	setutime+=clock()-sec;
	vector<LabelAndFamily>& uio=node.unaryInOrder();
	for(int i=0;(size_t)i<uio.size();i++)
	{
		LabelAndFamily& laf=uio[i];
		Label& oldLb=node[laf.term()];
		string rightPart=oldLb.nt();
		if(ruleSet.count(rightPart)>0)
		{
			int count=0;
			pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
			for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
			{
				string leftPart=iter->second.left;
				if(laf.ancestor().count(leftPart)>0)
				{
					double ruleScore=log(iter->second.prob);
					if(node.find(leftPart)==node.end())
					{
						Label newLabel;
						newLabel.init(leftPart,oldLb.start(),oldLb.stop());
						node[leftPart]=newLabel;
					}
					//cout<<"\n"<<leftPart<<"--->add Child: "<<rightPart<<endl;
					node[leftPart].addChild_viterbi(&oldLb,ruleScore);
					mark=true;
				}
			}
		}
	}
	return mark;
}

bool genUnaryNode_viterbi(RuleSet& ruleSet,Node& node)
{
	bool mark=false;
	vector<string> ntPool;
	map<string,int> ntMap;
	ntPool.clear();
	ntMap.clear();
	node.setUnaryInOrder(ruleSet,ntPool);

	for(int i=0;(size_t)i<ntPool.size();i++)
		ntMap[ntPool[i]]=1;

	for(int i=0;(size_t)i<ntPool.size();i++)
	{
		string rightPart=ntPool[i];
		//cout<<"here:"<<i<<","<<rightPart<<endl;
		Label& oldLb=node[rightPart];
		pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
		for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
		{
			string leftPart=iter->second.left;
			if(node.find(leftPart)==node.end())
			{
				if(ntMap.find(leftPart)==ntMap.end())
					ntPool.push_back(leftPart);
				Label newLabel;
				newLabel.init(leftPart,oldLb.start(),oldLb.stop());
				node[leftPart]=newLabel;
			}
			double ruleScore=log(iter->second.prob);
			//jout<<"\n"<<leftPart<<"--->add Child: "<<rightPart<<endl;
			node[leftPart].addChild_viterbi(&oldLb,ruleScore);
			mark=true;
		}
	}
	return mark;
}

bool genUnaryNode_viterbi_coarse_old(RuleSet& ruleSet,Node& node)
{
	bool mark=false;
	vector<string> ntPool;
	map<string,int> ntSet;
	ntPool.clear();
	ntSet.clear();
	for(Node::iterator iter=node.begin();iter!=node.end();iter++)
	{
		ntPool.push_back(iter->first);
		ntSet[iter->first]=0;
	}
	
	for(int i=0;(size_t)i<ntPool.size();i++)
	{
		string rightPart=ntPool[i];
		Label& oldLb=node[rightPart];
		//if(ruleSet.count(rightPart)==0)continue;
		pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
		for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
		{
			string leftPart=iter->second.left;
			if(ntSet.find(leftPart)==ntSet.end())
			{
				ntPool.push_back(leftPart);
				ntSet[leftPart]=0;
			}
			else if(ntSet[leftPart]!=0)continue;
			
			double ruleScore=log(iter->second.prob);
			if(node.find(leftPart)==node.end())
			{
				Label newLabel;
				newLabel.init(leftPart,oldLb.start(),oldLb.stop());
				node[leftPart]=newLabel;
			}
			//jout<<"\n"<<leftPart<<"--->add Child: "<<rightPart<<endl;
			node[leftPart].addChild_viterbi(&oldLb,ruleScore);
			ntSet[rightPart]=1;
			mark=true;
		}
	}
	return mark;
}
/*
bool genUnaryNode_viterbi(RuleSet& ruleSet,Node& node, int uf)
{
	if(uf==0)
		return genUnaryNode_viterbi_coarse(ruleSet, node);
	else if(uf==1)
		return genUnaryNode_viterbi_fine(ruleSet, node);
}
*/

bool genUnaryNode_io_old(RuleSet& ruleSet,Node& node)
{
	bool mark=false;
	clock_t sec=clock();
	node.setUnaryInOrder(ruleSet);
	setutime+=clock()-sec;
	vector<LabelAndFamily>& uio=node.unaryInOrder();
	for(int i=0;(size_t)i<uio.size();i++)
	{
		LabelAndFamily& laf=uio[i];
		Label& oldLb=node[laf.term()];
		string rightPart=oldLb.nt();
		if(ruleSet.count(rightPart)>0)
		{
			int count=0;
			pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
			for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
			{
				string leftPart=iter->second.left;
				if(laf.ancestor().count(leftPart)>0)
				{
					double ruleScore=iter->second.prob;
					if(node.find(leftPart)==node.end())
					{
						Label newLabel;
						newLabel.init(leftPart,oldLb.start(),oldLb.stop());
						node[leftPart]=newLabel;
					}
					//jout<<"\n"<<leftPart<<"--->add Child: "<<rightPart<<endl;
					node[leftPart].addChild_io(&oldLb,ruleScore);
					mark=true;
				}
			}
		}
	}
	return mark;
}

bool genUnaryNode_io(RuleSet& ruleSet,Node& node)
{
	bool mark=false;
	vector<string> ntPool;
	ntPool.clear();
	node.setUnaryInOrder(ruleSet,ntPool);
	
	for(int i=0;(size_t)i<ntPool.size();i++)
	{
		string rightPart=ntPool[i];
		Label& oldLb=node[rightPart];
		pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(rightPart);
		for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
		{
			string leftPart=iter->second.left;
			if(node.find(leftPart)==node.end())
			{
				ntPool.push_back(leftPart);
				Label newLabel;
				newLabel.init(leftPart,oldLb.start(),oldLb.stop());
				node[leftPart]=newLabel;
			}
			double ruleScore=iter->second.prob;
			//cerr<<"\n"<<leftPart<<"--->add Child: "<<rightPart<<endl;
			node[leftPart].addChild_io(&oldLb,ruleScore);
			mark=true;
		}
	}
	return mark;
}


bool
Forest::
init_viterbi(RuleSet& ruleSet)
{
	clock_t sec=clock();
	for(int i=0;(size_t)i<_sentence.size();i++)
	{
		Node& node=(*this)[i][i];
		string wrd=_sentence[i];
		pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(wrd);
		for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
		{
			double ruleScore=log(iter->second.prob);
			Label label;
			label.init(iter->second.left,i,i,wrd);
			//cout<<wrd<<endl;
			label.addPreTermScore(ruleScore);
			if(node.find(label.nt())==node.end())
				node[label.nt()]=label;
		}
		genUnaryNode_viterbi(ruleSet,(*this)[i][i]);
	}
	initime+=clock()-sec;
	//exit(1);
	return true;
}

bool
Forest::
init_io(RuleSet& ruleSet)
{
	clock_t sec=clock();
	for(int i=0;(size_t)i<_sentence.size();i++)
	{
		Node& node=(*this)[i][i];
		string wrd=_sentence[i];
		pair<RuleSet::RSIter,RuleSet::RSIter> iterPair=ruleSet.query(wrd);
		for(RuleSet::RSIter iter=iterPair.first;iter!=iterPair.second;iter++)
		{
			double ruleScore=iter->second.prob; //when io, we use the original prob, not log
			Label label;
			label.init(iter->second.left,i,i,wrd);
			//label.word()=wrd;
			//jout<<wrd<<endl;
			label.addPreTermScore_io(ruleScore);
			if(node.find(label.nt())==node.end())
				node[label.nt()]=label;
		}
		genUnaryNode_io(ruleSet,(*this)[i][i]);
	}
	initime+=clock()-sec;
	//exit(1);
	return true;
}


bool 
Forest::
viterbi(RuleSet& ruleSet)
{
	init_viterbi(ruleSet);
	for(int length=1;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//jout<<i<<","<<j<<endl;
				//jout<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				genNewNode_viterbi(ruleSet,n1,n2,(*this)[i][i+length]);
				bitime+=clock()-insec;
			}
			clock_t usec=clock();
			genUnaryNode_viterbi(ruleSet,(*this)[i][i+length]);
			unatime+=clock()-usec;
		}
	}
	//jout<<"\n\ndel interNode:"<<endl;
	for(int length=0;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			(*this)[i][i+length].replace_interLabel(Normal);
		}
	}

	/*
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
io(RuleSet& ruleSet, ERSet& erSet, string rootSymbole)
{
	init_io(ruleSet);
	//make the forest and comput inside prob along the way
	for(int length=1;(size_t)length<_sentence.size();length++)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			for(int j=i;j<i+length;j++)
			{
				Node& n1=(*this)[i][j];
				Node& n2=(*this)[j+1][i+length];
				//jout<<i<<","<<j<<endl;
				//jout<<j+1<<","<<i+length<<endl;
				clock_t insec=clock();
				genNewNode_io(ruleSet,n1,n2,(*this)[i][i+length]);
				bitime+=clock()-insec;
			}
			clock_t usec=clock();
			genUnaryNode_io(ruleSet,(*this)[i][i+length]);
			unatime+=clock()-usec;
		}
	}
	//compute outside prob top-down
	//first init the S1 node
	Node& node=(*this)[0][_sentence.size()-1];
	if(node.find(rootSymbole)!=node.end())
	{
		Label& lb=node[rootSymbole];
		lb.outsideProb()=1;
		lb.compute_children_outside();
		//jout<<"yes S1"<<endl;
	}
	else return false;
	//propogate
	for(int length=(int)_sentence.size()-1;length>=0;length--)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			Node& node=(*this)[i][i+length];
			node.compute_children_outside();
		}
	}
	double s1InProb=(*this)[0][_sentence.size()-1][rootSymbole].insideProb();
	for(int length=(int)_sentence.size()-1;length>=0;length--)
	{
		for(int i=0;(size_t)i<_sentence.size()-length;i++)
		{
			Node& node=(*this)[i][i+length];
			node.collect_rule_expectation(s1InProb,erSet);
		}
	}
	//node["S1"].collect_rule_expectation(node["S1"].insideProb(),erSet);
	return true;
}



Label*
Forest::
topS(string s1)
{
	Node& node=(*this)[0][_sentence.size()-1];
	if(node.find(s1)!=node.end())
		return &(node[s1]);
	else return NULL;
}

int 
Forest::
genTrees(int num,vector<Tree*>& trees,string s1)
{
	Label* p_lb;
	p_lb=topS(s1);
	if(p_lb==NULL) return 0;
	else 
	{
		p_lb->genTrees(num,trees);
	}
	return (int)trees.size();
}
bool 
Forest::
io_afterParse(string rootSymbol, IorO ioro, IOType iot)
{
	if(ioro==inside)
	{
		for(unsigned int length=0;length<_sentence.size();length++)
		{
			for(unsigned int i=0;i<_sentence.size()-length;i++)
			{
				(*this)[i][i+length].io_afterParse(ioro,iot);
			}
		}
	}
	else
	{
		Node& node=(*this)[0][_sentence.size()-1];
		vector<string>& uio=node.uio();
		for(unsigned int i=0;i<uio.size();i++)
		{
			string name=uio[i];
			if(name==rootSymbol)
				node[name].outsideProb()=1;
			else
				node[name].outsideProb()=0;
		}
		for(int length=static_cast<int>(_sentence.size()-1);length>=0;length--)
		{
			for(unsigned int i=0;i<_sentence.size()-length;i++)
			{
				(*this)[i][i+length].io_afterParse(ioro,iot);
			}
		}
	}
	return true;
}

bool 
Forest::
prune(const string& rootSymbol, double threshold)
{
	map<Label*,int> reachableLabels;
	Label* root=&(*this)[0][_sentence.size()-1][rootSymbol];
	reachableLabels[root]=1;
	double rootInsideProb=logrize(root->insideProb());
	
	for(int length=static_cast<int>(_sentence.size()-1);length>=0;length--)
	{
		for(unsigned int i=0;i<_sentence.size()-length;i++)
		{
			//process each node
			Node& node=(*this)[i][i+length];
			node.prune(reachableLabels,rootInsideProb,threshold);
		}
	}
	return true;
}

bool
Forest::
replace_interLabel(InterType it)
{
	for(unsigned int length=0;length<_sentence.size();length++)
	{
		for(unsigned int i=0;i<_sentence.size()-length;i++)
		{
			(*this)[i][i+length].replace_interLabel(it);
		}
	}
	for(unsigned int length=0;length<_sentence.size();length++)
	{
		for(unsigned int i=0;i<_sentence.size()-length;i++)
		{
			Node& node=(*this)[i][i+length];
			vector<string>& uio=node.uio();
			for(int uIter=static_cast<int>(uio.size()-1);uIter>=0;uIter--)
			{
				string& nt=uio[uIter];
				if(isInterNode(nt))
				{
					node.erase(node.find(nt));
					uio.erase(uio.begin()+uIter);
				}
			}
		}
	}
	return true;
}

