#include "utils.h"
string
intToString(int i)
{
  stringstream ss;
  ss<<i;
  string ans;
  ss>>ans;
  return ans;
}
string doubleToString(double i)
{
  stringstream ss;
  ss.precision(20);
  if(i==0)
	  return "0.0";
  ss<<i;
  string ans;
  ss>>ans;
  return ans;
}
string& replaceSubStr(string& in, string oldSub, string newSub)
{
	size_t pos=0;
	while(true)
	{
		pos=in.find(oldSub,pos);
		if(pos!=string::npos)
		{
			in.replace(pos,oldSub.length(),newSub);
			pos+=newSub.length();
		}
		else break;
	}
	return in;
}

bool endsWith(char * src, const char* tail)
{
	size_t srclen=strlen(src);
	size_t taillen=strlen(tail);
	return !strcmp(src+(srclen-taillen),tail);
}
