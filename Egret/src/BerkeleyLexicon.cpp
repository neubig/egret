#include "BerkeleyLexicon.h"
#include "utils.h"
Language language=English;

void 
BerkeleyLexicon::
init(string lexFile, const BerkeleyStateInfoOfOneLevel& bsi)
{
	_stateInfoOfcurLevel.clone_from(bsi);
	ifstream is(lexFile.c_str());
	while(!is.eof())
	{
		string curline;
		getline(is,curline);		
		string term,wrd;

		term=curline.substr(0,curline.find(" "));
		curline.erase(0,term.length()+1);
		//cerr<<curline<<endl;
		wrd=curline.substr(0,curline.find(" "));
		curline.erase(0,wrd.length()+1);
		
		replaceSubStr(curline,"[","[ ");
		replaceSubStr(curline,","," , ");
		stringstream ss(curline);
		
		//cerr<<term<<","<<wrd<<endl;
		if(term==""||wrd=="")
			break;	
		
		if(!_word2state.containX(wrd))
			_word2state.add(wrd,(int)_word2state.size());

		int wrdState,termState,termNumSubState;
		_word2state.queryX2Y(wrd,wrdState);
		termState=_stateInfoOfcurLevel.term2state(term);
		termNumSubState=_stateInfoOfcurLevel.state2numSubState(termState);
		//cerr<<term<<","<<wrd<<","<<termState<<" ,"<<termNumSubState<<" ";
		if(_unaryRuleHouse[wrd].find(term)==_unaryRuleHouse[wrd].end())
		{
			_unaryRuleHouse[wrd][term].set(wrdState,termState,1,termNumSubState);
			_unaryRuleHouse[wrd][term].set(wrd,term);
		}

		string temp;
		ss>>temp;
		for(int i=0;i<termNumSubState;i++)
		{
			double score;
			string comma;
			ss>>score;
			ss>>comma;
			//cerr<<score<<", ";
			_unaryRuleHouse[wrd][term].setScore(0,i,score);
		}
		//cerr<<endl;
	}
	//cerr<<"num of word:"<<_word2state.size()<<", num of state:"<<_stateInfoOfcurLevel.numOfState()<<endl;
	setRuleIntoVector();
}

void 
BerkeleyLexicon::
print(string lexFile)const
{
	ofstream os(lexFile.c_str());
	vector<string> toWrite;
	toWrite.clear();

	map<string, map<string, BerkeleyUnaryRule> >::const_iterator _unaryRuleHouseIter;
	map<string, BerkeleyUnaryRule>::const_iterator burMapIter;

	for(_unaryRuleHouseIter=_unaryRuleHouse.begin();_unaryRuleHouseIter!=_unaryRuleHouse.end();_unaryRuleHouseIter++)
	{
		const map<string, BerkeleyUnaryRule>& burMap=_unaryRuleHouseIter->second;
		for(burMapIter=burMap.begin();burMapIter!=burMap.end();burMapIter++)
			toWrite.push_back(burMapIter->second.toLexRuleString());
	}
	sort(toWrite.begin(),toWrite.end());
	for(int i=0;(size_t)i<toWrite.size();i++)
		os<<toWrite[i]<<endl;	
	os.close();
}


void 
BerkeleyLexicon::
print_by_ruleVector(string lexFile)const
{
	ofstream os(lexFile.c_str());
	vector<string> toWrite;
	toWrite.clear();

	int now=(int)numOfWord();
	int nos=(int)numOfState();
	for(int i=0;i<now;i++)
	{
		//vector<BerkeleyUnaryRule*>& tmp=_unaryRuleByWord[i];
		const vector<BerkeleyUnaryRule*>& tmp=getLexRulesByWord(i);
		for(int k=0;(size_t)k<tmp.size();k++)
		{
			if(tmp[k]!=NULL)
			{
				toWrite.push_back(tmp[k]->toLexRuleString());
				//cout<<state2word(i)<<"\t"<<tmp[k]->toLexRuleString()<<endl;
			}
		}
	}

	sort(toWrite.begin(),toWrite.end());
	for(int i=0;(size_t)i<toWrite.size();i++)
		os<<toWrite[i]<<endl;
	os.close();
}

void 
BerkeleyLexicon::
logrize()
{
	if(isLogrized())return;
	_logrized=true;
	int now=(int)numOfWord();
	int nos=(int)numOfState();
	for(int i=0;i<now;i++)
	{
		vector<BerkeleyUnaryRule*>& tmp=_unaryRuleByWord[i];
		for(int k=0;(size_t)k<tmp.size();k++)
		{
			if(tmp[k]!=NULL)
				tmp[k]->logrize();
		}
	}
}

void 
BerkeleyLexicon::
initRuleInVector()
{
	int now=(int)numOfWord();
	int nos=(int)numOfState();
	_unaryRuleByWord.clear();
	
	for(int i=0;i<now;i++)
	{
		vector<BerkeleyUnaryRule*> tmp1;
		_unaryRuleByWord.push_back(tmp1);
		_unaryRuleByWord[i].clear();
	}
}

void 
BerkeleyLexicon::
setRuleIntoVector()
{
	initRuleInVector();
	map<string, map<string, BerkeleyUnaryRule> >::iterator _unaryRuleHouseIter;
	map<string, BerkeleyUnaryRule>::iterator burMapIter;

	for(_unaryRuleHouseIter=_unaryRuleHouse.begin();_unaryRuleHouseIter!=_unaryRuleHouse.end();_unaryRuleHouseIter++)
	{
		map<string, BerkeleyUnaryRule>& burMap=_unaryRuleHouseIter->second;
		for(burMapIter=burMap.begin();burMapIter!=burMap.end();burMapIter++)
		{
			BerkeleyUnaryRule* p_bur=&(burMapIter->second);
			int pr,ch;
			pr=p_bur->parentState();ch=p_bur->childState();
			//cerr<<_unaryRuleByRule.size()<<","<<_unaryRuleByRule[ch].size()<<endl;
			_unaryRuleByWord[ch].push_back(p_bur);
		}
	}
}

const vector<BerkeleyUnaryRule*>& 
BerkeleyLexicon::
getLexRulesByWord(int wd)const
{
	/*cout<<state2word(wd)<<" "<<wd<<endl;
	for(int i=0;i<_unaryRuleByWord[wd].size();i++)
	{
		cout<<_unaryRuleByWord[wd][i]->toLexRuleString()<<endl;
	}*/
    if(wd >= _unaryRuleByWord.size())
                wd = word2state("UNK");
	return _unaryRuleByWord[wd];

}

const BerkeleyUnaryRule* 
BerkeleyLexicon::
getLexRuleByRule(const string& wrd , const string& tag)const
{
	if(_unaryRuleHouse.find(wrd)==_unaryRuleHouse.end())
		return NULL;
	else
	{
		const map<string, BerkeleyUnaryRule>& tmp=_unaryRuleHouse.find(wrd)->second;
		if(tmp.find(tag)==tmp.end())
			return NULL;
		else
		{
			return &(tmp.find(tag)->second);
		}
	}
}

const BerkeleyUnaryRule* 
BerkeleyLexicon::
getLexRuleByRule(int wrd, int tag)const
{
	string tag_str=state2tag(tag);
	string wrd_str=state2word(wrd);
	//cerr<<wrd_str<<" , "<<tag_str<<endl;
	return getLexRuleByRule(wrd_str,tag_str);
}

int  
BerkeleyLexicon::
word2state(const string& wrd, int pos)const
{
	string newWrd="";
	int result=naive_word2state(wrd);
	if(result<0)
	{
		switch(language)
		{
			case Chinese :	newWrd=chinese_unknownWord_category(wrd,pos);
			default:		newWrd=english_nknownWord_category(wrd,pos);
		}
		result=naive_word2state(newWrd);
	}
	return result;
}

string 
BerkeleyLexicon::
chinese_unknownWord_category(const string& wrd, int pos)const
{
	
	return "UNK";
}

string 
BerkeleyLexicon::
english_nknownWord_category(const string& wrd, int loc)const
{
	string result="UNK";
	int wlen = (int) wrd.length();
	char * lowered= (char*)malloc(sizeof(char)*(wlen+1));
	lowered[wlen]=0;
	int numCaps = 0;
	bool hasDigit = false;
	bool hasDash = false;
	bool hasLower = false;
    for (int i = 0; i < wlen; i++) {
		char ch = (char)wrd[i];
		lowered[i]=ch;
		if (isdigit(ch))
			hasDigit = true;
		else if (ch == '-')
			hasDash = true;
		else if (isalpha(ch)) 
		{
			if (islower(ch))
				hasLower = true;
			else
			{
				numCaps++;
				lowered[i]=tolower(ch);
			}
		}
	}

      char ch0 = wrd[0];          
      if (isupper(ch0))
	  {
			if(loc == 0 && numCaps == 1) 
			{
				result+="-INITC";
				if (naive_word2state(lowered)>0)
					result+="-KNOWNLC";
			 } 
			 else result+="-CAPS";
      } 
	  else if (isalpha(ch0) && numCaps > 0) 
			result+="-CAPS";
      else if (hasLower) 
			result+="-LC";

      if (hasDigit)
		  result+="-NUM";
      if (hasDash)
		  result+="-DASH";

	   if (endsWith(lowered,"s") && wlen >= 3) 
	   {
            // here length 3, so you don't miss out on ones like 80s
            char ch2 = lowered[wlen - 2];
            // not -ess suffixes or greek/latin -us, -is
            if (ch2 != 's' && ch2 != 'i' && ch2 != 'u') 
              result+="-s";
       } 
	   else if (wlen >= 5 && !hasDash && !(hasDigit && numCaps > 0)) 
	   {
            if (endsWith(lowered,"ed")) result+="-ed";
            else if (endsWith(lowered,"ing")) result+="-ing";
            else if (endsWith(lowered,"ion")) result+="-ion";
            else if (endsWith(lowered,"er")) result+="-er";
            else if (endsWith(lowered,"est")) result+="-est";
            else if (endsWith(lowered,"ly")) result+="-ly";
            else if (endsWith(lowered,"ity")) result+="-ity";
            else if (endsWith(lowered,"y")) result+="-y";
            else if (endsWith(lowered,"al")) result+="-al";
              // } else if (lowered.endsWith("ble")) {
              // sb.append("-ble");
              // } else if (lowered.endsWith("e")) {
              // sb.append("-e");
            
	   }
	return result;
}
