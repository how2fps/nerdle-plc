#ifndef MY_PROJECT_CONIO_H
#define MY_PROJECT_CONIO_H

#ifdef _WIN32
    #include <conio.h>
    int _getch(void); 
    #define getch _getch
#else
    #include <stdio.h>
    #include <termios.h>
    #include <unistd.h>

    static int getch(void)
    {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

#endif