//Source File for Skyler Ercoli
//Four Steps of Debugging:
//1. Identify bug, cause, attempt reproducing.
//2. Isolate to the specific lines of code that cause the issue. Determine
// what code is causing the problem and what code is working properly.
//3. Attempt fixing the code until the errors are gone.
//4. Review the code and ensure no other bugs have been caused.
#include "fonts.h"
#include <iostream>
#include <fstream>
#include "gameFoundation.h"
void showSkylerName(Rect rect)
{
    unsigned int c = 0x00726df9;
    ggprint8b(&rect, 16, c, "Skyler Ercoli");
}

void readScoretxt()
{

}

int incrementTestScore(int x)
{
    x++;
    return x;
}

void readOnlineScores()
{
    system("./lab3sgetcpp > scores.txt");
}

void ghettoRepeatRateFix()
{
	system("xset r rate 1 40");
}
void ghettoRepeatRateFixReset()
{
	system("xset r rate");	
}
void BoundaryCheck(Global &gl, Game &g)
{
    if (g.player.pos[0] < 0.00) {
        g.player.pos[0] = 0.01;
    }
    if (g.player.pos[0] > (float)gl.xres) {
        g.player.pos[0] = (float)gl.xres-0.01;
    }
    if (g.player.pos[1] < 0.0) {
        g.player.pos[1] = 0.01;
    }
    if (g.player.pos[1] > (float)gl.yres) {
        g.player.pos[1] = (float)gl.yres-0.01;
    }
    if (g.player.pos[0] < 0.00 && g.player.pos[1] > 0.00) {
        g.player.pos[0] = 0.01;
        g.player.pos[1] = 0.01;
    }
    if (g.player.pos[0] > (float)gl.xres && 
            g.player.pos[1] > (float)gl.yres) {
        g.player.pos[0] = gl.xres - 0.01;
        g.player.pos[1] = gl.yres - 0.01;
    }
}

void VelCheck(Global &gl, Game &g)
{
    if (g.player.pos[0] < 0.00 || g.player.pos[0] > (float)gl.xres) {
        g.player.vel[0] = 0.00;
    }
    if (g.player.pos[1] < 0.00 || g.player.pos[1] > (float)gl.yres) {
        g.player.vel[1] = 0.00;
    }
}

void move(Game &g, Global &gl)
{
	if (gl.upPressed==true && gl.rightPressed==true)
	{
		g.player.pos[0] += 10.0;
		g.player.pos[1] += 10.0;
	}
	else
	if (gl.upPressed==true && gl.leftPressed==true)
	{
		g.player.pos[0] -= 10.0;
		g.player.pos[1] += 10.0;
	}
	else
	if (gl.downPressed==true && gl.rightPressed==true)
	{
		g.player.pos[0] += 10.0;
		g.player.pos[1] -= 10.0;
	}
	else
	if (gl.downPressed==true && gl.leftPressed==true)
	{
		g.player.pos[0] -= 10.0;
		g.player.pos[1] -= 10.0;
	}
	else
	if (gl.leftPressed==true && gl.rightPressed==true)
	{
		gl.leftPressed=false;
		gl.rightPressed=false;
	}
	else
	if (gl.upPressed == true)
		g.player.pos[1] += 10.0;
	else
	if (gl.downPressed == true)
		g.player.pos[1] -= 10.0;
	else
	if (gl.rightPressed == true)
		g.player.pos[0] += 10.0;
	else
	if (gl.leftPressed == true)
		g.player.pos[0] -= 10.0;
	gl.upPressed = false;
	gl.downPressed = false;
	gl.leftPressed = false;
	gl.rightPressed = false;
}
//This is my Friday code
/*
void playerDecelerate(Game &g)
{
    g.player.vel[0] = g.player.vel[0]*0.95;
    g.player.vel[1] = g.player.vel[1]*0.95;
}
*/
