#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <shader.h>
#include <GL/glut.h>
#include <stdlib.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <unistd.h>
#include <sys/time.h>
#include<stdio.h>

int glevel = 0;

// settings
const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;
static Shader mshader;
unsigned int texture1;
unsigned int VBO, VAO, EBO;
GLboolean horizontal = true;
GLuint pingpongFBO[2];
GLuint pingpongBuffers[2];

float tex_offsetx;
float tex_offsety;

void reshape(int w, int h);
void display();
void resourceready();
void blur(int level);
void processNormalKeys(unsigned char key, int x, int y);
/*
GLbyte* colorArr = new GLbyte[ SCR_WIDTH * SCR_HEIGHT * 3 ];
void saveColorData(GLbyte*  _pt, const char *  _str) {
	FILE* pFile = NULL;
	pFile = fopen(_str, "wt");
	if(!pFile) { fprintf(stderr, "error \n"); exit(-1); }
 
	for(int i=0; i<SCR_WIDTH * SCR_HEIGHT * 3; i ++) {
		if(colorArr[i] == -1) { colorArr[i] = 255; }
	}
 
	for(int i=0; i<SCR_WIDTH * SCR_WIDTH * 3; i ++) {
		fprintf(pFile, "%d\n", colorArr[i]);
	}
	fclose(pFile);
	printf("color data saved! \n");
}
*/
int main(int argc, char **argv)
{

 	#define MILLION 1000000
	struct timeval tv1,tv2;
        long sec,usec;

     
        glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("SHADER_RECTANGLE");
	glewInit();


	const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
	const GLubyte* OpenGLVersion =glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
	const GLubyte* GLESVersion =glGetString(GL_SHADING_LANGUAGE_VERSION); //返回当前OpenGL实现的版本号
	printf("OpenGL实现厂商的名字：%s\n", name);
	printf("渲染器标识符：%s\n", biaoshifu);
	printf("OpenGL实现的版本号：%s\n",OpenGLVersion );
        printf("GLES语法的版本号：%s\n",GLESVersion );

        mshader= Shader("shader.vs", "shader.fs"); 
	resourceready();
	

        gettimeofday(&tv1, NULL);
        gettimeofday(&tv2, NULL);
	sec = (tv2.tv_sec - tv1.tv_sec);
	usec = (tv2.tv_usec - tv1.tv_usec);
        
       // std::cout << "---------time cost---------------" << sec <<"(s)  "<< usec<<"(us) " << std::endl;

	glutReshapeFunc(&reshape);
	glutDisplayFunc(&display);
	glutKeyboardFunc(&processNormalKeys);
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
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
	// positions          // colors           // texture coords
	 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {  
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture 
	// -------------------------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

        stbi_set_flip_vertically_on_load(true);	
	unsigned char *data = stbi_load("640.jpeg", &width, &height, &nrChannels, 0);
	if (data)
	{
                tex_offsetx = width;
                tex_offsety = height;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "texture1 width  " <<width << "  hight   " << height  <<"  nrChannels  " << nrChannels<< std::endl;
	}
	else
	{
	std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
		//大坑！！！这里的纹理尺寸需要与屏幕（视口）像素尺寸一致，当图片尺寸大于屏幕尺寸时，会导致纹理内存填不满
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		    std::cout << "Framebuffer not complete!" << std::endl;
	}

}

void blur(int level)
{
        struct timeval tv1,tv2;
        long sec,usec;

        gettimeofday(&tv1, NULL);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
	GLboolean first_iteration = true;
	GLuint amount = level;
	mshader.use();
        float atex_offsetx = (float)level / 5.0 / (float)tex_offsetx;
        float atex_offsety = (float)level / 5.0 / (float)tex_offsety;
        std::cout << "---------------------------loop:" << atex_offsetx  << "  "<< atex_offsety  << " " <<level<< std::endl;

        mshader.setFloat("tex_offsetx", atex_offsetx);
        mshader.setFloat("tex_offsety", atex_offsety);
	for (GLuint i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]); 
		glUniform1i(glGetUniformLocation(mshader.ID, "horizontal"), horizontal);
		glBindTexture(
		GL_TEXTURE_2D, first_iteration ? texture1 : pingpongBuffers[!horizontal]); 
		//std::cout << "---------------------------loop:" << i << std::endl;

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		/*  测试单步效果
	        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glutSwapBuffers();
		usleep(500 * 1000 );
                */
		horizontal = !horizontal;
		if (first_iteration)
		first_iteration = false;
	}
	glFinish();

        gettimeofday(&tv2, NULL);
        sec = (tv2.tv_sec - tv1.tv_sec);
        usec = (tv2.tv_usec - tv1.tv_usec);

        std::cout << "---------time cost---------------" << sec <<"(s)  "<< usec<<"(us) " << std::endl;

}

void display()
{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// render
	// ------
        blur(glevel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);

	// render container
	mshader.use();
        glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	/*GLint viewPort[4] = {0};
	glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_RGB, GL_UNSIGNED_BYTE, colorArr);
	printf("color data read !\n");
	saveColorData(colorArr, "tmpcolor.txt");
	*/
	
        glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int ){
	
	switch(key) 
	{
		case 'w' : //上移
			glevel += 1;
			break;
		case 's' : //下移
                         if(glevel >= 1)
			     glevel -= 1;
                        break;
	}
	//std::cout << " cameraPosx:" << cameraPos.x <<" cameraPosy: "<<cameraPos.y << " cameraPosz: "<<cameraPos.z<< std::endl;
	//std::cout << " cameraFrontx:" << cameraPos.x <<" cameraFronty: "<<cameraPos.y << " cameraFrontz: "<<cameraPos.z<< std::endl;	
	//std::cout << " yaw:" << yaw <<" pitch: " << pitch;
	display();
}
