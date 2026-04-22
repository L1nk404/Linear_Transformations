/******************************************************************************
 * Libraries Documentation:
 *  - SDL2: https://wiki.libsdl.org/SDL2
 *  - math: https://cplusplus.com/reference/cmath/
 *
 * Compiler: gcc version 15.2.1 20260202 (SUSE Linux)
 *  - `gcc main.c -o main.exe $(pkg-config --cflags --libs sdl2) -lm`
 ******************************************************************************/

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

// Window
#define WIDTH 900
#define HEIGHT 600
#define FRAME_DELAY 20 // Milisec

// centering window in 1920x1080 display
#define CENTER_Y (HEIGHT / 2)
#define CENTER_X (WIDTH / 2)

// constants
#define PI 3.14159265359
#define G 6.6743

// Other
#define POINT_RADIUS 3

// Colors
#define COLOR_BRACKGROUND 0x1E1F29
#define COLOR_PINK 0xf56f97
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0x00000000
#define COLOR_ORANGE 0xff763b

// ANSI Colors
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RESET "\x1b[0m"

/*
    STRUCTS
   =============================================================================
*/

typedef struct
{
    double x;
    double y;
    double radius;
} Circle;

typedef struct
{
    int center_x; // Center x coordinate
    int center_y; // Center y coordinate
    int s_x;      // x scale
    int s_y;      // y scale
    double phi;   // Rotation_Angle

} Point;

/*
    FUNCTIONS
   ==========================================================================
*/

/*
    Create a solid circle (point)
*/
void FillCircle(SDL_Surface *surface, Circle circle, Uint32 color)
{
    double low_x = circle.x - circle.radius;
    double low_y = circle.y - circle.radius;
    double high_x = circle.x + circle.radius;
    double high_y = circle.y + circle.radius;

    double radius_squared = circle.radius * circle.radius;

    for (int x = low_x; x < high_x; x++)
    {
        for (int y = low_y; y < high_y; y++)
        {
            // Check if ||x² + y²|| < r² (is inside ball)
            double center_distance_square = (x - circle.x) * (x - circle.x) +
                                            (y - circle.y) * (y - circle.y);

            if (center_distance_square < radius_squared)
            {
                // Creating a retangle object
                SDL_Rect pixel = (SDL_Rect){x, y,  // coordinates (x,y)
                                            1, 1}; // Width, Height
                // Coloring the retangle
                SDL_FillRect(surface, // Destination surface Pointer
                             &pixel,  // Rect object Pointer
                             color);  // Unitd32 color
            }
        }
    }
}

/*
 Plots equally spaced points on a circle based on angular divisions.

Calculates and draws N points distributed evenly around a circle's
circumference using an angle increment of 2π/N. Each point's position can be
offset from the circle's center and rotated in both clockwise and
counter-clockwise directions.
*/
void PlotPointArrounCircle(int n, Point *p, SDL_Surface *surface, Uint32 color)
{
    int circle_radius = 3;

    Circle center = {
        p->center_x, // x
        p->center_y, // y
        circle_radius * 1.5,
    };
    FillCircle(surface, center, COLOR_ORANGE);

    double theta = 2 * PI / n;

    for (int i = 0; i < n; i++)
    {

        double x, y, x_rot, y_rot;

        // Let's define the Point(x,y) centered in (0,0):
        x = p->s_x * cos(0 + i * theta);
        y = p->s_y * sin(0 + i * theta);

        // Let's rotate it:
        x_rot = x * cos(p->phi) - y * sin(p->phi);
        y_rot = x * sin(p->phi) + y * cos(p->phi);

        // Finally, let's translate to (center_x, center_y)
        x = p->center_x + x_rot;
        y = p->center_y + y_rot;

        Circle point = {x, y, circle_radius};

        FillCircle(surface, point, color);
    }
};

/*
    Just print parameter of the figure
*/
void print_parameters(int n, Point *p)
{
    system("clear");
    fprintf(stdout, "Comandos:\n\n");

    fprintf(stdout, "Aumentar N: =\n");
    fprintf(stdout, "Diminuir N: -\n");
    fprintf(stdout, "Translação: W, A, S, D\n");
    fprintf(stdout, "Rotação: Q, E\n");
    fprintf(stdout, "Escala: ↑, ↓, ←, →\n");
    fprintf(stdout, "Reset, : R\n\n");

    fprintf(stdout, "Parâmetros:\n\n");

    fprintf(stdout, "Número de pontos (n): %d\n", n);
    if (p->center_x == CENTER_X && p->center_y == CENTER_Y)
    {
        fprintf(stdout, "centro x: " ANSI_GREEN "%.2f" ANSI_RESET " px\n",
                p->center_x);
        fprintf(stdout, "centro y: " ANSI_GREEN "%.2f" ANSI_RESET " px\n",
                p->center_y);
    }
    else
    {
        fprintf(stdout, "centro x: %d px\n", p->center_x);
        fprintf(stdout, "centro y: %d px\n", p->center_y);
    }
    if (p->s_x == p->s_y)
    {
        fprintf(stdout, "Escala x: " ANSI_GREEN "%.2f" ANSI_RESET " px\n",
                p->s_x);
        fprintf(stdout, "Escala y: " ANSI_GREEN "%.2f" ANSI_RESET " px\n",
                p->s_y);
    }
    else
    {
        fprintf(stdout, "Escala em x: %d px\n", p->s_x);
        fprintf(stdout, "Escala em y: %d px\n", p->s_y);
    }
    // If phi is multiple of 2*PI
    if (fmod(p->phi, 2 * PI) == 0)
        fprintf(stdout,
                "Ãngulo de rotação: " ANSI_GREEN "%.2lf" ANSI_RESET " π\n\n",
                p->phi / PI);
    else
        fprintf(stdout, "Ângulo de rotação: %.2lf π\n\n", p->phi / PI);
}

void BouncyBall(Point *Moving_Circle)
{
    // Direction Flags
    int f_direction_x = 1;
    int f_direction_y = 1;

    int radius = Moving_Circle->s_x;

    /*
        Y Axis: y(t) = G * t , where G is the gravit acceleration
    */
    if (f_direction_y == 1) // Descending
    {
        // If it hit the floor, it must bounce
        if (Moving_Circle->center_y > (HEIGHT - (radius + 10)))
        {
            f_direction_y = -1 * f_direction_y; // change direction
        }
        Moving_Circle->center_y += G;
    }
    else if (f_direction_y == -1)
    {
        // Check if it hit the roof
        if (Moving_Circle->center_y < (radius + 10))
        {
            f_direction_y = -1 * f_direction_y;
        }
        Moving_Circle->center_y -= G;
    }

    /*
        X Axis: let a_x be the X acceleration, then
        x(t) =
    */
}

/*
    MAIN
   ===============================================================================
*/
int main()
{
    // Variables
    int n = 4;
    int center_x = CENTER_X;
    int center_y = CENTER_Y;
    int x_scale = 50;
    int y_scale = 50;
    double rotation_angle = 0;

    // Objects
    Point point = {
        center_x,       // x coordinate of center
        center_y,       //  y coordinate of center
        x_scale,        // x scale
        y_scale,        // y scale
        rotation_angle, // Clockwise rotation
    };

    // FLAGS
    int f_programming_running = 1;
    int f_animation_running = -1;

    // Initializing Graphic
    SDL_Init(SDL_INIT_VIDEO);

    // Creating window
    SDL_Window *window =
        SDL_CreateWindow("Computação Gráfica",   // Title of the SDL window
                         SDL_WINDOWPOS_CENTERED, // Position x of the window
                         SDL_WINDOWPOS_CENTERED, // Position y of the window
                         WIDTH, HEIGHT, // Width, height of the window in pixels
                         0);            // Additional flag(s)

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    SDL_Event event;
    SDL_Rect surface_background = (SDL_Rect){0, 0, // int x,y coordinates
                                             WIDTH, HEIGHT};

    SDL_FillRect(surface, &surface_background,
                 COLOR_BRACKGROUND); // Setting SurfaceBackground

    SDL_UpdateWindowSurface(window); // Update the window

    // First debug print:
    print_parameters(n, &point);

    while (f_programming_running)
    {

        // Track events
        while (SDL_PollEvent(&event))
        {
            /*
                EXIT
            */
            // if (X) pressed
            if (event.type == SDL_QUIT)
            {
                f_programming_running = 0;
                fprintf(stdout, "QUIT event received!\n");
            }
            // if press ESC => EXIT
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                f_programming_running = 0;
                fprintf(stdout, "QUIT event received!\n");
            }
            // if SPACE pressed => Run animation
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                // Checks if it is a circle
                if (point.s_x != point.s_y)
                {
                    fprintf(stdout, "Figura deve ser um círculo: "
                                    "Escala X = Escala Y\n");
                }
                else
                {
                    f_animation_running = -1 * f_animation_running;
                }
            }

            if (f_animation_running == 1)
            {
                BouncyBall(&point);
            }

            /*
                Linear Transformations
            */
            // else if (event.type == SDL_KEYDOWN)
            else
            {

                switch (event.key.keysym.sym)
                {
                    // if '=' pressed => increase d
                    case SDLK_EQUALS:
                        n++;
                        print_parameters(n, &point);
                        break;

                    // if '-' pressed = decrease d
                    case SDLK_MINUS:
                        // n cannot be 0!
                        if (n == 1)
                            break;
                        else
                            n--;
                        print_parameters(n, &point);
                        break;

                    // Translation =======
                    case SDLK_w:
                        // W
                        point.center_y = point.center_y - 30;
                        print_parameters(n, &point);
                        break;

                    case SDLK_s:
                        // S
                        point.center_y = point.center_y + 30;
                        print_parameters(n, &point);
                        break;

                    case SDLK_d:
                        // D
                        point.center_x = point.center_x + 30;
                        print_parameters(n, &point);
                        break;

                    case SDLK_a:
                        // A
                        point.center_x = point.center_x - 30;
                        print_parameters(n, &point);
                        break;

                    // Rotation ======
                    // Q
                    case SDLK_q:
                        point.phi = point.phi - 0.1;
                        print_parameters(n, &point);
                        break;
                    // E
                    case SDLK_e:
                        point.phi = point.phi + 0.1;
                        print_parameters(n, &point);
                        break;

                    // Scale =========
                    case SDLK_RIGHT:
                        // →
                        point.s_x = point.s_x + 10;
                        print_parameters(n, &point);
                        break;
                    case SDLK_LEFT:
                        // ←
                        point.s_x = point.s_x - 10;
                        print_parameters(n, &point);
                        break;
                    case SDLK_UP:
                        // ↑
                        point.s_y = point.s_y + 10;
                        print_parameters(n, &point);
                        break;
                    case SDLK_DOWN:
                        // ↓
                        point.s_y = point.s_y - 10;
                        print_parameters(n, &point);
                        break;
                    // Reset ========
                    case SDLK_r:
                        // R
                        n = 4;
                        point.center_x = CENTER_X;
                        point.center_y = CENTER_Y;
                        point.s_x = 50;
                        point.s_y = 50;
                        point.phi = 0;
                        print_parameters(n, &point);
                        break;
                    default:
                        break;
                }
            }
        }

        // Background
        SDL_FillRect(surface, &surface_background, COLOR_BRACKGROUND);

        // Plot points
        PlotPointArrounCircle(n, &point, surface, COLOR_PINK);

        // Update the window surface
        SDL_UpdateWindowSurface(window);

        // Delay: 20 Miliseconds = 50 fps
        SDL_Delay(FRAME_DELAY);
    }

    // Checks if window whas been created, if not, exits program
    if (window == NULL)
    {
        fprintf(stderr, "Janela falhou em iniciar: %s\n", SDL_GetError());
        return 1;
    }

    // Close window - Free Memory
    SDL_DestroyWindow(window);

    // Shuts down all SDL subsystem
    SDL_Quit();

    return 0;
}