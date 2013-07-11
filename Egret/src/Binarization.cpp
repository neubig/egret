#include "Rule.h"
#include "RuleSet.h"
#include "utils.h"
#include <vector>
#include <sstream>

using namespace std;

int binarize(Rule& rule, InterNodes& interNodes,string prefix, Direct dir)
{
	vector<string> wrds;
	wrds.clear();
	stringstream rss(rule.right.c_str());
	while(!rss.eof())
	{
		string curWrd="";
		rss>>curWrd;
		if(curWrd=="")break;
		wrds.push_back(curWrd);
	}
	vector<vector<string> > strMatrix;
	strMatrix.clear();
	for(int i=0;(size_t)i<wrds.size();i++)
	{
		vector<string> temp;
		strMatrix.push_back(temp);
		for(int j=0;(size_t)j<wrds.size();j++)
		{
			if(i==j)
				strMatrix[i].push_back(wrds[i]);
			else
				strMatrix[i].push_back("");
		}
	}
	for(int i=0;(size_t)i<wrds.size();i++)
	{
		for(int j=i+1;(size_t)j<wrds.size();j++)
		{
			strMatrix[i][j]=strMatrix[i][j-1]+wrds[j];
		}
	}
	jout<<"rightPart length:"<<(unsigned int)wrds.size()<<endl;

	for(int k=(int)wrds.size();k>1;k--)
	{
		for(int i=0;i<=(int)wrds.size()-k;i++)
		{
			if(dir==LEFT&&i!=wrds.size()-k)continue;
			if(dir==RIGHT&&i!=0)break;
			for(int j=i;j<i+k-1;j++)
			{
				if(dir==LEFT&&j!=i)break;
				if(dir==RIGHT&&j!=i+k-2)continue;
				cout<<i<<","<<j<<","<<k<<":["<<i<<","<<j<<"] ["<<j+1<<","<<i+k-1<<"]"<<endl;
				
				string par="";
				if(k==wrds.size())
					par=rule.left;
				else
					par=interNodes.query(strMatrix[i][i+k-1],prefix);
				string leftSon=strMatrix[i][j];
				if(j-i>0)
					leftSon=interNodes.query(leftSon,prefix);
				string rightSon=strMatrix[j+1][i+k-1];
				if(i+k-1-j-1>0)
					rightSon=interNodes.query(rightSon,prefix);
				Rule newRule;
				newRule.left=par;
				newRule.right=leftSon+" "+rightSon;
				if(dir==PERMUTE)
					newRule.prob=double(1)/(double)(k-1);
				else 
					newRule.prob=1;
				if(k==wrds.size())
					newRule.prob*=rule.prob;
				//newRule.print(cout);
				//cout<<endl;
			}
		}
	}
	return 0;
}

int binarize(Rule& rule, InterNodes& interNodes,string prefix, Direct dir, vector<Rule>& result)
{
	vector<string> wrds;
	wrds.clear();
	stringstream rss(rule.right.c_str());
	while(!rss.eof())
	{
		string curWrd="";
		rss>>curWrd;
		if(curWrd=="")break;
		wrds.push_back(curWrd);
	}
	vector<vector<string> > strMatrix;
	strMatrix.clear();
	for(int i=0;(size_t)i<wrds.size();i++)
	{
		vector<string> temp;
		strMatrix.push_back(temp);
		for(int j=0;(size_t)j<wrds.size();j++)
		{
			if(i==j)
				strMatrix[i].push_back(wrds[i]);
			else
				strMatrix[i].push_back("");
		}
	}
	for(int i=0;(size_t)i<wrds.size();i++)
	{
		for(int j=i+1;(size_t)j<wrds.size();j++)
		{
			strMatrix[i][j]=strMatrix[i][j-1]+wrds[j];
		}
	}
	jout<<"rightPart length:"<<(unsigned int)wrds.size()<<endl;

	for(int k=(int)wrds.size();k>1;k--)
	{
		for(int i=0;(size_t)i<=wrds.size()-k;i++)
		{
			if(dir==LEFT&&i!=wrds.size()-k)continue;
			if(dir==RIGHT&&i!=0)break;
			for(int j=i;j<i+k-1;j++)
			{
				if(dir==LEFT&&j!=i)break;
				if(dir==RIGHT&&j!=i+k-2)continue;
				cout<<i<<","<<j<<","<<k<<":["<<i<<","<<j<<"] ["<<j+1<<","<<i+k-1<<"]"<<endl;
				
				string par="";
				if(k==wrds.size())
					par=rule.left;
				else
					par=interNodes.query(strMatrix[i][i+k-1],prefix);
				string leftSon=strMatrix[i][j];
				if(j-i>0)
					leftSon=interNodes.query(leftSon,prefix);
				string rightSon=strMatrix[j+1][i+k-1];
				if(i+k-1-j-1>0)
					rightSon=interNodes.query(rightSon,prefix);
				Rule newRule;
				newRule.left=par;
				newRule.right=leftSon+" "+rightSon;
				if(dir==PERMUTE)
					newRule.prob=double(1)/(double)(k-1);
				else 
					newRule.prob=1;
				if(k==wrds.size())
					newRule.prob*=rule.prob;
				//newRule.print(cout);
				result.push_back(newRule);
				//cout<<endl;
			}
		}
	}
	return 0;
}

int binarize(Rule& rule, InterNodes& interNodes,string prefix, Direct dir, RuleSet& ruleSet)
{
	vector<string> wrds;
	wrds.clear();
	stringstream rss(rule.right.c_str());
	while(!rss.eof())
	{
		string curWrd="";
		rss>>curWrd;
		if(curWrd=="")break;
		wrds.push_back(curWrd);
	}
	vector<vector<string> > strMatrix;
	strMatrix.clear();
	for(int i=0;(size_t)i<wrds.size();i++)
	{
		vector<string> temp;
		strMatrix.push_back(temp);
		for(int j=0;(size_t)j<wrds.size();j++)
		{
			if(i==j)
				strMatrix[i].push_back(wrds[i]);
			else
				strMatrix[i].push_back("");
		}
	}
	for(int i=0;(size_t)i<wrds.size();i++)
	{
		for(int j=i+1;(size_t)j<wrds.size();j++)
		{
			strMatrix[i][j]=strMatrix[i][j-1]+wrds[j];
		}
	}
	//jout<<"rightPart length:"<<wrds.size()<<endl;
	if(wrds.size()==1) ruleSet.add(rule);
	for(int k=(int)wrds.size();k>1;k--)
	{
		for(int i=0;(size_t)i<=wrds.size()-k;i++)
		{
			if(dir==LEFT&&i!=wrds.size()-k)continue;
			if(dir==RIGHT&&i!=0)break;
			for(int j=i;j<i+k-1;j++)
			{
				if(dir==LEFT&&j!=i)break;
				if(dir==RIGHT&&j!=i+k-2)continue;
				//jout<<i<<","<<j<<","<<k<<":["<<i<<","<<j<<"] ["<<j+1<<","<<i+k-1<<"]"<<endl;
				
				string par="";
				if(k==wrds.size())
					par=rule.left;
				else
					par=interNodes.query(strMatrix[i][i+k-1],prefix);
				string leftSon=strMatrix[i][j];
				if(j-i>0)
					leftSon=interNodes.query(leftSon,prefix);
				string rightSon=strMatrix[j+1][i+k-1];
				if(i+k-1-j-1>0)
					rightSon=interNodes.query(rightSon,prefix);
				Rule newRule;
				newRule.left=par;
				newRule.right=leftSon+" "+rightSon;
				if(dir==PERMUTE)
					newRule.prob=double(1)/(double)(k-1);
				else 
					newRule.prob=1;
				if(k==wrds.size())
					newRule.prob*=rule.prob;
				//newRule.print(cout);
				ruleSet.add(newRule);
				//jout<<endl;
			}
		}
	}
	return 0;
}


string&
InterNodes::
query(string& str, string& prefix)
{
	if(find(str)==end())
	{
		(*this)[str]=prefix+"^g_"+intToString((int)size()+1);
	}
	return (*this)[str];
}
