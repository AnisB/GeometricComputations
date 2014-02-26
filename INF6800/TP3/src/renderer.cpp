/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 **/


#include "renderer.h"
#include "input.h"

void CheckGLState(const std::string& desc)
{
	if(desc=="FLUSH")
		return;
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) 
	{
		ERR_DEBUG("OpenGL error in: "<<desc.c_str()<<" "/*<<gluErrorString(e)*/<<" "<<e);
	}
	else
	{
		DEFAULT_DEBUG("No OpenGL errors@"<<desc);
	}
}


static void error_callback(int error, const char* description)
{
    ERR_DEBUG(error<<" "<<description);
}

// Class TRenderer
TRenderer::TRenderer()
: FWindowSize(TInt2())
, FIsFullScreen(false)
, FWindow(NULL)
, FIsRendering(false)
, FInitDone(false)
{

}

TRenderer::~TRenderer()
{
FIsRendering = false;
}

bool TRenderer::CreateRenderWindow(const TContextDetail& parContext)
{
	if(!FInitDone)
	{
	// Init
		if (!glfwInit())
		{
			ERR_DEBUG("Failed during glfw init.");
			return false;
		}
			// Init error call back
		glfwSetErrorCallback(error_callback);
			
			// Setting the context info
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, parContext.major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, parContext.minor);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


		// Window creation
			FWindow = glfwCreateWindow(parContext.width, parContext.lenght, parContext.windowName.c_str(), NULL, NULL);
			if(FWindow==NULL)
			{
				ERR_DEBUG("Failed creating the window: "<<parContext.width<<" "<<parContext.lenght<<" "<<parContext.windowName.c_str());
				return false;
			}
			// Setting context
		glfwMakeContextCurrent(FWindow);

	    // Initiating glew
		glewExperimental = GL_TRUE;
		GLenum glewReturn = glewInit();
		if(glewReturn)
		{
		    ERR_DEBUG("Glew returned: "<<glewGetErrorString(glewReturn));
				return false;
		}
		
		#ifdef DEBUG
		const GLubyte* renderer = glGetString (GL_RENDERER); 
		const GLubyte* version = glGetString (GL_VERSION); 
		DEFAULT_DEBUG("Renderer: "<<renderer);
		DEFAULT_DEBUG("Version: "<<version);
		#endif
		// Setting the rendering flag
		FIsRendering = true;
		CheckGLState("FLUSH");
		glfwSetInputMode(FWindow,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
	}
	else
	{
		DEFAULT_DEBUG("This window has already been created.");
		AssertRelease(FWindow != NULL);
		glfwShowWindow(FWindow);
	}
	return true;
}


void TRenderer::DestroyRenderWindow()
{
	DEFAULT_DEBUG("Destroying window.");	
	AssertRelease(FWindow != NULL);
	glfwTerminate();
	FWindow = NULL;
	FInitDone = false;

}	

bool TRenderer::Init()
{
	bool isOk = true;
	// Init the render thread context
	glfwMakeContextCurrent(FWindow);
	// initing the inputs
	InputInit();
	glClearColor(0.0,0.0,0.0,0.0); 	
	return isOk;
}

void TRenderer::preDraw()
{ 	 
	//Inits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TRenderer::postDraw()
{ 	 
	glfwSwapBuffers (FWindow);
}
void TRenderer::InputInit()
{
	glfwSetKeyCallback(FWindow, key_callback);
	glfwSetMouseButtonCallback(FWindow, mouse_button_callback);
	glfwSetScrollCallback(FWindow, mouse_scroll_callback);
	glfwSetCursorPosCallback(FWindow, mouse_pos_callback);
}

bool TRenderer::IsRendering()
{
	return (FIsRendering && !glfwWindowShouldClose(FWindow));

}

void TRenderer::SetRendering(bool parVal)
{
	FIsRendering = parVal;
}

