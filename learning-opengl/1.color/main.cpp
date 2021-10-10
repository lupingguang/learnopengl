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

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 640;

// camera

glm::vec3 cameraPos   = glm::vec3(-1.93647f, -0.800119f, 2.33573f);
glm::vec3 cameraFront = glm::vec3(-1.76397f, -0.900119f, 2.58118f);
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
unsigned int VBO, CubeVAO,lightCubeVAO, EBO ;


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
// set up vertex data (and buffer(s)) and configure vertex attributes
float vertices[] = {
-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
        glDepthFunc(GL_LEQUAL);

        lightingShader = Shader("shader.vs", "shader.fs"); 
	lightCubeShader= Shader("light.vs", "light.fs"); 
	
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
    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(CubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

void drawmodel()
{
 	// be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
	lightingShader.setVec3("lightPos", lightPos);

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
        lightCubeShader.setMat4("view", view); 
        lightingShader.setMat4("model", model);      
   	// render container
	lightingShader.use();
        // render the cube
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        // also draw the lamp object
        lightCubeShader.use();
        //M
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	//V    
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 
	//P
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightCubeShader.setMat4("model", model);
	lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
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
