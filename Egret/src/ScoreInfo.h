#ifndef ScoreInfo_H
#define ScoreInfo_H
#include <iostream>
#include <vector>
#include "Jstream.h"
using namespace std;

class IntArray : public vector<int>
{
public:
	friend ostream& operator <<(ostream& os, const IntArray& ia){ia.print(os);return os;}
	void print(ostream& os)const
	{
		for(size_t i=0;i<size();i++)
		{
			os<<(*this)[i];
			if(i!=size()-1) 
				os<<",";
		}
	}
	bool operator <(IntArray& ia)
	{
		if(size()<ia.size())return true;
		if(size()>ia.size())return false;
		for(size_t i=0;i<size();i++)
		{
			if((*this)[i]<ia[i])
				return true;
			else if((*this)[i]>ia[i])
				return false;
		}
		return false;
	}
	bool operator ==(IntArray& ia)
	{
		if(size()!=ia.size())return false;
		for(size_t i=0;i<size();i++)
		{
			if((*this)[i]!=ia[i])
				return false;
		}
		return true;
	}
	bool operator > (IntArray& ia)
	{
		if(size()<ia.size())return false;
		if(size()>ia.size())return true;
		for(size_t i=0;i<size();i++)
		{
			if((*this)[i]>ia[i])
				return true;
			else if((*this)[i]<ia[i])
				return false;
		}
		return false;
	}
	vector<IntArray>& shoulders(vector<IntArray>& m_ia)const
	{
		m_ia.clear();
		for(size_t i=0;i<size();i++)
		{
			m_ia.push_back((*this));
			m_ia[i][i]++;
		}
		return m_ia;
	}
};

class ScoreInfo
{
public:
	ScoreInfo(){_score=-1;_childPairID=-1;_posInChs.clear();}
	ScoreInfo(double sc, size_t cpi, IntArray& pic):
	  _score(sc),_childPairID(cpi),_posInChs(pic){}

	double score()const{return _score;}
	size_t childPairID()const{return _childPairID;}
	const IntArray& posInChs()const{return _posInChs;}

	double set_score(double sc){return _score=sc;}
	size_t set_childPairID(size_t cpi){return _childPairID=cpi;}
	const IntArray& set_posInChs(IntArray& ia){return _posInChs=ia;}

	void print(ostream& os)const{os<<"childPairID:"<<_childPairID<<" posInChs:"<<_posInChs<<" score:"<<_score<<endl;}
	
	bool operator < (const ScoreInfo& si)const
	{
/*		if(_score==si.score()&&!((*this)==si))
		{
			cout<<"compare:"<<endl;
			print(cout);
			si.print(cout);
		}
*/		if(_score<si.score())
		{
			//cout<<"<"<<endl;
			return true;
		}
		else if(_score>si.score())
			return false;
		else if(_childPairID<si.childPairID())
		{
			return true;
		}
		else if(_childPairID>si.childPairID())
			return false;
		else if(_posInChs<si.posInChs())
		{
			return true;
		}
		else return false;
	}
	
	bool operator == (const ScoreInfo& si)const
	{
		if( _score==si.score()&&
			_childPairID==si.childPairID()&&
			_posInChs==si.posInChs())
			return true;
		else
			return false;
	}
	/*bool operator > (const ScoreInfo& si)const
	{
		if(_score>si.score())
			return true;
		else if(_score<si.score())
			return false;
		else if(_childPairID<si.childPairID())
			return true;
		else if(_childPairID>si.childPairID())
			return false;
		else if(_posInChs>si.posInChs())
			return true;
		else return false;
	}*/

private:
	double _score;
	size_t _childPairID;
	IntArray _posInChs;
};

#endif
