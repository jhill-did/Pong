#include <windows.h>
#include <string>
#include <iostream>
#include "GL/glfw.h"
#include <gl/gl.h>
#include <ctime>
#include <stdio.h>
#include <string>


enum PaddleSide { TOP_BOTTOM, LEFT_RIGHT, NONE };

struct Paddle
{
        float x,y,sizeX,sizeY;
};

struct Ball
{
        float x,y;
        float oldX, oldY;
        float xspd, yspd;
        bool isColliding;
};

int leftScore, rightScore;

Ball ball;
Paddle left;
Paddle right;
float deltaTime;
float currentTime;
float lastTime;
bool gameOver;
float timeToReset;

PaddleSide inBounds(float x, float y, float left, float right, float top, float bottom)
{
    if(left <= x && x <= right && bottom <= y && y <= top)
        {
            //We hit, let's determine from which side
            if(ball.oldY >= top || ball.oldY <= bottom)
             return TOP_BOTTOM;
            return LEFT_RIGHT;
        }
    return NONE;
}

const char* zeroLayout = "xxx\nx x\nx x\nx x\nxxx\0";
const char* oneLayout  = "xx \n x \n x \n x \nxxx\0";
const char* twoLayout  = "xxx\n  x\n x \nx  \nxxx\0";
const char* threeLayout= "xxx\n  x\n x \n  x\nxxx\0";
const char* fourLayout = "x x\nx x\nxxx\n  x\n  x\0";
const char* fiveLayout = "xxx\nx  \nxxx\n  x\nxxx\0";

const char* getLayout(int value)
{
    switch(value)
    {
        case 0 : return zeroLayout; break;
        case 1 : return oneLayout;  break;
        case 2 : return twoLayout;  break;
        case 3 : return threeLayout;break;
        case 4 : return fourLayout; break;
        case 5 : return fiveLayout; break;
        default : return "";
    }
}

void DrawScore(const char* pattern, float x, float y)
{
    float originalX = x;
    float posX = x;
    float posY = y;

    float radius = 0.025f;
    glBegin(GL_QUADS);
        for(int x=0; pattern[x] != '\0'; x++)
        {
            switch(pattern[x])
            {
                case 'x' :
                    glVertex2f(posX - radius, posY + radius);
                    glVertex2f(posX + radius, posY + radius);
                    glVertex2f(posX + radius, posY - radius);
                    glVertex2f(posX - radius, posY - radius);
                    posX = posX + radius*2;
                break;

                case ' ' :
                    posX = posX + radius*2;
                break;

                case '\n':
                    posX = originalX;
                    posY = posY - radius*2;
                break;
            }
        }

    glEnd();

}

int main()
{

    int     width, height;
    bool    running = true;

    ball.x = ball.y = 0;
    ball.xspd = 0.55f;
    ball.yspd = 0.6f;
    ball.isColliding = false;
    printf("Default X Speed: 0.55\nDefault Y Speed: 0.6\n");
    printf("Enter X Speed:"); std::cin >> ball.xspd;
    printf("Enter Y Speed:"); std::cin >> ball.yspd;


    left.x=-0.95f; left.y=0;    left.sizeX = 0.10f; left.sizeY = 0.40f;
    right.x=0.85f; right.y=0;   right.sizeX = 0.10f; right.sizeY = 0.40f;

    lastTime =0;
    currentTime = (float)glfwGetTime();

    //Post GLFW stuff.
    glfwInit();


    if( !glfwOpenWindow( 800, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW ) )
    {
        glfwTerminate();
        return 0;
    }

    glfwSetWindowTitle("LUL LET'S PLAY PONG ROFLORFLROFLORLFORLRFOLROCL");
    printf("Game Start!\n");

    while(running)
    {
        //We'll have to calc time here every frame;
        lastTime = currentTime;
        currentTime = (float)glfwGetTime();
        deltaTime = (float)currentTime - (float)lastTime;
        //Setup stuff here.
        glfwGetWindowSize( &width, &height );
        height = height > 0 ? height : 1;
        glPointSize( (height+width)/80);
        glLineWidth( (height+width)/80);
            glEnable( GL_LINE_STIPPLE );

        glViewport( 0, 0, width, height );

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        //Setup Camera
        glColor3f(1,1,1);
        glMatrixMode( GL_PROJECTION );
        gluOrtho2D(-1,1,-1,1);
        glMatrixMode( GL_MODELVIEW );



        if(currentTime > timeToReset && gameOver == true)
        {

            gameOver = false;
            leftScore = 0;
            rightScore = 0;
            printf("MATCH START! lolololol\n");
        }

        if((leftScore>=5 || rightScore>=5) && gameOver == false)
        {
            printf(leftScore>=5?"Left side wins!\n":"Right side wins!\n");
            printf("4 Seconds till next game.\n");
            gameOver = true;
            timeToReset = currentTime+4;
        }


        //Update Ball
        ball.oldX = ball.x;
        ball.oldY = ball.y;
        if(gameOver == false)
        {
            ball.x += ball.xspd*(float)deltaTime;
            ball.y += ball.yspd*(float)deltaTime;
        }

        if(ball.x>1) { ball.x=0; ball.y=0; leftScore++;  ball.xspd = -ball.xspd;}
        if(ball.x<-1){ ball.x=0; ball.y=0; rightScore++; ball.xspd = -ball.xspd;}

                if(ball.y >= 1 || ball.y <= -1) { ball.yspd = -ball.yspd; ball.y = ball.oldY; }

        //Check for right paddle collision
        if(inBounds(ball.x,ball.y, right.x, right.x+right.sizeX, right.y, right.y-right.sizeY) == LEFT_RIGHT && ball.isColliding == false)
        {
            ball.xspd = -ball.xspd;
            ball.isColliding = true;
        }
        else if(inBounds(ball.x,ball.y, right.x, right.x+right.sizeX, right.y, right.y-right.sizeY) == TOP_BOTTOM && ball.isColliding == false)
        {
            ball.yspd = -ball.yspd;
            ball.isColliding = true;
        }
        //Check for left paddle Collision
        else if(inBounds(ball.x,ball.y, left.x, left.x+left.sizeX, left.y,left.y-left.sizeY) == LEFT_RIGHT && ball.isColliding == false)
        {
            ball.xspd = -ball.xspd;
            ball.isColliding = true;
        }
        else if(inBounds(ball.x,ball.y, left.x, left.x+left.sizeX, left.y, left.y-left.sizeY) == TOP_BOTTOM && ball.isColliding == false)
        {
            ball.yspd = -ball.yspd;
            ball.isColliding = true;
        }

        if(inBounds(ball.x, ball.y, left.x, left.x+left.sizeX, left.y, left.y-left.sizeY) == NONE && inBounds(ball.x,ball.y, right.x, right.x+right.sizeX, right.y, right.y-right.sizeY) == NONE)
        {
            ball.isColliding = false;
        }

        glLineStipple(14, 0xAAAA);
        //Draw Field
        glBegin( GL_LINES );
            glVertex2f(0,1);
            glVertex2f(0,-1);
        glEnd();

        //Draw Scores
        DrawScore(getLayout(rightScore),0.5f,0.80f);
        DrawScore(getLayout(leftScore),-0.65f,0.80f);

        //Draw Ball
        glBegin( GL_POINTS );
            glVertex2f(ball.x,ball.y);
        glEnd();


        //Update Paddles
        if(glfwGetKey(GLFW_KEY_DOWN)) right.y-=0.75f*deltaTime;
        if(glfwGetKey(GLFW_KEY_UP))   right.y+=0.75f*deltaTime;

        if(left.y-left.sizeY/2 < ball.y && ball.x < 0.10f) left.y += 0.5f*deltaTime;
        if(left.y-left.sizeY/2 > ball.y && ball.x < 0.10f) left.y -= 0.5f*deltaTime;

        //Draw Paddles
        glBegin( GL_QUADS );
            glVertex2f(right.x, right.y);
            glVertex2f(right.x+right.sizeX, right.y);
            glVertex2f(right.x+right.sizeX, right.y-right.sizeY);
            glVertex2f(right.x, right.y - right.sizeY);


            glVertex2f(left.x, left.y);
            glVertex2f(left.x+left.sizeX, left.y);
            glVertex2f(left.x+left.sizeX, left.y-left.sizeY);
            glVertex2f(left.x, left.y - left.sizeY);
        glEnd();

        glfwSwapBuffers();

        // exit if ESC was pressed or window was closed
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam( GLFW_OPENED);
    }

    glfwTerminate();

    return 0;
}



