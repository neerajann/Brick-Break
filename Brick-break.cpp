// download all the game audios and have them all at same location as of project file


#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <cstdio>
#include <cstring>
#include <windows.h>
#include <algorithm>
#include <mmsystem.h> // For sound -lwinmm


// Game configuration constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BRICK_ROWS 4
#define BRICK_COLUMNS 10 
#define NUMBER_OF_PARTICLES 100

// Global game state variables
int paddleWidth = 100;
int paddleHeight = 10;
int paddleX = WINDOW_WIDTH / 2 - paddleWidth / 2;  // Paddle X position
int paddleY = 50;  // Paddle Y position, fixed
int ballRadius = 10;  // Radius of the ball
int ballX = WINDOW_WIDTH / 2;  // Ball X position, starts at center
int ballY = WINDOW_HEIGHT / 2+100;  // Ball Y position, starts at center
int ballDX = 4;  // Ball velocity in X direction
int ballDY = -4;  // Ball velocity in Y direction
int paddleSpeed=25; // Paddle speed
bool bricks[BRICK_ROWS][BRICK_COLUMNS];   // Bricks layout
int score = 0;  // Player's score
int countdownTime = 3;  // Countdown timer for game start
int brickWidth = 78;
int brickHeight = 30; 
int marginSize = 3;


// Game state enumeration for managing different screens
enum GameState {
    START_SCREEN,
    COUNTDOWN,
    IN_GAME,
    GAME_OVER,
    INSTRUCTIONS_SCREEN,
    GAME_WON
};

GameState currentState = START_SCREEN;  // Initial game state

struct Particle { //Error on particle movement skip particles for now
    float x, y;     // Position
    float dx, dy;   // Velocity
};	
Particle particles[NUMBER_OF_PARTICLES];
	
// Function prototypes for game functionality
void playSound(const char* filename);
void initializeBricks();
void initializeParticles();
void drawPaddle();
void drawBall();
void drawCountdown();
void drawBricks();
void startGameTimer(int value);
void goToStartScreen();
void startGame();
void updateCountdown(int value);
void onStartGameButtonClicked();
void drawStartScreen();
void drawVictoryScreen();
void drawInstructionsScreen();
void drawGameOverScreen();
void drawScore();
void restartGame();
void display();
void checkGameOver();
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void update(int value);
void updateParticlesTimer(int value);
void reshape(int w, int h);

// Main function: Initializes the game and starts the main loop
int main(int argc, char** argv) {
    glutInit(&argc, argv);  // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  // Set display mode
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); 
    glutCreateWindow("Brick Break");  

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // Set background color

    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    initializeBricks();
	initializeParticles();
	
	playSound("game_music.wav"); //music for start screen and instruction screen
	
	glutTimerFunc(16, updateParticlesTimer, 0);
	
    glutMainLoop();  // Start the main GLUT loop

    return 0; 
}

void playSound(const char* filename) {
    PlaySound(TEXT(filename), NULL, SND_ASYNC);
}
void initializeBricks() {
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLUMNS; j++) {
            bricks[i][j] = true;
        }
    }
}

void initializeParticles() {
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        particles[i].x = rand() % WINDOW_WIDTH;
        particles[i].y = rand() % WINDOW_HEIGHT;
        particles[i].dx = (float(rand()) / RAND_MAX) * 1 - 0.5;  // Adjust velocity range as needed
        particles[i].dy = (float(rand()) / RAND_MAX) * 1 - 0.5;
    }
}

void drawStartScreen() {
    char msg[50];// Buffer to store the message
	int i; 
    // Draw the "Brick Break" title
    glColor3f(1.0f, 1.0f, 1.0f); 
    glRasterPos2i(320, 489);
    const char *title = "BRICK BREAK";
    for ( i = 0; i < strlen(title); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, title[i]);
    }

    // Draw the "Start Game" button
    glColor3f(49/255.0f, 176/255.0f, 107/255.0f); 
    glRecti(307, 323, 307 + 186, 323 + 51);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(360, 342);
    sprintf(msg, "Start Game");
    for (i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg[i]);
    }

    // Draw the "Instructions" button
    glColor3f(160/255.0f, 102/255.0f, 102/255.0f); 
    glRecti(307, 263, 307 + 186, 263 + 51); 
    glColor3f(1.0f, 1.0f, 1.0f); 
    glRasterPos2i(353, 285);
    sprintf(msg, "Instructions");
    for (i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg[i]);
    }

    
    glColor3f(1.0f, 1.0f, 1.0f); 
    glRasterPos2i(300, 53); 
    sprintf(msg, "Developed By Team .blend");
    for (int i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, msg[i]);
    }
}

void drawParticles() {
    glBegin(GL_POINTS);
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        glVertex2f(particles[i].x, particles[i].y);
    }
    glEnd();
}

void updateParticles() {
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        particles[i].x += particles[i].dx;
        particles[i].y += particles[i].dy;

        // Bounce off the edges of the window
        if (particles[i].x <= 0 || particles[i].x >= WINDOW_WIDTH) particles[i].dx *= -1;
        if (particles[i].y <= 0 || particles[i].y >= WINDOW_HEIGHT) particles[i].dy *= -1;
    }
}

void updateParticlesTimer(int value) {
    updateParticles(); // Update particle positions
    glutPostRedisplay(); // Redraw the screen to reflect particle movement
    glutTimerFunc(16, updateParticlesTimer, 0); // Continue updating particles
}

void drawInstructionsScreen() {
    char msg[150]; // Buffer to store the message

    // Draw "INSTRUCTION" title at the new position
     glColor3f(220/255.0f, 203/255.0f, 116/255.0f); // Color code #DCCB74
    glRasterPos2i(300, WINDOW_HEIGHT - 72); // Position remains the same
    sprintf(msg, "INSTRUCTION");
    for (int i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, msg[i]); // Larger font
    }

    // Instruction 1
    glColor3f(1.0f, 1.0f, 1.0f); // Text color (white for instructions)
    glRasterPos2i(80, WINDOW_HEIGHT - 207);
    sprintf(msg, "1. Move the paddle using the left and right arrow keys.");
    for (int i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
    }

    // Instruction 2
    glRasterPos2i(80, WINDOW_HEIGHT - 261);
    sprintf(msg, "2. Try to make the ball land on the paddle.");
    for (int i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
    }

    // Instruction 3
    glRasterPos2i(80, WINDOW_HEIGHT - 315);
    sprintf(msg, "3. Clear all the bricks to win.");
    for (int i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
    }
    
	glColor3f(87/255.0f, 110/255.0f, 192/255.0f); // Adjust the color as needed
    glRecti(310, 150, 310 + 129, 150 + 51); //Back Button

    // Center the "Back" text on the button
    const char *backText = "BACK";
    glColor3f(1.0f, 1.0f, 1.0f); // White

    // Position and draw the text
    glRasterPos2i(348, 170);
    for (const char* c = backText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

}

void onStartGameButtonClicked() {
    currentState = COUNTDOWN;
    countdownTime = 3;
    playSound(NULL); // Ensure no background music is playing
    glutTimerFunc(1000, updateCountdown, 0);
}

void drawCountdown() {
    char countdownMsg[50];
    glColor3f(1.0f, 1.0f, 1.0f);

    // Position the countdown message in the middle of the screen
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);

    sprintf(countdownMsg, "Starting in %d...", countdownTime);
    for (int i = 0; i < strlen(countdownMsg); i++) {
   		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, countdownMsg[i]); 
    }
}


void updateCountdown(int value) {
    if (currentState == COUNTDOWN && countdownTime > 0) {
    if (countdownTime > 1) {
    	playSound("count_down.wav");   // Regular countdown beep sound
    	countdownTime--;

	} else if (countdownTime == 1) {
	   	countdownTime--;
	    playSound("final_beep.wav");
     // Distinct sound for the final countdown step
 }
		
        glutPostRedisplay();
        if (countdownTime > 0) {
            glutTimerFunc(1000, updateCountdown, 0); // Continue the countdown
        } else {
            currentState = IN_GAME; // Transition to the in-game state
            glutTimerFunc(16, update, 0); // Start the game update loop
        }
    }
}

void drawBricks() {

    // Updated color palette with the specified colors
    float colorPalette[BRICK_ROWS][3] = {
        {0x24 / 255.0f, 0xAA / 255.0f, 0xB1 / 255.0f}, // #24AAB1
        {0xD2 / 255.0f, 0xC8 / 255.0f, 0xB0 / 255.0f}, // #D2C8B0
        {0xE5 / 255.0f, 0xA5 / 255.0f, 0x2D / 255.0f}, // #E5A52D
        {0xE6 / 255.0f, 0x40 / 255.0f, 0x40 / 255.0f}  // #E64040 
    };

	for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLUMNS; j++) {
            if (bricks[i][j]) {
                // Calculate the x position
                int x;
                if (j == 0) {
                    x = 0; // First brick in each row has no left margin
                } else {
                    // Subsequent bricks: position is previous brick's right edge plus margin
                    x = j * (brickWidth + marginSize);
                }
                int y = WINDOW_HEIGHT - (i + 1) * (brickHeight + marginSize) + marginSize;

                 glColor3fv(colorPalette[i]);
                glRecti(x, y, x + brickWidth, y + brickHeight);

                // Add a darker overlay for the shadow effect on the bottom and right sides
                glColor3f(colorPalette[i][0] * 0.75f, colorPalette[i][1] * 0.75f, colorPalette[i][2] * 0.75f); // Shadow color (darker)
                
                // Bottom shadow
                glRecti(x, y, x + brickWidth, y + 4); // Adjust the '4' for thicker/thinner shadow
                
                // Right side shadow
                glRecti(x + brickWidth - 4, y, x + brickWidth, y + brickHeight); // Adjust the '4' for thicker/thinner shadow
            }
        }
    }
}


void drawBall() {
    glColor3f(1.0f, 1.0f, 1.0f); 


    // Draw a circle using points
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ballX, ballY); // Center of the circle
    for (int i = 0; i <= 360; i += 5) { 
        float angle = i * 3.14159265f / 180.0f;
        float x = ballX + ballRadius * cos(angle);
        float y = ballY + ballRadius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawPaddle() {
    glRecti(paddleX, paddleY, paddleX + paddleWidth, paddleY + paddleHeight);
}

void drawScore() {
    char msg[50]; // Buffer to store the message
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(10, 20);
    sprintf(msg, "Score: %d", score);
    for (int i = 0; i < strlen(msg); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg[i]);
    }
}

void drawGameOverScreen() {

    // Draw the "Game Over" message 
    glColor3f(206/255.0f, 138/255.0f, 108/255.0f); // RGB for #CE8A6C
    const char *gameOverMsg = "Game Over!";
    glRasterPos2i(WINDOW_WIDTH / 2 - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)gameOverMsg) / 2, WINDOW_HEIGHT / 2 + 20);
    for (const char* c = gameOverMsg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // Draw the "Your Score:" message
     glColor3f(1.0f, 1.0f, 1.0f);
    char scoreMsg[30];
    sprintf(scoreMsg, "Your Score: %d", score); // Assuming 'score' is your score variable
    glRasterPos2i(WINDOW_WIDTH / 2 - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)scoreMsg) / 2, WINDOW_HEIGHT / 2 - 20);
    for (const char* c = scoreMsg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        
    }
    //Display "Press R to restart" message 
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    const char *restartMsg = "Press r to restart";
    glRasterPos2i(WINDOW_WIDTH / 2 - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)restartMsg) / 2, WINDOW_HEIGHT/2   -250);
    for (const char* c = restartMsg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void restartGame() {

    currentState = COUNTDOWN; // Set to COUNTDOWN state to start with the countdown
    countdownTime = 3; // Reset the countdown time
    score = 0;
  
    // Reset ball and paddle to starting positions and velocities
    ballX = WINDOW_WIDTH / 2;
    ballY = WINDOW_HEIGHT / 2+100;
    paddleX = WINDOW_WIDTH / 2 - paddleWidth / 2;
    paddleY = 50; // Assuming this is your default paddle Y position

    // Re-initialize the game setup
    initializeBricks();

    // Start the countdown timer
    glutTimerFunc(1000, updateCountdown, 0); // Start the countdown with a 1-second interval

    // Request to redraw the screen
    glutPostRedisplay();
}


void drawVictoryScreen() {
	

    // Draw the "Hurray!" message
    glColor3f(189/255.0f, 255/255.0f, 82/255.0f); 
    const char *hurrayMsg = "Hurray!";
    glRasterPos2i(WINDOW_WIDTH / 2 - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char *)hurrayMsg) / 2, WINDOW_HEIGHT / 2 + 30);
    for (const char* c = hurrayMsg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
 
    // Draw the "You won the game." message
    glColor3f(1.0,1.0,1.0);
    const char *winMsg = "You won the game.";
    glRasterPos2i(WINDOW_WIDTH / 2 - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)winMsg) / 2, WINDOW_HEIGHT / 2 - 10);
    for (const char* c = winMsg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    // Draw the "Press R to restart" message
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    const char *restartMsg = "Press r to replay.";
    glRasterPos2i(WINDOW_WIDTH / 2 - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)restartMsg) / 2, WINDOW_HEIGHT/2   -250);
    for (const char* c = restartMsg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);


    switch (currentState) {
        case START_SCREEN:
        	drawParticles();
            drawStartScreen();
            break;
		case INSTRUCTIONS_SCREEN:
   			 drawInstructionsScreen(); // Function to draw the instructions screen
   			 break;
        case COUNTDOWN:
        	drawPaddle();
            drawBall();
            drawBricks();
            drawScore();
            drawCountdown();  // To draw the countdown timer
            break;
        case IN_GAME:
            // Draw game elements
            drawPaddle();
            drawBall();
            drawBricks();
            drawScore();
            break;
        
    	case GAME_OVER:
        	drawGameOverScreen(); // Function to draw the game over message
       		break;
       	case GAME_WON:
        	drawVictoryScreen(); // Function to draw the victory message
        	break;
				
    }

    glutSwapBuffers();
}


void specialKeys(int key, int x, int y) {
	if(currentState==IN_GAME){
    switch (key) {
        case GLUT_KEY_LEFT: // Left arrow key
            if (paddleX - paddleSpeed >= 0) // Ensure the paddle stays within bounds
                paddleX -= paddleSpeed;
            break;
        case GLUT_KEY_RIGHT: // Right arrow key
            if (paddleX + paddleWidth + 2 <= WINDOW_WIDTH) // Ensure the paddle stays within bounds
                paddleX += paddleSpeed;
            break;
    }
    glutPostRedisplay();// Request to redraw the window with the updated positions
	}	 
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'r':
        case 'R': 
            if (currentState == GAME_OVER || currentState == GAME_WON) {
                restartGame();  // to restart game
            }
            break;
    }

}


void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // OpenGL's Y-coordinate starts from the bottom, so we invert the mouse's Y
        int openglY = WINDOW_HEIGHT - y;
        if (currentState==START_SCREEN) {
            // Check if the click is within the "Start Game" button's area
            if (x >= 307 && x <= 307 + 186 && openglY >= 323 && openglY <= 323 + 51) {
                playSound(NULL);
            	onStartGameButtonClicked();
            }
            // Check if the click is within the "Instructions" button's area
            else if (x >= 307 && x <= 307 + 186 && openglY >= 263 && openglY <= 263 + 51) {
                currentState=INSTRUCTIONS_SCREEN; // go to instruction screen
            }
       	 }
      	if (currentState==INSTRUCTIONS_SCREEN) {
        // Check if the click is within the "Back" button's area
        	if (x >= 310&& x <= 310 + 129&& openglY >= 150&& openglY <= 150+ 51) {
            //return to the start screen:
			currentState=START_SCREEN; 
        }
    }
}
}


void update(int value) {
		if (currentState == IN_GAME) {
			int i,j;
        	ballX += ballDX;
        	ballY += ballDY;

		// Check if the ball hits the top boundary
        if (ballY + ballRadius >= WINDOW_HEIGHT) {
        	playSound("wall_hit.wav");
            ballDY = -ballDY; // Reverse the vertical velocity
        }

        // Ball collision with walls
        if (ballX <= ballRadius || ballX >= WINDOW_WIDTH - ballRadius) {
        	playSound("wall_hit.wav");
            ballDX = -ballDX;   
        }

        /		// Check if the ball hits the top boundary
        if (ballY + ballRadius >= WINDOW_HEIGHT) {
        	playSound("wall_hit.wav");
            ballDY = -ballDY; // Reverse the vertical velocity
        }

		// Ball collision with side walls
		if (ballX <= ballRadius || ballX >= WINDOW_WIDTH - ballRadius) {
    	playSound("wall_hit.wav");
    	ballDX = -ballDX;  // Reverse horizontal direction

    	// Ensure minimum horizontal velocity is maintained
    	float minHorizontalVelocity = WINDOW_WIDTH / 400.0f; // Adjust as needed
    	if (fabs(ballDX) < minHorizontalVelocity) {
        ballDX = (ballDX < 0 ? -1 : 1) * minHorizontalVelocity;
    }
}
		// Ball collision with paddle
		if (ballY - ballRadius <= paddleY + paddleHeight && ballY + ballRadius >= paddleY && ballX >= paddleX && ballX <= paddleX + paddleWidth) {
		    playSound("paddle_hit.wav");
		    ballDY = -ballDY; // Reverse the vertical velocity
		
		    // Calculate hit position on the paddle (0 at center, -1 at left edge, +1 at right edge)
		    float hitPosition = (ballX - (paddleX + paddleWidth / 2.0)) / (paddleWidth / 2.0);
		
		    // Adjust the horizontal velocity based on where the ball hits the paddle
		    ballDX = hitPosition * 5; // The constant here (5) controls the maximum change in horizontal velocity.
		
		    // Ensure there's always some horizontal movement to prevent vertical-only motion
		    float minHorizontalVelocity = WINDOW_WIDTH / 400.0f; // Adjust this value as needed
		    if (fabs(ballDX) < minHorizontalVelocity) {
		        ballDX = (ballDX < 0 ? -1 : 1) * minHorizontalVelocity; // Apply minimum horizontal velocity
		    }
		
		    // Ensure ball doesn't get stuck vertically
		    ballY = paddleY + paddleHeight + ballRadius + 1; // Slightly adjust ball's Y position to prevent it from "sticking" to the paddle
		}
        if (ballY - ballRadius < paddleY) {
        playSound("game_over.wav");
    	currentState = GAME_OVER; // Change the state to GAME_OVER.
    	glutPostRedisplay(); // Request a redisplay to update the screen.
		}
	
        // Loop through all bricks to check for collision
       		for (int i = 0; i < BRICK_ROWS; i++) {
    		for (int j = 0; j < BRICK_COLUMNS; j++) {
        	if (bricks[i][j]) {
            int brickX = j * (brickWidth + marginSize);
            int brickY = WINDOW_HEIGHT - (i + 1) * (brickHeight + marginSize);

            		// Define the edges of the ball
           		 	int ballLeft = ballX - ballRadius;
            		int ballRight = ballX + ballRadius;
            		int ballTop = ballY + ballRadius;
            		int ballBottom = ballY - ballRadius;

            		// Check for collision with the brick edges
            		if (ballRight > brickX && ballLeft < brickX + brickWidth &&
                	ballTop > brickY && ballBottom < brickY + brickHeight) {
                		playSound("brick_hit.wav");// Play brick hit sound
                        bricks[i][j] = false; // Remove the brick
                        score++; // Increment score

                        // Determine ball's new direction
                        float ballCenterX = ballX;
                        float brickCenterX = brickX + brickWidth / 2;
                        float ballCenterY = ballY;
                        float brickCenterY = brickY + brickHeight / 2;

                        // Reverse ball's direction based on where it hit the brick
                        if (ballCenterX >= brickX && ballCenterX <= brickX + brickWidth) {
                            ballDY = -ballDY; // Vertical collision
                        } else if (ballCenterY >= brickY && ballCenterY <= brickY + brickHeight) {
                            ballDX = -ballDX; // Horizontal collision
                        } else {
                            ballDX = -ballDX; // Corner collision
                            ballDY = -ballDY;
                        }

                        break; // Exit loop after collision

                 }
                }
            }
        }
	bool allBricksBroken = true; // Assume all bricks are broken initially
	// Loop through all bricks to check if any are still unbroken
	for (int i = 0; i < BRICK_ROWS; i++) {
    for (int j = 0; j < BRICK_COLUMNS; j++) {
        if (bricks[i][j]) { // If any brick is found not broken
            allBricksBroken = false; // Not all bricks are broken
            break; // No need to check further
        }
    }
}
	// Change game state to GAME_WON only if all bricks are broken
	if (allBricksBroken) {
	playSound("game_win.wav");
    currentState = GAME_WON;
	}
	glutPostRedisplay();
    if (currentState!=GAME_OVER) {
    	glutTimerFunc(16, update, 0); // Update every 16 milliseconds (about 60 FPS)
		}
	}
}
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutPostRedisplay();
}
