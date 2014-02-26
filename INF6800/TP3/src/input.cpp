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

#include "input.h"

#include "camera.h"
Camera camera;


float OldX = 0.0;
float OldY = 0.0;
bool isInit = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        camera.Translate(Vector3(0.5,0.0,0.0));
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        camera.Translate(Vector3(-0.5,0.0,0.0));
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        camera.Translate(Vector3(0.0,0.0,0.5));
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        camera.Translate(Vector3(0,0.0,-0.5));
    }
}	

void FarmEvents()
{
	glfwPollEvents();
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

}
 	
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (float) height;
    float x = ratio*(2*xpos/(float)width - 1);
    float y = 2*-ypos/(float)height + 1;
    if(!isInit)
    {
        OldX =x;
        OldY =y;
        isInit= true;
        return;
    }
    std::cout<<(x-OldX)*3.14/180.0*20<<std::endl;
    camera.Yaw((x-OldX)*3.14/180.0*20);
    camera.Pitch((y-OldY)*3.14/180.0*20);
    OldX =x;
    OldY =y;
}	
