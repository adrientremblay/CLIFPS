#include <iostream>
using namespace std;

#include <Windows.h>

int screenWidth = 120;
int screenHeight = 40;

float playerX = 8.0f;
float playerY = 8.0f;
float playerA = 0.0f;

int mapHeight = 16;
int mapWidth = 16;

float FOV = 3.14159f / 4.0f;

float depth = 16.0f;

int main()
{
    wchar_t* screen = new wchar_t[screenWidth * screenHeight];
    HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(console);
    DWORD bytesWritten = 0;

    wstring map;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    // game loop
    while (true)
    {
        for (int x = 0; x < screenWidth; x++) {
            // for each column, calculate the projected ray angle into world space
            float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)screenWidth) * FOV;

            // calculating distance to wall
            float distanceToWall = 0;
            bool hitWall = false;
            while (!hitWall && distanceToWall < depth) {
                distanceToWall += 0.1f;

				// creating unit vector of ray angle
				float eyeX = sinf(rayAngle);
				float eyeY = cosf(rayAngle);

				int testX = (int)(playerX + eyeX * distanceToWall);
				int testY = (int)(playerY + eyeY * distanceToWall);

				// test if ray is out of bounds
				if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight) {
					hitWall = true;
					distanceToWall = depth;
				}
				else {
					// test if ray cell is a wall block
					if (map[testY * mapWidth + testX] == '#') {
						hitWall = true;
					}
				}
            }

            // calculating distance to ceiling and floor
            int ceiling = (float)(screenHeight / 2.0f) - screenHeight / ((float)distanceToWall);
            int floor = screenHeight - ceiling;

            for (int y = 0; y < screenHeight; y++) {
                if (y < ceiling) 
                    screen[y * screenWidth + x] = ' ';
                else if (y > ceiling && y <= floor)
                    screen[y * screenWidth + x] = '#';
                else 
                    screen[y * screenWidth + x] = ' ';
            }
        }

		screen[screenWidth * screenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
    }

    return 0;
}
