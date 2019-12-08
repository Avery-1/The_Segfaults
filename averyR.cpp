//Source File for Avery Raines
//
//Four Steps of Debugging:
//-Identify the bug
//-Isolate the bug
//-Fix the bug
//-Review/Test for other bugs


#include "fonts.h"
#include "gameFoundation.h"

Meteor *meteorhead = NULL;
int ndrops=1;
int totrain=0;
int maxrain=0;

void printAveryName(Rect rect)
{
    rect.bot = 415;
    rect.left = 450;
    rect.center = 0;
    unsigned int c = 0x00ffff44;
    ggprint8b(&rect, 16, c, "Avery Raines");
} 

void printScores(Rect rect)
{
	rect.bot = 415;
	rect.left=450;
	rect.center=0;
	unsigned int c = 0x00ffaa44;
	ggprint16(&rect, 32, c, "-High Scores-");
	ggprint16(&rect, 32, c, "1st. 450");
	ggprint16(&rect, 32, c, "2nd. 399");
	ggprint16(&rect, 32, c, "3rd. 250");
}



//This is my Friday Code
//This code contains functionality that will keep track of the player's
//current score

int incrementScore(int& currentScore, int increment)
{
	currentScore = currentScore + increment;
	return (currentScore);
}

//Meteor Shower Functionality
void createMeteors(const int n)
{
	int i;
	for (i=0; i<n; i++) {
		Meteor *node = (Meteor *)malloc(sizeof(Meteor));
		node->prev = NULL;
		node->next = NULL;
		node->sound=0;
		node->pos[0] = rnd() * (float)1536;
		node->pos[1] = rnd() * 100.0f + (float)624;
		VecCopy(node->pos, node->lastpos);
		node->vel[0] = 
		node->vel[1] = 0.0f;
		node->color[0] = rnd() * 0.2f + 0.8f;
		node->color[1] = rnd() * 0.2f + 0.8f;
		node->color[2] = rnd() * 0.2f + 0.8f;
		node->color[3] = rnd() * 0.5f + 0.3f; //alpha
		node->linewidth = random(8)+1;
		//larger linewidth = faster speed
		node->maxvel[1] = (float)(node->linewidth*16);
		node->length = node->maxvel[1] * 0.2f + rnd();
		//put raindrop into linked list
		node->next = meteorhead;
		if (meteorhead != NULL)
			meteorhead->prev = node;
		meteorhead = node;
		++totrain;
	}
}

void deleteMeteors(Meteor *node) 
{
	//remove a node from linked list
	if (node->prev == NULL) {
		if (node->next == NULL) {
			//Log("only 1 item in list.\n");
			meteorhead = NULL;
		} else {
			//Log("at beginning of list.\n");
			node->next->prev = NULL;
			meteorhead = node->next;
		}
	} else {
		if (node->next == NULL) {
			//Log("at end of list.\n");
			node->prev->next = NULL;
		} else {
			//Log("in middle of list.\n");
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
	}
	free(node);
	node = NULL;
}
void cleanupMeteors(void) 
{
	Meteor *s;
	while (meteorhead) {
		s = meteorhead->next;
		free(meteorhead);
		meteorhead = s;
	}
	meteorhead=NULL;
}

void checkMeteors()
{
	if (random(100) < 50) {
		createMeteors(ndrops);
	}
	//
	//move rain droplets
	Meteor *node = meteorhead;
	while (node) {
		//force is toward the ground
		node->vel[1] += gravity;
		VecCopy(node->pos, node->lastpos);
		node->pos[0] += node->vel[0] * timeslice;
		node->pos[1] += node->vel[1] * timeslice;
		if (fabs(node->vel[1]) > node->maxvel[1])
			node->vel[1] *= 0.96;
		node->vel[0] *= 0.999;
		node = node->next;
	}
	//check meteors
	int n=0;
	node = meteorhead;
	while (node) {
		if (node->pos[1] < -20.0f) {
			//rain drop is below the visible area
			Meteor *savenode = node->next;
			deleteMeteors(node);
			node = savenode;
			continue;
		}
		node = node->next;
	}
	if (maxrain < n)
		maxrain = n;
}

void drawMeteors()
{
	Meteor *node = meteorhead;
	while (node) {
		glPushMatrix();
		glTranslated(node->pos[0],node->pos[1],node->pos[2]);
		glColor4fv(node->color);
		glLineWidth(node->linewidth);
		glBegin(GL_LINES);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(0.0f, node->length);
		glEnd();
		glPopMatrix();
		node = node->next;
	}
	glLineWidth(1);
}
