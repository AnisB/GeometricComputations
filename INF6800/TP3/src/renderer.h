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


 #ifndef RENDERER
 #define RENDERER

#include "Defines.h"
#include "common.h"
#include "defines.h"



struct TContextDetail
{
	std::string windowName;
	int width;
	int lenght;
	int major;
	int minor;
	bool fullScreen;
};

class TRenderer
{
public:
	TRenderer();
	~TRenderer();

	bool CreateRenderWindow(const TContextDetail& parContext);
	void DestroyRenderWindow();

	bool Init();
	void preDraw();
	void postDraw();

	// Rendering activation and disable
	bool IsRendering();
	void SetRendering(bool parVal);

	private:
	void InputInit();

protected:

	// Window Infos
	bool FInitDone;
	GLFWwindow * FWindow;
	bool FIsFullScreen;
	TInt2 FWindowSize;
	bool FIsRendering;

};
// END CLASS DECLARATION

#endif // RENDERER