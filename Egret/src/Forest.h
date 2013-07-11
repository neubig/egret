#ifndef FOREST_H
#define FOREST_H
#include "Node.h"
#include "Rule.h"
#include "Tree.h"
#include "BerkeleyRuleSet.h"
#include "ExtentInfo.h"

typedef enum 
{
	VITERBI,IO
}DecodeType;

#include <time.h>

class Forest : public vector<vector<Node> >{
public:
	Forest(string sent);
    bool prune(const string& rootSymbol, double threshold);
//general PCFG
	bool viterbi(RuleSet& ruleSet);
	bool io(RuleSet& ruleSet, ERSet& erSet, string rootSymbole="ROOT");
	bool io_afterParse(string rootSymbol, IorO ioro, IOType iot);
	bool io_afterParse(string rootSymbol,IOType iot=SUM){io_afterParse(rootSymbol,inside,iot);io_afterParse(rootSymbol,outside,iot);return true;}

//berkeley
	bool io_berkeley(const BerkeleyRuleSet& brSet, double threshold);
	bool io_berkeley_withoutLabel(const BerkeleyRuleSet& brSet, double threshold, bool toNextLevel);
	bool io_berkeley_withoutLabel_initLevel(const BerkeleyRuleSet& ruleSet, double threshold);
	bool io_berkeley_withoutLabel_initLevel_ruleByParent(const BerkeleyRuleSet& ruleSet, double threshold);
	bool viterbi_berkeley(const BerkeleyRuleSet& brSet, double threshold);
	bool viterbi_berkeley_objectFunction(const BerkeleyRuleSet& brSet, ObjectFunctionType obt);

	bool berkeley_viterbi_coarse2fine(BerkeleyRuleSets& brSets,ObjectFunctionType oft=NONE);
	bool replace_interLabel(InterType it);

// get results
	int genTrees(int num,vector<Tree*>& trees,string s1="ROOT");
	Label * topS(string s1="ROOT");

	void print_hyperedges(ostream& os)const;

private:
	bool init_viterbi(RuleSet& ruleSet);
	bool init_io(RuleSet& ruleSet);

//berkeley
	bool init_berkeley(const BerkeleyRuleSet& brSet,DecodeType decodetype=VITERBI);
	bool init_berkeley_withoutLabel_for_io(const BerkeleyRuleSet& brSet);
	bool init_berkeley_withoutLabel_for_io_initLevel(const BerkeleyRuleSet& ruleSet);
	bool init_berkeley_withoutLabel_for_io_initLevel(
			const BerkeleyRuleSet& ruleSet,
			ExtentInfo& extentInfo);
	bool init_berkeley_objectFunction(const BerkeleyRuleSet& brSet,ObjectFunctionType oft);
	void genNewNode_withoutLabel_initLevel_berkeley_ruleByParent(
			const BerkeleyRuleSet& ruleSet,
			int start, 
			int stop, 
			IorO iot, 
			ExtentInfo& extentInfo);

	//berkeley object function parsing
	double getObjectFunctionBinaryRuleScore(
				const BerkeleyRuleSet& ruleSet, 
				int start,
				int stop, 
				int middle, 
				int parent, 
				int leftChild, 
				int rightChild, 
				ObjectFunctionType oft);
	double getObjectFunctionUnaryRuleScore(
				const BerkeleyRuleSet& ruleSet, 
				int start,
				int stop, 
				int parent, 
				int child, 
				ObjectFunctionType oft);
	double getObjectFunctionLexRuleScore(
				const BerkeleyRuleSet& ruleSet, 
				int position,
				int pos, 
				ObjectFunctionType oft);
	bool   genUnaryNode_objectFunction_constrained_berkeley(
				const BerkeleyRuleSet& ruleSet, 
				int start, 
				int stop, 
				ObjectFunctionType oft);
	bool   genNewNode_objectFunction_constrained_berkeley(
			const BerkeleyRuleSet& ruleSet, 
			int start,
			int stop, 
			int middle,
			ObjectFunctionType oft);
	bool   make_objectFunction_tempUnaryRuleSet_for_one_node(
			const BerkeleyRuleSet& ruleSet, 
			int start, 
			int stop, 
			ObjectFunctionType oft,
			vector<vector<double> >& tempUnrayRuleSet);

	vector<string> _sentence;
};

extern int unary_fine;
extern clock_t initime;
extern clock_t inbitime;
extern clock_t bitime;
extern clock_t unatime;
extern clock_t init_bitime;
extern clock_t init_unatime;
extern clock_t final_bitime;
extern clock_t final_unatime;
#endif
