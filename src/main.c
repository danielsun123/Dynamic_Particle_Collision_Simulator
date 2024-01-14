#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h> 

//define macros
#define SCREEN_WIDTH 640.0
#define SCREEN_HEIGHT 480.0
#define PI 3.14159265358979323846
#define numBalls 100

//global variables
float radius = 3.5;
float damping_coefficient = 0.99;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;



// declare structure for balls 
typedef struct Ball {
    float CircleX, CircleY;
    float vx, vy;
    float mass;
} BallType;

// create window and renderer
bool create () {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    else {
         window = SDL_CreateWindow("Solid Particles in a Frictionless Environment", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            if (window == NULL) {
                printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
                return 1;
            }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
                return 1;
            }
    }
}

// destroy window and renderer to clear memory
void destroy () {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// draw the circle 
void drawFilledCircle(SDL_Renderer *renderer, int positionX, int positionY, int radius) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, positionX + x, positionY + y);
            }
        }
    }
}

// vector operation (multiplying by a scalar)
void multiplyVectorByConstant(double vector[], int size, double constant) {
    for (int i = 0; i < size; i++) {
        vector[i] *= constant;
    }
}

// function for random initial position collision avoidance                                                         Note: array balls created for algorithm 
bool checkCollision(BallType* balls, int ballCount, int x, int y, int radius) {
    for (int i = 0; i < ballCount; i++) {
        int dx = x - balls[i].CircleX;
        int dy = y - balls[i].CircleY;
        int distanceSquared = dx * dx + dy * dy;

        int minDistance = 2 * radius; // Minimum distance between centers before collision occurs

        if (distanceSquared <= minDistance * minDistance) {
            return true; // collision detected
        }
    }
    return false; // No collision
}


// ------------------------------------------------------main code to execute------------------------------------------------------------

int main(int argc, char *argv[]) {
    
    // define variables for generating random position and velocities 
    int upperX = SCREEN_WIDTH - radius;
    int lowerX = radius;

    int upperY = SCREEN_HEIGHT - radius;
    int lowerY = radius;

    // seed random number generator
    srand(time(NULL));

    int ball_array[5][5]; // 2d array for collision detection (storing pairs of balls), initialized as 5x5, but dynamically allocated   

    // creating structures for balls
    BallType Ball[numBalls];

    for (int i = 0; i < numBalls; i++) {
        // Initialize each ball
        Ball[i].CircleX;   
        Ball[i].CircleY;
        Ball[i].vx; 
        Ball[i].vy;     // Note: in sdl, the y-direction is reversed, meaning that when I have a positive y velocity, the ball goes down
        Ball[i].mass; 
    }

    // create window and renderer
    
    create();

    for (int i = 0; i < numBalls; i++) {             // generate random positions, velocities, and masses for all balls 
        do {
            Ball[i].CircleX = rand() % (upperX - lowerX + 1) + lowerX;
            Ball[i].CircleY = rand() % (upperY - lowerY + 1) + lowerY;
        } while (checkCollision(Ball, i, Ball[i].CircleX, Ball[i].CircleY, radius));
        
        Ball[i].vx = rand() % (15 - (-15) + 1) + -15;        // random initial velocities      
        Ball[i].vy = rand() % (15 - (-15) + 1) + -15;
        Ball[i].mass = rand() % (10 - 5 + 1) + 5;
    }

    // infinite loop (ends with user input/event)

    while (1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                destroy();
                exit(0);
            }
        }

        //----------------------collision detection------------------------
        
        for (int j = 0; j < numBalls - 1; j++) {
            for (int k = j + 1; k < numBalls; k++) {   

                ball_array[j][k]; //creates array that stores collision combinations 

                // to check if pair is viable for collison detection
                float x_dist = Ball[k].CircleX - Ball[j].CircleX;
                float y_dist = Ball[k].CircleY - Ball[j].CircleY;
                float distance = sqrt(x_dist * x_dist + y_dist * y_dist);   
                
                float distance_squared = x_dist * x_dist + y_dist * y_dist;
                float sum_radii = radius + radius;

                if (distance <= sum_radii) {     // condition to check if collison occurs between ballj and ballk

                    // we will have to work with vectors here: the first step is to define what the vectors are before collision: vi(vector of velocity before collision),  v'i (vector of velocity after collision), Oi (the point of center), and xi (the vector of Oi position)
                   
                    double initial_velocity_vector_ballj[2] = {Ball[j].vx, Ball[j].vy};     // initial velocity vector for ballj
                    double position_ballj[2] = {Ball[j].CircleX, Ball[j].CircleY};     // initial position of ballj

                    double initial_velocity_vector_ballk[2] = {Ball[k].vx, Ball[k].vy};     // initial velocity vector for ballk
                    double position_ballk[2] = {Ball[k].CircleX, Ball[k].CircleY};      // initial position of ballk

                    float overlap = sum_radii - distance;

                    float separation_x = fabs((x_dist / distance) * (overlap * 0.5));
                    float separation_y = fabs((y_dist / distance) * (overlap * 0.5));

                    // Move the balls apart along normal contact plane to resolve the overlap
                    
                    if (Ball[j].CircleX > Ball[k].CircleX) {
                        Ball[j].CircleX += separation_x;
                        Ball[k].CircleX -= separation_x;
                    } else {
                        Ball[j].CircleX -= separation_x;
                        Ball[k].CircleX += separation_x;
                    }

                    if (Ball[j].CircleY > Ball[k].CircleY) {
                        Ball[j].CircleY += separation_y;
                        Ball[k].CircleY -= separation_y;
                    } else {
                        Ball[j].CircleY -= separation_y;
                        Ball[k].CircleY += separation_y;
                    }

                    if (Ball[j].CircleX == Ball[k].CircleX) {
                        if (Ball[j].CircleY > Ball[k].CircleY) {
                            Ball[j].CircleY += separation_y;
                            Ball[k].CircleY -= separation_y;
                        } else {
                            Ball[j].CircleY -= separation_y;
                            Ball[k].CircleY += separation_y;
                        }
                    } else {
                        if (Ball[j].CircleX > Ball[k].CircleX) {
                            Ball[j].CircleX += separation_x;
                            Ball[k].CircleX -= separation_x;
                        } else {
                            Ball[j].CircleX -= separation_x;
                            Ball[k].CircleX += separation_x;
                        }
                    }

                    if (Ball[j].CircleY == Ball[k].CircleY) {
                        if (Ball[j].CircleX > Ball[k].CircleX) {
                            Ball[j].CircleX += separation_x;
                            Ball[k].CircleX -= separation_x;
                        } else {
                            Ball[j].CircleX -= separation_x;
                            Ball[k].CircleX += separation_x;
                        }
                    } else {
                        if (Ball[j].CircleY > Ball[k].CircleY) {
                            Ball[j].CircleY += separation_y;
                            Ball[k].CircleY -= separation_y;
                        } else {
                            Ball[j].CircleY -= separation_y;
                            Ball[k].CircleY += separation_y;
                        }
                    }
                    
                    //printf("separation_x: %lf\n", separation_x);
                    //printf("separation_y: %lf\n\n", separation_y);

                    // ----------maths computation (below finds the final velocities in the x and y directions for ball0)-----------

                    double velocity_difference_ballj[2]; //      v1-v2          

                    for (int i = 0; i < 2; i++) {
                        velocity_difference_ballj[i] = initial_velocity_vector_ballj[i] - initial_velocity_vector_ballk[i];
                    }

                    double center_position_difference_ballj[2]; //     x1-x2     
                    for (int i = 0; i < 2; i++) {
                        center_position_difference_ballj[i] = position_ballj[i] - position_ballk[i];
                    }

                    float dotProduct = 0;
                    int size1 = sizeof(velocity_difference_ballj) / sizeof(velocity_difference_ballj[0]);

                    for (int i = 0; i < size1; i++) {
                        dotProduct += velocity_difference_ballj[i] * center_position_difference_ballj[i];
                    }

                    double result_divided_by_magnitude_squared_ballj = dotProduct / (distance_squared);   //           < v1-v2 || x1-x2 >  /  (||x1-x2||^2)

                    double result_divided_and_multiplied_by_mass_ballj = result_divided_by_magnitude_squared_ballj * ((2*Ball[k].mass) / (Ball[j].mass + Ball[k].mass));   //   2m/(m1+m2) * (...see documentation)

                    int size2 = sizeof(center_position_difference_ballj) / sizeof(center_position_difference_ballj[0]);
                    double constant = result_divided_and_multiplied_by_mass_ballj;

                    multiplyVectorByConstant(center_position_difference_ballj, size2, constant);

                    double final_velocity_vector_ballj[2];
                    for (int i = 0; i < 2; i++) {
                        final_velocity_vector_ballj[i] = initial_velocity_vector_ballj[i] - center_position_difference_ballj[i];
                    }

                    Ball[j].vx = final_velocity_vector_ballj[0];
                    Ball[j].vy = final_velocity_vector_ballj[1];

                    //printf("ball%d vxf is %lf\n", j, Ball[j].vx);
                    //printf("ball%d vyf is %lf\n\n", j, Ball[j].vy);


                    // now for final velocities in the x and y directions for ball k....
                        
                    double velocity_difference_ballk[2]; //   v2-v1

                    for (int i = 0; i < 2; i++) {
                        velocity_difference_ballk[i] = initial_velocity_vector_ballk[i] - initial_velocity_vector_ballj[i];
                    }

                    double center_position_difference_ballk[2]; //    x2-x1

                    for (int i = 0; i < 2; i++) {
                        center_position_difference_ballk[i] = position_ballk[i] - position_ballj[i];
                    }

                    int size3 = sizeof(velocity_difference_ballk) / sizeof(velocity_difference_ballk[0]);
                    float dotProduct1 = 0;
                
                    for (int i = 0; i < size3; i++) {
                        dotProduct1 += velocity_difference_ballk[i] * center_position_difference_ballk[i];
                    }

                    double result_divided_by_magnitude_squared_ballk = dotProduct1 / (distance_squared);   //     < v2-v1 || x2-x1 >    /   (||x1-x2||^2)

                    double result_divided_and_multiplied_by_mass_ballk = result_divided_by_magnitude_squared_ballk * ((2*Ball[j].mass) / (Ball[j].mass + Ball[k].mass));   // 2m / (m1+m2) * (...see documentation)

                    int size4 = sizeof(center_position_difference_ballk) / sizeof(center_position_difference_ballk[0]);
                    double constant2 = result_divided_and_multiplied_by_mass_ballk;

                    multiplyVectorByConstant(center_position_difference_ballk, size4, constant2);

                    double final_velocity_vector_ballk[2];
                    for (int i = 0; i < 2; i++) {
                        final_velocity_vector_ballk[i] = initial_velocity_vector_ballk[i] - center_position_difference_ballk[i];
                    }

                    Ball[k].vx = final_velocity_vector_ballk[0];
                    Ball[k].vy = final_velocity_vector_ballk[1];

                    //printf("ball%d vxf is %lf\n", k, Ball[k].vx);
                    //printf("ball%d vyf is %lf\n\n", k, Ball[k].vy);
                }
            }
        }

        float dt = 0.01;    //timestep  

        for (int i = 0; i < numBalls; i++) {

            Ball[i].CircleX += Ball[i].vx * dt;
            Ball[i].CircleY += Ball[i].vy * dt;

            // Collision detection with window
            if (Ball[i].CircleX <= radius || Ball[i].CircleX >= SCREEN_WIDTH - radius) {
                // Handle left and right boundary collisions
                if (Ball[i].CircleX <= radius) {
                    Ball[i].CircleX = radius; // Reset position to avoid sticking
                } else {
                    Ball[i].CircleX = SCREEN_WIDTH - radius; // Reset position to avoid sticking
                }
                Ball[i].vx *= -damping_coefficient; // Apply restitution and damping
            }

            if (Ball[i].CircleY <= radius || Ball[i].CircleY >= SCREEN_HEIGHT - radius) {
                // Handle top and bottom boundary collisions
                if (Ball[i].CircleY <= radius) {
                    Ball[i].CircleY = radius; // Reset position to avoid sticking
                } else {
                    Ball[i].CircleY = SCREEN_HEIGHT - radius; // Reset position to avoid sticking
                }
                Ball[i].vy *= -damping_coefficient; // Apply restitution and damping
            }
        }

        // render background color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Define an array of SDL_Color values for different ball colors
    

        SDL_Color ballColors[numBalls];

        for (int i = 0; i < numBalls; ++i) {
            ballColors[i].r = 255;
            ballColors[i].g = 255;
            ballColors[i].b = 255;
            ballColors[i].a = 255;
        }       
        // render circle 
        for (int i = 0; i < numBalls; ++i) {
            SDL_SetRenderDrawColor(renderer, ballColors[i].r, ballColors[i].g, ballColors[i].b, ballColors[i].a);
            drawFilledCircle(renderer, Ball[i].CircleX, Ball[i].CircleY, radius);
        }

        // update renderer
        SDL_RenderPresent(renderer);

    }

    destroy();

    return 0;
}

