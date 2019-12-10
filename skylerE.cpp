//Source File for Skyler Ercoli
//Four Steps of Debugging:
//1. Identify bug, cause, attempt reproducing.
//2. Isolate to the specific lines of code that cause the issue. Determine
// what code is causing the problem and what code is working properly.
//3. Attempt fixing the code until the errors are gone.
//4. Review the code and ensure no other bugs have been caused.
//Test Push
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

void playerCollision(Game &g, Asteroid *a)
{
	Flt dist0,dist1,dist;
	dist0 = g.player.pos[0] - a->pos[0];
	dist1 = g.player.pos[1] - a->pos[1];
	dist = (dist0*dist0+dist1*dist1);
	if (dist < (a->radius*a->radius) && g.hpiterator == true) {
		g.hp -= 1;
		g.hpiterator = false;
	}
	else
		g.hpiterator = true;

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
void moveUp(Game &g)
{
	g.player.pos[1] += 5.0;
}


void moveDown(Game &g)
{
	g.player.pos[1] -= 5.0;
}

void moveLeft(Game &g)
{
	g.player.pos[0] -= 10.0;
}

void moveRight(Game &g)
{
	g.player.pos[0] += 10.0;
}

//This is my Friday code
/*
void playerDecelerate(Game &g)
{
    g.player.vel[0] = g.player.vel[0]*0.95;
    g.player.vel[1] = g.player.vel[1]*0.95;
}
*/
