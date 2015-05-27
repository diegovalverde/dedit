#include "CSyntaxHighLine.h"
#include <fstream>
#include <sstream>




//--------------------------------------------------------------------------------
CSyntaxHighLine::CSyntaxHighLine()
{

	mStrFonts["white"] = FONT_WHITE;
	mStrFonts["black"] = FONT_BLACK;
	mStrFonts["blue"]  = FONT_BLUE;
	mStrFonts["green"] = FONT_GREEN;
	mStrFonts["red"]   = FONT_RED;
	
	mDefaultFontColor = FONT_WHITE;
}
//--------------------------------------------------------------------------------
CSyntaxHighLine::~CSyntaxHighLine()
{
	
}
//--------------------------------------------------------------------------------	
vector<pair<string,E_FONT_COLOR> > CSyntaxHighLine::GetBlocks(string aString )
{
	vector<pair<string,E_FONT_COLOR> > Out;
	if (aString.find_first_not_of("\n") == string::npos)
	{
		Out.push_back(std::make_pair("\n",mDefaultFontColor));
		return Out;
	}
	
	string Token,SubString;
	E_FONT_COLOR Color;
	stringstream ss;
	ss << aString;
	
	size_t pos = 0;
	
	while (ss >> Token)
	{
		if (mTokens.find( Token ) != mTokens.end() )
		{
			
			pos = aString.find( Token );
			SubString = aString.substr(0,pos);
			aString.erase(0,pos);
			pos = aString.find( Token );

			if (SubString.size())
				Out.push_back( std::make_pair(SubString ,mDefaultFontColor) ); 
			
			SubString = aString.substr(pos,Token.size() );
			aString.erase(pos,Token.size());
			Out.push_back( std::make_pair(SubString ,mTokens[ Token ]) );
			
			
		}		
	}
	
/*	
	if (Out.size() == 0)
		Out.push_back( std::make_pair(aString ,mDefaultFontColor) );
		*/
	pos = aString.find_last_of( Token );
	SubString = aString.substr(0,(pos + Token.size() + 1));// + "\n";
	Out.push_back( std::make_pair(SubString ,mDefaultFontColor) ); 
	
			
	return Out;
/*
	vector<pair<string,E_FONT_COLOR> > Out;
	if (aString.find_first_not_of("\n") == string::npos)
	{
		Out.push_back(std::make_pair("\n",mDefaultFontColor));
		return Out;
	}
	
	string Token,SubString;
	E_FONT_COLOR Color;
	stringstream ss;
	ss << aString;
	std::size_t Left = 0, Right = aString.size();
	
	while (ss >> Token)
	{
		if (mTokens.find( Token ) != mTokens.end() )
		{
			Right = aString.find( Token );
			SubString = aString.substr(Left,(Right-Left));
			Out.push_back( std::make_pair(SubString ,mDefaultFontColor) ); 
			
			SubString = aString.substr(Right,Token.size() );
			Out.push_back( std::make_pair(SubString ,mTokens[ Token ]) );
			Left = Right + Token.size();
		}		
	}
	
	Right = aString.find_last_of( Token );
	SubString = aString.substr(Left,(Right + Token.size() + 1 - Left));// + "\n";
	Out.push_back( std::make_pair(SubString ,mDefaultFontColor) ); 
	
	return Out;
	*/
}
//--------------------------------------------------------------------------------	
string CSyntaxHighLine::LoadFile( string aPath )
{
	ifstream ifs( aPath.c_str() );
	if (!ifs.good())
		return string("Could not open file '" + aPath + "'\n");
		
	while (ifs.good())
	{
		string Line, Token,strColor;
		std::getline(ifs,Line);
		
		size_t pos = 0;
		if ((pos = Line.find("//")) != string::npos)
			Line.erase(pos,string::npos);
			
		if (Line.size() == 0)
			continue;
			
		stringstream ss;
		ss << Line;
		ss >> Token >> strColor;
		
		if (mStrFonts.find( strColor ) == mStrFonts.end())
			return "CSyntaxHighLine::LoadFile Unsupported color '" + strColor + "'\n";
			
		mTokens[ Token ] = mStrFonts[strColor];
	}
	
	return "Load Syntax Done\n";
}
//--------------------------------------------------------------------------------	
bool CSyntaxHighLine::HasToken( string aToken )
{
	return (mTokens.find( aToken ) != mTokens.end());
}
//--------------------------------------------------------------------------------	
