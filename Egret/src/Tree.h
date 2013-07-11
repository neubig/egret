#ifndef TREE_H
#define TREE_H
#include <list>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "Jstream.h"
using namespace std;
class Tree;
typedef list<Tree*> SubTrees;
class NodeInLevelAndID;
class Tree
{
public:
	Tree(){_subTrees.clear();_parent=NULL;}
	~Tree(){}//{destroy();};
	bool readNormalTreeFromStream(istream& is);
	bool readNormalTreeFromString(string& str);
	string& term(){return _term;}
	string& word(){return _wrd;}
	int termID()const{return _termID;}
	int wordID()const{return _wrdID;}
	int id()const{if(_term!="")return termID();else return wordID();}
	void get_sunIDs(vector<int>& sun)const{sun.clear();for(SubTrees::const_iterator iter=_subTrees.begin();iter!=_subTrees.end();iter++)sun.push_back((*iter)->id());}
	int get_fatherID()const{if(_parent==NULL)return -1;else return _parent->id();}

	list<Tree*>& subTrees(){return _subTrees;}
	Tree*& parent(){return _parent;}
	void print(ostream& os);
	void print(jstream& os);
	void tinyPrint(ostream& os);
	void tinyPrint(jstream& os);
	void printRules(ostream& os);
	void printRules(jstream& os);
	//string toString();
	double& nodeScore(){return _nodeScore;}
	double& ruleScore(){return _ruleScore;}
	bool destroy();
	void delInterNode();
	void setLevel();
	int level(){return _level;}
	void cat_nodeInLevelAndID(NodeInLevelAndID& splitTreeNodes);
private:
	double _nodeScore;
	double _ruleScore;
	string _term,_wrd;
	int _termID,_wrdID,_level;
	list<Tree*> _subTrees;
	Tree* _parent;
};

class NodeInLevelAndID : public map<int, map<int,Tree*> >
{
public:
	void init(Tree& tree);
	void print(ostream& os);
	void hierarchy_print(ostream& os,int level,int id);
	void clone(NodeInLevelAndID& sun)const;
private:

};

inline bool isInterNode(string& str){return str.find("@")==0;}
string getUPennTerm(string& str);
#endif

