/*
#include <iostream>
#include <stdlib.h>
#include "Bijection.h"
#include "Corpus.h"
#include "CorpusRepByIDs.h"
using namespace std;
typedef hash_map<int,string>::const_iterator ISiter;
bool ISiterComp(ISiter a, ISiter b)
{
	return a->first<b->first;
}
template<>
int Bijection<int,string>::
printXY(ostream& os)const
{
	
	vector<ISiter> m_ISiter;
	ISiter iter=X2YMap.begin();
	for(;iter!=X2YMap.end();iter++)
		m_ISiter.push_back(iter);
	sort(m_ISiter.begin(),m_ISiter.end(),ISiterComp);
	for(int i=0;i<m_ISiter.size();i++)
		os<<m_ISiter[i]->first<<"\t"<<m_ISiter[i]->second<<endl;
	return X2YMap.size();
}
*/

