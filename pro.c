// dodajemy niezbędne bilbioteki
#include <stdio.h> // głowna biblioteka c
#include <stdlib.h> // biblioteka pomocnicza do c do malloc i free
#include <string.h> // bilbioteka do napisów w c
#include <sys/stat.h> // uzyskiwanie informacji o pliku
#include <unistd.h> // unix standar close/read/write/fork/sleep/exec
#include <fcntl.h> // open O_RDONLY  O_CREAT
#include <dirent.h> // opendir readdir closedir otwieranie zamykanie i czytanie z katalgoów
#include <sys/mman.h> // mapowanie plików mmap
#include <stdbool.h> // umozliwa korzystanie z zmiennej boolowskiej
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

void UsuwanieZDocelowego(char *a, char *b)
{
    DIR *in = opendir(a); // otwarcie katalogu a jako dir
    DIR *out = opendir(b); // otwarcie katalogu b jako di
    struct dirent *elemout = readdir(out); // wskaźnik elemnet w katalogu docelowym

    while (elemout != NULL)//przechodzi po wszystkich elementach zrodla
    {
        struct dirent *elem = readdir(in); // wskaźnik elemnet w katalogu
        bool czyBylWZrodle = false; //zmienna boolowska mowiaca czy dany plik byl w zrodle
        while(elem != NULL)// wskaźnik elemnet w katalogu zrodlowym
        {
            if(strcmp(elemout->d_name,elem->d_name)==0) //sprawdza czy element w zrodle i destynacji maja taka sama nazwe
            {
                czyBylWZrodle = true;// ustawia zmienna czyBylWZrodle
                break; //przerywa przechodzenie po elementach folderu zrodla
            }
            elem = readdir(in);// przechodzi do kolejnego pliku w zrodle
        }
        if(!czyBylWZrodle)// jesli nie plik byl w zrodle
        {
            char outPath[1000];// ścieżka do elementu w katalogu b
            snprintf(outPath, sizeof(outPath), "%s/%s", b, elemout->d_name);// połącz nazwę pliku ze pełną ścieżką
            remove(outPath);//usuwa plik
            printf("usunięto plik: %s\n", outPath);
        }
        rewinddir(in);//przewija folder zrodlowy do poczatku
        elemout = readdir(out);// przechodzi do kolejnego pliku w docelowym
    }
    closedir(in); // zamknij katalog a
    closedir(out); // zamknij katalog b
}

void synchronizuj(char *a, char *b, long int prog, int R)
{
    UsuwanieZDocelowego(a, b);// usun wszystkie pliki z folderu docelowego jesli nie byly w folderze zrodlowym
    DIR *in = opendir(a); // otwarcie katalogu a jako dir
    DIR *out = opendir(b); // otwarcie katalogu b jako dir
    
    struct dirent *elemin = readdir(in); // wskaźnik elemnet w katalogu
     
    while (elemin != NULL) // pętla przez wszystkie elementy katalogu a
    {
        // ls -la -> możemy sprawdzić że w każdym katalogu jest "." i ".."
        //pętla idzie przez wszystki elementy i też przez "." i ".." czy katalog w którym jesteśmy i katalog nadrzędny czyli musimy to wykluczyć żeby nie kopiować tego w nieskończoność
        if (strcmp(elemin->d_name, ".") != 0 && strcmp(elemin->d_name, "..") != 0) // Ignorowanie "." - katalogu w którym jesteśmy i ".." - katalogu nadrzędnego
        {
            char inPath[1000]; // ścieżka do elementu w katalogu a
            char outPath[1000]; // ścieżka do elementu w katalogu b

            snprintf(inPath, sizeof(inPath), "%s/%s", a, elemin->d_name); // połącz nazwę pliku ze pełną ścieżką
            snprintf(outPath, sizeof(outPath), "%s/%s", b, elemin->d_name); // połącz nazwę pliku ze pełną ścieżką

            struct stat info; // zmienna na informacje o pliku
            stat(inPath, &info); // pobranie informacji o pliku
            long int size = info.st_size; // pobranie rozmiaru

            if (S_ISDIR(info.st_mode)==1 && R == 1) // sprzwdzenie czy pobrany elemet to katalog
            {
                if (czy_katalog(outPath)==0) // Jeśli rekurencja jest włączona i katalog nie istnieje w katalogu docelowym
                {
                    mkdir(outPath, 0777); // Utwórz katalog w katalogu docelowym
                    printf("Utworzono katalog: %s\n", outPath);
                    
                }
                synchronizuj(inPath, outPath, prog, R); // Rekurencyjnie synchronizuj katalogi
            }
            else if(elemin->d_type == DT_REG) // jak jest plikiem to kopiuj
            {
                struct stat copyInfo;// zmienna na informacje o pliku
                if(stat(outPath,&copyInfo)==0)// sprawdza czy dany plik zawiera sie w folderze docelowym
                {
                    if(copyInfo.st_mtime != info.st_mtime) // sprawdza czy pliki maja rozne daty modyfikacji, i jesli sa to wykonuje kopie
                    {
                        printf("Wielkość pliku: %s wynosi: %ld bajtów\n", inPath, size);
                        if (size > prog) // Jeżeli plik większy niż podany próg
                            kopiujMap(inPath, outPath); // Kopiuj z użyciem mmap/write
                        else
                            kopiuj(inPath, outPath); // Kopiuj z użyciem read/write
                    }
                }else //jesli nie bylo pliku w folderze docelowym to go kopiuje
                {
                    printf("Wielkość pliku: %s wynosi: %ld bajtów\n", inPath, size);
                    if (size > prog) // Jeżeli plik większy niż podany próg
                        kopiujMap(inPath, outPath); // Kopiuj z użyciem mmap/write
                    else
                        kopiuj(inPath, outPath); // Kopiuj z użyciem read/write
                }
                
            }
        }
        elemin = readdir(in); // sprawdź następny element
    }
    
    closedir(in); // zamknij katalog a
    closedir(out); // zamknij katalog b
}

int program(char *a, char *b, long int prog, int R) // a - katalog zrodłowy, b - katalog docelowy, prog - próg zmiany funkcji do kopiowania na mapowanie, int R 1-rekurencyjne 0-nie
{
    //wywołanie funkcji do synchronizacji
    synchronizuj(a,b,prog,R);
    
    // jak wszystko ok to komunikat
    printf("!!! Zsynchronizowano pomyślnie !!!\n");
}