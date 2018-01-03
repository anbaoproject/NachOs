#include "syscall.h"

int main()
{

    SpaceId newProc;
    char buffer[64];
    int i = 0;
    char *exit_string = "exit\0";
    char *proDir = "./test/";
    CreateSemaphore("shell", 2);

    while (1)
    {
        PrintString("Ten chuong trinh: ");
        ReadString(buffer, 64);

        for (i = 0; i < 5; i++)
        {
            if (buffer[i] != exit_string[i])
            {
                break;
            }
        }
        if (i == 5)
            break;

        int isBG = 0;
        int startI = 0;
        if (buffer[0] == '&')
        {
            isBG = 1;
            startI = 1;
        }

        for (i = 7; i < 7 + 64 && buffer[i - 7 + startI] != '\0'; i++)
            proDir[i] = buffer[i - 7 + startI];
        proDir[i] = '\0';

        newProc = Exec(proDir);

        if (newProc >= 0 && isBG == 0)
        {
            Join(newProc);
        }
    }
}

// SpaceId newProc;
// OpenFileId input = ConsoleInput;
// OpenFileId output = ConsoleOutput;
// char prompt[2], ch, buffer[60];
// int i;

// prompt[0] = '-';
// prompt[1] = '-';

// while( 1 )
// {
// Write(prompt, 2, output);

// i = 0;

// do {

//     Read(&buffer[i], 1, input);

// } while( buffer[i++] != '\n' );

// buffer[--i] = '\0';

// if( i > 0 ) {
// 	newProc = Exec(buffer);
// 	Join(newProc);
// }
// }
}
