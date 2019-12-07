//Source File for Avery Raines
//
//Four Steps of Debugging:
//-Identify the bug
//-Isolate the bug
//-Fix the bug
//-Review/Test for other bugs


#include "fonts.h"
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

int incrementScore(int score){
	return (score+=25);
}

/*int showCurrentScore(Rect rect, int score){
	rect.bot = 415;
	rect.left = 450;
	rect.center = 0;
	return (score);



}*/


