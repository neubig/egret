#include "BerkeleyRuleSet.h"
#include "utils.h"
void 
BerkeleyRuleSets::
init(string prefix, int level)
{
	//double threds[]={-9.75,-10,-9.6,-9.66,-8.01,-7.4};//-9.75,-10,-9.6,-9.66,-8.01,-7.4
	double threds[]={-8,-12,-12,-11,-12,-12,-14,-14};
	clear();
	_thresholds.clear();

	string stateFile=prefix+intToString(level-1)+".stateInfo";
	_bsi.init(stateFile);

	for(int i=0;i<level;i++)
	{
		BerkeleyRuleSet tmp;
		push_back(tmp);
	}
	for(int i=0;i<level;i++)
	{
		BerkeleyStateInfoOfOneLevel bsio;
		bsio.set_value_by_stateInfo(_bsi,i);
		string grFile,lexFile;
		grFile=prefix+intToString(i)+".grammar";
		lexFile=prefix+intToString(i)+".lexicon";
		cerr<<"loading: "<<endl
			<<grFile<<endl
			<<lexFile<<endl;
		_thresholds.push_back(threds[i]);
		(*this)[i].init(grFile,lexFile,bsio);
		(*this)[i].level()=i;
		//(*this)[i].save_by_ruleVector(prefix+intToString(i)+".test");
	}
	//for(int i=0;i<size();i++)
	{
		//(*this)[i].grammar().setRuleIntoVector();
		//(*this)[i].lexicon().setRuleIntoVector();
		//(*this)[i].save_by_ruleVector(prefix+intToString(i)+".test.2");
	}
}

void 
BerkeleyRuleSets::
save(string prefix)
{
	for(int i=0;i<6;i++)
	{
		string filename=prefix+intToString(i);
		(*this)[i].save(filename);
	}
}

