#ifndef HEAP_H
#define HEAP_H

#include <algorithm>
#include <vector>
using namespace std;

template<class _type>
class Heap {
public:
    Heap(){
        make_heap(data.begin(), data.end());
    }
   
    void push(_type entry){
        data.push_back(entry);
        push_heap(data.begin(), data.end());
    }
   
    _type top(){
		if(empty())
			return nullData;
        return *(data.begin());
    }
   
    _type pop(){
		if(empty())
			return nullData;
        _type ret;
        ret = top();
        pop_heap(data.begin(), data.end());
        data.pop_back();
        return ret;
    }
   
    bool empty(){
        return size() == 0;
    }
   
    bool clear(){
        data.clear();
        make_heap(data.begin(), data.end());
		return true;
    }
   
    int size(){
        return data.size();
    }
	void print();
private:
    vector<_type> data;
	_type nullData;
};

#endif
