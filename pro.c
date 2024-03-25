// dodajemy niezbędne bilbioteki
#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c do malloc i free
#include <string.h> // bilbioteka do napisów w c
#include <sys/stat.h> // uzyskiwanie informacji o pliku
#include <unistd.h> // unix standar close/read/write/fork/sleep/exec
#include <fcntl.h> // open O_RDONLY  O_CREAT
#include <dirent.h> // opendir readdir closedir otwieranie zamykanie i czytanie z katalgoów
#include <sys/mman.h> // mapowanie plików mmap


#include <utime.h> // zmiana czasu modyfikacji pliku

int czy_katalog(char *path) // funkcja sprawdzajacy czy sciezka wskazuje na katalog
{
    struct stat info; // stat przechowuje informcje o pliku/katalogu
    stat(path, &info); // pobranie informacji o pliku ze sciezki do zmiennej s
    if(S_ISDIR(info.st_mode)==0)// sprawdzenia czy podana sciezka to katalog 
        return 0; 
    else
        return 1;
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

    struct stat info;// zmienna na informacje o pliku
    fstat(in, &info);// pobranie informacji o pliku źródłowym
    struct utimbuf time; // zmienna do przechowywania informacji o czasie modyfikacji pliku
    time.modtime = info.st_mtime; // wstawienie do zmiennej daty modyfikacji pliku z katalogu zrodlowego
    utime(b, &time); // aktualizacja czasu modyfkicaji pliku dla pliku wyjsciowego (kopii)

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
    
    struct utimbuf time; // zmienna do przechowywania informacji o czasie modyfikacji pliku
    time.modtime = info.st_mtime; // wstawienie do zmiennej daty modyfikacji pliku z katalogu zrodlowego
    utime(b, &time); // aktualizacja czasu modyfkicaji pliku dla pliku wyjsciowego (kopii)

    munmap(map, size); // zwolnienie zmapowanej pamięci
    close(in); // zamknięcie pliku źródłowego
    close(out); // zamknięcie pliku docelowego
}

void synchronizuj(char *a, char *b, long int prog, int R)
{
    DIR *in = opendir(a); // otwarcie katalogu a jako dir
    DIR *out = opendir(b); // otwarcie katalogu b jako dir

    struct dirent *elem = readdir(in); // wskaźnik elemnet w katalogu
    while (elem != NULL) // pętla przez wszystkie elementy katalogu a
    {
        // ls -la -> możemy sprawdzić że w każdym katalogu jest "." i ".."
        //pętla idzie przez wszystki elementy i też przez "." i ".." czy katalog w którym jesteśmy i katalog nadrzędny czyli musimy to wykluczyć żeby nie kopiować tego w nieskończoność
        if (strcmp(elem->d_name, ".") != 0 && strcmp(elem->d_name, "..") != 0) // Ignorowanie "." - katalogu w którym jesteśmy i ".." - katalogu nadrzędnego
        {
            char inPath[1000]; // ścieżka do elementu w katalogu a
            char outPath[1000]; // ścieżka do elementu w katalogu b

            snprintf(inPath, sizeof(inPath), "%s/%s", a, elem->d_name); // połącz nazwę pliku ze pełną ścieżką
            snprintf(outPath, sizeof(outPath), "%s/%s", b, elem->d_name); // połącz nazwę pliku ze pełną ścieżką

            struct stat info; // zmienna na informacje o pliku
            stat(inPath, &info); // pobranie informacji o pliku
            long int size = info.st_size; // pobranie rozmiaru

            if (S_ISDIR(info.st_mode)==1) // sprzwdzenie czy pobrany elemet to katalog
            {
                if (R == 1 && czy_katalog(outPath)==0) // Jeśli rekurencja jest włączona i katalog nie istnieje w katalogu docelowym
                {
                    mkdir(outPath, 0777); // Utwórz katalog w katalogu docelowym
                    printf("Utworzono katalog: %s\n", outPath);
                }
                synchronizuj(inPath, outPath, prog, R); // Rekurencyjnie synchronizuj katalogi
            }
            else if(elem->d_type == DT_REG) // jak jest plikiem to kopiuj
            {
                printf("Wielkość pliku: %s wynosi: %ld bajtów\n", inPath, size);
                if (size > prog) // Jeżeli plik większy niż podany próg
                    kopiujMap(inPath, outPath); // Kopiuj z użyciem mmap/write
                else
                    kopiuj(inPath, outPath); // Kopiuj z użyciem read/write
            }
        }
        elem = readdir(in); // sprawdź następny element
    }

    closedir(in); // zamknij katalog a
    closedir(out); // zamknij katalog b
}

int main(int count, char * arg[]) // count - liczba argumentów , arg - tablica zawierająca argumenty
{
    long int prog = 2000000; // domyślny próg rozmiaru do kopiowania
    long int sleep = 10000; // długośc spania
    int R = 0; // czy użyta opcja -R

    // sprawdzenie ilości argumentów  
    if(count==6){
        if(strcmp(arg[5],"-R")==0){ // ./pro a b SLEEP PROG -R
            R=1;
            prog= atol(arg[4]);
            sleep = atol(arg[3]);
        }
    }
    else if(count==5){
        if(strcmp(arg[4],"-R")==0){ //./pro a b SLEEP -R
            R=1;
            sleep = atol(arg[3]);
        }
        else{ // ./pro a b SLEEP PROG
            prog = atol(arg[4]);
            sleep = atol(arg[3]);
        }
    }
    else if(count==4){
        if(strcmp(arg[3],"-R")==0) // ./pro a b -R
            R=1;
        else
            sleep = atol(arg[3]); // ./pro a b SLEEP
    }
    else if(count!=3){ // sprawdzenie ilości argumentów, przerwanie jeżeli jest ich za mało lub za dużo     //./pro a b
        printf("!!! Podano za malo lub za duzo argumentow !!!\n");
        return 0;
    }

    printf("> Sleep=%ld Prog=%ld Rekrurencja=%d \n",sleep,prog,R);

    // przypisanie pobraych argumentów do zmienncyh
    char *a = arg[1];
    char *b = arg[2];

    // sprawdzenie czy pierwszy argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(a)==0)
    {
        printf("!!! Pierwszy argument: %s  nie jest katalogiem !!!\n",a);
        return 0;
    }
    printf("> Katalog 1: %s \n",a);

    // sprawdzenie czy drugi argument to katalog, przerwanie jeżeli nie jest katalogiem
    if(czy_katalog(b)==0)
    {
        printf("!!! Drugi argument: %s nie jest katalogiem !!!\n",b);
        return 0;
    }
    printf("> Katalog 2: %s \n",b);

    //wywołanie funkcji do synchronizacji
    synchronizuj(a,b,prog,R);
    
    // jak wszystko ok to komunikat
    printf("!!! Zsynchronizowano pomyślnie !!!\n");
}