// dodajemy niezbędne bilbioteki
#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c do malloc i free
#include <string.h> // bilbioteka do napisów w c
#include <sys/stat.h> // uzyskiwanie informacji o pliku
#include <unistd.h> // unix standar close/read/write/fork/sleep/exec
#include <fcntl.h> // open O_RDONLY  O_CREAT
#include <dirent.h> // opendir readdir closedir otwieranie zamykanie i czytanie z katalgoów

#include <sys/mman.h>

int czy_katalog(char *path) // funkcja sprawdzajacy czy sciezka wskazuje na katalog
{
    struct stat info; // stat przechowuje informcje o pliku/katalogu
    stat(path, &info); // pobranie informacji o pliku ze sciezki do zmiennej s
    return S_ISDIR(info.st_mode); // sprawdzenia czy podana sciezka to katalog 
}

void kopiuj(char *a,char *b) //funkcja kopiowania
{
    int in = open(a,O_RDONLY); // otwieranie pliku wejsciowego
    int out = open(b,O_WRONLY|O_CREAT,0644); // tworzenie pliku wyjsciowego
    int size = 2048; //ustalenie rozmiaru buforu
    char *buffor = (char *)malloc(size); //rezerwowanie pamięci na buffor
    while(1)
    {
        int r = read(in,buffor,size); //odczyt cześci pliku
        write(out,buffor,r);    // zapis czesci pliku
        if(r < size) //jeśli odczytano cały plik to break
            break;
    }
    printf("Kopia read/write : %s -> %s\n", a, b); // wypisz na ekranie informacje o kopiowaniu
    free(buffor); //zwolnij buffor
    close(in); //zamknij plik wejsciowy
    close(out); // zamkinj plik wyjsciowy
}

void kopiujMap(char *a, char *b) // funkcja kopiowania
{
    int in = open(a, O_RDONLY); // otwieranie pliku wejściowego
    int out = open(b, O_WRONLY | O_CREAT | O_TRUNC, 0644); // tworzenie pliku docelowego

    struct stat info; // zmienna na informacje o pliku
    fstat(in, &info); // pobranie informacji o pliku źródłowym
    long int size = info.st_size; // pobranie rozmiaru pliku
    
    char *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, in, 0); // mapowanie pliku źródłowego (bezpośredni dostęp do danych z pamieci operacyjnej)  // NULL - system sam wybiera adres w pamieci, PROT_READ - dostęp do oczytu, MAP_PRIVATE - mapowanie prywatne, 0 - przesunięcie
    write(out, map, size); // zapisanie zmapowanego pliku do pliku docelowego // map - wskaznik do daych ktore maja byc zapisane
    printf("Kopia mmap/write: %s -> %s\n", a, b); // wypisanie informacji o kopiowaniu
    
    munmap(map, size); // zwolnienie zmapowanej pamięci
    close(in); // zamknięcie pliku źródłowego
    close(out); // zamknięcie pliku docelowego
}

void synchronizuj(char *a,char *b,long int prog) 
{
    DIR *in = opendir(a); // otwarcie katalogu a jako dir
    DIR *out  = opendir(b);// otwarcie katalogu b jako dir
    
    struct dirent *elem  = readdir(in); // wskaźnik elemnet w katalogu
    while (elem != NULL) // pętla przez wszystki elementy katalogu a
    { 
        if (elem->d_type == DT_REG) // jak jest plikiem to kopiuj
        { 
            char inPath[1000]; // ścieżka do elementu w katalogu a
            char outPath[1000];// ścieżka do elementu w katalogu b

            snprintf(inPath, sizeof(inPath), "%s/%s", a, elem->d_name); // połącz nazwe pliku ze pełną ścieżką
            snprintf(outPath, sizeof(outPath), "%s/%s", b, elem->d_name); // połącz nazwe pliku ze pełną ścieżką

            struct stat info; // zmeinna na informacje o pliku
            stat(inPath, &info);// pobranie informacji o pliku
            long int size = info.st_size; //  pobranie rozmiaru
            printf("Wielkosc pliku: %s wynosi: %ld bajtow \n",inPath,size);

            if(size > prog) // jeżeli plik większy niż podany w argumencie wtedy
                kopiujMap(inPath, outPath); //funkcja kopiuj mmap/write
            else
                kopiuj(inPath, outPath); //funkcja kopiuj read/write
        }
        elem  = readdir(in); // sprawdza nastepny element
    }

    closedir(in); // zamkinj katalog a
    closedir(out); // zamknij katalog b
}

int main(int count, char * arg[]) // count - liczba argumentów , arg - tablica zawierająca argumenty
{
    long int prog = 2000000; // domyślny próg rozmiaru do kopiowania
    if(count==4)
        prog = atol(arg[3]); // przypisanie własnego progu 

    else if(count!=3) // sprawdzenie ilości argumentów, przerwanie jeżeli jest ich za mało lub za dużo
    { 
        printf("Podano za malo lub za duzo argumentow !!!\n");
        return 0;
    }
    // przypisanie pobraych argumentów do zmienncyh
    char *a = arg[1];
    char *b = arg[2];

    // sprawdzenie czy pierwszy argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(a)==0)
    {
        printf("Pierwszy argument: %s  nie jest katalogiem !!!\n",a);
        return 0;
    }
    printf("Katalog 1: %s \n",a);

    // sprawdzenie czy drugi argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(b)==0)
    {
        printf("Drugi argument: %s nie jest katalogiem !!!\n",b);
        return 0;
    }
    printf("Katalog 2: %s \n",b);

    //wywołanie funkcji do synchronizacji
    synchronizuj(a,b,prog);
    
    // jak wszystko ok to komunikat
    printf("Zsynchronizowano pomyślnie !!!\n");
}