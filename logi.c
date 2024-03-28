#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>


void wpisz_do_log(char* tekst)
{
    openlog("nasz demon", LOG_PID, LOG_DAEMON); // otwieramy log
    time_t rawtime; // pobieramy aktualny czas
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char do_log[200]; //zmienna na napis
    sprintf(do_log, "[%04d-%02d-%02d %02d:%02d:%02d] %s", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tekst);// wpisujemy do stringa date godzine i wiadomosc
    syslog(LOG_INFO, "%s",do_log); // zapisujemy stworzonego stringa w logu 
    closelog(); // zamykamy log
}

int main(int count, char* arg[])
{

    


    char message[200]; // Zakładam maksymalną długość 200 znaków, dostosuj rozmiar według potrzeb

    // Pobierz aktualny czas
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char text[] = "Tekst do dopisania";
    // Sformatuj wiadomość z datą, godziną i treścią
    sprintf(message, "[%04d-%02d-%02d %02d:%02d:%02d] %s", 
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, text);

    // Wpisz sformatowaną wiadomość do sysloga
    syslog(LOG_INFO, "%s", message);


    printf("Poszło \n");

    // Zamknij log systemowy
    closelog();

}