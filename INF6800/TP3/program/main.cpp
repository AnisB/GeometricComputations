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
#include "shadermanager.h"


TShader shader = TShader(0, "data/vertex.glsl", "data/tesscontrol.glsl", "data/tesseval.glsl", "data/geometry.glsl", "data/fragment.glsl");
// TShader shader = TShader(0, "data/vertex.glsl", "data/tesscontrol.glsl", "data/tesseval.glsl", "DEFAULT", "DEFAULT");
// TShader shader = TShader(0, "data/vertex2.glsl", "DEFAULT", "DEFAULT", "DEFAULT", "data/fragment2.glsl");

float cubeVertexLL[] = { 
	3.0f, -3.0f, -3.0f, 
	3.0f, -3.0f, 3.0f,
	-3.0f, -3.0f, 3.0f,
	-3.0f, -3.0f, -3.0f};

unsigned int cubeFacesLL[] = { 
	0, 1, 2,
	2, 3, 0};



GLuint CreateQuad()
{
	GLuint VAO;
	glGenVertexArrays (1, &VAO);
	glBindVertexArray (VAO);

	GLuint VBO;
	GLuint IBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexLL), cubeVertexLL, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeFacesLL), cubeFacesLL, GL_STATIC_DRAW);

	GLuint posAtt = glGetAttribLocation(shader.FProgramID, "position");

	glEnableVertexAttribArray (posAtt);

	glVertexAttribPointer (posAtt, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray (0);
	return VAO;

}

int main()
{
	TRenderer rend;
	TContextDetail newContext;
	newContext.windowName = "NurbsGeneration";
	newContext.width = 1280;
	newContext.lenght = 720;
	newContext.major = 4;
	newContext.minor = 1;
	rend.CreateRenderWindow(newContext);
	rend.Init();
	ShaderManager::Instance().CreateShader(shader);
	GLuint Quad = CreateQuad();
	camera.Translate(Vector3(0.0,0.0,-10));
	camera.DefinePerspective(45, 1280.0/720.0, 0.1, 1000);
	ShaderManager::Instance().EnableShader(shader);
	while(rend.IsRendering())
	{
		FarmEvents();
		ShaderManager::Instance().InjectMat4(shader,Matrix4(MatrixInit::Identity),"model");
		ShaderManager::Instance().InjectMat4(shader,camera.GetProjectionMatrix()*camera.GetViewMatrix()*Matrix4(MatrixInit::Identity),"modelviewprojection");
		ShaderManager::Instance().InjectMat4(shader,camera.GetViewMatrix(),"view");
		ShaderManager::Instance().InjectMat4(shader,camera.GetProjectionMatrix(),"projection");
		rend.preDraw();
	  	glBindVertexArray (Quad);
	  	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	  	glBindVertexArray (0);
		rend.postDraw();

	}
}