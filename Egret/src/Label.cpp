#include "Label.h"
#include "Jstream.h"
#include <iostream>
#include <sstream>
#include <string>
#include "ReplaceLabelMachine.h"
using namespace std;

int unary_fine=1;
/*
void 
UniqueHeap<BiRuleScoreInfo>::
print()
{
	for(int i=0;i<data.size();data++)
		data[i].score

}
*/

/********************Label*********************/

void 
Label::
print_hyperedges(ostream& os)const
{
	string labelRep=ntAndBoundary();
	const LabelArrays& childsPool=_childPairPool;
	for(unsigned int i=0;i<childsPool.size();i++)
	{
		const LabelArray& childs=childsPool[i];
		string rule=labelRep+"=> ";
		string rightPart="";
		rule+=childs.gen_string_represent(rightPart);
		rule+=" ||| ";
		rule+=doubleToString(childs.ruleScore());
		os<<rule<<endl;
	}
    
	if(childsPool.size()==0)
	{
		string rule=labelRep+"=> ";
		rule+=word()+" ";
		rule+="||| ";
		rule+=doubleToString(siList[0].score());
		os<<rule<<endl;
	}
}

bool
Label::
pop_from_heap_to_list()
{
	if(siHeap.empty()) return false;
	ScoreInfo si=siHeap.pop();
	siList.push_back(si);
	vector<ScoreInfo> m_si;

	LabelArray& lp=_childPairPool[si.childPairID()];
	//cout<<nt()<<endl;
	lp.shoulders(si.posInChs(),m_si,(int)si.childPairID());
	
	for(size_t i=0;i<m_si.size();i++)
	{
		siHeap.push(m_si[i]);
	}
	return true;
}

bool 
Label::
getNthBest(int nth, double& score)
{
	if(nth<=0) return false;
/*	cout<<nt()<<" ["<<start()<<","<<stop()<<"],"
		<<"heap size:"<<siHeap.size()
		<<", list size:"<<siList.size()
		<<", require: "<<nth<<endl;
*/
	while((int)siList.size()<nth)
	{
		if(siHeap.empty())
			break;
		pop_from_heap_to_list();
	}
//	cout<<"size:"<<siList.size()<<endl;
	if((int)siList.size()>=nth)
		score=siList[nth-1].score();
	else return false;
	return true;
}

void 
Label::
addChildPair_viterbi(Label* ch1, Label* ch2, double rs)
{
	if(siList.size()>0||notAcceptNewChilds())return;
	
	LabelArray lp(ch1,ch2,rs);
	//cout<<nt()<<"-->"<<ch1->nt()<<" "<<ch2->nt()<<" ruleScore:"<<lp.ruleScore()<<endl;
	_childPairPool.push_back(lp);

	double temp1,temp2;
	ch1->getNthBest(1,temp1);
	ch2->getNthBest(1,temp2);
	IntArray ia;
	ia.clear();
	ia.push_back(1);ia.push_back(1);
	ScoreInfo si(temp1+temp2+rs,_childPairPool.size()-1,ia);

/*	cout<<"\nlabel:"<<nt()<<":"<<start()<<","<<stop()<<endl;
	cout<<"_childPairPool size:"<<_childPairPool.size()<<endl;
	cout<<"add: "<<ch1->nt()<<","<<ch2->nt()<<endl;
	si.print(cout);
*/
	siHeap.push(si);
	ch1->set_notAcceptNewChilds()=true;
	ch2->set_notAcceptNewChilds()=true;
//	siHeap.top().print(cout);
}

bool 
Label::
addChild_viterbi(Label* ch1, double rs)
{
/*
	cout<<endl;
	cout<<"before add, _childPairPool size:"<<_childPairPool.size()<<endl;
*/	
	if(siList.size()>0||notAcceptNewChilds()) {return false;}
	vector<Label*> m_l;
	m_l.clear();
	m_l.push_back(ch1);
	LabelArray lp(m_l,rs);
	_childPairPool.push_back(lp);
	ch1->set_notAcceptNewChilds()=true;
	double temp1;
	if(!ch1->getNthBest(1,temp1))
	{
		cout<<"error here"<<endl;
		exit(1);
	}
	IntArray ia;
	ia.push_back(1);
	ScoreInfo si(temp1+rs,_childPairPool.size()-1,ia);
	/*
	cout<<"\nlabel:"<<nt()<<":"<<start()<<","<<stop()<<endl;
	cout<<"_childPairPool size:"<<_childPairPool.size()<<endl;
	cout<<"add: "<<ch1->nt()<<endl;
	cout<<"heap top"<<endl;
	siHeap.top().print(cout);
	cout<<"new score"<<endl;
	si.print(cout);
    cout<<endl;
	*/
	siHeap.push(si);

/*	cout<<"heap top"<<endl;
	siHeap.top().print(cout);
*/	
	//unaryChildSet().insert(ch1->unaryChildSet().begin(),ch1->unaryChildSet().end());
	//unaryChildSet().insert(ch1);
//	cout<<"after add, _childPairPool size:"<<_childPairPool.size()<<endl;
	return true;
}


void 
Label::
addChild_io(Label* ch1, double rs, IOType iot)
{
	if(notAcceptNewChilds()) {return;}
	vector<Label*> m_l;
	m_l.clear();
	m_l.push_back(ch1);
	LabelArray lp(m_l,rs);
	_childPairPool.push_back(lp);
	ch1->set_notAcceptNewChilds()=true;
	if(globalIOT==MAX)
		insideProb()=max(insideProb(),ch1->insideProb()*rs);
	else
		insideProb()+=ch1->insideProb()*rs;
	//cerr<<ch1->insideProb()<<","<<rs<<endl;
}

void 
Label::
addChildPair_io(Label* ch1, Label* ch2, double rs,IOType iot)
{
	if(notAcceptNewChilds()) {return;}
	LabelArray lp(ch1,ch2,rs);
	_childPairPool.push_back(lp);
	
	ch1->set_notAcceptNewChilds()=true;
	ch2->set_notAcceptNewChilds()=true;
	if(globalIOT==MAX)
		insideProb()=max(insideProb(),ch1->insideProb()*ch2->insideProb()*rs);
	else
		insideProb()+=ch1->insideProb()*ch2->insideProb()*rs;
	//cout<<"ch1"<<ch1->insideProb()<<",ch2:"<<ch2->insideProb()<<",rs:"<<rs<<endl;
}

void
Label::
addPreTermScore(double score)
{
	if(_stop-_start==0&&!_notAcceptNewChilds)
	{
		siList.clear();
		siHeap.clear();
		ScoreInfo si;
		si.set_score(score);
		si.set_childPairID(-100);
		siList.push_back(si);
		set_notAcceptNewChilds()=true;
	}
}

void 
Label::
addPreTermScore_io(double score)
{
	if(_stop-_start==0&&!_notAcceptNewChilds)
	{
		insideProb()=score;
		set_notAcceptNewChilds()=true;
	}
}

size_t
Label::
io_afterParse(IorO ioro, IOType iot)
{
	if(ioro==inside)
	{
		insideProb()=outsideProb()=0;
		if(_childPairPool.size()==0)
		{
			insideProb()=unLogrize(siList[0].score());
			return 0;
		}
		//cout<<nt()<<"["<<start()<<","<<stop()<<"]\tinside:"<<insideProb()<<",outside:"<<outsideProb()<<endl;
		for(unsigned int i=0;i<_childPairPool.size();i++)
		{
			LabelArray& lp=_childPairPool[i];
			
			const vector<Label*>& m_chs=lp.childs();
			double prob=1;
			for(size_t i=0;i<m_chs.size();i++)
			{
				Label* ch=m_chs[i];
				prob*=ch->insideProb();
			}
			prob*=unLogrize(lp.ruleScore());
			insideProb()=updateProb(insideProb(),prob,iot);
		}
		//cout<<nt()<<"["<<start()<<","<<stop()<<"]\tinside:"<<insideProb()<<",outside:"<<outsideProb()<<endl;
		return _childPairPool.size();
	}
	else
	{
		//cout<<nt()<<"["<<start()<<","<<stop()<<"]\tinside:"<<insideProb()<<",outside:"<<outsideProb()<<endl;
		for(unsigned int i=0;i<_childPairPool.size();i++)
		{
			LabelArray& lp=_childPairPool[i];
			
			const vector<Label*>& m_chs=lp.childs();
			
			for(size_t i=0;i<m_chs.size();i++)
			{
				Label* ch=m_chs[i];
				double prob=1;
				for(size_t j=0;j<m_chs.size();j++)
				{
					if(j!=i)
					{
						Label* ch_j=m_chs[j];
						prob*=ch_j->insideProb();
					}
				}
				prob*=outsideProb()*unLogrize(lp.ruleScore());
				ch->outsideProb()=updateProb(ch->outsideProb(),prob,iot);
			}
		}
		return _childPairPool.size();
	}
}

Tree* 
Label::
genTree(int nth, Tree& tree)
{
	tree.subTrees().clear();
	tree.parent()=NULL;
	tree.term()=nt();
	double score;
	if(!getNthBest(nth,score))
	{
		//cout<<nt()<< " here "<<nth<<" best NULL"<<endl;
		return NULL;
	}
	if(_childPairPool.size()>0)
	{
		ScoreInfo si=siList[nth-1];
		if(si.childPairID()<0)
		{
			tree.term()=nt();
			tree.word()=word();
			//cout<<"word is ";
			//cout<<word()<<endl;
			//cout<<siList[0].score<<" "<<nt()<<"-->"<<word()<<endl;
			tree.nodeScore()=si.score();
			tree.ruleScore()=si.score();
			return &tree;
		}
/*		cout<<nth<<", childPairID:"<<si.childPairID()<<" posInChs:";
		for(int i=0;i<si.posInChs().size();i++)
			cout<<si.posInChs()[i]<<",";
		cout<<" score : "<<si.score();
		cout<<endl;
*/		LabelArray& lp=_childPairPool[si.childPairID()];
		tree.nodeScore()=si.score();
		tree.ruleScore()=lp.ruleScore();

		if(lp.childs().size()==0)
		{
			cerr<<"NULL Child!"<<endl;
			return NULL;
		}
		else
		{
			for(size_t i=0;i<lp.childs().size();i++)
			{
				Tree* p_subTree= new Tree;
				lp.childs()[i]->genTree(si.posInChs()[i], *p_subTree);
				p_subTree->parent()=&tree;
				tree.subTrees().push_back(p_subTree);
			}
		}
		/*
		cout<<lp.ruleScore()<<" "<<tree.term()<<"-->";
		if(lp.child1!=NULL)cout<<lp.child1->nt()<<" ";
		if(lp.child2!=NULL)cout<<lp.child2->nt()<<" ";
		cout<<endl;
		*/
	}
	else if(word()!="")
	{
		tree.term()=nt();
		tree.word()=word();
		//cout<<"word is ";
		//cout<<word()<<endl;
		//cout<<siList[0].score<<" "<<nt()<<"-->"<<word()<<endl;
		tree.nodeScore()=siList[0].score();
		tree.ruleScore()=siList[0].score();
		return &tree;
	}
	return &tree;
}

int 
Label::
genTrees(int num, vector<Tree*>& trees)
{
	for(int i=1;i<=num;i++)
	{
		Tree* p_tree=new Tree;
		if(genTree(i,*p_tree)!=NULL)
		{
		/*	
			if(num>1)
				cout<<i<<"th best"<<endl;
			if(num>1)
				p_tree->print(cout);
			else
				p_tree->tinyPrint(cout);
			cout<<endl;
		*/	trees.push_back(p_tree);
		//p_tree->print(cout);
		}
	}
	return (int)trees.size();
}

//这样做的前提是子节点都已经生成siHeap
int
Label::
regenerate_nbest()
{
	if(word()!="") return 0;
	//if(nt().find("^g")!=string::npos)return 0;
	//cout<<nt()<<": regenerate nbest"<<endl;
	siList.clear();
	siHeap.clear();
	LabelArrays& las=_childPairPool;
	for(int i=0; (size_t)i<las.size();i++)
	{
		LabelArray& lp=las[i];
		const LabelPointers& chs=lp.childs();
		double score=0;
		IntArray ia;
		ia.clear();
		for(LabelPointers::const_iterator chIter=chs.begin();chIter!=chs.end();chIter++)
		{
			double temp;
			Label* p_label=*chIter;
			p_label->getNthBest(1,temp);
			score+=temp;
			ia.push_back(1);
		}
		score+=lp.ruleScore();//just to be the same calculation order as the original 

		string rep;
		lp.gen_string_represent(rep);
		//cout<<rep<<":"<<score<<","<<lp.ruleScore()<<endl;
		ScoreInfo si(score,i,ia);
		siHeap.push(si);
	}

	//double temp;
	//getNthBest(1,temp);
	//cout<<"here : "<<nt()<<"["<<start()<<","<<stop()<<"], "<<temp<<endl<<endl;
	return 0;
}

bool 
Label::
is_interNode(InterType it)
{
	if(it==Normal)
	{	
		//cout<<nt()<<" "<<(nt().find("^g_")!=string::npos)<<endl;
		return nt().find("^i_")!=string::npos;
	}
	else if(it==Berkeley)
	{
		return nt().find("@")!=string::npos;
	}
	return false;
}

bool 
Label::
replace_interLabel(InterType it)
{
	bool result=false;
	ReplaceLabelMachine rlm(_childPairPool);
	result=rlm.replace_interLabel(it);
	if(word()=="")
	{
		regenerate_nbest();
	}
	return result;
}

/**************************LabelArray********************************/

bool 
LabelArray::
pairscore(IntArray& ia, double& score)
{
	assert(ia.size()==_childs.size());
	if(ia.size()!=_childs.size())return false;
	score=ruleScore();
	for(size_t i=0;i<_childs.size();i++)
	{
		double temp;
		if(!_childs[i]->getNthBest(ia[i],temp))
			return false;
		score+=temp;
	}
	return true;
}

int 
LabelArray::
shoulders(const IntArray& ia, vector<ScoreInfo>& m_si,int posInChildPair)
{
	m_si.clear();
	vector<IntArray> m_ia;
	ia.shoulders(m_ia);
	for(int i=0;(size_t)i<m_ia.size();i++)
	{
		double sc;
		if(pairscore(m_ia[i],sc))
		{
			ScoreInfo si;
			m_si.push_back(si);
			m_si[m_si.size()-1].set_childPairID(posInChildPair);
			m_si[m_si.size()-1].set_posInChs(m_ia[i]);
			m_si[m_si.size()-1].set_score(sc);
			//m_si[m_si.size()-1].print(cout);
		}
	}
	return (int)m_si.size();
}

bool
LabelArray::
check_and_replace_interLabel(list<LabelArray>& lalist , InterType it)
{
	bool result=false;
	for(LabelPointers::iterator chIter=_childs.begin();chIter!=_childs.end();chIter++)
	{
		//Label* lpt=*chIter;
		if((*chIter)->is_interNode(it))
		{
			LabelPointers::iterator tempChIter=chIter;
			tempChIter++;
			LabelPointers before,behind;
			before.clear();behind.clear();
			before.insert(before.end(),_childs.begin(),chIter);
			behind.insert(behind.end(),tempChIter,_childs.end());
			
			const LabelArrays& las=(*chIter)->childPairPool();
			for(LabelArrays::const_iterator laIter=las.begin();laIter!=las.end();laIter++)
			{
				const LabelArray& middleLP=*laIter;
				const LabelPointers& middle=laIter->get_childs();

				LabelArray emptyLP;
				lalist.push_back(emptyLP);
				LabelArray& backLP=lalist.back();
				backLP.set_score(_ruleScore+middleLP.ruleScore());

				LabelPointers& newChilds=backLP.set_childs();
				newChilds.clear();
				newChilds.insert(newChilds.end(),before.begin(),before.end());
				newChilds.insert(newChilds.end(),middle.begin(),middle.end());
				newChilds.insert(newChilds.end(),behind.begin(),behind.end());
			}
			result=true;
			break;
		}
	}
	return result;
}

string
LabelArray::
gen_string_represent(string& represent)const
{
	represent="";
	stringstream ss;
	for(LabelPointers::const_iterator chIter=_childs.begin();chIter!=_childs.end();chIter++)	
	{
		ss<<(*chIter)->nt()<<"["<<(*chIter)->start()<<","<<(*chIter)->stop()<<"] ";
	}
	represent=ss.str();
	//cout<<represent<<endl;
	return represent;
}

void 
LabelArray::
print(ostream& os)const
{
	os<<"childPair: "<<", "; 
	for(int i=0;(size_t)i<_childs.size();i++)
		os<<_childs[i]->nt()<<"["
			<<_childs[i]->start()<<","
			<<_childs[i]->stop()<<"]  "; os<<" score:"<<_ruleScore<<endl;
}

/****************************LabelAndFamily**************************/
void 
LabelAndFamily::
print(ostream& os)
{
	os<<"\nlabel:"<<term()<<endl
		<<"ancestor:";
	for(set<string>::iterator iter=ancestor().begin();iter!=ancestor().end();iter++)
	{
		os<<*iter<<" ";
	}
	os<<"\ndescendant:";
	for(set<string>::iterator iter=descendant().begin();iter!=descendant().end();iter++)
	{
		os<<*iter<<" ";
	}
	os<<endl;
}

void 
LabelAndFamily::
print(jstream& os)
{
	os<<"\nlabel:"<<term()<<endl
		<<"ancestor:";
	for(set<string>::iterator iter=ancestor().begin();iter!=ancestor().end();iter++)
	{
		os<<*iter<<" ";
	}
	os<<"\ndescendant:";
	for(set<string>::iterator iter=descendant().begin();iter!=descendant().end();iter++)
	{
		os<<*iter<<" ";
	}
	os<<endl;
}


int 
Label::
collect_rule_expectation(double s1InProb, ERSet& erSet)
{
/*				cout<<"label:"<<nt()<<",";
				cout<<"inside:"<<insideProb()<<",";
				cout<<"outside:"<<outsideProb()<<","
					<<"s1InProb:"<<s1InProb<<endl;
*/	
	if(_childPairPool.size()==0&&(stop()-start()==0))//pre terminal
	{
			string leftPart=nt();
			string rightPart=word();
			double rcount=(outsideProb()*insideProb()/s1InProb);
			double lcount=(insideProb()*outsideProb()/s1InProb);
			//cout<<insideProb()<<","<<outsideProb()<<endl;
			//cout<<lcount<<","<<rcount<<endl;
			//erSet.addNtCount(leftPart,rcount);
				cout.precision(5);
				cout.setf(ios::fixed,ios::floatfield);
			jout<<"addcount: "<<rcount<<"\t"<<nt()<<" --> "<<rightPart<<"\t"
				<<" => "
				<<outsideProb()<<"*"<<insideProb()<<"/"<<s1InProb<<endl;
			erSet.addRuleCount(leftPart,rightPart,rcount);
	}

	for(int i=0;(size_t)i<_childPairPool.size();i++)
	{
		LabelArray& lp=_childPairPool[i];
		const vector<Label*>& m_chs=lp.childs();
		double lcount=outsideProb()*insideProb()/s1InProb;
		double rcount=outsideProb()*lp.ruleScore()/s1InProb;
		string leftPart=nt();
		string rightPart="";
		for(size_t i=0;i<m_chs.size();i++)
		{
			rcount*=m_chs[i]->insideProb();
			rightPart+=m_chs[i]->nt();
			if(i!=m_chs.size()-1)rightPart+=" ";
		}
		erSet.addRuleCount(leftPart,rightPart,rcount);
								cout.precision(5);
		cout.setf(ios::fixed,ios::floatfield);
		jout<<"addcount: "<<rcount<<"\t"<<nt()<<" --> "<<rightPart<<"\t"
				<<" => "
				<<outsideProb()<<"*";
		for(size_t i=0;i<m_chs.size();i++)
		{
			jout<<m_chs[i]->insideProb();
			if(i!=m_chs.size()-1)jout<<"*";
		}
		jout<<"*"<<lp.ruleScore()<<"/"<<s1InProb<<endl;
	}
	return (int)_childPairPool.size();
}


int 
Label::
compute_children_outside(IOType iot)
{
	//cout<<nt()<<"["<<start()<<","<<stop()<<"]\tinside:"<<insideProb()<<",outside:"<<outsideProb()<<endl;
	for(int i=0;(size_t)i<_childPairPool.size();i++)
	{
		LabelArray& lp=_childPairPool[i];
		
		const vector<Label*>& m_chs=lp.childs();
		
		for(size_t i=0;i<m_chs.size();i++)
		{
			Label* ch=m_chs[i];
			double prob=1;
			for(size_t j=0;j<m_chs.size();j++)
			{
				if(j!=i)
				{
					Label* ch_j=m_chs[j];
					if(j!=i)prob*=ch_j->insideProb();
				}
			}
			prob*=outsideProb()*lp.ruleScore();
			//cout<<"addoutside: "<<ch->nt()<<"["<<ch->start()<<","<<ch->stop()<<"]\t"<<prob<<endl;
			if(globalIOT==MAX)
				ch->outsideProb()=max(ch->outsideProb(),prob);
			else
				ch->outsideProb()+=prob;
		}
	}
	return (int)_childPairPool.size();
}
double updateProb(double prob1, double prob2, IOType iot)
{
	if(iot==MAX)
		return max(prob1,prob2);
	else
		return prob1+prob2;
}


