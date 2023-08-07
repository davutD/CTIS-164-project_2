/*********
   CTIS164 - Template Source Program
----------
STUDENT : DAVUT DURMAZ
SECTION : 164 - 04
HOMEWORK: 2ND CTIS 164 HOMEWORK
----------
PROBLEMS: NO PROBLEMS AT ALL - ALL BASE REQUIREMENTS HAVE BEEN MET
----------
ADDITIONAL FEATURES:
    1) GRADIENT BACKGROUND
    2) HEALTH BAR FOR TARGETS. THEREFORE, BARREL MUST FIRE MORE THAN 1 TIME TO KILL THE TARGET SUCCESSFULY. HEALTH BAR OF TARGETS IS RANDOMLY CREATED BY A FUNCTION
    3) THERE IS A RANDOMLY CREATED ITEM ON THE SCREEN, WHICH INCREASE HEALTH OF TARGET, IF PLAYER CLICKS ON THE ITEM ON A RIGHT POSITION. AND NUMBER OF SUCCESSFUL CLICK IS COUNTED
    4) THERE ARE TWO DIFFERENT MODES. FIRST ONE IS NORMAL MODE RESTRICTED WITH A 60 SECONDS AND SECOND ONE IS "RACE AGAINST TIME MODE". IN SECOND, WHENEVER PLAYER CLICKS ON HEALTH ITEMS FOR EACH SUCCESSFUL CLICK, HEALTH BAR INCREASES AND COUNTING TIME DECREASES 0.5 SECOND. IN THIS MODE FIRE SPEED IS FASTER THAN NORMAL MODE.

*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<time.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 700

#define TIMER_PERIOD 25 // Period for the timer.
#define TIMER_ON 1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define PI 3.14159265

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

double val = 180.0 / PI; // 1/D2R

/* User defined Types and Variables */
typedef struct {
    double x, y;
} point_t;

typedef struct {
    point_t start, end;
    float m, b;
    float dx, dy;
} line_t;

typedef struct {
    point_t pos;    // position of the object
    double   angle;  // view angle 
    float   r;
} player_t;

typedef struct {
    point_t pos;
    float angle;
    bool active;
} bullet_t;

typedef struct {
    int r, g, b;
} color_t;

typedef struct {
    point_t center;
    float r;
    color_t color;
} target_t;

int r, g, b; //VARIABLES REPRESENTING RANDOM RGB COLORS
int trueAngle = 1; //TO DETERMINE WHETHER BARREL MATHCHES WITH THE ANGLE OF THE SHAPE
double timer = 0; //TIMER VARIBLE FOR 60 MINS GAME 
int score = 0; //VARIBALE TO SHOW SUCCESSFUL KILLINGS OF COMPUTER
int targetHealth; //VARIBLE FOR TARGET HEALTH DEPENDING ON RANDOMLY CREATED NUMBERS
int counter = 0; //COUNTER FOR PER SUCCESSFUL HIT OF PLAYER TO THE TARGET 
bool start = 0; //VARIABLE FOR STARTING THE GAME WHENEVER PUSHED THE SPACE BUTTON
int xPos, yPos; //POSITIONS FOR MOUSE'S COORDS
bool click = false; //VARIABLE FOR WHETHER MOUSE CLICKED OR NOT
int healthCount = 0; //TO COUNT HEALTH ITEM SUCCESSFULY GET BY CLICKING ON A SPECIFIC AREA
bool mode = false; //TO DETERMINE TWO DIFFERENT GAME MODES
int fireSpeed = 27; //BULLET SPEEDS

player_t player = { {0,0}, 45, 70 };  //FOR THE AUTONOMOUS PLAYER
target_t target = { {200,200},40 };  //FOR THE TARGET SHAPE
line_t line; //FOR THE PROPERTIES OF LINE IF IT IS USED
player_t barrel = { {},90 }; //FOR THE TURNING BARREL OF THE PLAYER 
bullet_t bullet; //FOR PROPERTIES OF BULLET FIRED AND MOVING ALONG ANGLE OF BARREL UNTIL IT HITS TARGET 
target_t health = { {-200,-200},20 }; //FOR PROPERTIES OF BONUS HEALTH ITEM

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void resetTarget() // function to reset location of target randomly
{
    target.center.y = rand() % 550 - 300;
    target.center.x = rand() % 1000 - 560;
    targetHealth = rand() % 3 + 1;

    if (target.center.y<200 && target.center.y>-200 && target.center.x < 200 && target.center.x>-200)
        resetTarget();
    target.color.r = rand() % 256;
    target.color.g = rand() % 256;
    target.color.b = rand() % 256;
}
void resetHealthPos() //fuction to reset location of health item randomly 
{
    health.center.y = rand() % 550 - 300;
    health.center.x = rand() % 1000 - 560;

    if (health.center.y<200 && health.center.y>-200 && health.center.x < 200 && health.center.x>-200)
        resetHealthPos();
}

line_t calculateLine(line_t line, target_t target) // function to calculate line between target and player
{

    resetTarget();

    //delta(diff) calculations
    line.dx = target.center.x - line.start.x;
    line.dy = target.center.y - line.start.y;



    //slope Intercept Form
    if (line.dx != 0) { //if not a vertical line
        line.m = line.dy / line.dx;
        line.b = -line.m * line.start.x + line.start.y;
    }
    return line;
}

void objectFiring(player_t player) // function to draw player
{
    glColor3ub(204, 255, 153);
    circle(player.pos.x - player.r / sqrt(2), player.pos.y + player.r / sqrt(2), player.r - 40);
    circle(player.pos.x - player.r / sqrt(2), player.pos.y - player.r / sqrt(2), player.r - 40);
    circle(player.pos.x + player.r / sqrt(2), player.pos.y + player.r / sqrt(2), player.r - 40);
    circle(player.pos.x + player.r / sqrt(2), player.pos.y - player.r / sqrt(2), player.r - 40);

    glColor3ub(0, 102, 102);
    circle(player.pos.x, player.pos.y, player.r);

    glColor3ub(204, 102, 0);
    glRectf(player.pos.x - player.r / sqrt(2), player.pos.y + player.r / sqrt(2), player.pos.x + player.r / sqrt(2), player.pos.y - player.r / sqrt(2));

    glColor3ub(0, 204, 0);
    glLineWidth(20);
    glBegin(GL_LINES);
    glVertex2f(player.pos.x, player.pos.y);
    glVertex2f(barrel.pos.x + 100 * cos(barrel.angle * D2R), barrel.pos.y + 100 * sin(barrel.angle * D2R));
    glEnd();

    glColor3ub(51, 153, 255);
    circle(player.pos.x, player.pos.y, player.r - 40);

}

void targetShape(target_t target) // function to draw target shape changing its location randomly
{

    glColor3ub(target.color.r, target.color.g, target.color.b);
    circle(target.center.x, target.center.y, target.r);

    glColor3f(1, 0, 0);
    glRectf(target.center.x - 10 * (targetHealth + 1), target.center.y + 45, target.center.x + 10 * (targetHealth + 1), target.center.y + 55);
    glColor3f(0, 1, 0);
    glRectf(target.center.x - 10 * (targetHealth + 1 - counter), target.center.y + 45, target.center.x + 10 * (targetHealth + 1 - counter), target.center.y + 55);

    glColor3ub(255, 255, 255);
    circle(target.center.x + 20, target.center.y + 30, target.r - 30);
    circle(target.center.x - 20, target.center.y + 30, target.r - 30);

    glColor3ub(0, 0, 0);
    circle(target.center.x + 22, target.center.y + 25, target.r - 38);
    circle(target.center.x - 22, target.center.y + 35, target.r - 38);

    glColor3ub(255, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(target.center.x, target.center.y);
    glVertex2f(target.center.x + 5, target.center.y + 5);
    glVertex2f(target.center.x, target.center.y + 20);

    glVertex2f(target.center.x, target.center.y);
    glVertex2f(target.center.x - 5, target.center.y + 5);
    glVertex2f(target.center.x, target.center.y + 20);
    glEnd();

    glColor3ub(102, 204, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(target.center.x + 5, target.center.y - 10);
    glVertex2f(target.center.x + 20, target.center.y - 20);
    glVertex2f(target.center.x, target.center.y - 20);

    glVertex2f(target.center.x - 5, target.center.y - 10);
    glVertex2f(target.center.x - 20, target.center.y - 20);
    glVertex2f(target.center.x, target.center.y - 20);

    glVertex2f(target.center.x, target.center.y - 25);
    glVertex2f(target.center.x - 20, target.center.y - 20);
    glVertex2f(target.center.x + 20, target.center.y - 20);
    glEnd();

    glColor3f(1, 1, 0);
    glPointSize(3);
    glBegin(GL_POINTS);
    glVertex2f(target.center.x + 20, target.center.y + 10);
    glVertex2f(target.center.x + 30, target.center.y + 5);
    glVertex2f(target.center.x + 20, target.center.y + 0);

    glColor3f(0, 1, 1);
    glVertex2f(target.center.x - 20, target.center.y + 10);
    glVertex2f(target.center.x - 30, target.center.y + 5);
    glVertex2f(target.center.x - 20, target.center.y + 0);
    glEnd();
}

void drawBullet() // function to draw bullet
{

    glColor3f(1, 1, 0);
    circle(bullet.pos.x, bullet.pos.y, 7);
}


void drawBackground() // function to draw background
{
    glColor3ub(0, 204, 102);
    glRectf(-600, 350, 600, 310);

    glColor3ub(255, 255, 0);
    glRectf(-600, 350, -340, 310);

    glColor3ub(0, 0, 0);
    vprint(-580, 325, GLUT_BITMAP_HELVETICA_12, "TIMER: %0.2f", timer - 0.025);
    vprint(-495, 325, GLUT_BITMAP_HELVETICA_12, "SCORE: %d", score);
    vprint(-425, 325, GLUT_BITMAP_HELVETICA_12, "HEALTH: %d", targetHealth + 1 - counter);
    vprint(400, 325, GLUT_BITMAP_HELVETICA_12, "DAVUT DURMAZ / 21903291");
    glColor3ub(255, 0, 0);
    vprint(-320, 323, GLUT_BITMAP_HELVETICA_18, "PRESS F1 TO CHANGE THE MODE");
    vprint(40, 323, GLUT_BITMAP_HELVETICA_18, "MODE: ");

}
/* Draw the Line Segment with a red color from Start to End  */
void showLineSegment() // function to draw a line between target and player if it is necessary
{
    glColor3f(1, 0.3, 0.3);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(line.start.x, line.start.y);
    glVertex2f(target.center.x, target.center.y);
    glEnd();
    glLineWidth(1);
}

void bonusHealth(target_t health) // bonus health item giving bonus health if clicked on it successfully
{
    glColor3ub(r, g, b);
    vprint(health.center.x - 50, health.center.y + 25, GLUT_BITMAP_HELVETICA_18, "Bonus Health");
    glColor3ub(g, r, b);
    vprint(health.center.x - 8, health.center.y - 38, GLUT_BITMAP_HELVETICA_18, "%d", healthCount);
    glColor3ub(255, 0, 0);
    circle(health.center.x, health.center.y, health.r);
    glColor3ub(255, 255, 255);
    glRectf(health.center.x - 20, health.center.y + 20, health.center.x + 20, health.center.y - 20);

    glColor3ub(255, 0, 0);
    glLineWidth(10);
    glBegin(GL_LINES);
    glVertex2f(health.center.x - 20, health.center.y);
    glVertex2f(health.center.x + 20, health.center.y);
    glVertex2f(health.center.x, health.center.y + 20);
    glVertex2f(health.center.x, health.center.y - 20);
    glEnd();
}
//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0.7, 0.3, 0.9, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3ub(255, 128, 0);
    glBegin(GL_POLYGON);
    glVertex2f(-600, 350);
    glColor3ub(255, 153, 51);
    glVertex2f(-600, -350);

    glColor3ub(0, 255, 255);
    glVertex2f(600, -350);
    glColor3ub(255, 51, 255);
    glVertex2f(600, 350);

    glEnd();

    if (start == false)
    {
        glColor3ub(0, 204, 102);
        glRectf(-600, 350, 600, 310);

        glColor3ub(255, 0, 0);
        vprint(-110, 325, GLUT_BITMAP_HELVETICA_18, "PRESS SPACE TO START");

        objectFiring(player);
        vprint(-10, -5, GLUT_BITMAP_9_BY_15, "%0.0f", fabs(barrel.angle));

    }
    else
    {
        bonusHealth(health);
        drawBullet();

        objectFiring(player);
        targetShape(target);

        glColor3b(1, 0, 0);
        if (barrel.angle < 0)
            vprint(-10, -5, GLUT_BITMAP_9_BY_15, "%0.0f", 360 + barrel.angle);
        else
            vprint(-10, -5, GLUT_BITMAP_9_BY_15, "%0.0f", fabs(barrel.angle));

        drawBackground();

        glColor3ub(0, 102, 204);
        if (mode == false)
            vprint(110, 323, GLUT_BITMAP_HELVETICA_18, "NORMAL");
        else
            vprint(110, 323, GLUT_BITMAP_HELVETICA_18, "RACE AGAINST TIME");

    }


    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == 32)
        start = true;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    if (GLUT_KEY_F1)
        mode = !mode;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
    {
        click = true;
        xPos = x - WINDOW_WIDTH / 2;
        yPos = WINDOW_HEIGHT / 2 - y;
    }
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_UP)
        click = false;


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void turnPlayer(player_t* barrel, float turn) //function to turn angle of the barrel
{
    barrel->angle += turn;
    if (barrel->angle < -180)
        barrel->angle = 180;
    if (barrel->angle > 180)
        barrel->angle = -180;
}

double calculateDistance(bullet_t bullet, target_t target) // function to calculate distance between two points
{
    double distance;
    distance = sqrt((target.center.x - bullet.pos.x) * (target.center.x - bullet.pos.x) + (target.center.y - bullet.pos.y) * (target.center.y - bullet.pos.y));

    return (distance);
}

void randomRgb() // to create random rgb colors for bonus health item
{
    r = rand() % 255 + 1;
    g = rand() % 255 + 1;
    b = rand() % 255 + 1;
}

void modeFunction() // function to change options depending on the mode determined by the user
{
    if (mode == false)
        if (sqrt((health.center.x - xPos) * (health.center.x - xPos) + (health.center.y - yPos) * (health.center.y - yPos)) <= health.r && click == true)
        {
            fireSpeed = 27;
            counter = counter - 2;
            healthCount++;
            resetHealthPos();
        }
    if (mode == true)
        if (sqrt((health.center.x - xPos) * (health.center.x - xPos) + (health.center.y - yPos) * (health.center.y - yPos)) <= health.r && click == true)
        {
            fireSpeed = 37;
            counter = counter - 2;
            healthCount++;
            resetHealthPos();
            timer = timer - 0.5;
        }
}

void fireBullet() // function to fire bullets
{
    if (trueAngle == 1 && calculateDistance(bullet, target) != 0)
    {
        bullet.pos.x += fireSpeed * cos(barrel.angle * D2R);
        bullet.pos.y += fireSpeed * sin(barrel.angle * D2R);
    }

    if (trueAngle == 1 && calculateDistance(bullet, target) <= target.r)
    {
        if (counter != targetHealth)
        {
            counter++;
            bullet.pos.x = 0;
            bullet.pos.y = 0;
        }
        else
        {
            resetTarget();
            counter = 0;
            score++;
            bullet.pos.x = 0;
            bullet.pos.y = 0;
        }
    }
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    randomRgb();

    if (start == true) // if statement to start the game
        if (timer < 60) // timer counting 60 min game time
        {
            trueAngle = 1;

            if (atan2(target.center.y, target.center.x) * val < barrel.angle - 2) // if check statement to check whether barrel angle is greater or not
            {
                trueAngle = 0;
                if (fabs(atan2(target.center.y, target.center.x) * val - barrel.angle) < 180)
                    turnPlayer(&barrel, -2);
                if (fabs(atan2(target.center.y, target.center.x) * val - barrel.angle) > 180)
                    turnPlayer(&barrel, 2);
            }
            if (atan2(target.center.y, target.center.x) * val - 2 > barrel.angle) // if check statement to check whether target angle is greater or not
            {
                trueAngle = 0;
                if (fabs(atan2(target.center.y, target.center.x) * val - barrel.angle) < 180)
                    turnPlayer(&barrel, 2);
                if (fabs(atan2(target.center.y, target.center.x) * val - barrel.angle) > 180)
                    turnPlayer(&barrel, -2);
            }

            modeFunction(); // to change game mode
            fireBullet(); // to fire bullets to the randomly created target
            timer = timer + 0.025;
        }



    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("2nd 164 Homework  //  Davut Durmaz");

    srand(time(NULL));

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}