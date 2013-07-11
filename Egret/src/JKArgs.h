#ifndef JKARGS_H
#define JKARGS_H
#include <vector>
#include <string>
#include <map>
#include <fstream>
using namespace std;
class JKArgs{
   public:
	JKArgs(){argsPool.clear();};
	JKArgs(int argc,char *argv[]);
	JKArgs(const char * filename);
	void init(int argc,char *argv[]);
	void init(const char * filename);
	~JKArgs(void);
	bool is_set(string arg);
	string value(string arg);
	int num_of_args(void) {return (int)argsPool.size();}
   private:
	map<string, string> argsPool;
	void process_one_arg(string arg);
};
#endif
