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
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 360;

// camera

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.073f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -40.2f;	
// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  19.2f;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

static Shader lightingShader;
static Shader lightCubeShader;
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
        0.0f,  2.5f,  1.0f,  0.0f,  0.0f,
        0.0f, -2.5f,  1.0f,  0.0f,  1.0f,
        5.0f, -2.5f,  1.0f,  1.0f,  1.0f,

        0.0f,  2.5f,  1.0f,  0.0f,  0.0f,
        5.0f, -2.5f,  1.0f,  1.0f,  1.0f,
        5.0f,  2.5f,  1.0f,  1.0f,  0.0f
};

std::vector<glm::vec3> windows
{
        glm::vec3(0.0f, 0.0f, 0.0f),
        //glm::vec3( 1.5f, 0.0f, 0.51f),
        //glm::vec3( 0.0f, 0.0f, 0.7f),

};

int main(int argc, char **argv)
{

        glutInit(&argc, argv);
	//glut默认也是禁止深度检测的，需要enable
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("SHADER_RECTANGLE");
	glewInit();


	glEnable(GL_DEPTH_TEST);
   	//glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        lightingShader = Shader("stencil_testing.vs", "stencil_testing.fs"); 


	resourceready();

	glutReshapeFunc(&reshape); 
	glutDisplayFunc(&display);
	glutKeyboardFunc(&processNormalKeys);
	glutMotionFunc(&mousepress_and_move); //鼠标某一按键按下且移动鼠标时
	glutMouseFunc(&mousepress); //鼠标点击
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
    /*  floor图片本身问题
	OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。通常这并不会出现什么问题，因为大部分纹理的宽度都为4的倍数并/或每像素使用4个字节。但是这个图片是jpg并且宽高不是4的倍数，所以出现了问题。通过将纹理解压对齐参数设为1，这样才能确保不会有对齐问题
*/
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    texture2  = loadTexture("floor.jpg");
    texture3  = loadTexture("window.png");

}

void drawmodel()
{
	// sort the transparent windows before rendering
        // ---------------------------------------------
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(cameraPos - windows[i]);
            sorted[distance] = windows[i];
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setInt("texture1", 0);	
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
	
	lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view); 
        lightingShader.setMat4("model", model);      

        // render the cube
        //glBindVertexArray(cubeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, texture2);
        model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        

        // windows (from furthest to nearest)
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, texture3);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);


	    //2、用glm::radians转化为弧度，glm::vec3(0.0, 0.0, 1.0)表示沿Z轴旋转
model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
            std::cout << " model[0][0]-----------:" <<model[0][0]<<std::endl;
            std::cout << " model[0][2]-----------:" <<model[0][2]<<std::endl;
            std::cout << " model[2][0]-----------:" <<model[2][0]<<std::endl;
            std::cout << " model[2][2]-----------:" <<model[2][2]<<std::endl;

model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
            std::cout << " model[1][1]-----------:" <<model[1][1]<<std::endl;
            std::cout << " model[1][2]-----------:" <<model[1][2]<<std::endl;
            std::cout << " model[2][1]-----------:" <<model[2][1]<<std::endl;
            std::cout << " model[2][2]-----------:" <<model[2][2]<<std::endl;
            std::cout << " model[2][0]-----------:" <<model[2][0]<<std::endl;
            std::cout << " model[0][2]-----------:" <<model[0][2]<<std::endl;
            std::cout << " model[0][0]-----------:" <<model[0][0]<<std::endl;
            std::cout << " model[3][3]-----------:" <<model[3][3]<<std::endl;
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


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
		case 'q' : //上移
			cameraPos += cameraSpeed * cameraUp;
			break;
		case 'e' : //下移
			cameraPos -= cameraSpeed * cameraUp;
                        break;
		case 'w' : //前移
			cameraPos += cameraSpeed * cameraFront;
			break;
		case 's' : //后移
			cameraPos -= cameraSpeed * cameraFront;
			break;
		case 'a' : //左移
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
		case 'd' : //右移
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			break;
	}
	//std::cout << " cameraPosx:" << cameraPos.x <<" cameraPosy: "<<cameraPos.y << " cameraPosz: "<<cameraPos.z<< std::endl;
	//std::cout << " cameraFrontx:" << cameraPos.x <<" cameraFronty: "<<cameraPos.y << " cameraFrontz: "<<cameraPos.z<< std::endl;	
	//std::cout << " yaw:" << yaw <<" pitch: " << pitch;
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
