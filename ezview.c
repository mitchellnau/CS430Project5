#define GLFW_DLL 1
#define PI 3.14159265358979323846
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

//data type to store pixel rgb values
typedef struct Pixel
{
    unsigned char r, g, b;
} Pixel;


FILE* inputfp;
int width, height, maxcv; //global variables to store header information
GLFWwindow* window;


typedef struct
{
    float position[3];
    float color[4];
    float textcoord[2];
} Vertex;


Vertex Vertices[] =
{
    {{1, -1, 0},  {1, 0, 0, 1}, {0.99999,0.99999}},
    {{1, 1, 0},   {0, 1, 0, 1}, {0.99999,0}},
    {{-1, 1, 0},  {0, 0, 1, 1}, {0,0}},
    {{-1, -1, 0}, {0, 0, 0, 1}, {0,0.99999}}
};


const GLubyte Indices[] =
{
    0, 1, 2,
    2, 3, 0
};


char* vertex_shader_src =
    "attribute vec4 Position;\n"
    "attribute vec4 SourceColor;\n"
    "\n"
    "attribute vec2 TexCoordIn;\n"
    "varying lowp vec2 TexCoordOut;\n"
    "\n"
    "varying lowp vec4 DestinationColor;\n"
    "\n"
    "void main(void) {\n"
    "    TexCoordOut = TexCoordIn;\n"
    "    DestinationColor = SourceColor;\n"
    "    gl_Position = Position;\n"
    "}\n";


char* fragment_shader_src =
    "varying lowp vec4 DestinationColor;\n"
    "\n"
    "varying lowp vec2 TexCoordOut;\n"
    "uniform sampler2D Texture;\n"
    "\n"
    "void main(void) {\n"
    "    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
    "}\n";


GLint simple_shader(GLint shader_type, char* shader_src)
{

    GLint compile_success = 0;

    int shader_id = glCreateShader(shader_type);

    glShaderSource(shader_id, 1, &shader_src, 0);

    glCompileShader(shader_id);

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);

    if (compile_success == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(shader_id, sizeof(message), 0, &message[0]);
        printf("glCompileShader Error: %s %s\n", shader_src, message);
        exit(1);
    }

    return shader_id;
}

//This function reads the header data of the input file and stores the crucial parts into global variables for future use
int read_header(char input)  //take in the p type (3 or 6) as a char
{
    int i, line, endOfHeader, x;
    char a, b;
    char widthB[32], heightB[32], maxcvB[32]; //buffers to temporarily store the width, height, and max color value
    char comment[64];                         //buffer to store the comment

    a = fgetc(inputfp); //get the first two chars of the file (P#)
    b = fgetc(inputfp);
    if(a != 'P' || b != input)   //check to see if the input number matches the function input and if the P is there
    {
        fprintf(stderr, "Error: Improper header filetype.\n", 005);
        exit(1);                   //exit if the header is incorrect
    }
    a = fgetc(inputfp); //move past newline

    a = fgetc(inputfp); //get the comment # OR the first digit of the width
    if(a == '#')        //if there is a comment, move past it
    {
        while(a != '\n')  //move past comment with this loop
        {
            a = fgetc(inputfp);
        }
        a = fgetc(inputfp); //get the first digit of the width if there was a comment
    }


    i = 0;
    while(a != ' ')  //get the rest of the width
    {
        widthB[i] = a; //store it in a buffer
        a = fgetc(inputfp);
        i++;
    }
    width = atoi(widthB); //convert the buffer to an integer and store it


    a = fgetc(inputfp);  //get the first char of the height
    i = 0;
    while(a != '\n')     //get the rest of the height
    {
        heightB[i] = a;    //store it in a buffer
        a = fgetc(inputfp);
        i++;
    }
    height = atoi(heightB); //convert the buffer to an integer and store it

    a = fgetc(inputfp);  //get the first char of the max color value
    i = 0;
    while(a != '\n')     //get the rest of the max color value
    {
        maxcvB[i] = a;     //store it in a buffer
        a = fgetc(inputfp);
        i++;
    }
    maxcv = atoi(maxcvB); //convert the buffer to an integer and store it
    if(maxcv != 255)           //check to see if the max color value is not 8 bits per channel
    {
        fprintf(stderr, "Error: Color value exceeds limit.\n", 007);
        exit(1);              //exit the program if there isn't 8 bits per channel
    }

    return 1;
}

//This function reads p3 data from an input file and stores it in dynamically allocated memory, a pointer to which is passed into the function.
int read_p3(Pixel* image)
{
    int i;
    unsigned char check;
    char number[5];
    for(i=0; i < width*height; i++)  //for as many pixels in the image
    {

        fgets(number, 10, inputfp); //get the red value
        check = (char)atoi(number);       //store it in an intermediate variable
        if(check > maxcv)           //check to see if the color value is compliant with the max color value
        {
            fprintf(stderr, "Error: Color value exceeds limit.\n", 006);
            exit(1);                  //exit the program if the image's pixels don't comply with the max color value
        }
        image[i].r = check;

        fgets(number, 10, inputfp); //get the green value
        check = (char)atoi(number);       //store it in an intermediate variable
        if(check > maxcv)           //check to see if the color value is compliant with the max color value
        {
            fprintf(stderr, "Error: Color value exceeds limit.\n", 006);
            exit(1);                  //exit the program if the image's pixels don't comply with the max color value
        }
        image[i].g = check;

        fgets(number, 10, inputfp); //get the blue value
        check = (char)atoi(number);       //store it in an intermediate variable
        if(check > maxcv)           //check to see if the color value is compliant with the max color value
        {
            fprintf(stderr, "Error: Color value exceeds limit.\n", 006);
            exit(1);                  //exit the program if the image's pixels don't comply with the max color value
        }
        image[i].b = check;
    }
    return 1;
}

int simple_program()
{

    GLint link_success = 0;

    GLint program_id = glCreateProgram();
    GLint vertex_shader = simple_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLint fragment_shader = simple_shader(GL_FRAGMENT_SHADER, fragment_shader_src);





    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);

    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &link_success);

    if (link_success == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(program_id, sizeof(message), 0, &message[0]);
        printf("glLinkProgram Error: %s\n", message);
        exit(1);
    }

    return program_id;
}


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void pan(int direction)
{
    switch(direction)
    {
    case 0:
        printf("You pressed right arrow key.\n");
        Vertices[0].position[0] += 0.1;
        Vertices[1].position[0] += 0.1;
        Vertices[2].position[0] += 0.1;
        Vertices[3].position[0] += 0.1;
        break;
    case 1:
        printf("You pressed left arrow key.\n");
        Vertices[0].position[0] -= 0.1;
        Vertices[1].position[0] -= 0.1;
        Vertices[2].position[0] -= 0.1;
        Vertices[3].position[0] -= 0.1;
        break;
    case 2:
        printf("You pressed up arrow key.\n");
        Vertices[0].position[1] += 0.1;
        Vertices[1].position[1] += 0.1;
        Vertices[2].position[1] += 0.1;
        Vertices[3].position[1] += 0.1;
        break;
    case 3:
        printf("You pressed down arrow key.\n");
        Vertices[0].position[1] -= 0.1;
        Vertices[1].position[1] -= 0.1;
        Vertices[2].position[1] -= 0.1;
        Vertices[3].position[1] -= 0.1;
        break;
    default:
        printf("Something went wrong when trying to pan.\n");
        break;
    }
}

typedef float* V3;
static inline float v3_dot(V3 a, V3 b)
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void rotateImage(int direction)
{
    float theta = 22.5*(PI/180);
    float x, y;
    float rotationMatrixColA[3] = {cos(theta), sin(theta), 0};
    float rotationMatrixColB[3] = {-sin(theta), cos(theta), 0};


    switch(direction)
    {
    case 0:
        printf("You pressed W key.\n");
        x = v3_dot(Vertices[0].position, rotationMatrixColA);
        y = v3_dot(Vertices[0].position, rotationMatrixColB);
        Vertices[0].position[0] = x;
        Vertices[0].position[1] = y;

        x = v3_dot(Vertices[1].position, rotationMatrixColA);
        y = v3_dot(Vertices[1].position, rotationMatrixColB);
        Vertices[1].position[0] = x;
        Vertices[1].position[1] = y;

        x = v3_dot(Vertices[2].position, rotationMatrixColA);
        y = v3_dot(Vertices[2].position, rotationMatrixColB);
        Vertices[2].position[0] = x;
        Vertices[2].position[1] = y;

        x = v3_dot(Vertices[3].position, rotationMatrixColA);
        y = v3_dot(Vertices[3].position, rotationMatrixColB);
        Vertices[3].position[0] = x;
        Vertices[3].position[1] = y;

        break;
    case 1:
        printf("You pressed Q key.\n");
        theta = -theta;
        rotationMatrixColA[0] = cos(theta);
        rotationMatrixColA[1] = sin(theta);
        rotationMatrixColB[0] = -sin(theta);
        rotationMatrixColB[1] = cos(theta);

        x = v3_dot(Vertices[0].position, rotationMatrixColA);
        y = v3_dot(Vertices[0].position, rotationMatrixColB);
        Vertices[0].position[0] = x;
        Vertices[0].position[1] = y;

        x = v3_dot(Vertices[1].position, rotationMatrixColA);
        y = v3_dot(Vertices[1].position, rotationMatrixColB);
        Vertices[1].position[0] = x;
        Vertices[1].position[1] = y;

        x = v3_dot(Vertices[2].position, rotationMatrixColA);
        y = v3_dot(Vertices[2].position, rotationMatrixColB);
        Vertices[2].position[0] = x;
        Vertices[2].position[1] = y;

        x = v3_dot(Vertices[3].position, rotationMatrixColA);
        y = v3_dot(Vertices[3].position, rotationMatrixColB);
        Vertices[3].position[0] = x;
        Vertices[3].position[1] = y;
        break;
    default:
        printf("Something went wrong when trying to rotate.\n");
        break;
    }
}

void scaleImage(int direction)
{
    switch(direction)
    {
    case 0:
        printf("You pressed A key.\n");
        Vertices[0].position[0] *= 1.1;
        Vertices[0].position[1] *= 1.1;
        Vertices[1].position[0] *= 1.1;
        Vertices[1].position[1] *= 1.1;
        Vertices[2].position[0] *= 1.1;
        Vertices[2].position[1] *= 1.1;
        Vertices[3].position[0] *= 1.1;
        Vertices[3].position[1] *= 1.1;
        break;
    case 1:
        printf("You pressed S key.\n");
        Vertices[0].position[0] *= 1/1.1;
        Vertices[0].position[1] *= 1/1.1;
        Vertices[1].position[0] *= 1/1.1;
        Vertices[1].position[1] *= 1/1.1;
        Vertices[2].position[0] *= 1/1.1;
        Vertices[2].position[1] *= 1/1.1;
        Vertices[3].position[0] *= 1/1.1;
        Vertices[3].position[1] *= 1/1.1;
        break;
    default:
        printf("Something went wrong when trying to scale.\n");
        break;
    }
}

void shearImage(int direction)
{
    switch(direction)
    {
    case 0:
        printf("You pressed X key.\n");
        Vertices[2].position[0] += 0.1;
        Vertices[1].position[0] += 0.1;
        break;
    case 1:
        printf("You pressed Z key.\n");
        Vertices[2].position[0] -= 0.1;
        Vertices[1].position[0] -= 0.1;
        break;
    default:
        printf("Something went wrong when trying to shear.\n");
        break;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        pan(0);
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        pan(1);
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        pan(2);
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        pan(3);
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        rotateImage(1);
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
        rotateImage(0);
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        scaleImage(0);
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        scaleImage(1);
    else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        shearImage(0);
    else if (key == GLFW_KEY_X && action == GLFW_PRESS)
        shearImage(1);
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        printf("closing...");
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        printf("Resetting vertices...");
        Vertices[0].position[0] = 1;
        Vertices[0].position[1] = -1;
        Vertices[1].position[0] = 1;
        Vertices[1].position[1] = 1;
        Vertices[2].position[0] = -1;
        Vertices[2].position[1] = 1;
        Vertices[3].position[0] = -1;
        Vertices[3].position[1] = -1;
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Error: Incorrect parameter amount.\nProper input: input_filename\n\n");
        exit(1); //exit the program if there are insufficient arguments
    }
    inputfp = fopen(argv[1], "r"); //open input to read
    if (inputfp == 0)
    {
        fprintf(stderr, "Error: Input file \"%s\" could not be opened.\n", argv[1]);
        exit(1); //if the file cannot be opened, exit the program
    }
    read_header('3'); //read the header of a P3 file
    Pixel* data = calloc(width*height, sizeof(Pixel*)); //allocate memory to hold all of the pixel data
    printf("width: %d\nheight: %d\n", width, height);
    read_p3(&data[0]);
    printf("over \n");

    GLint program_id, position_slot, color_slot;
    GLuint vertex_buffer;
    GLuint index_buffer;

    glfwSetErrorCallback(error_callback);

    // Initialize GLFW library
    if (!glfwInit())
        return -1;

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create and open a window
    window = glfwCreateWindow(WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              "Mitchell Hewitt Project 5",
                              NULL,
                              NULL);

    if (!window)
    {
        glfwTerminate();
        printf("glfwCreateWindow Error\n");
        exit(1);
    }

    glfwMakeContextCurrent(window);



    GLuint myTexture;
    glGenTextures(1, &myTexture);
    glBindTexture(GL_TEXTURE_2D, myTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);


    program_id = simple_program();

    glUseProgram(program_id);

    position_slot = glGetAttribLocation(program_id, "Position");
    color_slot = glGetAttribLocation(program_id, "SourceColor");
    assert(position_slot != -1);
    assert(color_slot != -1);
    glEnableVertexAttribArray(position_slot);
    glEnableVertexAttribArray(color_slot);


    GLint texCoordSlot = glGetAttribLocation(program_id, "TexCoordIn");
    assert(texCoordSlot != -1);
    glEnableVertexAttribArray(texCoordSlot);
    GLint textureUniform = glGetUniformLocation(program_id, "Texture");
    assert(textureUniform != -1);


    // Create Buffer
    glGenBuffers(1, &vertex_buffer);

    // Map GL_ARRAY_BUFFER to this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // Send the data
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Repeat
    while (!glfwWindowShouldClose(window))
    {

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

        glClearColor(255.0/255.0, 20.0/255.0, 147.0/255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        glVertexAttribPointer(position_slot,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(Vertex),
                              0);

        glVertexAttribPointer(color_slot,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(Vertex),
                              (GLvoid*) (sizeof(float) * 3));

        glVertexAttribPointer(texCoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), sizeof(float)*7);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, myTexture);
        glUniform1i(textureUniform, 0);
        glDrawElements(GL_TRIANGLES,
                       sizeof(Indices) / sizeof(GLubyte),
                       GL_UNSIGNED_BYTE, 0);

        glfwSwapBuffers(window);
        glfwSetKeyCallback(window, key_callback);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
