#ifndef CSYNTAX_HIGH_LINE_H
#define CSYNTAX_HIGH_LINE_H
#include <string>
#include <vector>
#include <map>

using namespace std;


enum E_FONT_COLOR
		{
			FONT_WHITE,
			FONT_BLACK,
			FONT_BLUE,
			FONT_GREEN,
			FONT_RED,
			FONT_WHITE_OVER_BLUE,
			
		};
		
class CSyntaxHighLine
{
public:
	CSyntaxHighLine();
	~CSyntaxHighLine();
	
	
public:
		string LoadFile( string aPath );
		vector<pair<string,E_FONT_COLOR> > GetBlocks(string aString );
		bool HasToken( string aToken );

private:		
		map<string,E_FONT_COLOR>	mTokens;
		E_FONT_COLOR                mDefaultFontColor;
		map<string,E_FONT_COLOR>    mStrFonts;
};


#endif
