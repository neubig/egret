#include "UniqueHeap.h"
#include "ScoreInfo.h"
#include "Rule.h"
#include "Forest.h"
#include "JKArgs.h"
#include "Jstream.h"
#include "BerkeleyRuleSet.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include "utils.h"
#include <math.h>
extern int unary_fine;

using namespace std;
void usage()
{
	cerr<<"egret -lapcfg -i=text -oft=[variational|max_rule_sum|max_rule_product|max_prob|viterbi] [-data=grammar_directory] [-n=nbest] [-debug=dlevel]\n"
		<<"              [-grammarLevel=level] [-printForest] [-threshold=double] [-nbest4threshold=int]"<<endl
		<<"		do syntax parsing with latent annotation pcfg (in the same way as berkeley parser, and with same level accuracy)"<<endl
		<<"		oft is the Object Function Type, default is max_rule_product"<<endl
		<<"		data is the directory of the grammar, chn_grammar trained on CTB6, eng_grammar trained on PTB3, default is \"chn_grammar\""<<endl
		<<"		n, output nbest tree, default is 1"<<endl
		<<"		grammarLevel is the level of grammar to load, range from 0-5"<<endl
		<<"		printForest, output forest instead of nbest trees"<<endl
		<<"		threshold=x, if set, the hyperedges with margin larger than x to the viterbi path will be pruned"<<endl
		<<"		nbest4threshold=x, if set, the hyperedges not in the x nbest trees will be pruned"<<endl
		<<"		debug, output debug information, default is 0"<<endl
		<<"		example1: Egret -lapcfg -i=test-chn.txt -data=chn_grammar -n=100 > log.txt"<<endl
		<<"		example2: Egret -lapcfg -i=test-chn.txt -data=chn_grammar -nbest4threshold=100 -printForest > log.txt"<<endl
		<<endl
		<<"egret -pcfg -g=grammar -i=text -debug=dlevel [-n=nbest] -unary_fine=[0|1]"<<endl
		<<"		syntax parsing with pure pcfg"<<endl
		<<"		debug level default is 0,nbest default is 1"<<endl
		<<"		example: Egret -pcfg -g=testeng.gra -i=testeng.txt -n=3"<<endl
		<<""<<endl
		<<"egret -io -g=grammar -i=text -debug=dlevel -pass=iteration -threshold=a_float_number -unary_fine=[0|1]"<<endl
		<<"		inside-outside estimation"
		<<"		pass default is 1, threshold default is 0, debug default is 0"<<endl
		<<"		-unary_fine=[0|1], 0 to cope with unary rule in a coarse way, 1 in fine way"<<endl
		<<"		example: Egret -io -g=testeng.gra -i=testeng.txt -pass=3"<<endl
		<<""<<endl;
	exit(1);
}

clock_t bitime=0,alltime=0,init_bitime=0,init_unatime=0,final_unatime=0,final_bitime=0;
clock_t unatime=0,setutime=0,qutime=0,inbitime=0,initime=0,loading_time=0;
ObjectFunctionType string2oft(string str)
{
	if(str=="variational")
		return VARIATIONAL;
	else if(str=="max_rule_sum")
		return MAX_RULE_SUM;
	else if(str=="max_rule_product")
		return MAX_RULE_PRODUCT;
	else if(str=="max_prob")
		return MAX_PROB;
	else if(str=="viterbi")
		return NONE;
	return MAX_RULE_PRODUCT;
}
int main(int argc, char *argv[])
{
	JKArgs args(argc,argv);
	//seconds=time(NULL);
	bitime=0;
	unatime=0;
	setutime=0;
	qutime=0;
	inbitime=0;
	initime=0;
	alltime=0;

	alltime=clock();
	if(args.is_set("debug"))dlevel=atoi(args.value("debug").c_str());
	if(!(args.is_set("pcfg")||args.is_set("io")||args.is_set("lapcfg")))
		usage();

	if(args.is_set("unary_fine"))unary_fine=atoi(args.value("unary_fine").c_str());
	if(args.is_set("pcfg"))
	{
		if(!(args.is_set("g")&&args.is_set("i")))usage();
		ifstream ruleStream(args.value("g").c_str());
		cerr<<"load rule"<<endl;
		RuleSet ruleSet(ruleStream);
		cerr<<"done"<<endl;
		ruleSet.print(cout);
		ifstream textStream(args.value("i").c_str());
		if(!textStream.good())usage();
		int nbest=1;
		if(args.is_set("n")) 
			nbest=atoi(args.value("n").c_str());
		int dlevel=0;
		if(args.is_set("debug"))
			dlevel=atoi(args.value("debug").c_str());

		int count=0;
		while(!textStream.eof())
		{
			++count;
			string curline;
			getline(textStream, curline);
			if(curline=="")continue;
			
			vector<Tree*> trees;
			Forest forest(curline);
			forest.viterbi(ruleSet);
			if(nbest>1)
			{
				cout<<"sentence "<<count<<": \n"<<curline<<endl;
				cout<<nbest<<" best:"<<endl;
			}
			//forest.print_hyperedges(cout);
			forest.genTrees(nbest,trees);
			for(int i=0;(size_t)i<trees.size();i++)
			{
				//if(!args.is_set("interNode"))
				//	trees[i]->delInterNode();
				if(dlevel>0)
					trees[i]->print(cout);
				else
					trees[i]->tinyPrint(cout);
				trees[i]->destroy();
			}
		}
	}
	else if(args.is_set("io"))
	{
		//alltime=clock();
		if(!(args.is_set("g")&&args.is_set("i")))usage();
		ifstream ruleStream(args.value("g").c_str());
		RuleSet ruleSet(ruleStream);
		//ruleSet.print(cout);

		int nbest=1;

		int dlevel=0;
		if(args.is_set("debug"))
			dlevel=atoi(args.value("debug").c_str());
		int pass=1;
		if(args.is_set("pass"))
			pass=atoi(args.value("pass").c_str());
		double threshold=0;
		if(args.is_set("threshold"))
			threshold=atof(args.value("threshold").c_str());

		ERSet erSet;
		for(int i=0;i<pass;i++)
		{
			if(dlevel)
			{
				cout<<"pass:"<<i<<endl;
				ruleSet.print(cout);
				cout<<endl;
			}
			erSet.clear();
			int count=0;
			ifstream textStream(args.value("i").c_str());
			if((!textStream.good())||textStream.eof())usage();
			while(!textStream.eof())
			{
				string curline;
				getline(textStream, curline);
				if(curline=="")continue;
				//cerr<<count++<<":"<<curline<<endl;
				vector<Tree*> trees;
				Forest forest(curline);
				forest.io(ruleSet,erSet);
			}
			ruleSet.clear();
			erSet.thresholding(threshold);
			erSet.makeRuleSet(ruleSet);

			textStream.close();
		}
		cerr<<"new grammar generated!"<<endl;
		erSet.print(cout);
	}
	else if(args.is_set("lapcfg"))
	{
		if(!args.is_set("i"))usage();

		string grammar_directory="chn_grammar";
		if(args.is_set("data"))
			grammar_directory=args.value("data");
#ifdef WIN32
		grammar_directory+="\\level";
#else
		grammar_directory+="/level";
#endif
		int level=6;
		if(args.is_set("grammarLevel"))
			level=atoi(args.value("grammarLevel").c_str())+1;
		BerkeleyRuleSets ruleSets;
		ruleSets.init(grammar_directory,level);
		loading_time+=clock()-alltime;
		ifstream textStream(args.value("i").c_str());
		if(!textStream.good())usage();
		int nbest=1;
		if(args.is_set("n")) 
			nbest=atoi(args.value("n").c_str());
		int dlevel=0;
		if(args.is_set("debug"))
			dlevel=atoi(args.value("debug").c_str());

		ObjectFunctionType oft=MAX_RULE_PRODUCT;
		if(args.is_set("oft"))
			oft=string2oft(args.value("oft"));

		//cerr<<"rule loaded!"<<endl;
		for(int i=0;(size_t)i<ruleSets.size();i++)
		{
		//	ruleSets[i].save(intToString(i)+".temp");
		}		

		int start=0;
		double stop=1E10;
		if(args.is_set("range"))
		{
			string range=args.value("range");
			start=atoi(range.substr(0,range.find("-")).c_str());
			stop=atof(range.substr(range.find("-")+1).c_str());
			cerr<<"start:"<<start<<",stop:"<<stop<<endl;
		}

		int count=0;
		while(!textStream.eof())
		{
			cerr<<++count<<endl;
			//getchar();
			string curline;
			getline(textStream, curline);
			if(curline=="")continue;
			if(count<start)continue;
			if(count>stop)break;

			vector<Tree*> trees;
			Forest forest(curline);
			forest.berkeley_viterbi_coarse2fine(ruleSets,oft);

			string rootSymbol="ROOT";
			
			if(args.is_set("printForest"))
			{

				if(forest.topS(rootSymbol)==NULL)
				{
					cerr<<"parse failed!"<<endl;
					cout<<"sentence "<<count<<" :"<<endl;
					cout<<curline<<endl;
					cout<<"\n"<<endl;
					continue;
				}

				forest.io_afterParse(rootSymbol,MAX);
				//cerr<<"finished viterbi io"<<endl;
				double threshold=10;
				int nbest4threshold=100;

				if(args.is_set("threshold"))
					threshold=atof(args.value("threshold").c_str());
				else 
				{	
					if(args.is_set("nbest4threshold"))
						nbest4threshold=atoi(args.value("nbest4threshold").c_str());
					//cerr<<"computing threshold:"<<endl;
					double bestscore,lastscore;
					bestscore=lastscore=0;
					if(forest.topS(rootSymbol)!=NULL)
					{
						forest.topS(rootSymbol)->getNthBest(1,bestscore);
						lastscore=bestscore;
						forest.topS(rootSymbol)->getNthBest(nbest4threshold,lastscore);		
						threshold=bestscore-lastscore;
					}
				}
				if(threshold>0)
				{
					cerr<<"threshold : "<<threshold<<endl;
					forest.prune(rootSymbol,threshold);
					//cerr<<"finished prune"<<endl;
				}
				forest.replace_interLabel(Berkeley);
				
				cout<<"sentence "<<count<<" :"<<endl;
				cout<<curline<<endl;
				forest.print_hyperedges(cout);
				cout<<endl;
				continue;
			}

			if(nbest>1)
			{
				cout<<"sentence "<<count<<": \n"<<curline<<endl;
				cout<<nbest<<" best:"<<endl;
			}
			if(oft==NONE)
				forest.genTrees(nbest,trees,"ROOT_0");
			else
				forest.genTrees(nbest,trees,"ROOT");

			if(trees.size()==0)
				cout<<"(())"<<endl;
			else
			for(int i=0;(size_t)i<trees.size();i++)
			{
				//trees[i]->tinyPrint(cout);
				//	cout<<endl;
				if(!args.is_set("interNode"))
					trees[i]->delInterNode();
				if(dlevel>0)
				{
					trees[i]->print(cout);
					cout<<endl;
				}
				else
				{
					trees[i]->tinyPrint(cout);
					cout<<endl;
				}
				trees[i]->destroy();
			}
			
		}	
		alltime=clock()-alltime;
		//seconds=time(NULL)-seconds;
		cerr<<"all time:"<<(double)alltime/CLOCKS_PER_SEC<<"s"<<endl;
		cerr<<"rule loading time:"<<(double)loading_time/CLOCKS_PER_SEC<<"s"<<endl<<endl;

		cerr<<"init binary rule time:"<<(double)init_bitime/CLOCKS_PER_SEC<<"s"<<endl;
		cerr<<"init unary rule time:"<<(double)init_unatime/CLOCKS_PER_SEC<<"s"<<endl<<endl;

		cerr<<"middle binary rule time:"<<(double)bitime/CLOCKS_PER_SEC<<"s"<<endl;
		cerr<<"middle unary rule time"<<(double)unatime/CLOCKS_PER_SEC<<"s"<<endl<<endl;

		cerr<<"final binary rule time:"<<(double)final_bitime/CLOCKS_PER_SEC<<"s"<<endl;
		cerr<<"final unary rule time"<<(double)final_unatime/CLOCKS_PER_SEC<<"s"<<endl<<endl;
		
		cerr<<"set unary node time:"<<(double)setutime/CLOCKS_PER_SEC<<"s"<<endl;
		cerr<<"query time:"<<(double)qutime/CLOCKS_PER_SEC<<"s"<<endl;
		cerr<<"inside binart rule time:"<<(double)inbitime/CLOCKS_PER_SEC<<"s"<<endl;
		cerr<<"init time:"<<(double)initime/CLOCKS_PER_SEC<<"s"<<endl;
	}
	
	return 0;
}

