#ifndef UNIQUEHEAP_H
#define UNIQUEHEAP_H

#include <algorithm>
#include <vector>
#include <set>

using namespace std;

template<class _type>
class UniqueHeap{
public:
    UniqueHeap(){
		dataSet.clear();
		vector<_type> tempVec;
		tempVec=data;
		data.clear();
		for(size_t i=0;i<tempVec.size();i++)
		{
			if(dataSet.count(tempVec[i])==0)
			{
				data.push_back(tempVec[i]);
				dataSet.insert(tempVec[i]);
			}
		}
        make_heap(data.begin(), data.end());
    }
   
    void push(_type entry){
		if(dataSet.count(entry)==0)
		{
			data.push_back(entry);
			push_heap(data.begin(), data.end());
			dataSet.insert(entry);
		}
    }
   
    _type top(){
		if(empty())
			return nullData;
		else
			return *(data.begin());
    }
   
    _type pop(){
        _type ret;
		if(empty())
		{
			return nullData;
		}
		else
		{
			ret = top();
			dataSet.erase(ret);
			pop_heap(data.begin(), data.end());
			data.pop_back();
			return ret;
		}
    }
   
    bool empty(){
        return size() == 0;
    }
   
    bool clear(){
        data.clear();
        make_heap(data.begin(), data.end());
		dataSet.clear();
		return true;
    }
   
    size_t size(){
        return data.size();
    }

	bool check()
	{
		return data.size()==dataSet.size();
	}
	void print();
private:
    vector<_type> data;	
	set<_type> dataSet;
	_type nullData;
};

#endif
