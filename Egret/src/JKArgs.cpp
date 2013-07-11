#include "JKArgs.h"
JKArgs::
JKArgs(int argc,char *argv[])
{
	init(argc,argv);
}

JKArgs::
JKArgs(const char * filename)
{
	init(filename);
}

void
JKArgs::
init(int argc,char *argv[])
{
    for(int i=1;i<argc;i++)
    {
		process_one_arg(argv[i]);
    }
}

void
JKArgs::
init(const char *filename)
{
	ifstream file;
	file.open(filename);
	string arg;
    for(;!file.eof();)
    {
		getline(file,arg);
		process_one_arg(arg);
    }
	file.close();
}
void 
JKArgs::
process_one_arg(string arg)
{
	string value;
	int pos;
	if(arg[0]=='-'||arg[0]=='/')
    {
		arg.erase(0,1);
    }
	else return;
    pos=(int)arg.find("=");
    if(pos==string::npos)
    {
		value="";
    }
    else
    {
        value=arg.substr(pos+1,arg.size()-pos-1);
        arg.erase(pos,arg.size()-pos);
    }
    argsPool[arg]=value;
}
JKArgs::
~JKArgs()
{
	argsPool.clear();
}
bool
JKArgs:: 
is_set(string arg)
{
	if(argsPool.find(arg)==argsPool.end()) 
		return false;
	else return true;
}
string
JKArgs::
value(string arg)
{
	if(argsPool.find(arg)==argsPool.end()) 
		return "";
	else return argsPool[arg];
}
