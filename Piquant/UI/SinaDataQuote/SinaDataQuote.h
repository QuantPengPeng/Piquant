#ifndef _SINADATA_QUOTE_H
#define _SINADATA_QUOTE_H

#include "afxinet.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class  SinaDataQuote
{
public:
	SinaDataQuote();
	virtual ~SinaDataQuote();
	void getquote_DailyKLine(string instrumentID);
	void getquote_MiniKLine(int mimute, string instrumentID);
	friend  SinaDataQuote& get_SinaDataQuote();

private:
	string m_outputfile;
	char m_prequote[10000000];
	char m_buffer[10000000];
	size_t m_recordcount;
};

#endif 