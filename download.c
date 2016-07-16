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

#include <curl/curl.h>

static int downloadData(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded);
static size_t save_data(void *ptr, size_t size, size_t nmemb, void *buffer_dl);
static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE* input);

static double DOWNLOAD_SPEED;
static double FILE_EXPECTED_SIZE;
static double CURRENT_FILE_SIZE;

/**************************************************************
**                                                           **
**  Repertoire[0] == 0 => download into repertoire variable  **
**                                                           **
**************************************************************/

int download(char *adresse, char *repertoire)
{
    CURL *curl;
    CURLcode res; //Get return from download
    FILE* fichier = NULL;

    if(repertoire[0])
        fichier = fopen(repertoire, "wb");

    curl = curl_easy_init();

    if(curl && (fichier != NULL || !repertoire[0]))
    {
        curl_easy_setopt(curl, CURLOPT_URL, adresse); //URL
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, downloadData);
        if(fichier == NULL)
        {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, repertoire);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_data);
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fichier);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        }
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) //Si problème
        {
            printf("Failed at download anything =<\n");
            exit(0);
        }
        if(fichier != NULL)
      	  fclose(fichier);
    }
    return 0;
}

static int downloadData(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
    if(TotalToDownload == 0 || NowDownloaded > TotalToDownload)
        return 0;
    if((CURRENT_FILE_SIZE * 100 / FILE_EXPECTED_SIZE) != (NowDownloaded * 100 / TotalToDownload)) //Taille dl / temps mis x nombre de fois le temps mis dans une sec / 1024 pour retour au ko
    {
        int x = 0, pourcent = NowDownloaded * 100 / TotalToDownload;

        // Show the percentage complete.
        printf("\r%d%% [", pourcent);

        // Show the load bar.
        for (; x < pourcent / 4; x++)
           printf("=");

        for (; x < 25; x++)
           printf(" ");

        printf("]"); // ANSI Control codes to go back to the previous line and clear it.
    }
    FILE_EXPECTED_SIZE = TotalToDownload;
    DOWNLOAD_SPEED = (NowDownloaded - CURRENT_FILE_SIZE) / 1024;
    CURRENT_FILE_SIZE = NowDownloaded;
    return 0;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE* input)
{
    return fwrite(ptr, size, nmemb, input);
}

static size_t save_data(void *ptr, size_t size, size_t nmemb, void *buffer_dl)
{
    char *buffer = buffer_dl;
    int tailleBuffer = (int)buffer+1 * (int)buffer+2 * (int)buffer+3; //We get variable size
    if(size * nmemb == 0) //Nothing to write
        return 0;

    else if(size * nmemb < tailleBuffer) //Buffer is big enough
    {
        strcpy(buffer, ptr);
        if(buffer[strlen(buffer)])
            buffer[strlen(buffer)] = 0;
    }
    else //Troncated
    {
        int i = 0;
        char *output = ptr;
        for(; i < tailleBuffer; i++)
            buffer[i] = output[i];
    }
    return size*nmemb;
}

