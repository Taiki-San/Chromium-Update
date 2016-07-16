/*************************************************************************
**                                              						**
**					Code written (and poorly commented)					**
**                                              						**
**					 By Taiki <taiki@taiki.fr> in 2012					**
**                                              						**
**  This Source Code Form is subject to the terms of the Mozilla Public	**
**	License, v. 2.0. If a copy of the MPL was not distributed with this	**
**	file, You can obtain one at https://mozilla.org/MPL/2.0/.			**
**                                              						**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <string.h>
#include <windows.h>

#define VERSION_FILE_RAW "AppData\\Local\\Chromium\\Application\\version.txt"
#define BINARY_DIRECTORY_RAW "AppData\\Local\\Chromium\\Application\\update.exe"

char *VERSION_FILE = NULL;
char *BINARY_DIRECTORY = NULL;

int update();
int download(char *adresse, char *repertoire);
void updateVersion(char version[]);
char keyPressed ();

int main(int argc, char *argv[])
{
    /*We generate the path, code Windows only*/
    int i = 0, nombreBackSlash = 0;
    char *directory = NULL;
    if(argc == 0)
    {
        directory = malloc(250);
        getcwd(directory, 250);
    }
    else
    {
        directory = malloc(strlen(argv[0]));
        strcpy(directory, argv[0]); //We get the path
    }


    for(; i < 250 && directory[i] != 0; i++)
    {
        if(directory[i] == '\\')
            nombreBackSlash++;
    }

    if(nombreBackSlash >= 3 && directory[3] == 'U' && directory[4] == 's' && directory[5] == 'e' && directory[6] == 'r' && directory[7] == 's' && directory[8] == '\\' && directory[9] != 0)
    {
        char temp[250];

        for(i = nombreBackSlash = 0; directory[i] != 0 && i < 250 && nombreBackSlash < 3; i++)
        {
            temp[i] = directory[i];
            if(directory[i] == '\\')
                nombreBackSlash++;
        }
        temp[i] = 0;

        VERSION_FILE = malloc(strlen(temp) + strlen(VERSION_FILE_RAW));
        BINARY_DIRECTORY = malloc(strlen(temp) + strlen(BINARY_DIRECTORY_RAW));

        sprintf(VERSION_FILE, "%s%s", temp, VERSION_FILE_RAW);
        sprintf(BINARY_DIRECTORY, "%s%s", temp, BINARY_DIRECTORY_RAW);
    }
    else
    {
        printf("File isn't located in the good folder =< put it in AppData\n");
        printf("%s\n", directory);
        system("whoami");
        keyPressed();
        free(directory);
        return 0;
    }

    update();
    free(directory);
    free(VERSION_FILE);
    free(BINARY_DIRECTORY);
    return 0;
}

int update()
{
    char buildNumber[100] = {0, 100, 1, 1}; //Check download.c for details

    printf("Checking current version number\n\n");
    download("http://commondatastorage.googleapis.com/chromium-browser-continuous/Win/LAST_CHANGE", buildNumber);

    printf("\n\nCurrent build is %s. Checking installed version.\n", buildNumber);

    FILE *versionFile = NULL;
    versionFile = fopen(VERSION_FILE, "r");

    if(versionFile == NULL)
    {
        printf("First Run :o\n");
        updateVersion(buildNumber);
    }
    else
    {
        char previousBuild[100];
        fscanf(versionFile, "%s", previousBuild);
        if(previousBuild[0] && !strcmp(previousBuild, buildNumber)) //Chromium is up to date
        {
            printf("No update available :D\n");
            return 0;
        }
        fclose(versionFile);
        updateVersion(buildNumber);
    }
    printf("Update needed :(\n");

    //At this state, we have to upgrade Chromium

    //We dynamically allocate URL in case on very long buildNumber
    char* URL = malloc(10 + strlen(buildNumber) + strlen("http://commondatastorage.googleapis.com/chromium-browser-continuous/Win/%s/mini_installer.exe"));
    sprintf(URL, "http://commondatastorage.googleapis.com/chromium-browser-continuous/Win/%s/mini_installer.exe", buildNumber);

    printf("Downloading the update.\n\n");
    remove(BINARY_DIRECTORY);
    download(URL, BINARY_DIRECTORY); //We download the update

    printf("\nInstalling it, cya!");
    ShellExecute(NULL, "open", BINARY_DIRECTORY, NULL, NULL, SW_SHOWNOACTIVATE);
    //We're done =)

    return 1;
}

void updateVersion(char version[])
{
    FILE *versionFile = NULL;
    versionFile = fopen(VERSION_FILE, "w+");
    if(versionFile != NULL)
    {
        fprintf(versionFile, version);
        fclose(versionFile);
    }
}

char keyPressed ()
{
    //Detect <when a key is pressed (w/o enter)
    HANDLE hStdin = GetStdHandle (STD_INPUT_HANDLE);
    INPUT_RECORD irInputRecord;
    DWORD dwEventsRead;
    char cChar;

    while(ReadConsoleInputA (hStdin, &irInputRecord, 1, &dwEventsRead)) /* Read key press */
        if (irInputRecord.EventType == KEY_EVENT
            &&irInputRecord.Event.KeyEvent.wVirtualKeyCode != VK_SHIFT
            &&irInputRecord.Event.KeyEvent.wVirtualKeyCode != VK_MENU
            &&irInputRecord.Event.KeyEvent.wVirtualKeyCode != VK_CONTROL)
        {
            cChar = irInputRecord.Event.KeyEvent.uChar.AsciiChar;
            ReadConsoleInputA (hStdin, &irInputRecord , 1, &dwEventsRead); /* Read key release */
            return cChar;
        }
    return EOF;
}

