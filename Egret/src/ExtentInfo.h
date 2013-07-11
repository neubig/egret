#ifndef EXTENTINFO_H
#define EXTENTINFO_H
#include <vector>
using namespace std;
typedef struct
{	
	int asLC_RightExtentMin,asLC_RightExtentMax,asRC_LeftExtentMin,asRC_LeftExtentMax;
}Extent;

class ExtentInfo{
public:
	void init(int stentLength, int numOfState);
	bool update(int start, int end, int state);
	bool query_lc(int start, int state, int& REmin, int& REmax);
	bool query_rc(int end, int state, int& LEmin, int& LEmax);
private:
	int _sentLength,_numOfState;
	vector<vector<Extent> > _extentInfo;
};

#endif
