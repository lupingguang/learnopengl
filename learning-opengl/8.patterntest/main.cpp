#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
///#include "camera.h"
#include "shader.h"

#include <GL/glut.h>
#include <stdlib.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <time.h>
#include <iostream>
#include <vector>
#include <map>
// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 640;

// camera

glm::vec3 cameraPos   = glm::vec3(-4.23768f, 1.84723f, -3.76627f);
glm::vec3 cameraFront = glm::vec3(-4.19957f, 1.84723f, -3.85872f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = 22.4f;	
// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  -1.2f;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

static Shader shader;
static Shader shaderSingleColor;
unsigned int texture1;
unsigned int texture2;
unsigned int texture3;

unsigned int cubeVAO, cubeVBO;
unsigned int transparentVAO, transparentVBO;
unsigned int planeVAO, planeVBO, planeEBO;


// lighting
glm::vec3 lightPos(1.5f, 1.5f, 2.0f);

void reshape(int w, int h);
void display();
void processNormalKeys(unsigned char key, int x, int y);
void mousepress(int button,int state,int x,int y);
void mousepress_and_move(int x,int y);
void mousepress_in_out(int x);
void resourceready();
void drawmodel();
unsigned int loadTexture(char const * path);
// set up vertex data (and buffer(s)) and configure vertex attributes
float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
/*
float planeVertices[] = {
        // positions          // texture Coords 
         5.0f, -0.5f,  5.0f,  10.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 10.0f,

         5.0f, -0.5f,  5.0f,  10.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 10.0f,
         5.0f, -0.5f, -5.0f,  10.0f, 10.0f
};*/
float planeVertices[] = {
	// positions         // texture coords
	 5.0f, -0.5f,  5.0f,  10.0f, 10.0f, // top right
	 5.0f, -0.5f, -5.0f,  10.0f, 0.0f, // bottom right
	-5.0f, -0.5f, -5.0f,  0.0f, 0.0f, // bottom left
	-5.0f, -0.5f,  5.0f,  0.0f, 10.0f  // top left 
	};

unsigned int indices[] = {  
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

int main(int argc, char **argv)
{

        glutInit(&argc, argv);
	//glut??????????????????????????????????????????enable
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE|GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("SHADER_RECTANGLE");
	glewInit();

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

   	glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        shader = Shader("stencil_testing.vs", "stencil_testing.fs");
        shaderSingleColor = Shader("stencil_testing.vs", "stencil_single_color.fs");

	resourceready();

	glutReshapeFunc(&reshape); 
	glutDisplayFunc(&display);
	glutKeyboardFunc(&processNormalKeys);
	glutMotionFunc(&mousepress_and_move); //??????????????????????????????????????????
	glutMouseFunc(&mousepress); //????????????
	glutEntryFunc(&mousepress_in_out);
	glutMainLoop();




    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);


    return 0;
}

void reshape(int w, int h)
{
	glViewport(0, 0,(GLsizei)w, (GLsizei)h);
}

void resourceready()
{

   // cube VAO

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
   
    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
   

    // transparent VAO
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
     
    glBindVertexArray(transparentVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);


    texture1  = loadTexture("cuba.png");
    /*  floor??????????????????
	OpenGL???????????????????????????4?????????????????????????????????????????????4?????????????????????????????????????????????????????????????????????????????????????????????4????????????/??????????????????4?????????????????????????????????jpg??????????????????4???????????????????????????????????????????????????????????????????????????1??????????????????????????????????????????
*/
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    texture2  = loadTexture("floor.jpg");
    texture3  = loadTexture("grass.png");
}

void drawmodel()
{

      
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// be sure to activate shader when setting uniforms/drawing objects
        shader.use();
        shader.setInt("texture1", 0);	
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
    	
        glm::vec3 front;
    	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    	front.y = sin(glm::radians(pitch));
    	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    	cameraFront = glm::normalize(front);

        // M
    	glm::mat4 model = glm::mat4(1.0f);
	// V
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// P
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  	
	
	shader.setMat4("projection", projection);
        shader.setMat4("view", view); 
        shader.setMat4("model", model);      

        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, texture2);
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        //grass
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, texture3);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f,  0.0f, -0.48f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

    
        // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // --------------------------------------------------------------------
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
   /*     // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
        // the objects' size differences, making it look like borders.
        // -----------------------------------------------------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        shaderSingleColor.use();
        float scale = 1.3;
        // cubes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, texture1);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
*/

}
void display()
{
  
	// render
	// ------
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawmodel();
        glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y)
{
	float cameraSpeed = 0.1 ;
	
	switch(key) 
	{
		case 'q' : //??????
			cameraPos += cameraSpeed * cameraUp;
			break;
		case 'e' : //??????
			cameraPos -= cameraSpeed * cameraUp;
                        break;
		case 'w' : //??????
			cameraPos += cameraSpeed * cameraFront;
			break;
		case 's' : //??????
			cameraPos -= cameraSpeed * cameraFront;
			break;
		case 'a' : //??????
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
		case 'd' : //??????
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
	}
	std::cout << " cameraPosx:" << cameraPos.x <<" cameraPosy: "<<cameraPos.y << " cameraPosz: "<<cameraPos.z<< std::endl;
	std::cout << " cameraFrontx:" << cameraPos.x <<" cameraFronty: "<<cameraPos.y << " cameraFrontz: "<<cameraPos.z<< std::endl;	
	std::cout << " yaw:" << yaw <<" pitch: " << pitch;
	display();
}

void mousepress(int button,int state,int x,int y)
{

	firstMouse = true;

}
void mousepress_and_move(int x,int y)
{
	

   if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
	std::cout << "mouse first press_and_move x: "<<x<<"y: "<<y<< std::endl;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top
    lastX = x;
    lastY = y;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

   display();
}

void mousepress_in_out(int x)
{
	if(x == GLUT_LEFT)
        {
		firstMouse = true;
		std::cout << "mouse left "<< std::endl;
        }
        else
	{
		std::cout << "mouse in "<< std::endl;
	}
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
