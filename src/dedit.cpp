#include "CEditor.h"


CEdit Dedit;
#define MAX_FILENAME_LEN 512

char gFileName[ MAX_FILENAME_LEN ];

#define CTRL(c) ((c) & 037)

/* Global value of current cursor position */
int row, col;
void input();
static string gTextBuffer;

int main(int argc,char **argv)
{

try
{
   int i, n, l;
   int c;
   int line = 0;
  
 
	srand (time(NULL));
   if (argc != 2)
   {
       fprintf(stderr, "Usage: %s file\n", argv[0]);
       exit(1);
   }
   
   sprintf(gFileName,"%s",argv[1]);
   



   Dedit.LoadFile( gFileName );
   Dedit.Initialize();


   Dedit.GotoLine( 0 );
   Dedit.LoadPageView();

   move(0,0);
   refresh();
   //Edit();
   Dedit.HandleUserInput();

  

   endwin();
   return 0;
}

catch (std::string aError )
{
	cout << "-E- " << aError << "\n";
}
}
