//Source File for Loay Samha
/*
 * File: LoayS.cpp
 * Author: Loay Samha   
 * Program: Sleipnir vs Odin
 * Purpose: In this file, there will be functions to build and edit.
 *
 * First Modification: 
 * Date: Sep 24
 * Description:
 * 1) Creating a function to show my name for the credits. (DONE)
 * 2) Next I will try to play with background and make the camera
 *	be controlled with movement.
 * Second Modification
 * Date Oct 8th LAB
 * Descriptions:
 * 1) Trying to implement a botton to change the screen background into
 * 	desired picture.
 * (day after)	
 * 2) adding my own source file
 * {Four steps of debbugin}
 * 1-   Identify the bug
 * 2-   Create breaks on each function of specifc line
 * 3-   Work on the breaks simultameously, and create extra print statments
 * 4-   Find the bug and try not to create another bug
 *
 *
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
#include </usr/include/AL/alut.h>
#endif
#include "log.h"
#include "fonts.h"
#include "gameFoundation.h"

void showNameLoay(Rect credits)
{
        const int red = 0xff0000;

        credits.bot = 500;
        credits.left = 600;
        credits.center = 0;
        ggprint8b(&credits, 30, red,  "%s", "Loay Samha");

}

void mainScoreboard(Rect score)
{
	const int red = 0xff0000;
	const int blue = 0x0000ff;
	const int green = 0x33cc33;
	const int yellow = 0xffff00;
	const int white = 0xffffff;

	score.bot = 500;
        score.left = 600;
        score.center = 0;
        ggprint8b(&score, 30, red,  "%s", "SCOREBOARD");
        ggprint8b(&score, 30, blue,  "%s", "1ST Skyler");
        ggprint8b(&score, 30, green,  "%s", "2ND Loay");
        ggprint8b(&score, 30, yellow,  "%s", "3RD Rodrigo");
        ggprint8b(&score, 30, white,  "%s", "4TH Avery");
}

/*
void getScore(Rect credits)
{
        const char scoreText[] = "scores.txt";

        ifstream fin(scoreText);
        if (fin.fail()) {
                cout << "ERROR opening scores file. " << scoreText << endl;
                exit(0);
        }
        char text[100];
        fin >> text;
        while (!fin.eof()) {
                getline(fin, text);
                cout << text << endl;
        }
        fin.close();
}*/

//This is my Friday code 
// Trying to initilize sound 
// Defing sound var for the use
// Havent defineded the down vars yet

/*#ifdef USE_OPENAL_SOUND
ALuint srcSound;
ALuint backSrcSound;
ALuint soundBuffer;
#endif

extern void initialize_sounds()
{
    alutInit(0, NULL);
    if ( alGetError() != AL_NO_ERROR ) {
        printf("error initializing sound\n");
        return;
    }
    alGetError();

    float vec[6] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);

}
*/


void enemyWaves(Game &g, int size)
{
            for (int j=0; j<size; j++) {
                Asteroid *a = new Asteroid;
                a->nverts = 8;
                a->radius = rnd()*80.0 + 40.0;
                Flt r2 = a->radius / 2.0;
                Flt angle = 0.0f;
                Flt inc = (PI * 2.0) / (Flt)a->nverts;
                for (int i=0; i<a->nverts; i++) {
                    a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
                    a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
                    angle += inc;
                }
                a->pos[0] = (Flt)(rand() % 1024);
                a->pos[1] = (Flt)(rand() % 500);
                a->pos[0] = .2f;
               	a->pos[1] = .5f;
                a->pos[2] = 0.0f;
                a->angle = 0.0;
               a->rotate = rnd() * 4.0 - 2.0;
                a->color[0] = 0.8;
                a->color[1] = 0.8;
                a->color[2] = 0.7;
                a->vel[0] = (Flt)(rnd()*2.0-1.0);
                a->vel[1] = (Flt)(rnd()*2.0-1.0);
                //std::cout << "asteroid" << std::endl;
                //add to front of linked list
                a->next = g.ahead;
                if (g.ahead != NULL)
                    g.ahead->prev = a;
                g.ahead = a;
                ++g.nasteroids;
            }
            clock_gettime(CLOCK_REALTIME, &g.bulletTimer);
}


void waveMain(Game &g, Timers &t)
{
	int  waveRate = 20;
   	int  waveSize = 1;
      	int  waveNum = 1;
        struct timespec at;
        clock_gettime(CLOCK_REALTIME, &at);
        double ts = t.timeDiff(&t.timeStart, &at);
        int nseconds = (int)ts *1000000;
        if (nseconds % (waveRate*1000000) == 0) {
                enemyWaves(g,waveSize);
                waveSize++;
                waveRate--;
                waveNum++;
        }
}
