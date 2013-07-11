#include "ExtentInfo.h"
void
ExtentInfo::
init(int sentLength, int numOfState)
{
	_sentLength=sentLength;
	_numOfState=numOfState;
	_extentInfo.clear();
	for(int i=0;i<sentLength;i++)
	{
		vector<Extent> tmp;
		_extentInfo.push_back(tmp);
		_extentInfo[i].clear();
		for(int j=0;j<numOfState;j++)
		{
			Extent extent;
			_extentInfo[i].push_back(extent);
			_extentInfo[i][j].asLC_RightExtentMin=10000;
			_extentInfo[i][j].asLC_RightExtentMax=-10000;
			
			_extentInfo[i][j].asRC_LeftExtentMin=10000;
			_extentInfo[i][j].asRC_LeftExtentMax=-10000;
		}
	}
}

bool 
ExtentInfo::
update(int start, int end, int state)
{
	if(start<0||start>_sentLength)return false;
	if(end<0||end>_sentLength||start>end)return false;
	_extentInfo[start][state].asLC_RightExtentMin=(int)min(_extentInfo[start][state].asLC_RightExtentMin,end);
	_extentInfo[start][state].asLC_RightExtentMax=(int)max(_extentInfo[start][state].asLC_RightExtentMax,end);
	_extentInfo[end][state].asRC_LeftExtentMin=(int)min(_extentInfo[end][state].asRC_LeftExtentMin,start);
	_extentInfo[end][state].asRC_LeftExtentMax=(int)max(_extentInfo[end][state].asRC_LeftExtentMax,start);
	return true;
}

bool 
ExtentInfo::
query_lc(int start, int state, int& REmin, int& REmax)
{
	if(start<0||start>_sentLength)return false;
	REmin=_extentInfo[start][state].asLC_RightExtentMin;
	REmax=_extentInfo[start][state].asLC_RightExtentMax;
	return true;
}

bool 
ExtentInfo::
query_rc(int end, int state, int& LEmin, int& LEmax)
{
	if(end<0||end>_sentLength)return false;
	LEmin=_extentInfo[end][state].asRC_LeftExtentMin;
	LEmax=_extentInfo[end][state].asRC_LeftExtentMax;
	return true;
}
