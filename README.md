# 🎮 OpenGL Tic-Tac-Toe Game

## 👨‍💻 Team Member
**Name:** Lud Amare  UGR/0331/15
**Name:** Sekina Murad  UGR/3475/15
**Name:** Yanet Sisay  UGR/9157/15
**Name:** Sofia Yasin  UGR/7314/15
**Name:** Yasmin Anwar  UGR/7449/15

## 📌 Project Overview

This is a simple interactive Tic-Tac-Toe game implemented using C++ and OpenGL (GLFW and GLAD). The game renders a 3x3 grid, allows two players to take turns placing Xs and Os, checks for wins or draws, and displays a visual winning line. A restart button is also included in the UI for easy game reset.

---

## 🛠️ Technologies Used

- **C++**
- **OpenGL**
- **GLFW** – for window and input handling
- **GLAD** – for OpenGL function loading
- **stb_easy_font** *(optional)* – can be added for text rendering

---

## 🧠 What We Did (Implementation Summary)

1. **Window Initialization:**
   - Used GLFW to create an 800x800 OpenGL window.
   - GLAD was initialised to load OpenGL function pointers.

2. **Shaders:**
   - Wrote minimal vertex and fragment shaders to colour game elements dynamically.

3. **Grid Drawing:**
   - Used OpenGL lines to draw a 3x3 grid.

4. **X and O Rendering:**
   - `X`: Two red diagonal lines.
   - `O`: Circular blue lines drawn with triangle segments.

5. **Game Logic:**
   - Mouse clicks are processed to place pieces and determine the game state (win/draw).
   - Win detection includes rows, columns, and diagonals.
   - A green line is drawn across the winning combination.

6. **Restart Button:**
   - A gray rectangle acts as a restart button at the bottom of the screen.
   - Clicking it or pressing `R` resets the board.

---

## 📷 Screen Capture

https://github.com/user-attachments/assets/4ba3638f-8940-460c-af1a-ad02842876dc

*This video shows the gameplay, win detection, and restart button in action.*

---

## 🔗 GitHub Repository

[🔗 GitHub Repository Link](https://github.com/Y-S-B/OpenGL.git)

---

## ▶️ How to Run

1. Make sure you have a C++ compiler and the following libraries:
   - OpenGL
   - GLFW
   - GLAD

2. Compile the code using:

   ```bash
   g++ -o tictactoe main.cpp -lglfw -ldl -lGL

3. Run the executable:

  ```bash
  ./tictactoe
