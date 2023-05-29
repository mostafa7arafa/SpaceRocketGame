#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#define M_PI 3.14159265358979323846

int phyHeight = 700;
int phyWidth = 700;
int logWidth = 100;
int logHeight = 100;
int centerX = logWidth / 2;
int centerY = logHeight / 2;
float elapsedTime = 0.0f;
int remainingTime = 20;
int laserColorIndex = 0;
int score = 0;
bool gameOver = false;

void timer(int value);

int mouseX = centerX, mouseY = centerY;

struct Circle {
    float x, y;
    const float radius = 2.5f;
    const float speed = 1.0f;
    float delay;
    float r, g, b; // RGB color values
    bool active = true; // Indicates if the ball is alive
    int index;

    Circle(float _x, float _delay) : x(_x), y(logHeight+5), delay(_delay) {
        int colorIndex = rand() % 3; // Randomly select a color index (0, 1, or 2)

        // Set the color based on the selected index
        if (colorIndex == 0) {
            r = 1.0f; // Red
            g = 0.0f;
            b = 0.0f;
            index = 0;
        } else if (colorIndex == 1) {
            r = 0.0f; // Green
            g = 1.0f;
            b = 0.0f;
            index = 1;
        } else {
            r = 0.0f; // Blue
            g = 0.0f;
            b = 1.0f;
            index = 2;
        }
    }
};

struct Star {
    float x;
    float y;
};

std::vector<Star> stars;
std::vector<Circle> circles;

void init2D(float r, float g, float b) {
    glClearColor(r, g, b, 0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, logWidth, 0.0, logHeight);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
}

void createStars(int numStars, float width, float height) {
    stars.clear();

    for (int i = 0; i < numStars; ++i) {
        Star star;
        star.x = static_cast<float>(rand()) / RAND_MAX * width;
        star.y = static_cast<float>(rand()) / RAND_MAX * height;
        stars.push_back(star);
    }
}

void updateStars(float width, float height) {
    for (auto& star : stars) {
        star.y -= 4.0f; // Move the star downward
        // Wrap around the screen
        if (star.y < 0) {
            star.y = height;
            star.x = static_cast<float>(rand()) / RAND_MAX * width; // Generate a new random x-position
        }
    }
}

void Laser() {
    if (laserColorIndex == 0) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
    } else if (laserColorIndex == 1) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green
    } else if (laserColorIndex == 2) {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue
    }

    glBegin(GL_QUADS);
    glVertex2f(mouseX - 3, mouseY);
    glVertex2f(mouseX - 3, 100);
    glVertex2f(mouseX + 3, 100);
    glVertex2f(mouseX + 3, mouseY);
    glEnd();
}




void drawRocket() {
    // Draw the rocket with adjusted X position
    glColor3f(0.8, 0.6, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(mouseX - 3,mouseY - 3);
    glVertex2f(mouseX - 3,mouseY + 3);
    glVertex2f(mouseX + 3, mouseY + 3);
    glVertex2f(mouseX + 3, mouseY - 3);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(mouseX - 3,mouseY - 2);
    glVertex2f(mouseX - 3, mouseY + 1);
    glVertex2f(mouseX - 5, mouseY - 2);
    glVertex2f(mouseX - 3, mouseY - 2);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(mouseX + 3, mouseY - 2);
    glVertex2f(mouseX + 3, mouseY + 1);
    glVertex2f(mouseX + 5, mouseY - 2);
    glVertex2f(mouseX + 4, mouseY - 2);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex2f(mouseX - 3,mouseY + 3);
    glVertex2f(mouseX + 3, mouseY + 3);
    glVertex2f(mouseX, mouseY + 6);
    glEnd();


}

void printText(const char *str,int x, int y)
{
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2d(x,y);
    int length = strlen(str);
    for(int i=0; i<length; i++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
    glFlush();
}

void printTextFinal(const char *str,int x, int y)
{
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2d(x,y);
    int length = strlen(str);
    for(int i=0; i<length; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    glFlush();
}

void drawCircle(float x, float y, float radius, float r, float g, float b) {
    int numSegments = 100;
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= numSegments; i++) {
        float angle = i * 2.0f * M_PI / numSegments;
        float dx = radius * cos(angle);
        float dy = radius * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void updateCircles() {
    int currentTime = int(glutGet(GLUT_ELAPSED_TIME) / 1000.0f)%20; // Convert to seconds
    elapsedTime = currentTime;
    remainingTime = 20 - currentTime;


    for (auto& circle : circles) {
        if (currentTime >= circle.delay) {
            circle.y -= circle.speed;
        }
        if(circle.x == mouseX && circle.index == laserColorIndex && circle.y < logHeight && mouseY < circle.y)
        {
            circle.active = false;
        }
    }
}

void display(void) {

    if(gameOver)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_QUADS);
        glColor3f(0.0, 1.0, 0.0);
        glVertex2f(0,0);
        glVertex2f(100,0);
        glColor3f(0.0, 0.0, 0.0);
        glVertex2f(100, 100);
        glVertex2f(0, 100);
        glEnd();
        std::stringstream sss;
        sss << "Score: " << static_cast<int>(score);
        std::string score = sss.str();
        const char* scoreChar = score.c_str();
        printTextFinal(scoreChar, 46,50);
        printTextFinal("Press DEL Key to Restart", 34, 44);
    }
    else{
        glClear(GL_COLOR_BUFFER_BIT);
        Laser();
        drawRocket();
        glColor3f(1.0, 1.0, 1.0); // White color for stars
        glPointSize(1.0); // Adjust the size of stars as needed

        glBegin(GL_POINTS);  // Change this line
        for (const auto& star : stars) {
            glVertex2f(star.x, star.y);
        }
        glEnd();
        std::stringstream ss;
        ss << "Time: " << static_cast<int>(remainingTime);
        std::string timerText = ss.str();
        const char* timerTextChar = timerText.c_str();
        printText(timerTextChar, 88,97);

        int cFalse = 0;
        for(auto& circle : circles)
        {
            if(circle.active == false)
                cFalse++;
        }
        score = cFalse;

        std::stringstream sss;
        sss << "Score: " << static_cast<int>(score);
        std::string score = sss.str();
        const char* scoreChar = score.c_str();
        printText(scoreChar, 2,97);
        for (const auto& circle : circles) {
            if(circle.active == true)
                drawCircle(circle.x, circle.y, circle.radius, circle.r, circle.g, circle.b);
        }
    }

    glutSwapBuffers();
    glFlush();
}


void passiveMouse(int x, int y) {
    mouseX = x;
    mouseX = 0.5 + 1.0 * mouseX * logWidth / phyWidth;
    mouseY = phyHeight - y;
    mouseY=0.5+1.0*mouseY*logHeight/phyHeight;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 32) { // Spacebar key
        laserColorIndex = (laserColorIndex + 1) % 3; // Switch between 3 options (red, green, blue)
    }
    if(key == 127) { // Delete key
        elapsedTime = 0.0f;
        remainingTime = 20;
        laserColorIndex = 0;
        score = 0;
        gameOver = false;

        // Clear circles vector and reinitialize circles
        circles.clear();
        for (int i = 0; i < 25; i++) {
            float x = rand() % logWidth;
            float delay = static_cast<float>(rand()) / RAND_MAX * 15.0f; // Random delay between 0 and 20 seconds
            circles.emplace_back(x, delay);
        }

        glutTimerFunc(0, timer, 0);
    }
    glutPostRedisplay();
}

void timer(int value) {
    updateCircles();
    updateStars(logWidth, logHeight);

    if (elapsedTime >= 19.0f) {
        gameOver = true;
        glutPostRedisplay();

    } else {
        glutTimerFunc(16, timer, 0);
        glutPostRedisplay();
    }
}

int main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(phyWidth, phyHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Rocket");
    init2D(0.0, 0.0, 0.0);
    glutDisplayFunc(display);

    // Register the motion callback function
    glutPassiveMotionFunc(passiveMouse);
    glutKeyboardFunc(keyboard);

    // Initialize random seed
    srand(time(NULL));

    createStars(100, logWidth, logHeight);

    // Initialize circles
    for (int i = 0; i < 25; i++) {
        float x = rand() % logWidth;
        float delay = static_cast<float>(rand()) / RAND_MAX * 15.0f; // Random delay between 0 and 20 seconds
        circles.emplace_back(x, delay);
    }

    // Start the timer for circle movement
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
}
