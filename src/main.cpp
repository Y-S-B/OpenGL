#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

// Game constants
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const int BOARD_SIZE = 3;

// Game state
char board[BOARD_SIZE][BOARD_SIZE] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
char currentPlayer = 'X';
bool gameOver = false;
int moveCount = 0;

// Winning line state
int winRow1 = -1, winCol1 = -1, winRow2 = -1, winCol2 = -1;

// Button state
bool buttonHovered = false;

// Shader sources
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0);\n"
    "}\n\0";

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void renderBoard(unsigned int shaderProgram);
void drawX(float x, float y, unsigned int shaderProgram);
void drawO(float x, float y, unsigned int shaderProgram);
void drawGrid(unsigned int shaderProgram);
void drawWinningLine(int row1, int col1, int row2, int col2, unsigned int shaderProgram);
void drawButton(unsigned int shaderProgram);
void checkWin();
void resetGame();

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tic-Tac-Toe", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Build and compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        drawGrid(shaderProgram);
        renderBoard(shaderProgram);

        // Draw the winning line if there is a win
        if (gameOver && winRow1 != -1)
            drawWinningLine(winRow1, winCol1, winRow2, winCol2, shaderProgram);

        // Draw the restart button
        drawButton(shaderProgram);

        // Update window title if game is over
        if (gameOver)
        {
            if (moveCount == BOARD_SIZE * BOARD_SIZE)
                glfwSetWindowTitle(window, "Tic-Tac-Toe - Draw! Click Restart or press R to restart.");
            else
            {
                std::string title = "Tic-Tac-Toe - Player ";
                title += currentPlayer;
                title += " Wins! Click Restart or press R to restart.";
                glfwSetWindowTitle(window, title.c_str());
            }
        }
        else
        {
            glfwSetWindowTitle(window, "Tic-Tac-Toe");
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        resetGame();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Check if cursor is over the button
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    
    // Convert button position from normalized coordinates to screen coordinates
    float buttonX1 = (0.7f + 1.0f) * 0.5f * windowWidth;  // -0.2 in normalized -> 0.4 in screen
    float buttonY1 = (0.85f + 1.0f) * 0.5f * windowHeight; // -0.95 in normalized -> 0.025 in screen
    float buttonX2 = (0.9f + 1.0f) * 0.5f * windowWidth;   // 0.2 in normalized -> 0.6 in screen
    float buttonY2 = (0.95f + 1.0f) * 0.5f * windowHeight; // -0.85 in normalized -> 0.075 in screen
    
    buttonHovered = (xpos >= buttonX1 && xpos <= buttonX2 && ypos >= buttonY1 && ypos <= buttonY2);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (buttonHovered)
        {
            resetGame();
            return;
        }
        
        if (!gameOver)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            float cellWidth = windowWidth / (float)BOARD_SIZE;
            float cellHeight = windowHeight / (float)BOARD_SIZE;

            int col = xpos / cellWidth;
            int row = ypos / cellHeight;

            if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == ' ')
            {
                board[row][col] = currentPlayer;
                moveCount++;
                checkWin();
                if (!gameOver) currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }
    }
}

void renderBoard(unsigned int shaderProgram)
{
    float cellWidth = 2.0f / BOARD_SIZE;
    float cellHeight = 2.0f / BOARD_SIZE;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            float centerX = -1.0f + cellWidth / 2 + j * cellWidth;
            float centerY = 1.0f - cellHeight / 2 - i * cellHeight;

            if (board[i][j] == 'X')
                drawX(centerX, centerY, shaderProgram);
            else if (board[i][j] == 'O')
                drawO(centerX, centerY, shaderProgram);
        }
    }
}

void drawGrid(unsigned int shaderProgram)
{
    float lineVertices[] = {
        // Vertical lines
        -0.33f, -1.0f,
        -0.33f,  1.0f,
         0.33f, -1.0f,
         0.33f,  1.0f,
        // Horizontal lines
        -1.0f, -0.33f,
         1.0f, -0.33f,
        -1.0f,  0.33f,
         1.0f,  0.33f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform3f(vertexColorLocation, 0.0f, 0.0f, 0.0f);

    glDrawArrays(GL_LINES, 0, 8);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void drawX(float x, float y, unsigned int shaderProgram)
{
    float size = 0.2f;
    float vertices[] = {
        x - size, y - size,
        x + size, y + size,
        x - size, y + size,
        x + size, y - size
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform3f(vertexColorLocation, 1.0f, 0.0f, 0.0f); // Red

    glLineWidth(4.0f);
    glDrawArrays(GL_LINES, 0, 4);
    glLineWidth(1.0f);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void drawO(float x, float y, unsigned int shaderProgram)
{
    const int segments = 32;
    float radius = 0.2f;
    std::vector<float> vertices;

    for (int i = 0; i < segments; i++)
    {
        float angle = 2.0f * 3.1415926f * float(i) / float(segments);
        vertices.push_back(x + radius * cosf(angle));
        vertices.push_back(y + radius * sinf(angle));

        angle = 2.0f * 3.1415926f * float(i+1) / float(segments);
        vertices.push_back(x + radius * cosf(angle));
        vertices.push_back(y + radius * sinf(angle));
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform3f(vertexColorLocation, 0.0f, 0.0f, 1.0f); // Blue

    glLineWidth(4.0f);
    glDrawArrays(GL_LINES, 0, segments * 2);
    glLineWidth(1.0f);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void drawWinningLine(int row1, int col1, int row2, int col2, unsigned int shaderProgram)
{
    float cellWidth = 2.0f / BOARD_SIZE;
    float cellHeight = 2.0f / BOARD_SIZE;

    float x1 = -1.0f + cellWidth / 2 + col1 * cellWidth;
    float y1 = 1.0f - cellHeight / 2 - row1 * cellHeight;
    float x2 = -1.0f + cellWidth / 2 + col2 * cellWidth;
    float y2 = 1.0f - cellHeight / 2 - row2 * cellHeight;

    float vertices[] = {
        x1, y1,
        x2, y2
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform3f(vertexColorLocation, 0.0f, 1.0f, 0.0f); // Green

    glLineWidth(5.0f); // Make it thick
    glDrawArrays(GL_LINES, 0, 2);
    glLineWidth(1.0f); // Reset to default

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void drawButton(unsigned int shaderProgram)
{
    // Button coordinates (centered at bottom, below the grid)
    float buttonVertices[] = {
        // Button background (rectangle)
        -0.2f, -0.95f,
         0.2f, -0.95f,
         0.2f, -0.85f,
        -0.2f, -0.85f,
        
        // R letter (for "Restart")
        -0.18f, -0.89f, -0.18f, -0.87f,  // R vertical line
        -0.18f, -0.87f, -0.15f, -0.87f,  // R top horizontal
        -0.18f, -0.88f, -0.15f, -0.88f,   // R middle horizontal
        -0.15f, -0.88f, -0.15f, -0.89f,  // R diagonal
        
        // E letter
        -0.14f, -0.89f, -0.14f, -0.87f,   // E vertical
        -0.14f, -0.87f, -0.11f, -0.87f,   // E top
        -0.14f, -0.88f, -0.11f, -0.88f,   // E middle
        -0.14f, -0.89f, -0.11f, -0.89f,   // E bottom
        
        // S letter
        -0.10f, -0.87f, -0.07f, -0.87f,   // S top
        -0.10f, -0.87f, -0.10f, -0.88f,   // S left top
        -0.10f, -0.88f, -0.07f, -0.88f,   // S middle
        -0.07f, -0.88f, -0.07f, -0.89f,  // S right bottom
        -0.10f, -0.89f, -0.07f, -0.89f,   // S bottom
        
        // T letter
        -0.06f, -0.87f, -0.03f, -0.87f,  // T top
        -0.045f, -0.87f, -0.045f, -0.89f, // T vertical
        
        // A letter
        -0.02f, -0.89f, -0.02f, -0.87f,  // A left
        -0.02f, -0.87f, 0.01f, -0.87f,    // A top
        0.01f, -0.87f, 0.01f, -0.89f,    // A right
        -0.02f, -0.88f, 0.01f, -0.88f,    // A middle
        
        // R letter (second)
        0.02f, -0.89f, 0.02f, -0.87f,    // R vertical
        0.02f, -0.87f, 0.05f, -0.87f,     // R top
        0.02f, -0.88f, 0.05f, -0.88f,     // R middle
        0.05f, -0.88f, 0.05f, -0.89f,     // R diagonal
        
        // T letter (second)
        0.06f, -0.87f, 0.09f, -0.87f,     // T top
        0.075f, -0.87f, 0.075f, -0.89f   // T vertical
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buttonVertices), buttonVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Draw button background
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    if (buttonHovered)
        glUniform3f(vertexColorLocation, 0.8f, 0.8f, 0.8f); // Light gray when hovered
    else
        glUniform3f(vertexColorLocation, 0.9f, 0.9f, 0.9f); // Light gray

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Draw button border
    glUniform3f(vertexColorLocation, 0.0f, 0.0f, 0.0f); // Black
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    // Draw "RESTART" text (each letter is 2-5 line segments)
    glUniform3f(vertexColorLocation, 0.0f, 0.0f, 0.0f); // Black
    glLineWidth(2.0f);
    
    // Draw each letter (each letter is composed of line segments)
    // R (4 lines)
    glDrawArrays(GL_LINES, 4, 8);
    // E (4 lines)
    glDrawArrays(GL_LINES, 12, 8);
    // S (5 lines)
    glDrawArrays(GL_LINES, 20, 10);
    // T (2 lines)
    glDrawArrays(GL_LINES, 30, 4);
    // A (5 lines)
    glDrawArrays(GL_LINES, 34, 10);
    // R (4 lines)
    glDrawArrays(GL_LINES, 44, 8);
    // T (2 lines)
    glDrawArrays(GL_LINES, 52, 4);
    
    glLineWidth(1.0f);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void checkWin()
{
    // Check rows
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
        {
            gameOver = true;
            winRow1 = i; winCol1 = 0;
            winRow2 = i; winCol2 = 2;
            return;
        }
    }

    // Check columns
    for (int j = 0; j < BOARD_SIZE; j++)
    {
        if (board[0][j] != ' ' && board[0][j] == board[1][j] && board[1][j] == board[2][j])
        {
            gameOver = true;
            winRow1 = 0; winCol1 = j;
            winRow2 = 2; winCol2 = j;
            return;
        }
    }

    // Check diagonals
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        gameOver = true;
        winRow1 = 0; winCol1 = 0;
        winRow2 = 2; winCol2 = 2;
        return;
    }

    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        gameOver = true;
        winRow1 = 0; winCol1 = 2;
        winRow2 = 2; winCol2 = 0;
        return;
    }

    // Check for draw
    if (moveCount == BOARD_SIZE * BOARD_SIZE)
    {
        gameOver = true;
        winRow1 = winCol1 = winRow2 = winCol2 = -1;
    }
}

void resetGame()
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board[i][j] = ' ';
        }
    }
    currentPlayer = 'X';
    gameOver = false;
    moveCount = 0;
    winRow1 = winCol1 = winRow2 = winCol2 = -1;
} 