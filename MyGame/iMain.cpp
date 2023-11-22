# include "iGraphics.h"
# include <stdio.h>
# include <math.h>
# include <windows.h>



//Screen size and project name defined here
#define screenWidth 950
#define screenHeight 700
#define projectName "Stone the B'llon"


//some important constants and parameters
#define PI 3.14159265358979
#define g 98.1
#define numberOfBallons 7
#define MaxSpeed 200
#define maxBullets 100
#define ballonRadiusMax 60
#define ballonSpeedMax 4


//Game logics

//0-menu, 1-game, 2-help, 3-credits, 4-hall of fame
int gameStage = 0;//game stage is indicative to the phase of the application

double deltaCollisionTime = 10;
long long score=0;
char scoreText[100];


//text input
char name[1000]; 
int char_int = 0;


//game background, music and ui

//ui
char *ui[10] = {
	"image//ui.bmp",
	"image//ui_pg.bmp",
	"image//ui_i.bmp",
	"image//ui_c.bmp"
};
float uiLength = 441, uiHeight = 436;
int uiLayer = 0;
int background = 0, numOFBG=1;
//settings ui
double settingsX=30, settingsY=30;
char *set[] = {
	"image//settings.bmp",
	"image//sound-on.bmp",
	"image//sound-off.bmp",
	"image//music-on.bmp",
	"image//music-off.bmp",
	"image//background.bmp",
	"image//pause.bmp",
	"image//play.bmp"
};
char *set2[] = {
	"image//settings(2).bmp",
	"image//sound-on(2).bmp",
	"image//sound-off(2).bmp",
	"image//music-on(2).bmp",
	"image//music-off(2).bmp",
	"image//background(2).bmp",
	"image//pause(2).bmp",
	"image//play(2).bmp"
};
int settingsMode = 0;
int settingsOn = 0, soundOn = 0, musicOn = 0, pause = 0;


//creating my own time deviation
double oldTime = clock();
int prevSecond = 0;


//ballon structure and parameters
typedef struct Ballon{
	double x, y;
	float r;
	int cR, cG, cB;
	bool alive;
	float speed;
}Ballon;

double deltaBallonTime = 10;

Ballon ballons[numberOfBallons];


//bullet structure and parameters
typedef struct Bullet{
	double x;
	double y;
	double r=6;
	double vX, vY;
	double dx, dy;
	bool fired;
}Bullet;

int bulletCount = 0;
Bullet bullet[maxBullets];
long long deltaBulletTime = 10;


//bullet trajectory params
typedef struct currentTrajectory{
	double x, y;
	double vX, vY;
	double dx, dy;
	int show;
	double r = 1;
}currentTrajectory;

currentTrajectory ct;


//Launching projectile gun params and build logic
double r1 = 15, d = 12, l = 60, gunR=50;//I can literally change the structure of my launching gun with this one line
double theta = 0, deltheta = PI*(2) / 180;
double gunX = d/2, gunY = screenHeight / 2;
double alpha = asin(d / (r1 * 2));
double r2 = sqrt(pow(l + sqrt(pow(r1, 2) - pow(d / 2, 2)), 2) + pow(d / 2, 2));
double beta = asin(d / (2 * r2));



//Bullet Velocity gradient bar params and control logic
double lMax = l + sqrt(pow(r1, 2) - pow(d / 2, 2));
double lCurrent = lMax / 2;
double r2Bar = sqrt(pow(lCurrent, 2) + pow(d / 2, 2));
double betaSpeed = asin(d / (2 * r2Bar));
double deltaSpeed = 2;



//function iDraw() is called again and again by the system.
void iDraw() {
	long long deltaTime = clock() - oldTime;
	//printf("%lli\n", deltaTime);
	if ((clock() - oldTime) / 1000 > prevSecond)	prevSecond++;


	//place your drawing codes here     
	iClear();


	//placing the background layer
	iShowBMP(0, 0, "image//bg.bmp");


	if (gameStage == 1){

		//vertices of the desired polygon of the launching gun
		double polygonX[] = { gunX, gunX + r1*cos(theta - alpha), gunX + r2*cos(theta - beta), gunX + r2*cos(theta + beta), gunX + r1*cos(theta + alpha) };
		double polygonY[] = { gunY, gunY + r1*sin(theta - alpha), gunY + r2*sin(theta - beta), gunY + r2*sin(theta + beta), gunY + r1*sin(theta + alpha) };



		//shooter module
		iSetColor(0, 0, 0);
		iFilledCircle(0, screenHeight / 2, gunR);
		iSetColor(10, 10, 10);
		iCircle(0, screenHeight / 2, gunR);
		iSetColor(255, 255, 255);



		//Laucnhing gun module
		iSetColor(255, 255, 255);
		iFilledPolygon(polygonX, polygonY, 5);
		iSetColor(0, 255, 0);
		iPolygon(polygonX, polygonY, 5);



		//bullet speed module
		double bulletSpeedBarX[] = { gunX, gunX + r1*cos(theta - alpha), gunX + r2Bar*cos(theta - betaSpeed), gunX + r2Bar*cos(theta + betaSpeed), gunX + r1*cos(theta + alpha) };
		double bulletSpeedBarY[] = { gunY, gunY + r1*sin(theta - alpha), gunY + r2Bar*sin(theta - betaSpeed), gunY + r2Bar*sin(theta + betaSpeed), gunY + r1*sin(theta + alpha) };
		iSetColor(0, 255, 0);
		iFilledPolygon(bulletSpeedBarX, bulletSpeedBarY, 5);
		iSetColor(255, 255, 255);
		iPolygon(bulletSpeedBarX, bulletSpeedBarY, 5);



		//bullet trajectory
		if (ct.show){
			ct.x = gunX + (lMax - bullet[bulletCount].r)*cos(theta);
			ct.y = gunY + (lMax - bullet[bulletCount].r)*sin(theta);
			double v = (lCurrent / lMax)*MaxSpeed;
			ct.vX = v*cos(theta);
			ct.vY = v*sin(theta);
			ct.dx = (ct.vX / deltaBulletTime);
			ct.dy = (ct.vY / deltaBulletTime);
			while (ct.y >= 0 && ct.x <= screenWidth){
				ct.x += ct.dx;
				ct.vY -= g*deltaBulletTime / 1000;
				ct.dy = ct.vY / deltaBulletTime;
				ct.y += ct.vY / deltaBulletTime;
				iSetColor(200, 200, 200);
				iPoint(ct.x, ct.y, ct.r);
			}
		}

		//bullet module
		for (int i = 0; i < maxBullets; i++){
			if (bullet[i].fired){
				iSetColor(0, 0, 0);
				iFilledCircle(bullet[i].x, bullet[i].y, 1.3*bullet[i].r);
				iSetColor(255, 0, 0);
				iFilledCircle(bullet[i].x, bullet[i].y, bullet[i].r);
				if (bullet[i].y<0 || bullet[i].x>screenWidth){
					bullet[i].fired = 0;
				}
			}
		}



		//score module
		sprintf_s(scoreText, 100, "Score: %i", score);
		iSetColor(255, 255, 255);
		iText(50, screenHeight - 30, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

	}



	//ballon module
	for (int i = 0; i < numberOfBallons; i++){
		if (ballons[i].alive){
			iSetColor(0, 0, 0);
			iFilledCircle(ballons[i].x, ballons[i].y, ballons[i].r*1.125);
			iSetColor(ballons[i].cR, ballons[i].cG, ballons[i].cB);
			iFilledCircle(ballons[i].x, ballons[i].y, ballons[i].r);
		}
	}


	//ui layer
	if (gameStage == 0){
		iShowBMP2(screenWidth / 2 - uiLength / 2, screenHeight / 2 - uiHeight / 2, ui[uiLayer], -1);

	}

	//HIGH SCORE MODULE...HALL OF FAME
	iSetColor(255, 255, 255);
	iText(screenWidth / 2, screenHeight / 2, name, GLUT_BITMAP_TIMES_ROMAN_24);



	//settings menu
	char *s[5] = { set[0],set[1],set[3],set[5],set[6]};

	if (soundOn)	s[1] = set[2];
	else   s[1] = set[1];

	if (musicOn)	s[2] = set[4];
	else   s[2] = set[3];

	if (pause)	s[4] = set[7];
	else   s[4] = set[6];

	if (settingsOn && settingsMode == 0)	s[0] = set2[0];
	switch (settingsMode){
	case 1:
		if(soundOn)	s[1] = set2[2];
		else   s[1] = set2[1];
		break;
	case 2:
		if (musicOn)	s[2] = set2[4];
		else   s[2] = set2[3];
		break;
	case 3:
		s[3] = set2[5];
		break;
	case 4:
		if (pause)	s[4] = set2[7];
		else   s[4] = set2[6];
		break;
	}

	iShowBMP(settingsX, settingsY, s[0]);
	if (settingsOn){
		iShowBMP(settingsX + 55, settingsY, s[1]);
		iShowBMP(settingsX + 110, settingsY, s[2]);
		iShowBMP(settingsX + 165, settingsY, s[3]);
		iShowBMP(settingsX + 220, settingsY, s[4]);
	}

}



///function iMouseMove() is called when the user presses and drags the mouse.
//(mx, my) is the position where the mouse pointer is.
void iMouseMove(int mx, int my) {
	//printf("x = %d, y= %d\n", mx, my);
}



//function iMouse() is called when the user presses/releases the mouse.
//(mx, my) is the position where the mouse pointer is.
void iMouse(int button, int state, int mx, int my) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		if (gameStage == 0){
			printf("%i %i\n", mx, my);
			if (mx >= 345 && mx < 615 && my >= 345 && my <= 385){
				uiLayer = 1;
				gameStage = 1;
			}
			else if (mx >= 345 && mx < 615 && my >= 270 && my <= 310){
				uiLayer = 2;
			}
			else if (mx >= 345 && mx < 615 && my >= 195 && my <= 235){
				uiLayer = 3;
			}
			else{
				uiLayer = 0;
			}
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (mx>settingsX && mx<settingsX + 50 && my>30 && my < settingsY + 50){
			settingsOn = 1 - settingsOn;
			settingsMode = 0;
		}
	}

}



//function iKeyboard() is called whenever the user hits a key in keyboard.
//key- holds the ASCII value of the key pressed.
void iKeyboard(unsigned char key) {
	if (key == 'q') {
		exit(0);
	}
	if (gameStage == 1){
		if (key == 13 || key== 32){
			//printf("..\n");
			while (bullet[++bulletCount].fired);
			if (bulletCount >= maxBullets)	bulletCount = 0;
			bullet[bulletCount].fired = 1;
			PlaySound("music\\bulletShot.wav", NULL, SND_ASYNC);
			bullet[bulletCount].x = gunX + (lMax - bullet[bulletCount].r)*cos(theta);
			bullet[bulletCount].y = gunY + (lMax - bullet[bulletCount].r)*sin(theta);
			double v = (lCurrent / lMax)*MaxSpeed;
			bullet[bulletCount].vX = v*cos(theta);
			bullet[bulletCount].vY = v*sin(theta);
			bullet[bulletCount].dx = (bullet[bulletCount].vX / deltaBulletTime);
			bullet[bulletCount].dy = (bullet[bulletCount].vY / deltaBulletTime);
			bulletCount++;
		}

		if (key == 'm'){
			ct.show = 1 - ct.show;
		}
	}

	//registering the setting call
	if (key == 's'){
		settingsOn = 1 - settingsOn;
		settingsMode = 0;
	}
	if (settingsOn){
		if (key == 13){
			switch (settingsMode){
			case 1:
				soundOn = 1 - soundOn;
				break;
			case 2:
				musicOn = 1 - musicOn;
				break;
			case 3:
				background = (background + 1);
				break;
			case 4:
				pause = 1 - pause;
				break;
			}
		}
	}
	//text input
	//if (gameStage == 0){
	//	name[char_int++] = key;
	//}

}



//function iSpecialKeyboard() is called whenver user hits special keys like-
//function keys, home, end, pg up, pg down, arraows etc. you have to use
//appropriate constants to detect them. A list is:
//GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
//GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
//GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
//GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
void iSpecialKeyboard(unsigned char key) {

	if (key == GLUT_KEY_END) {
		exit(0);
	}

	//settings clickers
	if (settingsOn){
		if (key == GLUT_KEY_RIGHT){
			settingsMode = (settingsMode + 1) % 5;
		}
		if (key == GLUT_KEY_LEFT){
			if (settingsMode == 0)	settingsMode = 4;
			else    settingsMode = (settingsMode - 1);
		}
	}

	//gameplay moves
	if (gameStage == 1){

		//Controlling the launching angle
		if (key == GLUT_KEY_UP && theta<PI / 2){
			theta += deltheta;
		}
		if (key == GLUT_KEY_DOWN && theta>-PI / 2){
			theta -= deltheta;
		}

		//Controlling the launching velocity
		if (key == GLUT_KEY_RIGHT && lCurrent < lMax){
			lCurrent += deltaSpeed;
			r2Bar = sqrt(pow(lCurrent, 2) + pow(d / 2, 2));
			betaSpeed = asin(d / (2 * r2Bar));
		}
		if (key == GLUT_KEY_LEFT && lCurrent>sqrt(pow(r1, 2) - pow(d / 2, 2))){
			lCurrent -= deltaSpeed;
			r2Bar = sqrt(pow(lCurrent, 2) + pow(d / 2, 2));
			betaSpeed = asin(d / (2 * r2Bar));
		}
	}


	if (gameStage == 0){
		if (key == GLUT_KEY_DOWN){
			uiLayer=(uiLayer+1)%4;
		}
		if (key == GLUT_KEY_UP){
			if (uiLayer)	uiLayer--;
			else    uiLayer = 3;
		}
	}
	//editing input

}



//bullet movement update
void bulletMovement(){
	for (int i = 0; i < maxBullets; i++){
		if (bullet[i].fired){
			bullet[i].x += bullet[i].dx;
			bullet[i].vY -= g*deltaBulletTime / 1000;
			bullet[i].dy = bullet[i].vY / deltaBulletTime;
			bullet[i].y += bullet[i].vY / deltaBulletTime;
			iFilledCircle(bullet[i].x, bullet[i].y, bullet[i].r);
			printf("%g\n", bullet[i].y);
			if (bullet[i].y<0 || bullet[i].x>screenWidth){
				bullet[i].fired = 0;
			}
		}
	}
}



//ballon movement update
void ballonMovement(){
	for (int i = 0; i < numberOfBallons; i++){
		if (!ballons[i].alive){
			ballons[i].x = screenWidth / 3 + i * 2 * screenWidth / (3 * numberOfBallons) + rand() % (long long)((2 * screenWidth / 3)/ numberOfBallons - 2*ballons[i].r) + ballons[i].r;
			ballons[i].y = -50-300*(2+rand()%numberOfBallons);
			ballons[i].cR = rand() % 255; ballons[i].cG = rand() % 255; ballons[i].cB = rand() % 255;
			ballons[i].alive = 1;
			ballons[i].r = ballonRadiusMax / (1 + rand() % 4);
			ballons[i].speed = ballonSpeedMax*ballons[i].r / ballonRadiusMax;
			//printf("%g %g %i\n", ballons[i].r, ballons[i].speed, i);
		}
		else{
			ballons[i].y += ballons[i].speed;
		}
		if (ballons[i].y>screenHeight+ballons[i].r){
			ballons[i].alive = 0;
		}
	}
}



//collision detection
void collision(){
	for (int i = 0; i < numberOfBallons; i++){
		if (ballons[i].alive){
			for (int j = 0; j < maxBullets; j++){
				double distance = pow(ballons[i].x - bullet[j].x, 2) + pow(ballons[i].y - bullet[j].y, 2);
				if (distance < pow(ballons[i].r + bullet[j].r, 2) && bullet[j].fired){
					PlaySound("music\\ballonBurst.wav", NULL, SND_ASYNC);
					score++;
					ballons[i].alive = 0;
					bullet[j].fired = 0;
					bullet[j].x = gunX + (lMax - bullet[j].r)*cos(theta);
					bullet[j].y = gunY + (lMax - bullet[j].r)*sin(theta);
				}
			}
		}
	}
}



int main() {
	//place your own initialization codes here.
	iSetTimer(deltaBulletTime, bulletMovement);
	iSetTimer(deltaBallonTime, ballonMovement);
	iSetTimer(deltaCollisionTime, collision);
	iInitialize(screenWidth, screenHeight, projectName);
	return 0;
}
