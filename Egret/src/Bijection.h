#ifndef BIJECTION_H
#define BIJECTION_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>


#ifdef WIN32
#include <hash_map>
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif
using namespace std;
//#ifdef _DEBUG
//using namespace stdext;
//#endif

#define hash_map map

template <class TypeX,class TypeY>
class Bijection{
	private:
		map<TypeX,TypeY> X2YMap;
		map<TypeY,TypeX> Y2XMap;
	public:
		bool queryX2Y(const TypeX& input, TypeY& result)const;
		bool queryY2X(const TypeY& input, TypeX& result)const;
		bool add(const TypeX& X, const TypeY& Y);
		bool del(const TypeX& X, const TypeY& Y);
		bool containX(const TypeX& X)const;
		bool containY(const TypeY& Y)const;
		bool containXY(const TypeX& X, const TypeY& Y)const;
		int printXY(ostream& os)const;
		size_t size()const{return X2YMap.size();}
		void clone(Bijection<TypeX,TypeY>& sun)const;
		void clear(){X2YMap.clear();Y2XMap.clear();};
};

template <class TypeX,class TypeY>
bool Bijection<TypeX,TypeY>::
	queryX2Y(const TypeX& input, TypeY& result)const
{
	if(X2YMap.find(input)!=X2YMap.end())
	{
		result=X2YMap.find(input)->second;
		return true;
	}
	else
	{
		return false;
	}
}

template <class TypeX,class TypeY>
bool Bijection<TypeX,TypeY>::
	queryY2X(const TypeY& input, TypeX& result)const
{
	if(Y2XMap.find(input)!=Y2XMap.end())
	{
		result=Y2XMap.find(input)->second;
		return true;
	}
	else
	{
		return false;
	}
}

template <class TypeX,class TypeY>
bool Bijection<TypeX,TypeY>::
	add(const TypeX& X, const TypeY& Y)
{
	if(!containX(X)&&!containY(Y))
	{
		X2YMap[X]=Y;
		Y2XMap[Y]=X;
		return true;
		
	}
	else return false;	
}

template <class TypeX,class TypeY>
bool Bijection<TypeX,TypeY>::
	del(const TypeX& X, const TypeY& Y)
{
	if(containXY(X,Y))
	{
		X2YMap.erase(X2YMap.find(X));
		Y2XMap.erase(Y2XMap.find(Y));
		return true;
	}
	else return false;
}

template <class TypeX,class TypeY>
bool Bijection<TypeX,TypeY>::
	containX(const TypeX& X)const
{
	return (X2YMap.find(X)!=X2YMap.end());
}

template <class TypeX,class TypeY>
bool Bijection<TypeX,TypeY>::
	containY(const TypeY& Y)const
{
	return (Y2XMap.find(Y)!=Y2XMap.end());
}

template <class TypeX,class TypeY>
bool Bijection<TypeX,TypeY>::
	containXY(const TypeX& X, const TypeY& Y)const
{
	if(containX(X)&&containY(Y))
	{
		TypeY tempY;
		if(!queryX2Y(X,tempY))//this should be always false
			return false;
		if(tempY==Y)
			return true;
	}
	else return false;
}

template <class TypeX,class TypeY>
void Bijection<TypeX,TypeY>::
clone(Bijection<TypeX,TypeY>& sun)const
{
	typename map<TypeX,TypeY>::const_iterator iter;
	iter=X2YMap.begin();
	for(;iter!=X2YMap.end();iter++)
	{
		sun.add(iter->first,iter->second);
	}
}

#endif

