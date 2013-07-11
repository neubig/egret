#include "ReplaceLabelMachine.h"

ReplaceLabelMachine::
ReplaceLabelMachine(LabelArrays& las):original_las(las)
{
	for(LabelArrays::iterator lpiter=las.begin();lpiter!=las.end();lpiter++)
	{
		_lalist.push_back(*lpiter);
	}
	_str2la.clear();
}

//这样做会快，有一个前提，那就是，处理当前Label的时候，它的从属Label都已经被替换完毕，就可以想DP一样节省时间
//如果不满足这个条件，也可完成任务，就是会慢一点
bool 
ReplaceLabelMachine::
replace_interLabel(InterType it)
{
	bool result=false;
//	cout<<"\nbefore replace"<<endl;
	for(list<LabelArray>::iterator laIter=_lalist.begin(),tempIter;laIter!=_lalist.end();laIter=tempIter)
	{
		LabelArray& la=*laIter;
		string represent;
		la.gen_string_represent(represent);
		
//		cout<<"\trepresent: " <<represent<<endl;
		
		if(!la.check_and_replace_interLabel(_lalist,it))
		{
			if(_str2la.find(represent)==_str2la.end())
			{
				_str2la[represent]=la;
			}
			else
			{
				LabelArray& la_ref=_str2la[represent];
				double score=0;
				//score=log(exp(la_ref.ruleScore())+exp(la.ruleScore()));
				score=max(la_ref.ruleScore(),la.ruleScore());
				la_ref.set_score(score);
			}
			result=true;
		}	
		//以下两行不能往上提，也不能往下，否则均会出错
		tempIter=laIter;
		tempIter++;
		_lalist.erase(laIter);
	}
/*	cout<<"replace finished"<<endl;
	if(result)
	for(map<string,LabelArray>::iterator laIter=_str2la.begin();laIter!=_str2la.end();laIter++)
	{
		LabelArray& la=laIter->second;
		string represent;
		la.gen_string_represent(represent);
		cout<<"\trepresent: " <<represent<<endl;
	}
*/	
	if(result)
	{
		dump(original_las);
	}
	return result;
}

bool 
ReplaceLabelMachine::
dump(LabelArrays& las)
{
	las.clear();
	for(map<string,LabelArray>::iterator mIter=_str2la.begin();mIter!=_str2la.end();mIter++)
	{
		las.push_back(mIter->second);
	}
	return true;
}
