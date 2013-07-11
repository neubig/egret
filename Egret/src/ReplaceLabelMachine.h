#ifndef REPLACELABELMACHINE_H
#define REPLACELABELMACHINE_H
#include "Node.h"
#include <math.h>

class ReplaceLabelMachine
{
public:
	ReplaceLabelMachine(LabelArrays& las);
	bool replace_interLabel(InterType it);
	bool dump(LabelArrays& las);
private:
	LabelArrays& original_las;
	list<LabelArray> _lalist;
	map<string,LabelArray> _str2la;
};

#endif
