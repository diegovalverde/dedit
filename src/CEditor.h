#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "CSyntaxHighLine.h"

using namespace std;

#define STATUS_BAR_HEIGHT 1
#define PAGE_LAST_TEXT_ROW (LINES - STATUS_BAR_HEIGHT-1)
#define PAGE_ROW_COUNT PAGE_LAST_TEXT_ROW
#define MAX_VIEW_ROWS 100
#define CTRL(c) ((c) & 037)
#define MAX_CALL_BACKS 100


//-----------------------------------------------------------------------------------
class CEdit
{
	public:
		CEdit();
		~CEdit();
	private:
		typedef enum 
		{
			SCROLL_UP,
			SCROLL_DOWN,
			SCROLL_PAGE_DOWN,
			SCROLL_PAGE_UP,
		} E_SCROLL_DIRECTION;
		
	public:
		void Initialize( void );
		void ExitApplication( void );
		void SaveFile( string aName );
		void LoadFile( string aName );
		void HandleUserInput( void );
		void ExecuteCommand( void );
		void LoadPageView( void );
		void Find( void );
		void ReloadFile( void );
		void FindNext( void );
		void Undo( void );
		void GotoLine( void );
		void InsertLine( void );
		void GotoLine( unsigned int aLineNumber );
		
	private:
		string CallBack_LoadSyntax( vector<string> aPath );		
		string CallBack_ToggleOption( vector<string> aPath );		
	
	public:
		void PrintMessage( string aMessage );
		void PrintStatusBar( string aMessage );
		void SetCurrentMessage(string aMessage );
		string mCurrentCustomMessage;
		string mLastFindString;
		bool mPrintCustomMessage;
		
		string Prompt( string aMessage, vector<string> aHistory );
		void SlidePageViewBuffer( E_SCROLL_DIRECTION aDirection );
		unsigned int GetNumberOfCharactersInVBufferLine( unsigned int aLineNumber );
		void DeleteLines();
	private:
		int mRow,mCol;
		int mCurrentLine;
		bool mUnsavedChanges;
		bool mColorSupport;
		
		
		
		typedef struct T_File
		{
			string 		mName;
			vector<string> 	mLineBuffer;				//The main text buffer
		} TFile ;
		
		
		
		typedef struct T_TextSection
		{
			string 			mText;
			E_FONT_COLOR 	mFontColor;
			
		} TTextSection;
		
		
		
		TFile 					mCurrentFile;
		int        				mPageViewTopRow;
		string				 	mPageView[MAX_VIEW_ROWS];

	public:		
	
		typedef struct T_EditorOptions
		{
			bool 	mActive;
			string	mName;
			string  mHelp;
			
		} TEditorOptions;
		
		
		typedef string (CEdit::*TCallBack)(vector<string> aArgs);
		TCallBack mCallBacks[ MAX_CALL_BACKS ];

		ofstream                        mLogFile;
		map<string,TEditorOptions> 		mOptions;
		map<string,int> 				mCallBackIndex;
		CSyntaxHighLine			   		mSyntaxHighLine;

	private:
		typedef enum
		{
			E_ADD_CHAR=0,	
			E_DEL_CHAR,	
			E_DEL_LINE,	
			E_ADD_LINE
		
		} E_ACTIONS;

		
		typedef struct T_UndoBufferEntry
		{
			E_ACTIONS	mAction;
			string		mValue;
			unsigned int	mLineNumber;
			unsigned int	mColumn,mRow;
		} TUndoBufferEntry;	
		
		vector<TUndoBufferEntry> mUndoBuffer;   //One undo per Line
		void RefreshPageView(void);
		
		void AddAction(E_ACTIONS aAction, char * Value = NULL);
	private:
		typedef enum
		{
			E_EDIT_READY,
			E_SELECTION,
			E_READONLY
		} E_MODES;
		
		E_MODES mEditorMode;

		
};

//-----------------------------------------------------------------------------------
