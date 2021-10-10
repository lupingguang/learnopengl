#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
///#include "camera.h"
#include "shader.h"

#include <GL/glut.h>
#include <stdlib.h>

///usr/local/freetype/lib/libfreetype.so 


#include </usr/local/freetype/include/freetype2/ft2build.h>
#include FT_FREETYPE_H  


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
static Shader shadertext;
static Shader shaderinstance;
unsigned int texture1;
unsigned int texture2;
unsigned int texture3;

unsigned int cubeVAO, cubeVBO;
unsigned int transparentVAO, transparentVBO, instanceVBO;
unsigned int planeVAO, planeVBO, planeEBO;
unsigned int textVAO, textVBO;

glm::vec3 translations[100];

FT_Library ft; 	
FT_Face face;
// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;
    glm::ivec2 Bearing; // bearing 这里翻译成方位/方向
    GLuint Advance;
};

std::map<GLchar, Character> Characters;

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
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
void loadTextTexture();
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
        //glutSetOption(GLUT_MULTISAMPLE, 8);
     	//glut默认也是禁止深度检测的，需要enable
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE|GLUT_DEPTH |GLUT_MULTISAMPLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("lpg");
	glewInit();

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

   	//启用混合
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);
        
        shader = Shader("object.vs", "object.fs");
	shadertext = Shader("text.vs", "text.fs");
        shaderinstance = Shader("grass.vs", "grass.fs");

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

	texture1  = loadTexture("cuba.png");
	/*  floor图片本身问题
	OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。
	这个图片是jpg并且宽高不是4的倍数，所以出现了问题。通过将纹理解压对齐参数设为1，这样才能确保不会有对齐问题
	*/
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	texture2  = loadTexture("floor.jpg");
	texture3  = loadTexture("grass.png");
        loadTextTexture();
	
        // test VAO
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

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
	glGenBuffers(1, &instanceVBO);
	
        glBindVertexArray(transparentVAO);

	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
		    glm::vec3 translation;
		    translation.x = (float)x / 10.0f + offset;
		    translation.z = (float)y / 10.0f + offset;
		    translation.y = 0.2;
		    translations[index++] = translation;
		}
	}

	// store instance data in an array buffer
	// -------------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 100, &translations[0], GL_STATIC_DRAW);
	// also set instance data
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glEnableVertexAttribArray(2);	
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//1、编译添加-g选项
        //2、gdb ./test
        //3、b 302设置断点
        //4、r 运行
        //5、到达断点后 s单步运行，结果到这个函数，为空指针
	/*
         GLEW that gets function pointers for OpenGL entry points on startup? 
         If that's the case, it's possible that glVertexAttribDivisor is a null function pointer

        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
        */
}

void drawmodel()
{

      
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	// be sure to activate shader when setting uniforms/drawing objects
        shaderinstance.use();
        glActiveTexture(GL_TEXTURE0);
    	
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
	
	shaderinstance.setMat4("projection", projection);
        shaderinstance.setMat4("view", view);       

        //grass
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, texture3);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f,  0.0f, -0.48f));
        shaderinstance.setMat4("model", model);
        /* 也是个空指针！！！
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
        */
	shader.use();
	shader.setMat4("projection", projection);
        shader.setMat4("view", view); 
        
        //floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, texture2);
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

	//text
        projection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
        shadertext.use();
	shadertext.setMat4("projection", projection);
        RenderText(shadertext, "lpg", 25.0f, 25.0f, 1.0f, glm::vec3(0.1f, 0.2f, 0.2f));
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

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    shader.use();
    shader.setVec3("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);
    
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            {xpos,      ypos + h, 0.0, 0.0},
            {xpos,      ypos,     0.0, 1.0},
            {xpos + w,  ypos,     1.0, 1.0},
            {xpos,      ypos + h, 0.0, 0.0},
            {xpos + w,  ypos,     1.0, 1.0},
            {xpos + w,  ypos + h, 1.0, 0.0},
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void loadTextTexture()
{
	// FreeType
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft)) {
	std::cout << "ERROR::freetype: Could not init FreeType Library" << std::endl;
	}
	// Load font as face
	if (FT_New_Face(ft, "Antonio-Bold.ttf", 0, &face)) {
	    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (GLubyte c = 0; c < 128; c++) {
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
		    std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
		    continue;
		}
		
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
		             GL_TEXTURE_2D,
		             0,
		             GL_RED,
		             face->glyph->bitmap.width,
		             face->glyph->bitmap.rows,
		             0,
		             GL_RED,
		             GL_UNSIGNED_BYTE,
		             face->glyph->bitmap.buffer);
		
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Now store character for later use
		Character character = {
		    texture,
		    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
		    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
		    static_cast<GLuint>(face->glyph->advance.x)
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
    	}    
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft); 
}
