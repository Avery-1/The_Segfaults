// Source File for Rodrigo Ortiz
// Author: Rodrigo Ortiz   

/*
   Four steps of Debugging (From a bug report)
   1) Reproduce the Bug
   2) Locate the bad code
   3) Fix the code
   4) Test the fix and the whole program if any new bugs appeared

   ------------------------------------------------------------------
Duties: I will handle most things relating to sprites and images.
1) Create and scroll a texture/game background  COMPLETE!
2) Create a player sprite   COMPLETE!
3) Create an enemy sprite   COMPLETE!
4) Create bullet sprites    COMPLETE!
5) Create Main Menu     COMPLETE!

Stretch goals:
1) Animate player   COMPLETE! (has an odd loop)
2) Animate enemy    COMPLETE!
3) Include a foreground   COMPLETE!
4) Implement audio system and audio effects    COMPLETE!
5) Retrieve and update High Scores from a remote location
	(UPDATE: Avery Raines will handle the scoring)
*/

#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <GL/glx.h>

#ifdef OPENAL_ENABLE
//#include <AL/al.h>
//#include <AL/alut.h>
#include </usr/include/AL/alut.h>
//#include <AL/alc.h>
#endif

#include "log.h"
#include "fonts.h"
#include "gameFoundation.h"

extern void moveUp(Game &);
extern void moveDown(Game &);
extern void moveLeft(Game &);
extern void moveRight(Game &);

typedef float Flt;
typedef float Vec[3];
typedef Flt Matrix[4][4];

using namespace std;

void showRodrigosName(Rect credits)
{
    credits.bot = 200;
    credits.left = 380;
    credits.center = 0;
    ggprint8b(&credits, 30, 0x4ca64c, "Rodrigo Ortiz");
}

void renderMenu(Global &gl)
{
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, gl.gameMenu);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2i(0, 0);
    glTexCoord2f(0, 0); glVertex2i(0, gl.yres);
    glTexCoord2f(1, 0); glVertex2i(gl.xres, gl.yres);
    glTexCoord2f(1, 1); glVertex2i(gl.xres, 0);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderTutorial(Global &gl)
{
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, gl.gameTutorial);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2i(0, 0);
    glTexCoord2f(0, 0); glVertex2i(0, gl.yres);
    glTexCoord2f(1, 0); glVertex2i(gl.xres, gl.yres);
    glTexCoord2f(1, 1); glVertex2i(gl.xres, 0);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderCredits(Global &gl)
{
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, gl.gameCredits);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2i(0, 0);
    glTexCoord2f(0, 0); glVertex2i(0, gl.yres);
    glTexCoord2f(1, 0); glVertex2i(gl.xres, gl.yres);
    glTexCoord2f(1, 1); glVertex2i(gl.xres, 0);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderNav(Game &g, Global &gl)
{
    // position the navArrow according to the selection
    int navX, navY;

    if (g.gameSelection == 1) {
        navX = gl.xres/2 - 290;
        navY = gl.yres/2 + 90;
    } else if (g.gameSelection == 2) {
        navX = gl.xres/2 - 230;
        navY = gl.yres/2 + 40;
    } else if (g.gameSelection == 3) {
        navX = gl.xres/2 - 220;
        navY = gl.yres/2 - 20;
    } else if (g.gameSelection == 4) {
        navX = gl.xres/2 - 150;
        navY = gl.yres/2 - 80;
    }

    glPushMatrix();
    // x,y,z
    glTranslatef(navX, navY, 0);
    glBindTexture(GL_TEXTURE_2D, gl.arrowSpriteSilhouette);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2i(0, 0);
    glTexCoord2f(0, 0); glVertex2i(0, 64);
    glTexCoord2f(1, 0); glVertex2i(64, 64);
    glTexCoord2f(1, 1); glVertex2i(64, 0);
    glEnd();

    glPopMatrix();
    glDisable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderBackgroundLayer(int xres, int yres, float x[], 
        float y[], GLuint bgTexture)
{
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, bgTexture);

    glBegin(GL_QUADS);
    glTexCoord2f(x[0], y[1]); glVertex2i(0, 0);
    glTexCoord2f(x[0], y[0]); glVertex2i(0, yres);
    glTexCoord2f(x[1], y[0]); glVertex2i(xres, yres);
    glTexCoord2f(x[1], y[1]); glVertex2i(xres, 0);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void scrollBackground(float &pos1, float &pos2)
{
    pos1 += 0.001;
    pos2 += 0.001;
}

void renderAsteroidLayer(int xres, int yres, float x[], 
        float y[], GLuint asteroidSpriteSil)
{
    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, asteroidSpriteSil);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    glBegin(GL_QUADS);
    glTexCoord2f(x[2], y[3]); glVertex2i(0, 0);
    glTexCoord2f(x[2], y[2]); glVertex2i(0, yres);
    glTexCoord2f(x[3], y[2]); glVertex2i(xres, yres);
    glTexCoord2f(x[3], y[3]); glVertex2i(xres, 0);
    glEnd();

    glPopMatrix();
    glDisable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void scrollAsteroidLayer(float &pos1, float &pos2)
{
    pos1 += 0.002;
    pos2 += 0.002;
}

void renderPlayer(int walkFrame, float x, float y, GLuint PlayerSpriteSil)
{
    // Where the texture should spawn
    //float cy = yres / 2.0;
    float cx = x;
    float cy = y;

    // height and width of texture (how big or small)
    float h = 160.0;
    float w = h * 0.5;

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, PlayerSpriteSil);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    // % VALUE, VALUE is number of frames sprite has    
    int ix = walkFrame % 6;
    int iy = 0;
    if (walkFrame >= 6)
        iy = 1;

    // Play around with ix /  VALUE, VALUE tells it where to loop
    // (forward, backward, still) (VALUE ~ number of frames)
    float tx = (float)ix / 6.24;
    float ty = (float)iy / 1.0;

    glBegin(GL_QUADS);

    glTexCoord2f(tx, ty + 1.0);
    glVertex2i(cx - w, cy - h);
    glTexCoord2f(tx, ty);
    glVertex2i(cx - w, cy + h);

    // tx + VALUE, where VALUE crops the image for the animation loop
    glTexCoord2f(tx + .170, ty);
    glVertex2i(cx + w, cy + h);
    glTexCoord2f(tx + .170, ty + 1.0);
    glVertex2i(cx + w, cy - h);

    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}


void renderEnemy(int walkFrame, float x, float y, GLuint EnemySpriteSil)
{
    float cx = x;
    float cy = y;

    float h = 100.0;
    float w = h * 1.14;

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, EnemySpriteSil);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);

    int ix = walkFrame % 4;
    int iy = 0;
    if (walkFrame >= 4)
        iy = 1;

    float tx = (float)ix / 3.95;
    float ty = (float)iy / 1.0;

    glBegin(GL_QUADS);

    glTexCoord2f(tx, ty + 1.0);
    glVertex2i(cx - w, cy - h);
    glTexCoord2f(tx, ty);
    glVertex2i(cx - w, cy + h);

    glTexCoord2f(tx + .290, ty);
    glVertex2i(cx + w, cy + h);
    glTexCoord2f(tx + .290, ty + 1.0);
    glVertex2i(cx + w, cy - h);

    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}

void renderBullets(Game &g, Global &gl)
{
    float h = 32.0;
    float w = h * 1.5;

    Bullet *b = g.barr;

    for (int i = 0; i < g.nbullets; i++) {
        float cx = b->pos[0];
        float cy = b->pos[1];
        glPushMatrix();

        glBindTexture(GL_TEXTURE_2D, gl.bulletSpriteSilhouette);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glColor4ub(255, 255, 255, 255);

        glBegin(GL_QUADS);

        glTexCoord2f(0, 0);
        glTexCoord2f(0, 1.0f);
        glVertex2i(cx - w, cy - h);
        glTexCoord2f(0, 0);
        glVertex2i(cx - w, cy + h);

        glTexCoord2f(1.0f, 0);
        glVertex2i(cx + w, cy + h);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(cx + w, cy - h);

        glEnd();
        glPopMatrix();
        glDisable(GL_ALPHA_TEST);
        glBindTexture(GL_TEXTURE_2D, 0);
        b++;
    }
}

void repositionBullets(Timers &t, Game &g, Global &gl, Bullet *b)
{
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    int i = 0;

    while (i < g.nbullets) {
        b = &g.barr[i];

        // Bullet lifetime
        double ts = t.timeDiff(&b->time, &bt);

        if (ts > 2.5) {
            // Delete old bullet
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;
            continue;
        }

        // Move the bullet
        b->pos[0] += b->vel[0];
        b->pos[1] += b->vel[1];

        // Delete the bullet if it went off screen
        if (b->pos[0] < 0.0) {
            b->pos[0] += (float)gl.xres;
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;
        }
        else if (b->pos[0] > (float)gl.xres) {
            b->pos[0] -= (float)gl.xres;
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;
        }
        else if (b->pos[1] < 0.0) {
            b->pos[1] += (float)gl.yres;
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;
        }
        else if (b->pos[1] > (float)gl.yres) {
            b->pos[1] -= (float)gl.yres;
            memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
            g.nbullets--;
        }

        i++;
    }
}

void fireBullets(Timers &t, Game &g, Bullet *b)
{
    struct timespec bt;
    clock_gettime(CLOCK_REALTIME, &bt);
    double ts = t.timeDiff(&g.bulletTimer, &bt);

    if (ts > 0.1) {
        t.timeCopy(&g.bulletTimer, &bt);

        // Fire a bullet
        if (g.nbullets < MAX_BULLETS) {
            b = &g.barr[g.nbullets];
            t.timeCopy(&b->time, &bt);
            b->pos[0] = g.player.pos[0];
            b->pos[1] = g.player.pos[1];
            b->vel[0] = g.player.vel[0];
            b->vel[1] = g.player.vel[1];

            //convert player angle to radians
            Flt rad = ((g.player.angle+90.0) / 360.0f) * PI * 2.0;

            //convert angle to a vector
            Flt xdir = cos(rad);
            Flt ydir = sin(rad);

            b->pos[0] += xdir*20.0f;
            b->pos[1] += ydir*20.0f;
            b->vel[0] += xdir*6.0f + rnd()*0.1;
            b->vel[1] += ydir*6.0f + rnd()*0.1;
            b->color[0] = 1.0f;
            b->color[1] = 1.0f;
            b->color[2] = 1.0f;
            g.nbullets++;
        }
    }
}

// gameScreen currently set to MENU
// gameSelection is from game class, set to 1 by default

void keyLeft(Game &g, gameStates &gameScreen)
{
    if (gameScreen == MENU) {
    }
    else if (gameScreen == TUTORIAL) {
    }
    else if (gameScreen == INGAME) {
	moveLeft(g);    
        /*g.player.angle += 4.0;

        if (g.player.angle >= 360.0f)
            g.player.angle -= 360.0f;*/
	    
    }
}

void keyRight(Game &g, gameStates &gameScreen)
{
    if (gameScreen == INGAME) {
	moveRight(g);
	/*g.player.angle -= 4.0;

        if (g.player.angle < 0.0f)
            g.player.angle += 360.0f;*/
	    
    }
}

void keyDown(Game &g, gameStates &gameScreen, char caller)
{
    if (gameScreen == MENU) {
        // physics registers key presses too fast, a char is used to check if
        // physics or check_keys called the function
        if (caller == 'k') {
            g.gameSelection++;

            if (g.gameSelection > 4)
                g.gameSelection = 1;
        }
    }
    else if (gameScreen == INGAME) {
	    moveDown(g);
    }
    else if (gameScreen == HIGHSCORES) {
    }
    else if (gameScreen == CREDITS) {
    }
    else if (gameScreen == GAMEOVER) {
    }
}

void keyUp(Game &g, gameStates &gameScreen, char caller)
{
    if (gameScreen == MENU) {
        if (caller == 'k') {
            g.gameSelection--;

            if (g.gameSelection < 1)
                g.gameSelection = 4;
        }
    }
    else if (gameScreen == INGAME) {
	    moveUp(g);
    }           
    else if (gameScreen == HIGHSCORES) {
    }
    else if (gameScreen == CREDITS) {
    }
    else if (gameScreen == GAMEOVER) {
    }
}

// Going INGAME causes bullet to shoot, must use different key to select option
void keyEnter(Game &g, gameStates &gameScreen)
{
    if (gameScreen == MENU) {
        if (g.gameSelection == 1)
            gameScreen = INGAME;
        else if (g.gameSelection == 2)
            gameScreen = TUTORIAL;
        else if (g.gameSelection == 3)
            gameScreen = CREDITS;
        else if (g.gameSelection == 4)
            exit(1);
    } else if (gameScreen == TUTORIAL) {
        gameScreen = MENU;
    } else if (gameScreen == HIGHSCORES) {
        gameScreen = MENU;
    } else if (gameScreen == CREDITS) {
        gameScreen = MENU;
    } else if (gameScreen == GAMEOVER) {
        gameScreen = MENU;
    }
}

void keySpace(Timers &t, Game &g, Bullet *b, gameStates &gameScreen)
{
    if (gameScreen == INGAME) {
        fireBullets(t, g, b);
    }           
}

#ifdef OPENAL_ENABLE
ALuint alSource[1];
ALuint alBuffer[1];
#endif

void init_openAL()
{
    // This is my Friday code
    // I included ifdef statements according to the openal as a requirement
#ifdef OPENAL_ENABLE
    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR)
        printf("Audio setup error\n");

    // clear error state
    alGetError();

    // setup listener
    float vec[6] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);

    alBuffer[0] = alutCreateBufferFromFile("./sounds/soundtrack.wav");

    alGenSources(1, alSource);
    alSourcei(alSource[0], AL_BUFFER, alBuffer[0]);
    alSourcef(alSource[0], AL_GAIN, 1.0f);
    alSourcef(alSource[0], AL_PITCH, 1.0f);
    alSourcei(alSource[0], AL_LOOPING, AL_TRUE);

    alSourcePlay(alSource[0]);
    if (alGetError() != AL_NO_ERROR)
        printf("Audio setup error\n");

#endif
}

void clean_openAL()
{
#ifdef OPENAL_ENABLE
    alDeleteSources(1, &alSource[0]);
    alDeleteBuffers(1, &alBuffer[0]);
    ALCcontext *Context = alcGetCurrentContext();
    ALCdevice *Device = alcGetContextsDevice(Context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(Context);
    alcCloseDevice(Device);
#endif
}
