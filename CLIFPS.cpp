#include <iostream>
#include <chrono>
#include <Windows.h>
using namespace std;

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

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();


    // game loop
    while (true)
    {
        // smoothing frames
		tp2 = chrono::system_clock::now();
        chrono::duration<float> timeDifference = tp2 - tp1;
        tp1 = tp2;
        float elapsedTime = timeDifference.count();

        // controls
        // handle rotation
        float angularSpeed = 0.1f;
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            playerA -= (0.5f) * elapsedTime;
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            playerA += (0.5f) * elapsedTime;
        // handle movement
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            playerX += cosf(playerA) * 5.0f * elapsedTime;
            playerY += sinf(playerA) * 5.0f * elapsedTime;
        }
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            playerX -= cosf(playerA) * 5.0f * elapsedTime;
            playerY -= sinf(playerA) * 5.0f * elapsedTime;
        }

        for (int x = 0; x < screenWidth; x++) {
            // for each column, calculate the projected ray angle into world space
            float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)screenWidth) * FOV;

            float distanceToWall = 0;
            bool hitWall = false;
            while (!hitWall && distanceToWall < depth) {
                // calculating distance to wall by incrementing each loop
                distanceToWall += 0.1f;

				// creating unit X and Y projections of ray angle
				float eyeX = cosf(rayAngle);
				float eyeY = sinf(rayAngle);

                // creating test coordinates on the map 
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
            int ceiling = (int)((float)(screenHeight / 2.0f) - screenHeight / ((float)distanceToWall));
            int floor = screenHeight - ceiling;

            short shade;
            if (distanceToWall <= depth / 4.0f)
                shade = 0x2588;
            else if (distanceToWall <= depth / 3.0f)
                shade = 0x2593;
            else if (distanceToWall <= depth / 2.0f)
                shade = 0x2592;
            else if (distanceToWall <= depth)
                shade = 0x2591;
            else
                shade = ' ';

            for (int y = 0; y < screenHeight; y++) {
                if (y < ceiling) 
                    screen[y * screenWidth + x] = ' ';
                else if (y > ceiling && y <= floor)
                    screen[y * screenWidth + x] = shade;
                else 
                    screen[y * screenWidth + x] = ' ';
            }
        }

		screen[screenWidth * screenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
    }

    return 0;
}
