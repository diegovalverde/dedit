#include "CEditor.h"
#include <unistd.h> //For sleep

string gModeStrings[] =
{
	"[Ready]",
	"[Select]",
	"[Read-Only]"
};

string gExitMessages[] =
{
	"Leaving so soon?",
	"Are you really really really sure? (really?)",
	"Life is too short. Quit?",
	"Pooooooooofffffff I'm outta here.. Quit?",
	"Have... to... Sleep.... Quit ",
	"Quit? Seriously?",
	"No! Dont leave me! Quit?",
	"Is not so late, you quit already?",
};
	
string gNoExitMessages[] =
{
	"I knew that you really didn't mean it",
	"I forgive you",
	"Welcome back!"
};

string gUndoBufferEmptyMessages[] =
{
	"I can't undo what can't be undone",
	"Not so sure of yourself uhh?",
	"I say no more undoing!",
	"Can't undo",
	"I said can't undo",
	"I can't do that",
	"Seriously I can't",
	"Nothing else to do... errr not do... err undo",
};


string gCommandMessages[] =
{
	"You want me to do what?",
	"Are you serious?",
	"Nope, I can't do that",
	"I wish I could do that",
};

string gStringNotFoundMessage[] =
{
	"Sorry dont know where that is",
	"I cant find it!",
	"Nop not here",
	"Aha!,  mmm sorry no, not found",
	"nowhere to be found",
 };

CEdit::TEditorOptions gEditorOptions[] =
{
	{false,	"line-numbers",				"Shows line numbers"},
	{true,	"syntax-highline",	"Enables the syntax highlining"}
};
//--------------------------------------------------------------------------------------
CEdit::CEdit()
{
	mPageViewTopRow = 0;
	mCurrentLine = 0;
	mUnsavedChanges = false;
	mPrintCustomMessage = false;
	mEditorMode = E_EDIT_READY;
	mLogFile.open(".dedit.log");
	mLogFile << "Created Log\n";
	
}
//--------------------------------------------------------------------------------------
CEdit::~CEdit()
{
	mLogFile.close();
}
//--------------------------------------------------------------------------------------
void CEdit::Initialize()
{
   def_shell_mode();
   initscr();
   noecho();
   raw();
 
   idlok(stdscr, TRUE);
   keypad(stdscr, TRUE);


	for (int i = 0; i < (sizeof(gEditorOptions)/sizeof(gEditorOptions[0])); i++)
		mOptions[gEditorOptions[i].mName] = gEditorOptions[i];

	mCallBackIndex["load_syntax"] = 0;
	mCallBacks[ mCallBackIndex["load_syntax"] ] = &CEdit::CallBack_LoadSyntax;
	
	mCallBackIndex["toggle"] = 1;
	mCallBacks[ mCallBackIndex["toggle"] ] = &CEdit::CallBack_ToggleOption;
	
}
//--------------------------------------------------------------------------------------
string CEdit::CallBack_LoadSyntax( vector<string> aPath )
{
	if (aPath.size() < 2)
		return "The path? Where is the path?";
		
	string Out = mSyntaxHighLine.LoadFile( aPath[1] );
	
	RefreshPageView();
	mOptions["syntax-highline"].mActive = true;
	
	return Out;
}
//--------------------------------------------------------------------------------------
string CEdit::CallBack_ToggleOption( vector<string> aOptions )
{
	for (int i = 1; i < aOptions.size(); i++)
	{
		if (mOptions.find(aOptions[i]) == mOptions.end())
			return "Cannot set '"+ aOptions[i] + "'";
		
		mOptions[ aOptions[i] ].mActive = !mOptions[ aOptions[i] ].mActive;
	}
	RefreshPageView();
	return "Done";
}		
//--------------------------------------------------------------------------------------
unsigned int CEdit::GetNumberOfCharactersInVBufferLine( unsigned int aLineNumber )
{
   int LineLen = COLS - 1;

   while (LineLen >= 0 && mvinch(aLineNumber, LineLen) == ' ')
       LineLen--;
   return LineLen + 1;
}
//--------------------------------------------------------------------------------------
void CEdit::LoadFile( string aPath )
{

  ifstream ifs;
  ifs.open(aPath.c_str());
 if (! ifs.good())
{
    std::cout << "Thy file '" << aPath << "' does not exist, shall I create it sir? (y/n)";
    std::fflush(stdout);
  char Answer = 0;
   while (Answer != 'y' && Answer != 'n')
   	std::cin >> Answer;
   
    if (Answer == 'y')
	system(string("touch " + aPath).c_str());
    else
       exit(0);

  ifs.open(aPath.c_str());
  if (! ifs.good())
      throw std::string("Could not open " + aPath );
}
  string Line;
  int i = 0;

  while (ifs.good())
  {

    std::getline(ifs,Line);
    mCurrentFile.mLineBuffer.push_back( Line );

  }

  ifs.close();	
  mCurrentFile.mName = aPath;

}
//--------------------------------------------------------------------------------------
void CEdit::GotoLine( unsigned int aLineNumber )
{
	int j = 0;
	 for (int i = aLineNumber; (i < mCurrentFile.mLineBuffer.size() && j <= PAGE_LAST_TEXT_ROW); i++, j++)
  	{
		mPageView[j] = mCurrentFile.mLineBuffer[i] + "\n";
  	}
	for (; j <  PAGE_LAST_TEXT_ROW; j++)
		mPageView[j] = "";
//	mCurrentLine = aLineNumber;
//	mPageViewTopRow = aLineNumber;
//	mRow = 0;
	move(mRow,mCol);
}
//--------------------------------------------------------------------------------------
void CEdit::SlidePageViewBuffer( E_SCROLL_DIRECTION aDirection )
{

	switch (aDirection)
	{
		case SCROLL_DOWN:
			if (mPageViewTopRow < mCurrentFile.mLineBuffer.size())
				mPageViewTopRow++;
			GotoLine(mPageViewTopRow);
			return;
		
		case SCROLL_UP:
			if (mPageViewTopRow>0)
				mPageViewTopRow--;
			GotoLine(mPageViewTopRow);
			return;
		
		case SCROLL_PAGE_DOWN:
			mPageViewTopRow += PAGE_LAST_TEXT_ROW;
			GotoLine(mPageViewTopRow);
			return;

		case SCROLL_PAGE_UP:
			mPageViewTopRow -= PAGE_LAST_TEXT_ROW;
			GotoLine( mPageViewTopRow );
			return;

		default:

			return;

	}
		
}
//--------------------------------------------------------------------------------------
void CEdit::RefreshPageView()
{
	int j = 0;
	
	for (int i = mPageViewTopRow; (i < mPageViewTopRow + PAGE_LAST_TEXT_ROW && i < mCurrentFile.mLineBuffer.size() ); i++)
	{
			
		
		mPageView[j++] =   mCurrentFile.mLineBuffer[i] + "\n";
	}
	LoadPageView();
}
//--------------------------------------------------------------------------------------
void CEdit::LoadPageView( void )
{
	move(0,0);			//Get back to the beginning of the page
	//clear();
	for (int i = 0; i <= PAGE_LAST_TEXT_ROW; i++)
	{

		
		if (mOptions["line-numbers"].mActive)
		{
			char Buffer[2048];
			sprintf(Buffer,"%03d\t",mPageViewTopRow + i);
		    addstr( Buffer  );
		} 
	
		if (mOptions["syntax-highline"].mActive)
		{
		
		
			vector< pair<string,E_FONT_COLOR> > StringBlocks = mSyntaxHighLine.GetBlocks( mPageView[i] );
			for (int k = 0; k < StringBlocks.size(); k++)
			{
				attron(COLOR_PAIR(StringBlocks[k].second));
				mLogFile << i << " '" << StringBlocks[k].first << "' * ";
				addstr( StringBlocks[k].first.c_str()  ); 
			
				attroff(COLOR_PAIR(StringBlocks[k].second));
			}
			mLogFile << "&&&\n";
		} else {
			addstr( mPageView[i].c_str()  ); 
		}

	}
	
}
//--------------------------------------------------------------------------------------
void CEdit::SaveFile( string aName )
{

   ofstream ofs( aName.c_str() );
   for (int i = 0; i < mCurrentFile.mLineBuffer.size(); i++)
	ofs << mCurrentFile.mLineBuffer[i] << "\n";
	
    
   ofs.close();
   mUnsavedChanges = false;
}
//--------------------------------------------------------------------
void CEdit::PrintMessage( string aMessage )
{
if (mColorSupport)
		attron(COLOR_PAIR(FONT_WHITE_OVER_BLUE));
		

	aMessage = " " + aMessage;
	for (int i = aMessage.size(); i < COLS-1; i++)
		aMessage += " ";
	mvaddstr(LINES - 1, 1, aMessage.c_str());


	move(mRow, mCol);
	
	if (mColorSupport)
		attroff(COLOR_PAIR(FONT_WHITE_OVER_BLUE));
}
//--------------------------------------------------------------------
void CEdit::PrintStatusBar( string aMessage )
{
	if (mColorSupport)
		attron(COLOR_PAIR(FONT_WHITE_OVER_BLUE));

		
	stringstream ss;
	char Buffer[256];
	sprintf(Buffer, "%s\t\t %d/%d %d (%d,%d) %s %s\t\t undo: %d %s",
	mCurrentFile.mName.c_str(),
	mCurrentLine,mCurrentFile.mLineBuffer.size(),
	mPageViewTopRow,
	mRow,mCol,aMessage.c_str(),
	((mUnsavedChanges)?"Modified":""),
	mUndoBuffer.size(),
	((mOptions["syntax-highline"].mActive)?"S":"")
	);
	
	PrintMessage( Buffer );
	
	if (mColorSupport)
		attroff(COLOR_PAIR(FONT_WHITE_OVER_BLUE));
	
}
//------------------------------------------------------------------
string CEdit::Prompt( string aMessage, vector<string> aHistory )
{
  string ReturnString;
  standout();
  aMessage = " " + aMessage;
if (mColorSupport)
	attron(COLOR_PAIR(FONT_WHITE_OVER_BLUE));
 
  mvaddstr(LINES - 1, 1, aMessage.c_str());
  
  for (int i = aMessage.size(); i < COLS-1; i++)
		delch();
		
  
  int Row = LINES - 1;
  int Col = 1 + aMessage.size();
  int c;
  int HistoryIndex = aHistory.size()-1;
  while (1)
  {
    c = getch();
	switch (c)
	{
		case KEY_LEFT:
			 move(Row,--Col);
			break;
			
		case KEY_RIGHT:
			move(Row,++Col);
			break;
			
		case KEY_UP:
			//history
			if (aHistory.size())
			{
				mvaddstr(LINES - 1, 1, aMessage.c_str());
				for (int i = aMessage.size(); i < COLS-1; i++)
					delch();
				if (HistoryIndex >= 0)
				{
					ReturnString = aHistory[HistoryIndex];
					addstr(ReturnString.c_str());
					HistoryIndex--;
				}
			}
			break;
			
		case KEY_DOWN:
			//history
			if (aHistory.size())
			{
				mvaddstr(LINES - 1, 1, aMessage.c_str());
				for (int i = aMessage.size(); i < COLS-1; i++)
					delch();
				if (HistoryIndex < aHistory.size())
				{
					ReturnString = aHistory[HistoryIndex];
					addstr(ReturnString.c_str());
					HistoryIndex++;
				}
			}
			break;
		
		case 10:
		case KEY_ENTER:
			move(Row,Col);
			standend();
			return ReturnString;
			break;

		case CTRL('C'):	//cancel
			 move(mRow,mCol);
			standend();
			return "CANCEL";
			
		case KEY_DC:
            		delch();
            		break;
			 
		case KEY_BACKSPACE:	
			move(Row, --Col);
			delch();
			break;
			
		default:
			insch(c);
			ReturnString += c;
			move(Row,++Col);
			
	}
	refresh();
 
  }
if (mColorSupport)
	attroff(COLOR_PAIR(FONT_WHITE_OVER_BLUE));
  return ReturnString;
}
//-------------------------------------------------------------------
void CEdit::ReloadFile()
{
	static vector<string> ReloadHistory;
	string Answer = Prompt("Are: you sure you want to reload (y/n) ", ReloadHistory);	
	
	if (Answer == "y")
		LoadFile(mCurrentFile.mName);
	else	
		SetCurrentMessage("reaload: y/n my friend, only y/n...");
}
//-------------------------------------------------------------------
void CEdit::GotoLine()
{
	 static vector<string> GotoHistory;
	 string strLine = Prompt("Where to?: ", GotoHistory);	
	GotoHistory.push_back( strLine );			
	stringstream ss;
	unsigned int LineNumber = 0;
	ss << strLine;
	ss >> LineNumber;
	
	if (LineNumber >= mCurrentFile.mLineBuffer.size() || LineNumber < 0)
	{
		SetCurrentMessage("goto: I can go where no other text editor can go, except there");
		return;
	}
	
	GotoLine(LineNumber);
	mCurrentLine = LineNumber;
	mPageViewTopRow = LineNumber;
	mRow = 0;
	move(mRow,mCol);
}
//-------------------------------------------------------------------
void CEdit::InsertLine( void )
{
	std::vector<string>::iterator I;
	I = mCurrentFile.mLineBuffer.begin() + mCurrentLine+1;
	if (mCurrentLine+1 < mCurrentFile.mLineBuffer.size())
		mCurrentFile.mLineBuffer.insert(I,1,string(""));
	else
		mCurrentFile.mLineBuffer.push_back("");

	mCol = 0;
	move(++mRow,mCol);
	mCurrentLine++;
	RefreshPageView();
}
//-------------------------------------------------------------------
void CEdit::Find()
{
 static vector<string> FindHistory;
 string Pattern = Prompt("Find what?: ", FindHistory);
 mLastFindString = Pattern;
 FindHistory.push_back( Pattern );

 for (int i = mCurrentLine; i < mCurrentFile.mLineBuffer.size(); i++)
{
	int pos = 0;
	if ((pos=mCurrentFile.mLineBuffer[i].find( Pattern )) != string::npos)
	{
		
		GotoLine(i);
		mCurrentLine = i;
		mPageViewTopRow = i;
		mRow = 0;
		mCol = pos;
		move(mRow,mCol);
		return;
	}
}
  
  int Index = rand() % (sizeof(gStringNotFoundMessage) / sizeof(string));
  SetCurrentMessage("find: " + gStringNotFoundMessage[Index] );
}
//--------------------------------------------------------------------
void CEdit::FindNext()
{
	 for (int i = mCurrentLine+1; i < mCurrentFile.mLineBuffer.size(); i++)
	{
		int pos = 0;
		if ((pos=mCurrentFile.mLineBuffer[i].find( mLastFindString )) != string::npos)
		{
		
			GotoLine(i);
			mCurrentLine = i;
			mPageViewTopRow = i;
			mRow = 0;
			mCol = pos;
			move(mRow,mCol);
			return;
		}
	}
  
  int Index = rand() % (sizeof(gStringNotFoundMessage) / sizeof(string));
  SetCurrentMessage("find: " + gStringNotFoundMessage[Index] );
}
//--------------------------------------------------------------------
void CEdit::ExitApplication()
{
	
	
	int Index = rand() % (sizeof(gExitMessages) / sizeof(string));
	string Message = gExitMessages[Index] + " (y/n) ";
	
	PrintMessage(Message);
	char c= getch();
	switch (c)
	{
		case 'n':
		case 'N':
			Index = rand() % (sizeof(gNoExitMessages) / sizeof(string));
			PrintMessage(gNoExitMessages[Index]);
			refresh();
			sleep(1);
			PrintStatusBar("[Ready]");
			return;
			break;
		case 'Y':
		case 'y':
			reset_shell_mode();		//Restore the original shell mode
			erase();				        //Clear the screen before we quit
			endwin();				//Clean up CURSES
			exit(0);				        //Exit the application
		default:
			PrintMessage("What part of y/n you did not understand?");
			refresh();
			sleep(1);
			ExitApplication();
	}
	
}
//--------------------------------------------------------------------------------------
void  CEdit::DeleteLines()
{
	stringstream ss;
	unsigned int LineCount = 0;	
	vector<string> History;
	ss <<  Prompt("Delete how many lines (default is 1): ",History);
	
	
	if (ss.str() == "CANCEL")
		return;
		
	if (ss.str() == "")
		LineCount = 1;
	else
		ss >> LineCount ;
		
	move(mRow,mCol);		
	for (int i = 0; i < LineCount; i++)
	{
		deleteln();
		mCurrentFile.mLineBuffer.erase( mCurrentFile.mLineBuffer.begin() + mCurrentLine + i );
	}
	RefreshPageView();
	PrintMessage(string("Deleted ") + ss.str() + string(" lines"));
	refresh();
	sleep(1);
}
//--------------------------------------------------------------------------------------
void CEdit::SetCurrentMessage(string aMessage )
{
	mCurrentCustomMessage = aMessage;
	mPrintCustomMessage =  true;
}
//--------------------------------------------------------------------------------------

void CEdit::HandleUserInput( void )
{
int c;
   bool SelectMode = false;
   
   move(mRow, mCol);
   mColorSupport = has_colors();
   if ( mColorSupport == false)
	SetCurrentMessage("No color terminal? Common!");
   else
   {
/*	start_color();
	use_default_colors(); // this prevents ncurses from forcing a white-on-black colour scheme,
						  // regardless of what scheme is used by the terminal.
	init_pair(E_BLACK_OVER_WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair((int)E_WHITE_OVER_GREEN, COLOR_GREEN, COLOR_BLACK);
	*/
        start_color();			/* Start color 			*/
        use_default_colors();		// this prevents ncurses from forcing a white-on-black colour scheme, regardless of what scheme is used by the terminal.
		
	init_pair(FONT_WHITE, COLOR_WHITE, -1); //The -1 stands for default background
	init_pair(FONT_BLACK, COLOR_BLACK, -1); 
	init_pair(FONT_BLUE,  COLOR_BLUE,  -1); 
	init_pair(FONT_GREEN, COLOR_GREEN, -1); 
	init_pair(FONT_RED,   COLOR_RED,   -1); 
	init_pair(FONT_WHITE_OVER_BLUE, COLOR_WHITE, COLOR_BLUE);

	
	PrintStatusBar(gModeStrings[mEditorMode]);
	
   }
   refresh();
   
  while (1) 
   {
	/*	move(mRow,0);
		chgat(-1,A_NORMAL,0,NULL);
		move(mRow,mCol);*/
   
       c = getch();
       if ( c == KEY_EIC)
           break;
       mPrintCustomMessage = false;
	   
		switch (c)
		{
			//-------------------------------------------------------	
			case KEY_LEFT:
				if (mCol > 0)
					move(mRow, --mCol);
				break;
			//-------------------------------------------------------		
			case KEY_RIGHT:	
                		if (SelectMode)
                		{
		                   c = mvinch(mRow,mCol);
                		   delch();
		                   insch(c);
                		}
				if (mCol < GetNumberOfCharactersInVBufferLine(mRow))
					if (move(mRow, ++mCol) == ERR)
						throw "KEY_RIGHT: move error";
				break;
			//-------------------------------------------------------	
			case KEY_DOWN:
           
				if (mCurrentLine >= mCurrentFile.mLineBuffer.size())
					break;
					
				if (mRow == PAGE_LAST_TEXT_ROW)
				{
					
						SlidePageViewBuffer( SCROLL_DOWN );
						LoadPageView();
				} else{
					
					if (move(++mRow, mCol))
						throw "KEY_RIGHT: move error";
				}
				
				mCurrentLine++;
				break;	
			//-------------------------------------------------------
			case 10:
				InsertLine();
				
				break;
			//-------------------------------------------------------		
			case KEY_UP:
				
				if (mRow == 0)
				{
					SlidePageViewBuffer( SCROLL_UP );
					LoadPageView();
				} else{
					
					move(--mRow, mCol);
				}
				if (mCurrentLine > 0)
					mCurrentLine--;
				break;
			//-------------------------------------------------------
			case KEY_HOME:
				move( mRow, (mCol=0) );
				break;
			//-------------------------------------------------------
			case KEY_END:
				move( mRow, (mCol=GetNumberOfCharactersInVBufferLine(mRow)) );
				break;
			//-------------------------------------------------------	        				
			case KEY_NPAGE:	
				if ((mCurrentLine + (PAGE_LAST_TEXT_ROW + 1)) < mCurrentFile.mLineBuffer.size())
				{
					mCurrentLine += PAGE_LAST_TEXT_ROW;
					SlidePageViewBuffer(SCROLL_PAGE_DOWN);
					LoadPageView();
				}
				break;
			//-------------------------------------------------------	        				
			case KEY_PPAGE:	
				if ((mCurrentLine - PAGE_LAST_TEXT_ROW ) >= 0)
				{
					mCurrentLine -= PAGE_LAST_TEXT_ROW;
					SlidePageViewBuffer(SCROLL_PAGE_UP);
					LoadPageView();
				}
				break;
	                
                        //-------------------------------------------------------	        			
                        case KEY_STAB:
                                {
                                  char c = ' ';
                                  AddAction(E_ADD_CHAR,&c);
                                }
                                break;
                        //-------------------------------------------------------	        			
			case KEY_DC:
                	// delch();
			AddAction(E_DEL_CHAR);
                 	break;
			//-------------------------------------------------------		 
			case KEY_BACKSPACE:	
				move(mRow, --mCol);
				delch();
				break;
			//-------------------------------------------------------		 
			case CTRL('E'):
				ExecuteCommand();
			break;
			//-------------------------------------------------------		 
			case CTRL('B'): //Mark Line
			PrintMessage("Bookmark comming soon... marks also marks bookmarks with nice color");
			move(mRow, 0);	
			
		
					refresh();
					sleep(1);
			
			break;
			//----------------------------------------
			case CTRL('L'): //goto line
			
			      GotoLine();
			      LoadPageView();
			      move(mRow, mCol);
			      break; 
		
			//-------------------------------------------------------		
                       case CTRL('F'):
                                Find();
				LoadPageView();
				move(mRow, mCol);
                                break;
			//-------------------------------------------------------	
                       case KEY_SRIGHT:

                       case CTRL('K'):
                               SelectMode = !SelectMode;
				 
                                if (SelectMode)
				{
				        mEditorMode = E_SELECTION;
				        attron(A_STANDOUT);
                 	         }else{  
			               mEditorMode = E_EDIT_READY;
				       attroff(A_STANDOUT);
				}

			      if (mCol < GetNumberOfCharactersInVBufferLine(mRow))
					if (move(mRow, ++mCol) == ERR)
						throw "KEY_RIGHT: move error";
                               break;
			//-------------------------------------------------------	
			case CTRL('Z'):
				Undo();
				break;
			//-------------------------------------------------------	
		        case CTRL('S'):
				SaveFile( mCurrentFile.mName );
				PrintMessage(string("File '") + mCurrentFile.mName + string("' Saved"));
                                refresh();
                                sleep(1);
				break;
			//-------------------------------------------------------		
			case CTRL('Q'):
				ExitApplication();
				break;	
			//-------------------------------------------------------	
			case CTRL('D'):
				DeleteLines();
               	                break;		
			//-------------------------------------------------------
			case CTRL('N'):
				FindNext();
				LoadPageView();
				move(mRow, mCol);
				break;
			//-------------------------------------------------------
			case KEY_F(3):
				ReloadFile();
				LoadPageView();
				move(mRow, mCol);
				break;
			//-------------------------------------------------------
			case KEY_F(2):
				mOptions["line-numbers"].mActive = !mOptions["line-numbers"].mActive;
				LoadPageView();
				break;
			default:
				//insch(c);
				//move(mRow, ++mCol);	
				if (c == 'x')
				{
	//init_pair(1, COLOR_BLUE, COLOR_WHITE);
					attron(COLOR_PAIR(FONT_GREEN));
				}
				AddAction(E_ADD_CHAR,(char *)&c);
			//	attroff(COLOR_PAIR(1));

			//-------------------------------------------------------		
		}
	
            if (mPrintCustomMessage)
			PrintMessage(mCurrentCustomMessage);
	   else
			PrintStatusBar(gModeStrings[mEditorMode]);

			
/*		move(mRow, 0);	
		chgat(-1,A_STANDOUT,0,NULL);*/
		
		move(mRow,mCol);
       refresh();
   }
   move(LINES - 1, COLS - 20);
   clrtoeol();
   move(mRow, mCol);
   refresh();
}
//------------------------------------------------------------------
void CEdit::ExecuteCommand( void )
{
	int Index = rand() % (sizeof(gCommandMessages) / sizeof(string));
	static vector<string> History;
	vector<string>  Arguments;
	string Command =  Prompt("What shall I do for you Master?: ",History);
	stringstream ss;
	ss << Command;
	string Token;
	
	while (ss >> Token)
		Arguments.push_back( Token );
	
	if (Command == "CANCEL" || Arguments.size() == 0)
		return;
	else
		History.push_back( Command );	

	if (mCallBackIndex.find( Arguments[0] ) == mCallBackIndex.end())
		return SetCurrentMessage("execute: " + gCommandMessages[Index]);
	else
		SetCurrentMessage("execute: " + (this->*mCallBacks[ mCallBackIndex[ Arguments[0] ] ] )( Arguments ) );
	
		
	
		
	refresh();
	
}
//------------------------------------------------------------------
void CEdit::Undo()
{

	
	
	int Index = rand() % (sizeof(gUndoBufferEmptyMessages) / sizeof(string));
	
	if (mUndoBuffer.size() == 0)
		return SetCurrentMessage("undo: " + gUndoBufferEmptyMessages[Index]);
	

	TUndoBufferEntry UndoEntry = mUndoBuffer.back();
	mUndoBuffer.pop_back();
	mRow 		= 	UndoEntry.mRow;
	mCol 		= 	UndoEntry.mColumn;
	mCurrentLine=	UndoEntry.mLineNumber;
	GotoLine( UndoEntry.mLineNumber - mRow );
	switch (UndoEntry.mAction)
	{
		case E_DEL_CHAR:
		if (UndoEntry.mLineNumber < mCurrentFile.mLineBuffer.size() && UndoEntry.mColumn < mCurrentFile.mLineBuffer[UndoEntry.mLineNumber].size())
			mCurrentFile.mLineBuffer[UndoEntry.mLineNumber].erase(UndoEntry.mColumn,1);

		PrintMessage("You are not so sure of yourself are you?");
		refresh();
		break;

		case E_ADD_CHAR:
		mCurrentFile.mLineBuffer[UndoEntry.mLineNumber].insert(UndoEntry.mColumn,UndoEntry.mValue); 
				
		break;
	}
	LoadPageView();
	//RefreshPageView();


		
}
//------------------------------------------------------------------
void CEdit::AddAction(E_ACTIONS aAction, char * aValue )
{
	TUndoBufferEntry UndoEntry;
	UndoEntry.mLineNumber 	= mCurrentLine;
	UndoEntry.mColumn 	= mCol;
	UndoEntry.mRow	  	= mRow;
	if (aValue)
		UndoEntry.mValue	= string(aValue);
	mUnsavedChanges   	= true;
	
	switch (aAction)
	{
		case E_ADD_CHAR:
		{

			insch(*(int*)aValue);
			UndoEntry.mAction = E_DEL_CHAR;
			if (mCol > mCurrentFile.mLineBuffer[mCurrentLine].size())
			{
				int InitialLineSize = mCurrentFile.mLineBuffer[mCurrentLine].size();
				for (int i = 0; i < (mCol-InitialLineSize); i++)
					mCurrentFile.mLineBuffer[mCurrentLine] += " ";
				mCurrentFile.mLineBuffer[mCurrentLine] += aValue;
			}else
				mCurrentFile.mLineBuffer[mCurrentLine].insert(mCol,(const char *)aValue); 
			move(mRow,++mCol);

                           if (*aValue == ' ' &&  mOptions["syntax-highline"].mActive)
				RefreshPageView();
				
			break;
		}
		case E_DEL_CHAR:
		{
			UndoEntry.mAction = E_ADD_CHAR;
			UndoEntry.mValue	= mCurrentFile.mLineBuffer[mCurrentLine][mCol];
			mCurrentFile.mLineBuffer[mCurrentLine].erase(mCol,1);
			delch();	
			break;
		}
	};

	mUndoBuffer.push_back( UndoEntry );
}
//--------------------------------------------------------------------------------------

