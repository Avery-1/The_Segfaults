#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <time.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

//macros
#define rnd() (((Flt)rand())/(Flt)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                             (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define PI 3.141592653589793
#define ALPHA 1
const int MAX_BULLETS = 11;
const Flt MINIMUM_ASTEROID_SIZE = 60.0;

enum gameStates {
    MENU = 1,
    TUTORIAL,
    INGAME,
    GAMEOVER,
    CREDITS,
    HIGHSCORES
};

//enum gameStates gameScreen = INGAME;

//gameStates gameScreen = gameStates::INGAME;
//gameStates gameScreen = INGAME;
//extern gameStates gameScreen;
//gameStates gameScreen;

class Image {
    public:
        int width, height;
        unsigned char *data;
        ~Image() { delete [] data; }
        Image(const char *fname) {
            if (fname[0] == '\0')
                return;
            //printf("fname **%s**\n", fname);
            int ppmFlag = 0;
            char name[40];
            strcpy(name, fname);
            int slen = strlen(name);
            char ppmname[80];
            if (strncmp(name+(slen-4), ".ppm", 4) == 0)
                ppmFlag = 1;
            if (ppmFlag) {
                strcpy(ppmname, name);
            } else {
                name[slen-4] = '\0';
                //printf("name **%s**\n", name);
                sprintf(ppmname,"%s.ppm", name);
                //printf("ppmname **%s**\n", ppmname);
                char ts[100];
                //system("convert eball.jpg eball.ppm");
                sprintf(ts, "convert %s %s", fname, ppmname);
                system(ts);
            }
            //sprintf(ts, "%s", name);
            FILE *fpi = fopen(ppmname, "r");
            if (fpi) {
                char line[200];
                fgets(line, 200, fpi);
                fgets(line, 200, fpi);
                //skip comments and blank lines
                while (line[0] == '#' || strlen(line) < 2)
                    fgets(line, 200, fpi);
                sscanf(line, "%i %i", &width, &height);
                fgets(line, 200, fpi);
                //get pixel data
                int n = width * height * 3;			
                data = new unsigned char[n];			
                for (int i=0; i<n; i++)
                    data[i] = fgetc(fpi);
                fclose(fpi);
            } else {
                printf("ERROR opening image: %s\n",ppmname);
                exit(0);
            }
            if (!ppmFlag)
                unlink(ppmname);
        }
};

// -- Timers & Sprite from: Walk2 -- //
class Timers {
    public:
        double physicsRate;
        double oobillion;
        struct timespec timeStart, timeEnd, timeCurrent, timePause;
        struct timespec walkTime;
        double timeSpan;
        double physicsCountdown;
        Timers() {
            physicsRate = 1.0 / 60.0;
            oobillion = 1.0 / 1e9;
        }
        double timeDiff(struct timespec *start, struct timespec *end) {
            return (double)(end->tv_sec - start->tv_sec ) +
                (double)(end->tv_nsec - start->tv_nsec) * oobillion;
        }
        void timeCopy(struct timespec *dest, struct timespec *source) {
            memcpy(dest, source, sizeof(struct timespec));
        }
        void recordTime(struct timespec *t) {
            clock_gettime(CLOCK_REALTIME, t);
        }
};

class Sprite {
    public:
        int onoff;
        int frame;
        double delay;
        Vec pos;
        Image *image;
        GLuint tex;
        struct timespec time;
        Sprite() {
            onoff = 0;
            frame = 0;
            image = NULL;
            delay = 0.1;
        }
};
// ------------------------ //


//-- Textures from: Background --//
class Texture {
    public:
        Image *titleImage;
        GLuint titleTexture;
        Image *backImage;
        GLuint backTexture;

        // change array size +2 when adding a new layer
        float xc[10];
        float yc[10];
};

class Global {
    public:
        int xres, yres;
        char keys[65536];
        Texture tex;	
        GLuint Silhouette;
        GLuint playerSprite;
        GLuint playerSpriteSilhouette;
        GLuint asteroidSprite;
        GLuint asteroidSpriteSilhouette;
        GLuint enemySprite;
        GLuint enemySpriteSilhouette;
        GLuint bulletSprite;
        GLuint bulletSpriteSilhouette;
        GLuint arrowSprite;
        GLuint arrowSpriteSilhouette;
        GLuint gameMenu;
        GLuint gameTutorial;
        GLuint gameCredits;

        // -- walk animation -- //
        int done;
        int walk;
        int walkFrame;
        double delay;
        Vec box[20];

        Rect credits;
        bool cPressed = false;
        bool lPressed = false;
        bool iPressed = false;
	bool upPressed = false;
	bool downPressed = false;
	bool leftPressed = false;
	bool rightPressed = false;
	
	int showMeteor;
	
        int score = 0;

        Global() {
            // sets where player spawns (use default res for in the middle)
            showMeteor = 0;
            xres = 512;
            yres = 416;
            memset(keys, 0, 65536);

            // -- walk animation -- //
            done=0;
            walk=1;
            walkFrame=0;
            delay = 0.1;

            for (int i=0; i<20; i++) {
                box[i][0] = rnd() * xres;
                box[i][1] = rnd() * (yres-220) + 220.0;
                box[i][2] = 0.0;
            }
        }
};
class Meteor {
public:
	int type;
	int linewidth;
	int sound;
	Vec pos;
	Vec lastpos;
	Vec vel;
	Vec maxvel;
	Vec force;
	float length;
	float color[4];
	Meteor *prev;
	Meteor *next;
};


class Player {
    public:
        Vec dir;
        Vec pos;
        Vec vel;
        float angle;
        float color[3];
    public:
        Player() {
            VecZero(dir);
            // player position spawn
            // set to Left side, Middle of Screen
            pos[0] = (Flt)200;
            pos[1] = (Flt)200;
            pos[2] = 0.0f;
            VecZero(vel);
            angle = 270.0;
            color[0] = color[1] = color[2] = 1.0;
        }
};

class Bullet {
    public:
        Vec pos;
        Vec vel;
        float color[3];
        struct timespec time;
    public:
        Bullet() { }
};

class Asteroid {
    public:
        Vec pos;
        Vec vel;
        int nverts;
        Flt radius;
        Vec vert[8];
        float angle;
        float rotate;
        float color[3];
        struct Asteroid *prev;
        struct Asteroid *next;
    public:
        Asteroid() {
            prev = NULL;
            next = NULL;
        }
};

class Game {
    public:
        Player player;
        Asteroid *ahead;
        Bullet *barr;
        int nasteroids;
        int nbullets;
		int currentScore;
        struct timespec bulletTimer;
        struct timespec mouseThrustTimer;
        bool ThrustOn;
        int gameSelection;
    public:
        Game() {
            gameSelection = 0;
            
            ahead = NULL;
            barr = new Bullet[MAX_BULLETS];
            nasteroids = 0;
            nbullets = 0;
			currentScore = 0;
            ThrustOn = false;
            //build 10 asteroids...
            for (int j=0; j<10; j++) {
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
                //a->pos[0] = .2f;
                //a->pos[1] = .5f;
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
                a->next = ahead;
                if (ahead != NULL)
                    ahead->prev = a;
                ahead = a;
                ++nasteroids;
            }
            clock_gettime(CLOCK_REALTIME, &bulletTimer);
        }
        ~Game() {
            delete [] barr;
        }
};
