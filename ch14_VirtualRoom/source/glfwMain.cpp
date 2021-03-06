//#############################################################################
//  File:      glfwMain.cpp
//  Purpose:   Implementation of the GUI with the GLFW3 (http://www.glfw.org/)
//  Author:    Marcus Hudritsch
//  Date:      01-NOV-13
//  Copyright: 2002-2014 Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h>
#ifdef SL_MEMLEAKDETECT       // set in SL.h for debug config only
#include <debug_new.h>        // memory leak detector
#endif

#include <GLFW/glfw3.h>
#include <SLInterface.h>
#include <SLSceneView.h>
#include <VRSceneView.h>
#include <SLEnums.h>

//-----------------------------------------------------------------------------
// GLobal application variables
GLFWwindow* window;              //!< The global glfw window handle.
SLint       svIndex;                //!< SceneView index
SLint       scrWidth;            //!< Window width at start up
SLint       scrHeight;           //!< Window height at start up
SLfloat     scr2fbX;             //!< Factor from screen to framebuffer coords
SLfloat     scr2fbY;             //!< Factor from screen to framebuffer coords
SLint       mouseX;              //!< Last mouse position x in pixels
SLint       mouseY;              //!< Last mouse position y in pixels
SLint       touchX2;             //!< Last finger touch 2 position x in pixels
SLint       touchY2;             //!< Last finger touch 2 position y in pixels
SLint       touchDeltaX;         //!< Delta between two fingers in x
SLint       touchDeltaY;         //!< Delta between two fingers in <
SLint       lastWidth;           //!< Last window width in pixels
SLint       lastHeight;          //!< Last window height in pixels
SLint       lastMouseWheelPos;   //!< Last mouse wheel position
SLfloat     lastMouseDownTime = 0.0f; //!< Last mouse press time
SLKey       modifiers=KeyNone;   //!< last modifier keys
SLbool      fullscreen = false;  //!< flag if window is in fullscreen mode

//-----------------------------------------------------------------------------
/*! 
onClose event handler for deallocation of the scene & sceneview. onClose is
called glfwPollEvents, glfwWaitEvents or glfwSwapBuffers.
*/
void onClose(GLFWwindow* window)
{
   slTerminate();
}

//-----------------------------------------------------------------------------
/*!
onPaint: Paint event handler that passes the event to the slPaint function. 
For accurate frame rate meassurement we have to take the time after the OpenGL 
frame buffer swapping. The FPS calculation is done in slGetWindowTitle.
*/
SLbool onPaint()
{
   bool updated = slPaint(svIndex);
   
   // Fast copy the back buffer to the front buffer. This is OS dependent.
   glfwSwapBuffers(window);
   
   // Show the title generetad by the scene library (FPS etc.)
   glfwSetWindowTitle(window, slGetWindowTitle(svIndex).c_str());
   return updated;
}

//-----------------------------------------------------------------------------
//! Maps the GLFW key codes to the SLKey codes
const SLKey mapKeyToSLKey(SLint key)
{  
   switch (key)
   {  case GLFW_KEY_SPACE:        return KeySpace;
      case GLFW_KEY_ESCAPE:       return KeyEsc;
      case GLFW_KEY_F1:           return KeyF1;
      case GLFW_KEY_F2:           return KeyF2;
      case GLFW_KEY_F3:           return KeyF3;
      case GLFW_KEY_F4:           return KeyF4;
      case GLFW_KEY_F5:           return KeyF5;
      case GLFW_KEY_F6:           return KeyF6;
      case GLFW_KEY_F7:           return KeyF7;
      case GLFW_KEY_F8:           return KeyF8;
      case GLFW_KEY_F9:           return KeyF9;
      case GLFW_KEY_F10:          return KeyF10;
      case GLFW_KEY_F11:          return KeyF11;
      case GLFW_KEY_F12:          return KeyF12;      
      case GLFW_KEY_UP:           return KeyUp;
      case GLFW_KEY_DOWN:         return KeyDown;
      case GLFW_KEY_LEFT:         return KeyLeft;
      case GLFW_KEY_RIGHT:        return KeyRight;
      case GLFW_KEY_LEFT_SHIFT:   return KeyShift;
      case GLFW_KEY_RIGHT_SHIFT:  return KeyShift;
      case GLFW_KEY_LEFT_CONTROL: return KeyCtrl;
      case GLFW_KEY_RIGHT_CONTROL:return KeyCtrl;
      case GLFW_KEY_LEFT_ALT:     return KeyAlt;
      case GLFW_KEY_RIGHT_ALT:    return KeyAlt;
      case GLFW_KEY_LEFT_SUPER:   return KeySuper; // Apple command key
      case GLFW_KEY_RIGHT_SUPER:  return KeySuper; // Apple command key
      case GLFW_KEY_TAB:          return KeyTab;
      case GLFW_KEY_ENTER:        return KeyEnter;
      case GLFW_KEY_BACKSPACE:    return KeyBackspace;
      case GLFW_KEY_INSERT:       return KeyInsert;
      case GLFW_KEY_DELETE:       return KeyDelete;
      case GLFW_KEY_PAGE_UP:      return KeyPageUp;
      case GLFW_KEY_PAGE_DOWN:    return KeyPageDown;
      case GLFW_KEY_HOME:         return KeyHome;
      case GLFW_KEY_END:          return KeyEnd;
      case GLFW_KEY_KP_0:         return KeyNP0;
      case GLFW_KEY_KP_1:         return KeyNP1;
      case GLFW_KEY_KP_2:         return KeyNP2;
      case GLFW_KEY_KP_3:         return KeyNP3;
      case GLFW_KEY_KP_4:         return KeyNP4;
      case GLFW_KEY_KP_5:         return KeyNP5;
      case GLFW_KEY_KP_6:         return KeyNP6;
      case GLFW_KEY_KP_7:         return KeyNP7;
      case GLFW_KEY_KP_8:         return KeyNP8;
      case GLFW_KEY_KP_9:         return KeyNP9;
      case GLFW_KEY_KP_DIVIDE:    return KeyNPDivide;
      case GLFW_KEY_KP_MULTIPLY:  return KeyNPMultiply;
      case GLFW_KEY_KP_SUBTRACT:  return KeyNPSubtract;
      case GLFW_KEY_KP_ADD:       return KeyNPAdd;
      case GLFW_KEY_KP_DECIMAL:   return KeyNPDecimal;
   }
   return (SLKey)key;
}

//-----------------------------------------------------------------------------
/*!
onResize: Event handler called on the resize event of the window. This event
should called once before the onPaint event.
*/
static void onResize(GLFWwindow* window, int width, int height)
{  
   lastWidth = width;
   lastHeight = height;

   // width & height are in screen coords.
   // We need to scale them to framebuffer coords.
   slResize(svIndex, (int)(width*scr2fbX), (int)(height*scr2fbY));
}


//-----------------------------------------------------------------------------
/*!
Mouse button eventhandler forwards the events to the slMouseDown or slMouseUp.
Two finger touches of touch devices are simulated with ALT & CTRL modifiers.
*/
static void onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
   SLint x = mouseX;
   SLint y = mouseY;
   
   // Translate modifiers
   modifiers=KeyNone;
   if (mods & GLFW_MOD_SHIFT)    modifiers = (SLKey)(modifiers|KeyShift);
   if (mods & GLFW_MOD_CONTROL)  modifiers = (SLKey)(modifiers|KeyCtrl);
   if (mods & GLFW_MOD_ALT)      modifiers = (SLKey)(modifiers|KeyAlt);

   if (action==GLFW_PRESS)
   {  
      // simulate double touch from touch devices
      if (modifiers & KeyAlt) 
      {  
         // Do parallel double finger move
         if (modifiers & KeyShift)
         {  if (slTouch2Down(svIndex, x, y, x - touchDeltaX, y - touchDeltaY))
               onPaint();
         } else // Do concentric double finger pinch
         {  if (slTouch2Down(svIndex, x, y, touchX2, touchY2))
               onPaint();
         }
      } 
      else  // Do standard mouse down
      {  
         SLfloat mouseDeltaTime = (SLfloat)glfwGetTime() - lastMouseDownTime;
         lastMouseDownTime = (SLfloat)glfwGetTime();

         // handle double click 
         if (mouseDeltaTime < 0.3f)
         {  
            switch (button)
            {  case GLFW_MOUSE_BUTTON_LEFT:
                  if (slDoubleClick(svIndex, ButtonLeft, x, y, modifiers)) onPaint();
                  break;
               case GLFW_MOUSE_BUTTON_RIGHT:
                  if (slDoubleClick(svIndex, ButtonRight, x, y, modifiers)) onPaint();
                  break;
               case GLFW_MOUSE_BUTTON_MIDDLE:
                  if (slDoubleClick(svIndex, ButtonMiddle, x, y, modifiers)) onPaint();
                  break;
            }
         } 
         else // normal mouse clicks
         {
            switch (button)
            {  case GLFW_MOUSE_BUTTON_LEFT:
                  if (slMouseDown(svIndex, ButtonLeft, x, y, modifiers)) onPaint();
                  break;
               case GLFW_MOUSE_BUTTON_RIGHT:
                  if (slMouseDown(svIndex, ButtonRight, x, y, modifiers)) onPaint();
                  break;
               case GLFW_MOUSE_BUTTON_MIDDLE:
                  if (slMouseDown(svIndex, ButtonMiddle, x, y, modifiers)) onPaint();
                  break;
            }
         }
      }
   }
   else
   {  
      // simulate double touch from touch devices
      if (modifiers & KeyAlt) 
      {  
         // Do parallel double finger move
         if (modifiers & KeyShift)
         {  if (slTouch2Up(svIndex, x, y, x - (touchX2 - x), y - (touchY2 - y)))
               onPaint();
         } else // Do concentric double finger pinch
         {   if (slTouch2Up(svIndex, x, y, touchX2, touchY2))
               onPaint(); 
         }   
      } 
      else  // Do standard mouse down
      {  switch (button)
         {  case GLFW_MOUSE_BUTTON_LEFT:
               if (slMouseUp(svIndex, ButtonLeft, x, y, modifiers)) onPaint(); 
               break;
            case GLFW_MOUSE_BUTTON_RIGHT:
               if (slMouseUp(svIndex, ButtonRight, x, y, modifiers)) onPaint(); 
               break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
               if (slMouseUp(svIndex, ButtonMiddle, x, y, modifiers)) onPaint(); 
               break;
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
Mouse move eventhandler forwards the events to slMouseMove or slTouch2Move.
*/
static void onMouseMove(GLFWwindow* window, double x, double y)
{
   // x & y are in screen coords.
   // We need to scale them to framebuffer coords
   x *= scr2fbX;
   y *= scr2fbY;
   mouseX  = (int)x;
   mouseY  = (int)y;
   
   // Offset of 2nd. finger for two finger simulation
   
   // Simulate double finger touches   
   if (modifiers & KeyAlt) 
   {  
      // Do parallel double finger move
      if (modifiers & KeyShift)
      {  slTouch2Move(svIndex, (int)x, (int)y, (int)x - touchDeltaX, (int)y - touchDeltaY);
      } 
      else // Do concentric double finger pinch
      {  int scrW2 = lastWidth / 2;
         int scrH2 = lastHeight / 2;
         touchX2 = scrW2 - ((int)x - scrW2);
         touchY2 = scrH2 - ((int)y - scrH2);
         touchDeltaX = (int)x - touchX2;
         touchDeltaY = (int)y - touchY2;
         slTouch2Move(svIndex, (int)x, (int)y, touchX2, touchY2);
      }
   } else // Do normal mouse move
      if (slMouseMove(svIndex, (int)x, (int)y)) onPaint();
}

//-----------------------------------------------------------------------------
/*!
Mouse wheel eventhandler forwards the events to slMouseWheel
*/
static void onMouseWheel(GLFWwindow* window, double xscroll, double yscroll)
{
   // make sure the delta is at least one integer
   int dY = (int)yscroll;
   if (dY==0) dY = (int)(SL_sign(yscroll));

   if (slMouseWheel(svIndex, dY, modifiers)) onPaint();
}

//-----------------------------------------------------------------------------
/*!
Key action eventhandler sets the modifier key state & forewards the event to
the slKeyPress function.
*/
static void onKeyAction(GLFWwindow* window, int GLFWKey, int scancode, int action, int mods)
{     
   SLKey key = mapKeyToSLKey(GLFWKey);
    
   if (action==GLFW_PRESS)
   {  switch (key)
      {  case KeyCtrl:  modifiers = (SLKey)(modifiers|KeyCtrl);  return;
         case KeyAlt:   modifiers = (SLKey)(modifiers|KeyAlt);   return;
         case KeyShift: modifiers = (SLKey)(modifiers|KeyShift); return;
      }
   } else 
   if (action==GLFW_RELEASE)
   {  switch (key)
      {  case KeyCtrl:  modifiers = (SLKey)(modifiers&~KeyCtrl);  return;
         case KeyAlt:   modifiers = (SLKey)(modifiers&~KeyAlt);   return;
         case KeyShift: modifiers = (SLKey)(modifiers&~KeyShift); return;
      }
   }
   
   // Special treatment for ESC key
   if (key == KeyEsc && action==GLFW_RELEASE) 
   {  
      if (fullscreen)
      {  fullscreen = !fullscreen;
         glfwSetWindowSize(window, scrWidth, scrHeight);
         glfwSetWindowPos(window, 10, 30);   
      } else 
      if (slKeyPress(svIndex, key, modifiers)) // ESC during RT stops it and returns false
      {  onClose(window);
         glfwSetWindowShouldClose(window, GL_TRUE);
      }
   } else 
   // Toggle fullscreen mode
   if (key == KeyF9 && action==GLFW_PRESS)
   {
      fullscreen = !fullscreen;

      if (fullscreen ) 
      {  GLFWmonitor* primary = glfwGetPrimaryMonitor();
         const GLFWvidmode* mode = glfwGetVideoMode(primary);
         glfwSetWindowSize(window, mode->width, mode->height);
         glfwSetWindowPos(window, 0, 0);
      } else 
      {  glfwSetWindowSize(window, scrWidth, scrHeight);
         glfwSetWindowPos(window, 10, 30);            
      }
   } else
   {  
      if (action==GLFW_PRESS)
         slKeyPress(svIndex, key, modifiers);
      else if (action==GLFW_RELEASE)
         slKeyRelease(svIndex, key, modifiers);
   }
}
//-----------------------------------------------------------------------------
/*!
Error callback handler for GLFW.
*/
void onGLFWError(int error, const char* description)
{
   fputs(description, stderr);
}

SLuint createTestSceneView()
{
    VRSceneView* instance = new VRSceneView;
    return instance->index();
}
//-----------------------------------------------------------------------------
/*!
The C main procedure running the GLFW GUI application.
*/
int main(int argc, char *argv[])
{  
	// set command line arguments
	vector<string> cmdLineArgs;
	for(int i = 1; i < argc; i++)
		cmdLineArgs.push_back(argv[i]);

   if (!glfwInit())
   {  fprintf(stderr, "Failed to initialize GLFW\n");
      exit(EXIT_FAILURE);
   }

   glfwSetErrorCallback(onGLFWError);
   
   // Enable fullscreen anti aliasing with 4 samples
   glfwWindowHint(GLFW_SAMPLES, 4);
   //glfwWindowHint(GLFW_DECORATED, false); // start without any window frame

   scrWidth = 600;
   scrHeight = 480;

   window = glfwCreateWindow(scrWidth, scrHeight, "My Title", NULL, NULL);
   if (!window)
   {  glfwTerminate();
      exit(EXIT_FAILURE);
   }
   
   // Get the currenct GL context. After this you can call GL   
   glfwMakeContextCurrent(window);

   // On some systems screen & framebuffer size are different
   // All commands in GLFW are in screen coords but rendering in GL is
   // in framebuffer coords
   SLint fbWidth, fbHeight;
   glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
   scr2fbX = (float)fbWidth / (float)scrWidth;
   scr2fbY = (float)fbHeight / (float)scrHeight;

   /* Include OpenGL via GLEW
   The goal of the OpenGL Extension Wrangler Library (GLEW) is to assist C/C++ 
   OpenGL developers with two tedious tasks: initializing and using extensions 
   and writing portable applications. GLEW provides an efficient run-time 
   mechanism to determine whether a certain extension is supported by the 
   driver or not. OpenGL core and extension functionality is exposed via a 
   single header file. Download GLEW at: http://glew.sourceforge.net/
   */
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      exit(EXIT_FAILURE);
   }

   glfwSetWindowTitle(window, "SLProject Test Application");
   glfwSetWindowPos(window, 10, 30);

   
   // Set number of monitor refreshes between 2 buffer swaps
   glfwSwapInterval(1);

   // Set your own physical screen dpi
   int dpi = (int)(142 * scr2fbX);
   cout << "GUI     : GLFW" << endl;
   cout << "DPI     : " << dpi << endl;
   
    slCreateScene("../_globals/oglsl/",
                  "../_data/models/",
                  "../_data/images/textures/");

    svIndex = slCreateSceneView((int)(scrWidth  * scr2fbX),
                                (int)(scrHeight * scr2fbY),
                                dpi, 
                                (SLCmd)SL_STARTSCENE,
                                cmdLineArgs,
                                (void*)&onPaint,
                                0,
                                createTestSceneView);
    // Set GLFW callback functions
    glfwSetKeyCallback(window, onKeyAction);
    glfwSetWindowSizeCallback(window, onResize);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetScrollCallback(window, onMouseWheel);
    glfwSetWindowCloseCallback(window, onClose);

    // Event loop
    // set max fps
    float fps = 60.0f;
    double deltaTime = 1.0f/fps;
    double time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        if(time+deltaTime > glfwGetTime())
        {
            Sleep((time+deltaTime-glfwGetTime())*1000);
        }

        time = glfwGetTime();

        // if no updated occured wait for the next event (power saving)
        if (!onPaint()) 
            glfwWaitEvents();
        else glfwPollEvents();
    }
   
   glfwDestroyWindow(window);
   glfwTerminate();
   exit(0);
}
//-----------------------------------------------------------------------------
