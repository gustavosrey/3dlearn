/****************************************
*   Nehe MFC by Yvo van Dillen based on *
*   Jeff Molofee's Basecode Example     *
*          nehe.gamedev.net             *
*             2001/2004                 *
*                                       *
*****************************************/

#include "stdafx.h"
#include "NeheMFC.h"
#include "NeheWindow.h"
#include "Main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static float angle=0,rot1,rot2;

CMain::CMain()
{
	// Start Of User Initialization	
}

CMain::~CMain()
{

}

// Function name	: CMain::KeyPressed
// Description	    : Checks for a key press
//                    if( KeyPressed(VK_ESCAPE) ) { ... Escape was pressed ... }
// Return type		: BOOL 
// Argument         : int nCode
BOOL CMain::KeyPressed(int nCode)
{
	if( nCode >= 0 && nCode <= 255 )
	{
		return theApp.keyDown[ nCode ];
	}
	return FALSE;
}


// Function name	: CMain::Initialize
// Description	    : This function will initialize your opengl application
//					  Put in what you need
// Return type		: BOOL 
//					  return TRUE on success or FALSE on error (example :texture/model not found = FALSE)
BOOL CMain::Initialize()
{
	angle		= 0.0f;											// Set Starting Angle To Zero

	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
	glClearDepth (1.0f);										// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
	glEnable (GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel (GL_SMOOTH);									// Select Smooth Shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate


	//// �����ӿڵĴ�С
	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	return TRUE;
}


// Function name	: CMain::Deinitialize
// Description	    : This function will Deinitialize your opengl application
//					  destroy all objects here
// Return type		: void 
void CMain::Deinitialize()
{

}


// Function name	: CMain::Update
// Description	    : this function will be called every xx milliseconds
//					  update your variables through milliseconds
// Return type		: void 
// Argument         : DWORD milliseconds
void CMain::Update(DWORD milliseconds)
{	
	if (KeyPressed(VK_ESCAPE) == TRUE)						// Is ESC Being Pressed?
	{
		theApp.TerminateApplication ();						// Terminate The Program
	}

	if (KeyPressed(VK_F1) == TRUE)							// Is F1 Being Pressed?
	{		
		theApp.ToggleFullScreen ();							// Toggle Fullscreen Mode
	}

	angle += (float)(milliseconds) / 5.0f;					// Update angle Based On The Clock
}


// Function name	: CMain::Draw
// Description	    : this function will draw (blt) your opengl scene to the window
// Return type		: void 
void CMain::Draw()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();	

	gluLookAt(5.0f, 5.0f , 5.0f , 0.0f ,0.0f ,0.0f ,0.0f ,0.0f , 1.0f );
	// Translate 6 Units Into The Screen
	
	//glTranslatef (0.0f, 0.0f, -6.0f);

	DrawAxis();
	glRotatef (angle, 0.0f, 1.0f, 0.0f);						// Rotate On The Y-Axis By angle


	glColor3f(1.0f , 1.0f,0.0f);

	glBegin(GL_LINE_LOOP);							//  ����������
	glVertex3f(-1.0f, 1.0f, 0.0f);					// ����
	glVertex3f( 1.0f, 1.0f, 0.0f);					// ����
	glVertex3f( 1.0f,-1.0f, 0.0f);					// ����
	glVertex3f(-1.0f,-1.0f, 0.0f);					// ����
	glEnd();		

	glFlush ();													// Flush The GL Rendering Pipeline
}


void CMain::DrawAxis()
{
	CONST float axsisLen = 4.0f;
	glBegin(GL_LINES);	
	glColor3f(1.0f , 0.0f,0.0f);
	glVertex3f(-0, 0.0f, 0.0f);		
	glVertex3f( axsisLen, 0.0f, 0.0f);	

	glColor3f(0.0f , 1.0f,0.0f);
	glVertex3f( 0.0f,-0, 0.0f);		
	glVertex3f(0.0f,axsisLen, 0.0f);		

	glColor3f(0.0f , 0.0f,1.0f);
	glVertex3f( 0.0f,-0.0f, -0 );		
	glVertex3f(0.0f,-0.0f, axsisLen );		

	glEnd();		


}