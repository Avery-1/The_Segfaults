//
//program: asteroids.cpp
//author:  Gordon Griesel
//date:    2014 - 2018
//mod spring 2015: added constructors
//This program is a game starting point for a 3350 project.
//
//
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <time.h>
#include <cmath>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
/*
*/
#ifdef OPENAL_ENABLE
#include </usr/include/AL/alut.h>
#endif

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "gameFoundation.h"

Global gl;
Game g;
Timers t;
gameStates gameScreen;



Image img[9] = {
    "./images/gameBackground.png",
    "./images/playerSprite.png",
    "./images/gameAsteroidLayer.png",
    "./images/enemySprite.png",
    "./images/bulletSprite.png",
    "./images/navArrow.png",
    "./images/gameMenu.png",
    "./images/gameTutorial.png",
    "./images/gameCredits.png"
};

//X Windows variables
class X11_wrapper {
    private:
        Display *dpy;
        Window win;
        GLXContext glc;
    public:
        X11_wrapper() { 
            GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
            //GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };

            // what resolution the window will be created as
            setup_screen_res(1536, 624);
            dpy = XOpenDisplay(NULL);
            if(dpy == NULL) {
                printf("\n\tcannot connect to X server\n\n");
                exit(EXIT_FAILURE);
            }
            Window root = DefaultRootWindow(dpy);

            XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
            if(vi == NULL) {
                printf("\n\tno appropriate visual found\n\n");
                exit(EXIT_FAILURE);
            }
            Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
            XSetWindowAttributes swa;
            swa.colormap = cmap;
            swa.event_mask =
                ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask |
                ButtonPressMask | ButtonReleaseMask |
                StructureNotifyMask | SubstructureNotifyMask;
            win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
                    vi->depth, InputOutput, vi->visual,
                    CWColormap | CWEventMask, &swa);
            set_title();
            glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
            glXMakeCurrent(dpy, win, glc);

        }
        ~X11_wrapper() {
            XDestroyWindow(dpy, win);
            XCloseDisplay(dpy);
        }
        void set_title() {
            //Set the window title bar.
            XMapWindow(dpy, win);
            XStoreName(dpy, win, "They Came From Space!");
        }
        void check_resize(XEvent *e) {
            //The ConfigureNotify is sent by the
            //server if the window is resized.
            if (e->type != ConfigureNotify)
                return;
            XConfigureEvent xce = e->xconfigure;
            if (xce.width != gl.xres || xce.height != gl.yres) {
                //Window size did change.
                reshape_window(xce.width, xce.height);
            }
        }
        void reshape_window(int width, int height) {
            //window has been resized.
            setup_screen_res(width, height);
            glViewport(0, 0, (GLint)width, (GLint)height);
            glMatrixMode(GL_PROJECTION); glLoadIdentity();
            glMatrixMode(GL_MODELVIEW); glLoadIdentity();
            glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
            set_title();
        }
        void setup_screen_res(const int w, const int h) {
            gl.xres = w;
            gl.yres = h;
        }
        void swapBuffers() {
            glXSwapBuffers(dpy, win);
        }
        bool getXPending() {
            return XPending(dpy);
        }
        XEvent getXNextEvent() {
            XEvent e;
            XNextEvent(dpy, &e);
            return e;
        }

        void show_mouse_cursor(const int onoff) {
            if (onoff) {
                //this removes our own blank cursor.
                XUndefineCursor(dpy, win);
                return;
            }
            //vars to make blank cursor
            Pixmap blank;
            XColor dummy;
            char data[1] = {0};
            Cursor cursor;
            //make a blank cursor
            blank = XCreateBitmapFromData (dpy, win, data, 1, 1);
            if (blank == None)
                std::cout << "error: out of memory." << std::endl;
            cursor = XCreatePixmapCursor(dpy, blank, blank, &dummy, &dummy, 0, 0);
            XFreePixmap(dpy, blank);
            //this makes you the cursor. then set it using this function
            XDefineCursor(dpy, win, cursor);
            //after you do not need the cursor anymore use this function.
            //it will undo the last change done by XDefineCursor
            //(thus do only use ONCE XDefineCursor and then XUndefineCursor):
        }
} x11;//(0, 0);

//function prototypes
void init_opengl(void);
int check_keys(XEvent *e);
void physics();
void render();

extern void showSkylerName(Rect rect);
// -- Loay Functions -- //
extern void enemyWaves(Game &, Global &, int);
extern void showNameLoay(Rect rect);
extern void readScores();
extern int incrementTestScore(int x);
extern void readOnlineScores();
extern void showHighScores();
// -- Skyler's Functions -- //
extern void BoundaryCheck(Global &, Game &);
extern void VelCheck(Global &, Game &);
//extern void moveUp(Game &);
//extern void moveDown(Game &);
//extern void moveLeft(Game &);
//extern void moveRight(Game &);
extern void ghettoRepeatRateFix();
extern void ghettoRepeatRateFixReset();
extern void playerCollision(Game &g, Asteroid *a);
// -- Avery's Functions -- //
extern void printAveryName(Rect rect);
extern void printScores(Rect rect);
extern void createMeteors(const int n);
extern void drawMeteors();
extern void deleteMeteors(Meteor *node);
extern void cleanupMeteors(void);
extern void checkMeteors();
extern int incrementScore(int&, int);
extern int showCurrentScore(Rect rect, int);
// -- Rodrigo's Functions -- //
extern void renderBackgroundLayer(int, int, float [], float [], GLuint);
extern void scrollBackground(float &, float &);
extern void scrollAsteroidLayer(float &, float &);
extern void renderAsteroidLayer(int, int, float [], float [], GLuint);
extern void renderPlayer(int, float, float, GLuint);
extern void renderEnemy(int, float, float, GLuint);
extern void renderBullets(Game &, Global &);
extern void repositionBullets(Timers &, Game &, Global &, Bullet *);
extern void fireBullets(Timers &, Game &, Bullet *);
extern void keyUp(Game &, gameStates &, char);
extern void keyDown(Game &, gameStates &, char);
extern void keyLeft(Game &, gameStates &);
extern void keyRight(Game &, gameStates &);
extern void keySpace(Timers &t, Game &, Bullet *, gameStates &);
extern void keyEnter(Game &, gameStates &);
extern void showRodrigosName(Rect rect);
extern void renderNav(Game &, Global &);
extern void renderMenu(Global &);
extern void renderTutorial(Global &);
extern void renderCredits(Global &);
extern void init_openAL();
extern void clean_openAL();

//==========================================================================
// M A I N
//==========================================================================
int main()
{
    logOpen();
    init_opengl();
    srand(time(NULL));
    clock_gettime(CLOCK_REALTIME, &t.timePause);
    clock_gettime(CLOCK_REALTIME, &t.timeStart);
    int done=0;
    init_openAL();
    // Set the game screen and selection
    gameScreen = MENU;
    g.gameSelection = 1; 

    while (!done) {
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.check_resize(&e);
            done = check_keys(&e);
        }

        clock_gettime(CLOCK_REALTIME, &t.timeCurrent);
        t.timeSpan = t.timeDiff(&t.timeStart, &t.timeCurrent);
        t.timeCopy(&t.timeStart, &t.timeCurrent);
        t.physicsCountdown += t.timeSpan;

        while (t.physicsCountdown >= t.physicsRate) {
            physics();
            t.physicsCountdown -= t.physicsRate;
        }

        //physics();
        render();
        x11.swapBuffers();
    }

    ghettoRepeatRateFixReset();
    cleanup_fonts();
    clean_openAL();
    logClose();
    return 0;
}

// -- From: Rainforest -- //
unsigned char *buildAlphaData(Image *img)
{
    //add 4th component to RGB stream...
    int i;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *)img->data;
    newdata = (unsigned char *)malloc(img->width * img->height * 4);
    ptr = newdata;
    unsigned char a,b,c;
    //use the first pixel in the image as the transparent color.
    unsigned char t0 = *(data+0);
    unsigned char t1 = *(data+1);
    unsigned char t2 = *(data+2);
    for (i=0; i<img->width * img->height * 3; i+=3) {
        a = *(data+0);
        b = *(data+1);
        c = *(data+2);
        *(ptr+0) = a;
        *(ptr+1) = b;
        *(ptr+2) = c;
        *(ptr+3) = 1;
        if (a==t0 && b==t1 && c==t2)
            *(ptr+3) = 0;
        //-----------------------------------------------
        ptr += 4;
        data += 3;
    }
    return newdata;
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, gl.xres, gl.yres);

    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    //This sets 2D mode (no perspective)
    glOrtho(0, gl.xres, 0, gl.yres, -1, 1);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);


    //Clear the screen to black
    glClearColor(1.0, 1.0, 1.0, 1.0);

    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();

    //-- From: Background/Rainforest --//
    //load the images file into a ppm structure.
    gl.tex.backImage = &img[0];

    //create opengl texture elements
    glGenTextures(1, &gl.tex.backTexture);
    glGenTextures(1, &gl.playerSprite);
    glGenTextures(1, &gl.playerSpriteSilhouette);
    glGenTextures(1, &gl.asteroidSprite);
    glGenTextures(1, &gl.asteroidSpriteSilhouette);
    glGenTextures(1, &gl.bulletSprite);
    glGenTextures(1, &gl.bulletSpriteSilhouette);
    glGenTextures(1, &gl.arrowSprite);
    glGenTextures(1, &gl.arrowSpriteSilhouette);
    glGenTextures(1, &gl.gameMenu);
    glGenTextures(1, &gl.gameTutorial);
    glGenTextures(1, &gl.gameCredits);

    int w = img[0].width;
    int h = img[0].height;
    int w1 = img[1].width;
    int h1 = img[1].height;
    int w2 = img[2].width;
    int h2 = img[2].height;
    int w3 = img[3].width;
    int h3 = img[3].height;
    int w4 = img[4].width;
    int h4 = img[4].height;
    int w5 = img[5].width;
    int h5 = img[5].height;
    int w6 = img[6].width;
    int h6 = img[6].height;
    int w7 = img[7].width;
    int h7 = img[7].height;
    int w8 = img[8].width;
    int h8 = img[8].height;

    // Keep this, allows for rendering of background
    gl.tex.xc[0] = 0.0;
    gl.tex.xc[1] = 1.0;
    gl.tex.yc[0] = 0.0;
    gl.tex.yc[1] = 1.0;

    glBindTexture(GL_TEXTURE_2D, gl.tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[0].data);

    // -- PLAYER AND PLAYER SILHOUETTE -- //
    glBindTexture(GL_TEXTURE_2D, gl.playerSprite);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w1, h1, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[1].data);


    // Silhouette
    glBindTexture(GL_TEXTURE_2D, gl.playerSpriteSilhouette);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    unsigned char *PlayerSpriteSilhouetteData = buildAlphaData(&img[1]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w1, h1, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, PlayerSpriteSilhouetteData);
    free(PlayerSpriteSilhouetteData);
    // ---------------------------------- //
    // -- ASTEROID AND ASTEROID SILHOUETTE -- //
    glBindTexture(GL_TEXTURE_2D, gl.asteroidSprite);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w2, h2, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[2].data);

    // Rendering of asteroid, kinda
    gl.tex.xc[2] = 0.0;
    gl.tex.xc[3] = 1.0;
    gl.tex.yc[2] = 0.0;
    gl.tex.yc[3] = 1.0;

   // Background   
    glBindTexture(GL_TEXTURE_2D, gl.asteroidSpriteSilhouette);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    unsigned char *AsteroidSpriteSilhouetteData = buildAlphaData(&img[2]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w2, h2, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, AsteroidSpriteSilhouetteData);
    free(AsteroidSpriteSilhouetteData);
    // ---------------------------------- //
    // -- ENEMY AND ENEMY SILHOUETTE -- //
    glBindTexture(GL_TEXTURE_2D, gl.enemySprite);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w3, h3, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[3].data);

    // Silhouette
    glBindTexture(GL_TEXTURE_2D, gl.enemySpriteSilhouette);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    unsigned char *EnemySpriteSilhouetteData = buildAlphaData(&img[3]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w3, h3, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, EnemySpriteSilhouetteData);
    free(EnemySpriteSilhouetteData);
    // ---------------------------------- //
    // -- BULLET AND BULLET SILHOUETTE -- //
    glBindTexture(GL_TEXTURE_2D, gl.bulletSprite);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w4, h4, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[4].data);


    // Silhouette
    glBindTexture(GL_TEXTURE_2D, gl.bulletSpriteSilhouette);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    unsigned char *BulletSpriteSilhouetteData = buildAlphaData(&img[4]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w4, h4, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, BulletSpriteSilhouetteData);
    free(BulletSpriteSilhouetteData);
    
    // ---------------------------------- //
    // -- ARROW AND ARROW SILHOUETTE -- //
   
    glBindTexture(GL_TEXTURE_2D, gl.arrowSprite);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w5, h5, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[5].data);

    // Silhouette
    
    glBindTexture(GL_TEXTURE_2D, gl.arrowSpriteSilhouette);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    unsigned char *ArrowSpriteSilhouetteData = buildAlphaData(&img[5]);	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w5, h5, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, ArrowSpriteSilhouetteData);
    free(ArrowSpriteSilhouetteData);

//******************---------------------------Arrow is above this.
    // ---------------------------------- //
    // -- MENU --//
    glBindTexture(GL_TEXTURE_2D, gl.gameMenu);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w6, h6, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[6].data);
    // ---------------------------------- //
    // -- TUTORIAL --//
    glBindTexture(GL_TEXTURE_2D, gl.gameTutorial);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w7, h7, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[7].data);
    // ---------------------------------- //
    // -- CREDITS --//
    glBindTexture(GL_TEXTURE_2D, gl.gameCredits);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w8, h8, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img[8].data);
    // ---------------------------------- //
}

void normalize2d(Vec v)
{
    Flt len = v[0]*v[0] + v[1]*v[1];
    if (len == 0.0f) {
        v[0] = 1.0;
        v[1] = 0.0;
        return;
    }
    len = 1.0f / sqrt(len);
    v[0] *= len;
    v[1] *= len;
}

int check_keys(XEvent *e)
{
    //keyboard input?
    static int shift=0;
    int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
    //Log("key: %i\n", key);

    if (e->type == KeyRelease) {
        gl.keys[key]=0;
        if (key == XK_Shift_L || key == XK_Shift_R)
            shift=0;
        return 0;
    }

    if (e->type == KeyPress) {
        //std::cout << "press" << std::endl;
        gl.keys[key]=1;
        if (key == XK_Shift_L || key == XK_Shift_R) {
            shift=1;
            return 0;
        }
    } else {
        return 0;
    }
    if (shift){}
    switch (key) {
        case XK_Escape:
            return 1;
        case XK_l:
            if (gl.lPressed == false)
                gl.lPressed = true;
            else
                gl.lPressed=false;
            break;   
        case XK_i:
            if (gl.iPressed == false) {
                gl.iPressed = true;
                incrementTestScore(gl.score);
            }
            else
                gl.iPressed = false;
            break;
        case XK_f:
            break;
        case XK_s:
            break;
        case XK_w:
            t.recordTime(&t.walkTime);
            gl.walk = 1;
            break;
        case XK_r:
	    gl.showMeteor ^= 1;
	    break;    
        case XK_equal:
            break;
        case XK_minus:
            break;
        case XK_Up:
            keyUp(g,gameScreen, 'k');
            break;
        case XK_Down:
            keyDown(g,gameScreen, 'k');
	    break;
	case XK_Left:
	    keyLeft(g,gameScreen);
	    break;
	case XK_Right:
	    keyRight(g,gameScreen);
	    break;
        case XK_Return:
            keyEnter(g, gameScreen);
            break;

            // -- Player Movement and Menu Navigation -- //
            /*
               case XK_Left:
               keyLeft(g);
               break;
               case XK_Right:
               keyRight(g);
               break;
               case XK_Up:
               keyUp(g);
               break;
               case XK_space:
            // init a bullet
            Bullet *b = NULL;
            keySpace(t, g, b);
            break;
            // -- End Movement and Menu -- //
            }
            if (g.ThrustOn) {
            //should thrust be turned off
            struct timespec mtt;
            clock_gettime(CLOCK_REALTIME, &mtt);
            double tdif = t.timeDiff(&mtt, &g.mouseThrustTimer);
            //std::cout << "tdif: " << tdif << std::endl;
            if (tdif < -0.3)
            g.ThrustOn = false;
            }
            */
}
return 0;
}

void deleteAsteroid(Game *g, Asteroid *node)
{
    //Remove a node from doubly-linked list
    //Must look at 4 special cases below.
    if (node->prev == NULL) {
        if (node->next == NULL) {
            //only 1 item in list.
            g->ahead = NULL;
        } else {
            //at beginning of list.
            node->next->prev = NULL;
            g->ahead = node->next;
        }
    } else {
        if (node->next == NULL) {
            //at end of list.
            node->prev->next = NULL;
        } else {
            //in middle of list.
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }
    delete node;
    node = NULL;
}

void buildAsteroidFragment(Asteroid *ta, Asteroid *a)
{
    //build ta from a
    ta->nverts = 8;
    ta->radius = a->radius / 2.0;
    Flt r2 = ta->radius / 2.0;
    Flt angle = 0.0f;
    Flt inc = (PI * 2.0) / (Flt)ta->nverts;
    for (int i=0; i<ta->nverts; i++) {
        ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
        ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
        angle += inc;
    } 
    ta->pos[0] = a->pos[0] + rnd()*10.0-5.0;
    ta->pos[1] = a->pos[1] + rnd()*10.0-5.0;
    ta->pos[2] = 0.0f;
    ta->angle = 0.0;
    ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
    ta->color[0] = 0.8;
    ta->color[1] = 0.8;
    ta->color[2] = 0.7;
    ta->vel[0] = a->vel[0] + (rnd()*2.0-1.0);
    ta->vel[1] = a->vel[1] + (rnd()*2.0-1.0);
    //std::cout << "frag" << std::endl;
}

void physics()
{
    scrollBackground(gl.tex.xc[0], gl.tex.xc[1]);
    scrollAsteroidLayer(gl.tex.xc[2], gl.tex.xc[3]);
    if (gl.showMeteor)
		checkMeteors();
    // init a bullet
    Bullet *b = NULL;

    if (gl.walk) {
        //man is walking...
        //when time is up, advance the frame.
        t.recordTime(&t.timeCurrent);
        double timeSpan = t.timeDiff(&t.walkTime, &t.timeCurrent);
        if (timeSpan > gl.delay) {
            //advance
            ++gl.walkFrame;
            if (gl.walkFrame >= 16)
                gl.walkFrame -= 16;
            t.recordTime(&t.walkTime);
        }
        for (int i=0; i<20; i++) {
            gl.box[i][0] -= 2.0 * (0.05 / gl.delay);
            if (gl.box[i][0] < -10.0)
                gl.box[i][0] += gl.xres + 10.0;
        }
    }

    Flt d0,d1,dist;

    //Update player position
    //g.player.pos[0] += g.player.vel[0];
    //g.player.pos[1] += g.player.vel[1];
    VelCheck(gl, g);
    //Check for collision with window edges
    /* if (g.player.pos[0] < 0.0) {
       g.player.pos[0] += (float)gl.xres;
       }
       else if (g.player.pos[0] > (float)gl.xres) {
       g.player.pos[0] -= (float)gl.xres;
       }
       else if (g.player.pos[1] < 0.0) {
       g.player.pos[1] += (float)gl.yres;
       }
       else if (g.player.pos[1] > (float)gl.yres) {
       g.player.pos[1] -= (float)gl.yres;
       } */
    BoundaryCheck(gl, g);

    // update bullet positions
    repositionBullets(t, g, gl, b);

    //Update asteroid positions
    Asteroid *a = g.ahead;
    while (a) {
        a->pos[0] += a->vel[0];
        a->pos[1] += a->vel[1];
        //Check for collision with window edges
        if (a->pos[0] < -100.0) {
            a->pos[0] += (float)gl.xres+200;
        }
        else if (a->pos[0] > (float)gl.xres+100) {
            a->pos[0] -= (float)gl.xres+200;
        }
        else if (a->pos[1] < -100.0) {
            a->pos[1] += (float)gl.yres+200;
        }
        else if (a->pos[1] > (float)gl.yres+100) {
            a->pos[1] -= (float)gl.yres+200;
        }
        a->angle += a->rotate;
        a = a->next;
    }
    //
    //Asteroid collision with bullets?
    //If collision detected:
    //     1. delete the bullet
    //     2. break the asteroid into pieces
    //        if asteroid small, delete it
    a = g.ahead;
    while (a) {
        //is there a bullet within its radius?
        int i=0;
	playerCollision(g,a);
        while (i < g.nbullets) {
            Bullet *b = &g.barr[i];
            d0 = b->pos[0] - a->pos[0];
            d1 = b->pos[1] - a->pos[1];
            dist = (d0*d0 + d1*d1);
            if (dist < (a->radius*a->radius)) {
                //std::cout << "asteroid hit." << std::endl;
                //this asteroid is hit.
                if (a->radius > MINIMUM_ASTEROID_SIZE) {
                    //break it into pieces.
                    Asteroid *ta = a;
                    buildAsteroidFragment(ta, a);
                    int r = rand()%10+5;
                    for (int k=0; k<r; k++) {
                        //get the next asteroid position in the array
                        Asteroid *ta = new Asteroid;
                        buildAsteroidFragment(ta, a);
                        //add to front of asteroid linked list
                        ta->next = g.ahead;
                        if (g.ahead != NULL)
                            g.ahead->prev = ta;
                        g.ahead = ta;
                        g.nasteroids++;
                    }
				//add 25 to player score for killing bigger enemy
				incrementScore(g.currentScore, 25);
				//std::cout << g.currentScore << std::endl;
                } else {
                    a->color[0] = 1.0;
                    a->color[1] = 0.1;
                    a->color[2] = 0.1;
                    //asteroid is too small to break up
                    //delete the asteroid and bullet
                    Asteroid *savea = a->next;
                    deleteAsteroid(&g, a);
                    a = savea;
                    g.nasteroids--;
					//add 50 to player score for killing small enemy
					incrementScore(g.currentScore, 50);
					//std::cout << g.currentScore << std::endl;
                }
                //delete the bullet...
                memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
                g.nbullets--;
                if (a == NULL)
                    break;
            }
            i++;
        }
        if (a == NULL)
            break;
        a = a->next;
    }
    if (g.ThrustOn) {
        //should thrust be turned off
        struct timespec mtt;
        clock_gettime(CLOCK_REALTIME, &mtt);
        double tdif = t.timeDiff(&mtt, &g.mouseThrustTimer);
        //std::cout << "tdif: " << tdif << std::endl;
        if (tdif < -0.3)
            g.ThrustOn = false;
    }
    //---------------------------------------------------
    //check keys pressed now
    if (gl.keys[XK_Left]) {
        //keyLeft(g, gameScreen);
    }
    if (gl.keys[XK_Right]) {
        //keyRight(g, gameScreen);
    }
    if (gl.keys[XK_Up]) {
        keyUp(g, gameScreen, 'p');
    }
    if (gl.keys[XK_Down]) {
        keyDown(g, gameScreen, 'p');
    }
    if (gl.keys[XK_space]) {
        keySpace(t, g, b, gameScreen);
    }

    /*
    if (gl.keys[XK_Return]) {
        keyEnter(g, gameScreen, 'p');
    }
    */

    if (g.ThrustOn) {
        //should thrust be turned off
        struct timespec mtt;
        clock_gettime(CLOCK_REALTIME, &mtt);
        double tdif = t.timeDiff(&mtt, &g.mouseThrustTimer);
        //std::cout << "tdif: " << tdif << std::endl;
        if (tdif < -0.3)
            g.ThrustOn = false;
    }
}

void render()
{
    Rect r;
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameScreen == MENU) {
	ghettoRepeatRateFixReset();
        renderMenu(gl);
        renderNav(g, gl);
    } else if (gameScreen == TUTORIAL) {
        renderTutorial(gl);
    } else if (gameScreen == CREDITS) {
        renderCredits(gl);
    } else if (gameScreen == INGAME) {
	ghettoRepeatRateFix();
        // Render the background
        //--------------
        renderBackgroundLayer(gl.xres, gl.yres, gl.tex.xc, gl.tex.yc,
                gl.tex.backTexture);

        // Player Sprite
        //--------------
        renderPlayer(gl.walkFrame, g.player.pos[0], g.player.pos[1],
                gl.playerSpriteSilhouette);

        // Render Asteroid Layer
        // --------------
        renderAsteroidLayer(gl.xres, gl.yres, gl.tex.xc, gl.tex.yc,
                gl.asteroidSpriteSilhouette);

        // Menu 
        //--------------
        r.bot = gl.yres - 20;
        r.left = 10;
        r.center = 0;
        if (gl.showMeteor)
			drawMeteors();
		unsigned int color = 0x00ffff00;
        //ggprint8b(&r, 16, 0x00ff0000, "3350 - Asteroids");
        //ggprint8b(&r, 16, 0x00ffff00, "n bullets: %i", g.nbullets);
        //ggprint8b(&r, 16, 0x00ffff00, "n asteroids: %i", g.nasteroids);
        ggprint8b(&r, 16, color, "L - Scores Screen");
        ggprint8b(&r, 16, color, "R - Meteor Shower");
	ggprint8b(&r, 16, color, "Current Score: %i", g.currentScore);
	ggprint8b(&r, 16, color, "Current HP: %i", g.hp);

        //-------------------------------------------------------------------------
        //Draw the player
	/*
        glColor3fv(g.player.color);
        glPushMatrix();
        glTranslatef(g.player.pos[0], g.player.pos[1], g.player.pos[2]);
        //float angle = atan2(player.dir[1], player.dir[0]);
        glRotatef(g.player.angle, 0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        //glVertex2f(-10.0f, -10.0f);
        //glVertex2f(  0.0f, 20.0f);
        //glVertex2f( 10.0f, -10.0f);
        glVertex2f(-12.0f, -10.0f);
        glVertex2f(  0.0f, 20.0f);
        glVertex2f(  0.0f, -6.0f);
        glVertex2f(  0.0f, -6.0f);
        glVertex2f(  0.0f, 20.0f);
        glVertex2f( 12.0f, -10.0f);
        glEnd();
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(0.0f, 0.0f);
        glEnd();
        glPopMatrix();
	*/

        /* This will NOT be included in the final program
         * Our player flies, not explodes
         * This will be kept for visual clarity temporarily
        if (gl.keys[XK_Up] || g.ThrustOn) {
            int i;
            //draw thrust
            Flt rad = ((g.player.angle+90.0) / 360.0f) * PI * 2.0;
            //convert angle to a vector
            Flt xdir = cos(rad);
            Flt ydir = sin(rad);
            Flt xs,ys,xe,ye,r;
            glBegin(GL_LINES);
            for (i=0; i<16; i++) {
                xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
                ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
                r = rnd()*40.0+40.0;
                xe = -xdir * r + rnd() * 18.0 - 9.0;
                ye = -ydir * r + rnd() * 18.0 - 9.0;
                glColor3f(rnd()*.3+.7, rnd()*.3+.7, 0);
                glVertex2f(g.player.pos[0]+xs,g.player.pos[1]+ys);
                glVertex2f(g.player.pos[0]+xe,g.player.pos[1]+ye);
            }
            glEnd();
        }
         */
        //-------------------------------------------------------------------------
        //Draw the asteroids
        {
            Asteroid *a = g.ahead;
            while (a) {
                // Enemy Sprite
                renderEnemy(gl.walkFrame, a->pos[0], a->pos[1],
                        gl.enemySpriteSilhouette);

                //Log("draw asteroid...\n");
                glColor3fv(a->color);
                glPushMatrix();
                glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
                glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
                //Log("%i verts\n",a->nverts);
                for (int j=0; j<a->nverts; j++) {

                    glVertex2f(a->vert[j][0], a->vert[j][1]);
                }
                glEnd();
                glBegin(GL_LINES);
                glVertex2f(0,   0);
                glVertex2f(a->radius, 0);
                glEnd();
                glPopMatrix();
                glColor3f(1.0f, 0.0f, 0.0f);
                glBegin(GL_POINTS);
                glVertex2f(a->pos[0], a->pos[1]);
                glEnd();
                a = a->next;
            }
        }
        //-------------------------------------------------------------------------
        //Draw the bullets
        renderBullets(g, gl);

        //Prints Scores to the screen
        if (gl.lPressed==true) {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            printScores(gl.credits);
        }
    }
}
