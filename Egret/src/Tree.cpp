#include "Tree.h"
#include <sstream>
#include <stdlib.h>
using namespace std;
string LRB="(",RRB=")";
void 
Tree::
print(ostream& os)
{
	string lrb,rrb;
	lrb=LRB,rrb=RRB;
	if(term()=="")	
	{
		os<<word();
		return;
	}
	// if(term()=="PU")
	// {
	// 	os<<lrb<<word();
	// 	//cerr<<"here"<<endl;
	// }
	// else	
	os<<lrb<<term();

	if(subTrees().empty())
	{
		os<<" "<<word();
		os<<rrb;
	}
	else
	{
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
		{
			os<<" ";
			(*iter)->print(os);
		}
		os<<rrb;
	}
	if(term()=="S1"||term()=="ROOT_0"||term()=="ROOT")
	{
		os<<"\nLog prob: "<<nodeScore()<<endl;
		os<<"ruleUsed:"<<endl;
		printRules(os);
	}
	return;
}

void 
Tree::
print(jstream& os)
{
	if(term()=="")	
	{
		os<<word();
		return;
	}
	os<<LRB<<term();
	if(subTrees().empty())
	{
		os<<" "<<word();
		os<<RRB;
	}
	else
	{
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
		{
			os<<" ";
			(*iter)->print(os);
		}
		os<<RRB;
	}
	if(term()=="S1"||term()=="ROOT")
	{
		os<<"\nLog prob: "<<nodeScore()<<endl;
		os<<"ruleUsed:"<<endl;
		printRules(os);
	}
	return;
}

void 
Tree::
tinyPrint(ostream& os)
{
	string lrb,rrb;
	lrb=LRB;rrb=RRB;

	//if(term()!="ROOT_0")
		os<<lrb;//<<term();
	if(subTrees().empty())
	{
		os<<" "<<word();
		os<<rrb;
	}
	else
	{
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
		{
			os<<" ";
			(*iter)->print(os);
		}
		//if(term()!="ROOT_0")
			os<<rrb;
	}
	//if(term()=="S1"||term()=="ROOT")
	//	os<<endl;
	return;
}

void 
Tree::
tinyPrint(jstream& os)
{
	string lrb,rrb;
	lrb="(";rrb=")";

	os<<lrb<<term();
	if(subTrees().empty())
	{
		os<<" "<<word();
		os<<rrb;
	}
	else
	{
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
		{
			os<<" ";
			(*iter)->print(os);
		}
		os<<rrb;
	}
	if(term()=="S1"||term()=="ROOT")
		os<<endl;
	return;
}

void 
Tree::
printRules(ostream& os)
{
	string children="";
	if(subTrees().empty())
	{
		string testRuleLength;
		stringstream tempss;
		tempss<<ruleScore();
		tempss>>testRuleLength;

		os<<ruleScore()<<"\t";
		if(testRuleLength.length()<8) os<<"\t";

		os<<term()<<" --> ";
		os<<word()<<endl;

		return;
	}
	else
	{
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
		{
			children+=(*iter)->term()+" ";
			(*iter)->printRules(os);
		}
	}
	string testRuleLength;
	stringstream tempss;
	tempss<<ruleScore();
	tempss>>testRuleLength;
	os<<ruleScore()<<"\t";
	if(testRuleLength.length()<8) os<<"\t";

	os<<term()<<" --> "<<children<<endl;
}

void 
Tree::
printRules(jstream& os)
{
	string children="";
	if(subTrees().empty())
	{
		string testRuleLength;
		stringstream tempss;
		tempss<<ruleScore();
		tempss>>testRuleLength;

		os<<ruleScore()<<"\t";
		if(testRuleLength.length()<8) os<<"\t";

		os<<term()<<" --> ";
		os<<word()<<endl;

		return;
	}
	else
	{
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
		{
			children+=(*iter)->term()+" ";
			(*iter)->printRules(os);
		}
	}
	string testRuleLength;
	stringstream tempss;
	tempss<<ruleScore();
	tempss>>testRuleLength;
	os<<ruleScore()<<"\t";
	if(testRuleLength.length()<8) os<<"\t";

	os<<term()<<" --> "<<children<<endl;
}

bool 
Tree::
destroy()
{
	//cerr<<_subTrees.size()<<endl;
	if(_subTrees.size()==0)
	{
	}
	else
	{
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
		{
			(*iter)->destroy();
			delete (*iter);
		}
		_subTrees.clear();
	}
	if(term()=="S1"||term()=="ROOT")
		delete this;
	return true;
}


void 
Tree::
delInterNode()
{
	if(subTrees().empty())
	{
	}
	else
	{
		SubTrees::iterator titer;
		for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter=titer)
		{
			titer=iter;
			titer++;
			(*iter)->delInterNode();
			if((*iter)->term().find("@")!=string::npos)
			{
				//cout<<"del "<<(*iter)->term()<<endl;
				SubTrees& subT=(*iter)->subTrees();
				for(SubTrees::iterator siter=subT.begin();siter!=subT.end();siter++)
				{
					(*siter)->parent()=this;
				}
				subTrees().insert(iter,subT.begin(),subT.end());
				ruleScore()+=(*iter)->ruleScore();
				delete *iter;
				subTrees().erase(iter);
			}
			else 
			{
				(*iter)->term()=getUPennTerm((*iter)->term());
			}
		}
	}
	return;
}

bool 
Tree::
readNormalTreeFromString(string& str)
{
	if(!subTrees().empty()) return false;
	size_t pos=0;
	while(true)
	{
		pos=str.find("[",pos);
		if(pos==string::npos)
			break;
		str.insert(pos+1," ");
		pos+=2;
	}
	pos=0;
	while(true)
	{
		pos=str.find("]",pos);
		if(pos==string::npos)
			break;
		str.insert(pos," ");
		pos+=2;
	}
	//cout<<str<<endl;
	stringstream ss(str);
	readNormalTreeFromStream(ss);
	return true;
}

bool
Tree::
readNormalTreeFromStream(istream& is)
{
	_subTrees.clear();
	string wd;
	is>>wd;
	if(wd=="]")
	{
		return false;
	}
	else if(wd=="[")
	{
		is>>wd;
		_term=wd;
		_wrd="";
		while(true)
		{
			Tree* p_tree=new Tree;
			if(p_tree->readNormalTreeFromStream(is))
			{
				_subTrees.push_back(p_tree);
				p_tree->parent()=this;
			}
			else
			{
				delete p_tree;
				break;
			}
		}
	}
	else
	{
		_wrd=wd;
		_term="";
	}
	_termID=atoi(_term.c_str());
	_wrdID=atoi(_wrd.c_str());
	if(parent()==NULL)setLevel();
	return true;
}

void 
Tree::
setLevel()
{
	if(parent()==NULL)
		_level=0;
	else
		_level=parent()->level()+1;
	for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
	{
		(*iter)->setLevel();
	}
}

void 
Tree::
cat_nodeInLevelAndID(NodeInLevelAndID& splitTreeNodes)
{
	if(parent()==NULL)
	{
		splitTreeNodes.clear();
	}
	int id;
	if(term()!="")
		id=termID();
	else id=wordID();

	splitTreeNodes[level()][id]=this;
	for(SubTrees::iterator iter=subTrees().begin();iter!=subTrees().end();iter++)
	{
		(*iter)->cat_nodeInLevelAndID(splitTreeNodes);
	}
}

/***************************************/
void
NodeInLevelAndID::
init(Tree& tree)
{
	tree.cat_nodeInLevelAndID(*this);
};

void
NodeInLevelAndID::
print(ostream& os)
{
	hierarchy_print(os,0,0);
}

void 
NodeInLevelAndID::
hierarchy_print(ostream& os,int level,int id)
{
	if((*this).find(level)==this->end())return;
	if((*this)[level].find(id)==(*this)[level].end())return;
	Tree* p_tree=(*this)[level][id];
	
	if(p_tree->subTrees().size()==0)
	{
		os<<p_tree->id();
		return;
	}
	else
	{
		os<<"["<<p_tree->id();
		for(SubTrees::iterator iter=p_tree->subTrees().begin();iter!=p_tree->subTrees().end();iter++)
		{
			os<<" ";
			hierarchy_print(os,level+1,(*iter)->id());
		}
		os<<"]";
	}
}

void 
NodeInLevelAndID::
clone(NodeInLevelAndID& sun)const
{
	sun.clear();
	NodeInLevelAndID::const_iterator iter1;
	map<int,Tree*>::const_iterator iter2;
	for(iter1=begin();iter1!=end();iter1++)
	{
		const map<int,Tree*>& tmpMap=iter1->second;
		for(iter2=tmpMap.begin();iter2!=tmpMap.end();iter2++)
		{
			sun[iter1->first][iter2->first]=iter2->second;
		}
	}
}

/**************utils**********************/
string getUPennTerm(string& str)
{
	string result;
	size_t pos=0;
	
	result=str;
	pos=str.find("_");
	if(pos!=string::npos)
		result=str.substr(0,pos);
	
	pos=str.find("^g");
	if(pos!=string::npos)
	{
		result=str.substr(0,pos);
	}
	pos=result.find("u*");
	if(pos!=string::npos)
	{
		result=result.substr(pos+2);
	}
	return result;
	
}
