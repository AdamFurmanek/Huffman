#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

typedef struct drzewo drzewo;                                               //zdefiniowanie nazwy dla struktury
typedef struct kolejka kolejka;                                             //zdefiniowanie nazwy dla kolejki czytajacej kod

/** \brief drzewo
* - Struktura bedaca poczatkowo lista, a ostatecznie zamieniona w drzewo, przechowuje wprowadzane znaki oraz ilosc ich wystapien , a takze kod ktory otrzymaja poszczegolne komorki w zaleznosci od odleglosci od korzenia.*/
struct drzewo                                                               //implementacja struktury
{
  drzewo* nast;                                                             //wskaznik wykorzystujacy strukture jako lista
  drzewo* lewo;
  drzewo* prawo;                                                            //wskazniki wykorzystujace strukture jako drzewo
  char znak;                                                                //znak wprowadzonego ciagu
  int ile_razy;                                                             //liczba wystapien znaku
  int kod;                                                                  //kod jaki zostanie nadany poszczegolnym znakom
};

/** \brief kolejka
* - Struktura wykorzystywana przy dekodowaniu, zawierajaca kolejne cyfry kodu binarnego odczytanego z pliku.*/
struct kolejka                                                              //implementacja kolejki
{
    int kod;                                                                //cyfra kodu
    kolejka* nast;                                                          //wskaznik na kolejna komorke
};
                                                                            //prototypy funkcji
drzewo* dodaj_do_listy(drzewo *wskaznik, char znak);                        //dodaje znak do struktury(w formie listy)
char* dodaj_do_tablicy(char *tablica, int rozmiar, char znak);              //funkcja dodajaca znak do dynamicznej tablicy
drzewo* zmien_liste_na_drzewo(drzewo *wskaznik);                            //funkcja tworzaca drzewo ze stworzonej wczesniej listy
void sortuj(drzewo *wskaznik);                                              //funkcja sortujaca liste na podstawie ilosci wystapien znaku
void wyswietl_ile(drzewo *wskaznik);                                        //funkcja wyswietlajaca znaki i ilosc ich wystapien
void wyswietl_kod(drzewo *wskaznik);                                        //funkcja wyswietlajaca znaki i ich kod
void zapisanie_klucza(drzewo *wskaznik, FILE *plik);                        //funkcja zapisujaca klucz do pliku
void ustal_kod(drzewo *wskaznik, int kod);                                  //funkcja nadajacy poszczegolnym znakom kod
void wizualizacja(drzewo *wskaznik, int poziom);                            //funkcja wizualizujaca drzewo
void znajdz_kod(drzewo *wskaznik, char znak, FILE *plik, int *kod);         //funkcja znajdujaca kody poszczegolnym znakom oryginalnego ciagu
void kodowanie_po_cyfrze(FILE *plik, int *kod, char *litera, int *licznik, int *suma); //funkcja konwertujaca kod binarny w porcjach po 8 bitow na liczbe dzisietna
void kodowanie_po_znaku(FILE *plik, char *litera, int *licznik, int *suma, int pom);   //funkcja zapisujaca otrzymana liczbe dziesietna jako znak w pliku
void usun_strukture(drzewo *wskaznik);                                      //funkcja usuwajaca strukture

void odczyt_klucza(FILE *plik, drzewo **wskaznik);                          //funkcja odczytujaca klucz z pliku
void odwroc_liste(drzewo **wskaznik);                                       //funkcja odwracajaca liste
void dodaj_do_kolejki(kolejka **czolo, kolejka **ogon, int liczba);         //funkcja dodajaca kolejne cyfry wlasciwego kodu do kolejki
int usun_z_kolejki(kolejka **czolo, kolejka **ogon);                        //funkcja wyjmujaca cyfry z kolejki
void szukanie(drzewo *wskaznik, int kod, FILE *plik, int *znaleziony);      //funkcja przydzielajaca znak na podstawie kodu z kolejki

void kodowanie();                                                           //funkcje zawierajace cale instrukcje operacji
void dekodowanie();                                                         //kodowania i dekodowania
void kodowanie_sciezka(char** argv);                                        //funkcje podobne do powyzszych, lecz
void dekodowanie_sciezka(char** argv);                                      //oblugujace sciezki do odczytywanych i zapisywanych plikow

/** \brief main
* - Funkcja glowna zawierajaca 3 rodzaje "menu". Pierwsze obslugujace program uruchiomiony z piku .exe pozwalajace na zakodowanie i odkodowanie plikow z domyslna sciezka i nazwami. Pozostale dwa obsluguja program przy uzyciu komend z wykorzystaniem konsoli cmd. Pierwsze z nich uruchamiane jest jesli zostanie uzyty tylko jeden parametr i pozwala na kodowanie oraz dekodowanie operujac na domyslnych ustawieniach tak jak w pierwszym menu. Ostatnie przyjmuje dwa dodatkowe paramtery do ktorych podaje sie sciezki z plikami. W zaleznosci od wybranego sposobu uzycia programu, funkcja main wywoluje funckje "kodowanie" i "dekodowanie" (domyslne), "kodowanie_sciezka" i "dekodowanie_sciezka" (operujace na wprowadzonych sciezkach), lub moze tez wylaczyc program.*/
int main(int argc, char** argv)
{
    int wybor;                                                              //zmienna pozwalajaca na wybor odpowiedniej funkcji programu
    while(argc==1){                                                         //klasyczne wywolanie menu bez paramteru (przy uruchomieniu .exe)
        printf("Naci\230nij 1 aby zakodowa\206 tekst.\nNaci\230nij 2 aby odkodowa\206 tekst.\nNaci\230nij 3 aby zako\344czy\206.\n");
        wybor=getch()-48;                                                   //wybranie funkcji przez uzytkownika
        if(wybor==1){                                                       //kodowanie
            kodowanie();
            printf("Naci\230nij dowolny klawisz aby kontynuowa\206.\n\n");
            getch();
        }
        if(wybor==2){                                                       //dekodowanie
            dekodowanie();
            printf("\n\nNaci\230nij dowolny klawisz aby kontynuowa\206.\n\n");
            getch();
        }
        if(wybor==3){                                                       //koniec programu
            return 0;
        }
        system("cls");                                                      //wyczyszczenie terminalu
    }

    if(argc==2){                                                            //korzystanie z programu przy pomocy paramterow wybieranych w konsoli
        if(strcmp(argv[1], "pomoc")==0){                                    //komenda "pomoc"
           printf("Aby zakodowac z klawiatury uzyj komendy <projekt koduj>\nAby zakodowac z plik uzyj komendy <projekt koduj sciezka_pliku_odczytu sciezka_pliku_zapisu>\nAby odkodowac domyslny plik uzyj komendy <projekt dekoduj>\nAby odkodowac z wybranego pliku uzyj komendy <projekt dekoduj sciezka_pliku_odczytu sciezka_pliku_zapisu>\nNalezy pamietac ze przy wpisywaniu sciezki nalezy uzywac podowjnych slashy np. c:\\program files");
        }
        else if(strcmp(argv[1], "koduj")==0){                               //komenda "koduj"
            kodowanie();
        }
        else if(strcmp(argv[1], "dekoduj")==0){                             //komenda "dekoduj"
            dekodowanie();
        }
        else{                                                               //bledna komenda
            printf("Wprowadzono z\210\245 komend\251. U\276yj <pomoc> aby uzyska\206 informacje.");
        }
        return 0;                                                           //wykonanie komendy, koniec programu
    }
    else if(argc==4){                                                       //korzystanie z programu przy pomocy paramterow ze sciezkami wybieranych w konsoli
        FILE *test=NULL;                                                    //sprawdzenie czy sciezki podane przez uzytkownika sa prawidlowe
        test=fopen(("%s", argv[2]), "r");
        if(test==NULL){
            printf("\230cie\276ka jest niepoprawna.");
            return 0;                                                       //zakonczenie programu jesli sciezka jest nieprawidlowa
        }
        close(test);
        test=fopen(("%s", argv[3]), "r");
        if(test==NULL){
            printf("\230cie\276ka jest niepoprawna.");
            return 0;                                                       //zakonczenie programu jesli sciezka jest nieprawidlowa
        }
        close(test);

        if(strcmp(argv[1], "koduj")==0){                                    //komenda "koduj" ze sciezkami
            kodowanie_sciezka(argv);
        }
        else if(strcmp(argv[1], "dekoduj")==0){                             //komenda "dekoduj" ze sciezkami
            dekodowanie_sciezka(argv);
        }
        else{                                                               //bledna komenda
            printf("Wprowadzono z\210\245 komend\251. U\276yj <pomoc> aby uzyska\206 informacje.");
        }
        return 0;                                                           //wykonanie komendy, koniec programu
    }
    else{                                                                   //bledna komenda
        printf("Wprowadzono z\210\245 komend\251. U\276yj <pomoc> aby uzyska\206 informacje.");
    }
}

/** \brief kodowanie
* - Domyslna funkcja kodujaca, przechodzaca przez poszczegolne etapy tego procesu i wywolujaca odpowiednie funkcje. Na poczatku wprowadzane sa znaki przez uzytkownika jednoczesnie do dynamicznej tablicy i do listy.
* w miedzyczasie liczony jest rozmiar wprowadzonego ciagu, a ten konczy sie po wprowadzeniu znaku ENTER. Funkcja zostaje przerwana jesli ciag znakow wynosi mniej niz 2 znaki. Nastepnie zapisywany jest klucz  z informacjami na temat uzytych znakow i ilosci ich wystapien oraz rozmiaru calego ciagu do pliku "rezultat.txt" ktory zostaje utworzony w katalogu programu.
* Lista zostaje posortowana w zaleznosci od czestotliwosci wystepowania znakow, a nastepnie wyswietlony zostaje histogram. Kolejnym etapem jest przemianowanie listy w drzewo, oraz nadanie kodu binarnego wszystkim komorkom drzewa. Wyswietlona zostaje wizualizacja drzewa oraz kody ktore otrzymuja poszczegolne znaki. Przy pomocy tablicy z oryginalnym tekstem oraz kodow z drzewa, powstaje kod wlasciwy ktory jest zapisywany do pliku. Drzewo zostaje usuniete i funkcja sie konczy.
*/
void kodowanie()
{
        FILE *plik;                                                                 //deklaracja wskaznika na plik

        int rozmiar=0;                                                              //zmienna przechowujaca rozmiar tablicy w ktorej zapisany bedzie ciag
        char *tablica=(char*)malloc(sizeof(char));                                  //stworzenie pierwszej komorki tablicy dynamicznej
        drzewo *wskaznik=NULL;                                                      //deklaracja glownego wskaznika na strukture
        char znak;                                                                  //zmienna przechowujaca znak

        printf("Wprowad\253 ci\245g znak\242w do skompresowania.\n");
        while(1){                                                                   //petla pozwalajaca na wczytywanie znakow podanych przez uzytkownika
            scanf("%c", &znak);                                                     //pobranie jednego znaku do zmiennej "znak"
            if(znak=='\n'){                                                         //petla konczy sie po nacisnieciu ENTER
                break;
            }
            wskaznik=dodaj_do_listy(wskaznik, znak);                                //dodanie znaku do listy
            rozmiar++;                                                              //zliczanie ilosci znakow, czyli rozmiaru tablicy dynamicznej
            tablica=dodaj_do_tablicy(tablica, rozmiar, znak);                       //dodanie znaku do tablicy
        }
        if(rozmiar<2){                                                              //zabezpieczenie przed operacja kodowania na za malym lancuchu
            printf("Kodowany tekst musi sk\210ada\206 si\251 z przynajmniej dw\242ch znak\242w.\n");
        }
        else{
            plik=fopen("rezultat.txt", "w");                                        //otworzenie pliku
            zapisanie_klucza(wskaznik, plik);                                       //zapisanie klucza pozwalajacego na odkodowanie do pliku
            fprintf(plik, "%d\n", rozmiar);                                         //zapisanie ilosci wszystkich znakow ktore wystapily w ciagu do pliku
            sortuj(wskaznik);                                                       //sortowanie listy na podstawie ilosci wystapien
            printf("\nIlo\230\206 wyst\245pie\344 znak\242w ci\245gu:\n");
            wyswietl_ile(wskaznik);                                                 //wyswietlenie posortowanej listy znakow i ilosci ich powtorzen
            wskaznik=zmien_liste_na_drzewo(wskaznik);                               //utworzenie drzewa z listy
            ustal_kod(wskaznik, 0);                                                 //nadanie poszczegolnym literom odpowiedniego kodu
            printf("\nWizualizacja powsta\210ego drzewa:\n");
            wizualizacja(wskaznik, 0);                                              //zwizualizowanie drzewa
            printf("\nZnaki otrzymuj\245 nast\251puj\245ce kody:\n");
            wyswietl_kod(wskaznik);                                                 //wyswietlenie drzewa znakow i ich kodow

            int i;                                                                  //licznik dla petli
            int kod;                                                                //zmienna ktora bedzie przyjmowala kod binarny w postaci jedynek i dwojek na podstawie znakow
            char litera;                                                            //zmienna ktora bedzie przyjmowala znak w zaleznosci od przydzielonego kodu binarnego
            int licznik=7;                                                          //licznik liczacy kiedy nastapi przypisanie i zapisanie znaku (po uzupelnieniu 8 bitow)
            int suma=0;                                                             //zmienna bedaca dziesietnym odpowiednikiem powstalego kodu porcjowanego po 8 ktora zostanie zapisana jako znak w pliku
            printf("\nPowsta\210y kod oraz zapisywane znaki:\n");
            for(i=0;i<rozmiar;i++){                                                 //petla pozwalajace przejsc przez wszystkie komorki tablice zawierajacej oryginalny ciag
                znajdz_kod(wskaznik,tablica[i],plik, &kod);                         //odnalezenie wlasciwego kodu dla konkretnego znaku
                while(kod!=0){                                                      //od kodu znaku bedzie "odkrajana" pierwsza cyfra az do momentu kiedy kod bedzie wynosil 0
                kodowanie_po_cyfrze(plik, &kod, &litera, &licznik, &suma);          //"odkrojenie" cyfry, porcjowanie cyfr po 8 i zapis dziesietnego odpowiednika jako znak do pliku
                }
            }
            litera=suma;                                                            //zapisanie ostatniej, "niepelnej" porcji osmio bitowej do zmiennej litera
            fprintf(plik, "%c", litera);                                            //zapisanie ostatniego znaku w pliku
            printf(" (%c) ", litera);

            printf("\n\nKod zosta\210 zapisany w pliku <rezultat.txt>\n\n");
            fclose(plik);                                                           //zamkniecie pliku
            free(tablica);                                                          //zwolnienie pamieci zajmowanej przez tablice
            usun_strukture(wskaznik);                                               //zwolnienie pamieci zajmowanej przez strukture
        }
}

/** \brief kodowanie
* - Domyslna funkcja dekodujaca, przechodzaca przez poszczegolne etapy tego procesu i wywolujaca odpowiednie funkcje. Najpierw zostaje odcyztany klucz z pliku "rezultat.txt" o ile ten zostanie otworzony. Na jego podstawie tworzone jest identyczne drzewo jak to podczas kodowania.
* Wyswietlane sa te same elementy co przy kodowaniu (tj. histogram, wizualizacja drzewa oraz przydzielane kody do znakow). Nastepnie odczytywany jest kod wlasciwy ktory zamieniony na kod binarny pozwala na odczytanie oryginalnych znakow. Ciag zostaje zapisany w pliku "oryginal.txt" w tym samym katalogu co program, a funkcja konczy dzialanie.
*/
void dekodowanie()
{
        FILE *plik;                                                                //deklaracja wskaznika na plik
        drzewo *wskaznik=NULL;                                                     //deklaracja glownego wskaznika na strukture
        kolejka *czolo=NULL;                                                        //deklaracja wskaznika na czolo kolejki
        kolejka *ogon=NULL;                                                         //deklaracja wskaznika na ogon kolejki
        plik=fopen("rezultat.txt", "r");                                            //otworzenie pliku
        if(plik==NULL){                                                             //zabezpieczenie przed proba dekodowania bez pliku z kodem
            printf("Nie mo\276na znale\253\206 pliku <rezultat.txt>");
        }
        else{
            odczyt_klucza(plik, &wskaznik);                                         //funkcja odczytujaca klucz z pliku "rezultat.txt" i zapisujaca znaki do listy
            odwroc_liste(&wskaznik);                                                //funkcja odwracajaca liste (klucz w pliku byl odwrocony)
            sortuj(wskaznik);                                                       //sortowanie listy na podstawie ilosci wystapien
            printf("Ilo\230\206 wyst\245pie\344 znak\242w ci\245gu:\n");
            wyswietl_ile(wskaznik);                                                 //wyswietlenie posortowanej listy znakow i ilosci ich powtorzen
            wskaznik=zmien_liste_na_drzewo(wskaznik);                               //utworzenie drzewa z listy
            ustal_kod(wskaznik, 0);                                                 //nadanie poszczegolnym literom odpowiedniego kodu
            printf("\nWizualizacja powsta\210ego drzewa:\n");
            wizualizacja(wskaznik, 0);                                              //zwizualizowanie drzewa
            printf("\nZnaki otrzymuj\245 nast\251puj\245ce kody:\n");
            wyswietl_kod(wskaznik);                                                 //wyswietlenie drzewa znakow i ich kodow

            char a;                                                                 //zmienna pomocnicza odczytujaca znaki z pliku
            int rozmiar;                                                            //zmienna do ktorej przypiszemy wartosc z pliku pod kluczem oznaczajaca ilosc wszystkich znakow razem
            fscanf(plik,"%c", &a);                                                  //odczyt pierwszej cyfry
            rozmiar=0;
            while(a!='\n'){                                                         //dopoki kolejne znaki to cyfry, program bedzie je zapisywal
                rozmiar=rozmiar*10+(a-48);                                          //i przypisze je jako liczbe do zmiennej rozmiar
                fscanf(plik,"%c", &a);                                              //odczytywanie kolejnych znakow za do napotkania znaku nowej linii
            }

            int liczba;                                                             //zmienna pomocnicza ktora bedzie odzyskiwala liczbe w systemie dziesietnym z pobranego znaku z kodu wlasciwego zapisanego w pliku
            int binarna;                                                            //zmienne bedaca narzedziem i jednoczesnie licznikiem do konwersji liczby dziesietnej na kod binarny
            while(1){                                                               //petla pozwalajaca na pobranie znaku w formie liczby dzisietnej, ktora zostanie zapisana jako kod binarny w kolejce
                fscanf(plik,"%c", &a);                                              //pobranie znaku
                if(feof(plik)!=0){                                                  //przypadek osiagniecia konca pliku
                    break;                                                          //przerwanie dalszego pobierania
                }
                liczba=a;                                                           //przypisanie wartosci pobranego znaku do zmiennej liczba
                if(liczba<0){                                                       //poniewaz char ma 8 bitow, a liczba z pierwszym bitem bedacym 1 jest ujemna, do liczby (ktora ma typ integer) przypisywane sa ujemne wartosci
                    liczba=liczba+256;                                              //nalezy sie ich pozbyc "odwracajac" ich wartosc, czyli dodajac 256
                }
                binarna=128;                                                        //binarnej przypisujemy najwieksza potege dwojki mogacej zmiescic sie na 8 bitach
                while(binarna!=0){                                                  //petla konwertujaca liczba na system binarny, zapisujaca kolejne cyfry w kolejce
                    if(liczba>=binarna){                                            //przypadek w ktorym liczba jest wieksza lub rownej binarnej
                        dodaj_do_kolejki(&czolo,&ogon,2);                           //do kolejki zostaje dodana dwojka
                        liczba=liczba-binarna;                                      //liczba jest pomniejszana o binarna
                    }
                    else{                                                           //przypadek w ktorym liczba jest mniejsza od binarnej
                        dodaj_do_kolejki(&czolo,&ogon,1);                           //do kolejki zostaje dodana jedynka
                    }
                    binarna=binarna/2;                                              //binarna jest zmniejszana to nizszej potegi dwojki, tak aby zajac sie mlodszymi bitami
                }
            }

            fclose(plik);                                                           //zamkniecie pliku rezultat
            plik=fopen("oryginal.txt", "w");                                        //otworzeniue lub utworzenie pliku "oryginal.txt."

            int znaleziony;                                                         //zmienna mowiaca o tym czy znaleziono litere czy nie
            int kod;                                                                //zmienna bedace kodem ktory sprawdzany jest w drzewie
            printf("\nOryginalny ci\245g zosta\210 zapisany w pliku <oryginal.txt> i wygl\245da nast\251puj\245co:\n\n");
            while(czolo!=NULL&&rozmiar>0){                                          //petla pozwalajace przejsc do konca kolejki, badz po odnalezieniu wszystkich znakow
                rozmiar--;                                                          //jedno przejscie petli oznacza jeden znak mniej do konca calego ciagu
                znaleziony=0;                                                       //ustalenie znalezionej jako negatywnej
                kod=usun_z_kolejki(&czolo, &ogon);                                  //zapisanie do kodu pierwszej cyfry
                szukanie(wskaznik,kod,plik, &znaleziony);                           //pierwsze szukanie kodu
                while(znaleziony==0){                                               //petla pozwalajace dodawac kolejne cyfry w przypadku nie odnalezienia kodu w drzewie
                    kod=kod*10+usun_z_kolejki(&czolo, &ogon);                       //dodanie kolejnej cyfry
                    szukanie(wskaznik,kod,plik, &znaleziony);                       //ponowne szukanie (w przypakdu odnalezienia, znaleziony otrzymuje wartosc 1)
                }
            }
            fclose(plik);                                                           //zamkniecie pliku
            usun_strukture(wskaznik);                                               //zwolnienie pamieci zajmowanej przez strukture
        }
}

/** \brief kodowanie_sciezka
* - Funkcja blizniacza do "kodowanie". Rozni sie ona tym, ze korzysta ze sciezek wprowadzonych przez uzytkownika, wskazujacych miejsca i nazwy plikow odczytu i zapisu (przekazywane sa z maina jako argumenty). Dodatkowo oznacza to ze uzytkownik nie wprowadza danych sam, ale program czyta je z podanego pliku.
*/
void kodowanie_sciezka(char** argv)
{
        FILE *plik=fopen(("%s", argv[2]), "r");                                     //deklaracja wskaznika na plik
        int rozmiar=0;                                                              //zmienna przechowujaca rozmiar tablicy w ktorej zapisany bedzie ciag
        char *tablica=(char*)malloc(sizeof(char));                                  //stworzenie pierwszej komorki tablicy dynamicznej
        drzewo *wskaznik=NULL;                                                      //deklaracja glownego wskaznika na strukture
        char znak;                                                                  //zmienna przechowujaca znak

        while(1){                                                                   //petla pozwalajaca na wczytywanie znakow z pliku
            znak=fgetc(plik);                                                       //pobranie jednego znaku do zmiennej "znak"
            if(znak==EOF||znak=='\n'){                                              //petla konczy sie przy koncu pliku lub przy koncu wiersza
                break;
            }
            wskaznik=dodaj_do_listy(wskaznik, znak);                                //dodanie znaku do listy
            rozmiar++;                                                              //zliczanie ilosci znakow, czyli rozmiaru tablicy dynamicznej
            tablica=dodaj_do_tablicy(tablica, rozmiar, znak);                       //dodanie znaku do tablicy
        }
        close(plik);
        if(rozmiar<2){                                                              //zabezpieczenie przed operacja kodowania na za malym lancuchu
            printf("Kodowany tekst musi sk\210ada\206 si\251 z przynajmniej dw\242ch znak\242w.\n");
        }
        else{
            plik=fopen(("%s", argv[3]), "w");                                       //otworzenie pliku
            zapisanie_klucza(wskaznik, plik);                                       //zapisanie klucza pozwalajacego na odkodowanie do pliku
            fprintf(plik, "%d\n", rozmiar);                                         //zapisanie ilosci wszystkich znakow ktore wystapily w ciagu do pliku
            sortuj(wskaznik);                                                       //sortowanie listy na podstawie ilosci wystapien
            printf("\nIlo\230\206 wyst\245pie\344 znak\242w ci\245gu:\n");
            wyswietl_ile(wskaznik);                                                 //wyswietlenie posortowanej listy znakow i ilosci ich powtorzen
            wskaznik=zmien_liste_na_drzewo(wskaznik);                               //utworzenie drzewa z listy
            ustal_kod(wskaznik, 0);                                                 //nadanie poszczegolnym literom odpowiedniego kodu
            printf("\nWizualizacja powsta\210ego drzewa:\n");
            wizualizacja(wskaznik, 0);                                              //zwizualizowanie drzewa
            printf("\nZnaki otrzymuj\245 nast\251puj\245ce kody:\n");
            wyswietl_kod(wskaznik);                                                 //wyswietlenie drzewa znakow i ich kodow

            int i;                                                                  //licznik dla petli
            int kod;                                                                //zmienna ktora bedzie przyjmowala kod binarny w postaci jedynek i dwojek na podstawie znakow
            char litera;                                                            //zmienna ktora bedzie przyjmowala znak w zaleznosci od przydzielonego kodu binarnego
            int licznik=7;                                                          //licznik liczacy kiedy nastapi przypisanie i zapisanie znaku (po uzupelnieniu 8 bitow)
            int suma=0;                                                             //zmienna bedaca dziesietnym odpowiednikiem powstalego kodu porcjowanego po 8 ktora zostanie zapisana jako znak w pliku
            printf("\nPowsta\210y kod oraz zapisywane znaki:\n");
            for(i=0;i<rozmiar;i++){                                                 //petla pozwalajace przejsc przez wszystkie komorki tablice zawierajacej oryginalny ciag
                znajdz_kod(wskaznik,tablica[i],plik, &kod);                         //odnalezenie wlasciwego kodu dla konkretnego znaku
                while(kod!=0){                                                      //od kodu znaku bedzie "odkrajana" pierwsza cyfra az do momentu kiedy kod bedzie wynosil 0
                kodowanie_po_cyfrze(plik, &kod, &litera, &licznik, &suma);          //"odkrojenie" cyfry, porcjowanie cyfr po 8 i zapis dziesietnego odpowiednika jako znak do pliku
                }
            }
            litera=suma;                                                            //zapisanie ostatniej, "niepelnej" porcji osmio bitowej do zmiennej litera
            fprintf(plik, "%c", litera);                                            //zapisanie ostatniego znaku w pliku
            printf(" (%c) ", litera);

            printf("\n\nKod zosta\210 zapisany w sciezce %s \n\n", argv[3]);
            fclose(plik);                                                           //zamkniecie pliku
            free(tablica);                                                          //zwolnienie pamieci zajmowanej przez tablice
            usun_strukture(wskaznik);                                               //zwolnienie pamieci zajmowanej przez strukture
        }
}

/** \brief dekodowanie_sciezka
* - Funkcja blizniacza do "dekodowanie". Rozni sie ona tym, ze korzysta ze sciezek wprowadzonych przez uzytkownika, wskazujacych miejsca i nazwy plikow odczytu i zapisu (przekazywane sa z maina jako argumenty).
*/
void dekodowanie_sciezka(char** argv)
{
        FILE *plik;                                                                 //deklaracja wskaznika na plik
        drzewo *wskaznik=NULL;                                                      //deklaracja glownego wskaznika na strukture
        kolejka *czolo=NULL;                                                        //deklaracja wskaznika na czolo kolejki
        kolejka *ogon=NULL;                                                         //deklaracja wskaznika na ogon kolejki
        plik=fopen(("%s", argv[2]), "r");                                           //otworzenie pliku

            odczyt_klucza(plik, &wskaznik);                                         //funkcja odczytujaca klucz z pliku podanego w sciezce i zapisujaca znaki do listy
            odwroc_liste(&wskaznik);                                                //funkcja odwracajaca liste (klucz w pliku byl odwrocony)
            sortuj(wskaznik);                                                       //sortowanie listy na podstawie ilosci wystapien
            printf("Ilo\230\206 wyst\245pie\344 znak\242w ci\245gu:\n");
            wyswietl_ile(wskaznik);                                                 //wyswietlenie posortowanej listy znakow i ilosci ich powtorzen
            wskaznik=zmien_liste_na_drzewo(wskaznik);                               //utworzenie drzewa z listy
            ustal_kod(wskaznik, 0);                                                 //nadanie poszczegolnym literom odpowiedniego kodu
            printf("\nWizualizacja powsta\210ego drzewa:\n");
            wizualizacja(wskaznik, 0);                                              //zwizualizowanie drzewa
            printf("\nZnaki otrzymuj\245 nast\251puj\245ce kody:\n");
            wyswietl_kod(wskaznik);                                                 //wyswietlenie drzewa znakow i ich kodow

            char a;                                                                 //zmienna pomocnicza odczytujaca znaki z pliku
            int rozmiar;                                                            //zmienna do ktorej przypiszemy wartosc z pliku pod kluczem oznaczajaca ilosc wszystkich znakow razem
            fscanf(plik,"%c", &a);                                                  //odczyt pierwszej cyfry
            rozmiar=0;
            while(a!='\n'){                                                         //dopoki kolejne znaki to cyfry, program bedzie je zapisywal
                rozmiar=rozmiar*10+(a-48);                                          //i przypisze je jako liczbe do zmiennej rozmiar
                fscanf(plik,"%c", &a);                                              //odczytywanie kolejnych znakow za do napotkania znaku nowej linii
            }

            int liczba;                                                             //zmienna pomocnicza ktora bedzie odzyskiwala liczbe w systemie dziesietnym z pobranego znaku z kodu wlasciwego zapisanego w pliku
            int binarna;                                                            //zmienne bedaca narzedziem i jednoczesnie licznikiem do konwersji liczby dziesietnej na kod binarny
            while(1){                                                               //petla pozwalajaca na pobranie znaku w formie liczby dzisietnej, ktora zostanie zapisana jako kod binarny w kolejce
                fscanf(plik,"%c", &a);                                              //pobranie znaku
                if(feof(plik)!=0){                                                  //przypadek osiagniecia konca pliku
                    break;                                                          //przerwanie dalszego pobierania
                }
                liczba=a;                                                           //przypisanie wartosci pobranego znaku do zmiennej liczba
                if(liczba<0){                                                       //poniewaz char ma 8 bitow, a liczba z pierwszym bitem bedacym 1 jest ujemna, do liczby (ktora ma typ integer) przypisywane sa ujemne wartosci
                    liczba=liczba+256;                                              //nalezy sie ich pozbyc "odwracajac" ich wartosc, czyli dodajac 256
                }
                binarna=128;                                                        //binarnej przypisujemy najwieksza potege dwojki mogacej zmiescic sie na 8 bitach
                while(binarna!=0){                                                  //petla konwertujaca liczba na system binarny, zapisujaca kolejne cyfry w kolejce
                    if(liczba>=binarna){                                            //przypadek w ktorym liczba jest wieksza lub rownej binarnej
                        dodaj_do_kolejki(&czolo,&ogon,2);                           //do kolejki zostaje dodana dwojka
                        liczba=liczba-binarna;                                      //liczba jest pomniejszana o binarna
                    }
                    else{                                                           //przypadek w ktorym liczba jest mniejsza od binarnej
                        dodaj_do_kolejki(&czolo,&ogon,1);                           //do kolejki zostaje dodana jedynka
                    }
                    binarna=binarna/2;                                              //binarna jest zmniejszana to nizszej potegi dwojki, tak aby zajac sie mlodszymi bitami
                }
            }

            fclose(plik);                                                           //zamkniecie pliku rezultat
            plik=fopen(("%s", argv[3]), "w");                                       //otworzenie lub utworzenie pliku podanego w sciezce

            int znaleziony;                                                         //zmienna mowiaca o tym czy znaleziono litere czy nie
            int kod;                                                                //zmienna bedace kodem ktory sprawdzany jest w drzewie
            printf("\nOryginalny ci\245g zosta\210 zapisany w sciezce %s i wygl\245da nast\251puj\245co:\n\n", argv[3]);
            while(czolo!=NULL&&rozmiar>0){                                          //petla pozwalajace przejsc do konca kolejki, badz po odnalezieniu wszystkich znakow
                rozmiar--;                                                          //jedno przejscie petli oznacza jeden znak mniej do konca calego ciagu
                znaleziony=0;                                                       //ustalenie znalezionej jako negatywnej
                kod=usun_z_kolejki(&czolo, &ogon);                                  //zapisanie do kodu pierwszej cyfry
                szukanie(wskaznik,kod,plik, &znaleziony);                           //pierwsze szukanie kodu
                while(znaleziony==0){                                               //petla pozwalajace dodawac kolejne cyfry w przypadku nie odnalezienia kodu w drzewie
                    kod=kod*10+usun_z_kolejki(&czolo, &ogon);                       //dodanie kolejnej cyfry
                    szukanie(wskaznik,kod,plik, &znaleziony);                       //ponowne szukanie (w przypakdu odnalezienia, znaleziony otrzymuje wartosc 1)
                }
            }
            fclose(plik);                                                           //zamkniecie pliku
            usun_strukture(wskaznik);                                               //zwolnienie pamieci zajmowanej przez strukture
}

/** \brief dodaj_do_listy
* - Ma za zadanie dodac otrzymany znak do listy (ktore przekazane sa jako argumenty). Jesli znak pojawia sie po raz pierwszy tworozna jest nowa komorka. W przeciwnym wypadku, licznik w komorce z danym znakiem zostaje zwiekszony.
*/
drzewo* dodaj_do_listy(drzewo *wskaznik, char znak)
{
    //w tej funkcji, glowna struktura ze znakami bedzie traktowana
    //jako zwykla lista przy uzycia wskaznika "nast" z jej wnetrza

    drzewo *pomocnik=wskaznik;

    while(pomocnik!=NULL){                                                  //przeszukiwanie listy czy nowy znak nie pojawil sie wczesniej
        if(pomocnik->znak==znak){                                           //odnalezeinie komorki z takim samym znakiem
            pomocnik->ile_razy++;                                           //zwiekszenie zmiennej oznaczajacej ilosc powtorzen
            break;                                                          //wyjscie z przeszukiwania
        }
        else{                                                               //przeszukiwanie kolejnych komorek
            pomocnik=pomocnik->nast;
        }
    }
    if(pomocnik==NULL){                                                     //przypadek nie odnalezienia takiego samego znaku - stworzenie nowej komorki
    pomocnik=(drzewo*)malloc(sizeof(drzewo));
    pomocnik->znak=znak;
    pomocnik->ile_razy=1;
    pomocnik->nast=wskaznik;
    pomocnik->lewo=NULL;
    pomocnik->prawo=NULL;
    pomocnik->kod=0;
    wskaznik=pomocnik;                                                      //ustawienie wskaznik glownego na nowa komorke
    }
    return wskaznik;                                                        //zwrocenie kopii wskaznika glownego by zapisac go w oryginale
}

/** \brief dodaj_do_tablicy
* - Ma za zadanie dodac otrzymany znak do tablicy, aby zapisac caly oryginalny ciag. Przyjmuje wskaznik na te tablica, jej rozmiar, oraz kolejny znak do dodania, zwraca wskaznik na tablice.
*/
char* dodaj_do_tablicy(char *tablica, int rozmiar, char znak)
{
    //ta tablica zapisuje caly, oryginalny ciag wprowadzony przez uzytkownika
    //bedzie ona potrzebna na koniec, by zapisac jej wszystki znaki w formie kodu

    tablica=(char*)realloc(tablica,rozmiar*sizeof(char));                   //zwiekszenie tablicy o jeden element
    tablica[rozmiar-1]=znak;                                                //zapisanie znaku

    return tablica;                                                         //zwrocenie kopii wskaznika na tablice by zapisac go w oryginale
}

/** \brief sortuj
* - Sortuje liste na podstawie wystapien znakow. Wskazniki miedzy komorkami nie zmieniaja sie, przenoszone sa wszystkie dane miedzy komorkami. Otrzymuje wskaznik na drzewo.
*/
void sortuj(drzewo *wskaznik)
{
    //funkcja sortuje liste na podstawie wystapien znaku,
    //metoda przenoszenia danych miedzy komorkami, bez ruszania wskaznikow "nast"
    //funkcja sortuj poza koduj/dekoduj jest wielokrotnie wykorzystywana przez funkcje "zmien_liste_na_drzewo"
    //dlatego oprocz znakow i ilosci powtorzen w komorkach przenoszone sa takze wskazniki potomkow (lewo i prawo)

    drzewo *pomocnik1, *pomocnik2;
    pomocnik1=wskaznik;
    char pom_znak;
    int pom_ile_razy;
    drzewo *pom;

    while(pomocnik1!=NULL){                                                 //przejscie wszystkich komorek listy
        pomocnik2=pomocnik1->nast;                                          //rozpoczecie porownywania od kolejnej komorki
        while(pomocnik2!=NULL){                                             //i przejscie przez wszystkie kolejne komorki (sortowanie babelkowe)
            if(pomocnik1->ile_razy>pomocnik2->ile_razy){                    //przypadek odnalezienia wartosci mniejszej (poniewaz sortujemy rosnaco)
                pom_ile_razy=pomocnik2->ile_razy;
                pom_znak=pomocnik2->znak;
                pomocnik2->ile_razy=pomocnik1->ile_razy;
                pomocnik2->znak=pomocnik1->znak;
                pomocnik1->znak=pom_znak;
                pomocnik1->ile_razy=pom_ile_razy;                           //zamienienie zmiennych "znak" i "ile_razy"

                pom=pomocnik1->lewo;
                pomocnik1->lewo=pomocnik2->lewo;
                pomocnik2->lewo=pom;
                pom=pomocnik1->prawo;
                pomocnik1->prawo=pomocnik2->prawo;
                pomocnik2->prawo=pom;                                       //zamienienie wskaznikow "prawo" i "lewo"
            }
            pomocnik2=pomocnik2->nast;                                      //przejscie do kolejnego elementu przy przeszukiwaniu
        }
        pomocnik1=pomocnik1->nast;                                          //przejscie do kolejnego elementu po skonczonym przeszukiwaniu
    }
}

/** \brief wyswietl_ile
* - Wyswietla histogram na podstawie posortowanej listy. Otrzymuje wskaznik na drzewo.
*/
void wyswietl_ile(drzewo *wskaznik)
{
    drzewo *pomocnik=wskaznik;

    while(pomocnik!=NULL){
        printf("%c - %d razy\n",pomocnik->znak,pomocnik->ile_razy);
        pomocnik=pomocnik->nast;
    }
    printf("\n");
}

/** \brief wyswietl_kod
* - Wyswietla znaki oraz przydzielone im kody w drzewie. Otrzymuje wskaznik na drzewo.
*/
void wyswietl_kod(drzewo *wskaznik)
{
    if(wskaznik->lewo)
        wyswietl_kod(wskaznik->lewo);
    if(wskaznik->prawo)
        wyswietl_kod(wskaznik->prawo);
    if(wskaznik->znak!='\0')
        printf("%c - %d\n", wskaznik->znak, wskaznik->kod);
}

/** \brief zapisanie_klucza
* - Klucz zapisuje informacje jakie znaki sa uzywane oraz ile razy wystapily w pliku. Otrzymuje argumenty: wskaznik na drzewo, wskaznik na plik.
*/
void zapisanie_klucza(drzewo *wskaznik, FILE *plik)
{
    //klucz zapisuje informacje jakie znaki sa uzywane oraz ile razy wystapily
    //wazna jest tez ich kolejnosc wzgledem siebie, poniewaz funkcja sortowanie zwraca uwage tylko na ilosc wystapien
    //w sytuacji gdyby dwa znaki o tej samej ilosci powtorzen zostaly zamienione w kluczu
    //zauwazylibysmy rowniez zamiane tych znakow podczas dekodowania

    drzewo *pomocnik=wskaznik;

    while(pomocnik!=NULL){                                                   //przejscie przez wszystkie komorki listy
        fprintf(plik, "%c\n%d\n",pomocnik->znak,pomocnik->ile_razy);         //zapisywanie w nowych linijkach: najpierw znak, potem ilosc powtorzen
        pomocnik=pomocnik->nast;
    }
    fprintf(plik,"\n");                                                      //odzdzielenie klucza od rozmiaru i kodu wlasciwego w pliku tak by program dekodujacy mogl rozpoznac koniec klucza
}

/** \brief zmien_liste_na_drzewo
* - Funkcja zamieniajace liste na drzewo. Wielokrotnie tworzac nowe wzely i sortujac ustawia komorki w drzewo w zaleznosci od wystapien znakow. Im znak czesciej wystepuje tym blizej korzenia bedzie umieszczony, a co za tym idzie jego kod bedzie krotszy. Otrzymuje wskaznik na liste i oddaje ten sam (ale od tej chwili na korzen drzewa)
*/
drzewo* zmien_liste_na_drzewo(drzewo *wskaznik)
{
    //skomplikowana funkcja tworzaca nowe, puste komorki zwane WEZLAMI
    //sluza one stworzeniu optymalnej drogi do poszczegolnych znakow w zaleznosci od ich czestotliwosci
    drzewo *pomocnik;
    drzewo *element1;
    drzewo *element2;

    while (1){
        element1=wskaznik;                                                    //ustawienie pomocniczych wskaznikow lokalnych na dwa najrzadziej uzywane WEZLY lub KOMORKI ZE ZNAKIEM
        element2=element1->nast;

        if(element2==NULL){                                                   //przypadek w ktorym cala lista stanie sie drzewem
            break;                                                            //zakonczenie funkcji
        }
        pomocnik=(drzewo*)malloc(sizeof(drzewo));                             //stworzenie nowego wezla
        pomocnik->kod=2;
        pomocnik->znak='\0';
        pomocnik->lewo=element1;                                              //przypisanie pod jego wskazniki dwoch elementow o ktorych byla mowa
        pomocnik->prawo=element2;
        pomocnik->ile_razy=element1->ile_razy+element2->ile_razy;             //jego zmiennej oznaczajacej czestotliwosc przypisywane sa ilosc wystapien komorek/wezlow pod nim
        pomocnik->nast=element2->nast;                                        //wstawienie wezla w powrotem do list podmieniajac poprzednie dwa elementy (ktore teraz sa pod nim)
        wskaznik=pomocnik;                                                    //ustawienie glownego wskaznika na nowy wezel
        sortuj(wskaznik);                                                     //sortowanie sluzace ustawieniu wezla w odpowiednim miejscu na liscie
    }

    return wskaznik;                                                          //zwrocenie kopii wskaznika glownego by zapisac go w oryginale

}

/** \brief ustal_kod
* - Funkcja przechodzi rekurencyjnie cale drzewo od korzenia do ostatniego potomka. Idac w lewo dodaje "1" do kod kazdego wezla pod tym od ktorego wyszlismy. Analogicznie, idac w prawo, kazdy kolejny w kodzie otrzyma "2". OZnacza to ze im dalej jesty potomek, tym dluzszy kod dostanie. Otrzymuje argumenty: wskaznik na drzewo i kod
*/
void ustal_kod(drzewo *wskaznik, int kod)
{
    //funkcje rekurencyjna nadajaca wlasciwy kod powstalemu drzewu

    if(wskaznik->lewo!=NULL){                                                 //idac po drzewie do wezla/komorki w lewo
        kod=kod*10+1;                                                         //dodamy na koniec kody cyfre 1
        wskaznik->lewo->kod=kod;
        ustal_kod(wskaznik->lewo, kod);
        kod=(kod-1)/10;
    }
    if(wskaznik->prawo!=NULL){                                                 //idac po drzewie do wezla/komorki w prawo
        kod=kod*10+2;                                                          //dodamy na konic kodu cyfre 2
        wskaznik->prawo->kod=kod;
        ustal_kod(wskaznik->prawo, kod);
        kod=(kod-2)/10;
    }

}

/** \brief wizualizacja
* - Funkcja rekurencyjnie wyswietlajaca drzewo, na podstawie sposobu analogicznego do wykorzystanego w funkcji "ustal_kod". Im dalej jest potomek tym wiecej odstepow otrzyma, a przez to uwidocznione zostana roznice w poziomach miedzy wezlami. Otrzymuje wskaznik na drzewo oraz informacje o poziomie na ktorym sie znajduje.
*/
void wizualizacja(drzewo *wskaznik, int poziom)
{
    int i;
    for(i=0;i<poziom;i++)                                                   //petla wypisujaca ilosc odstepow w zaleznosci od poziomu w drzewiw
        printf("     ");
    if(wskaznik->znak!='\0')                                                //przypadek gdy jest to potomek
        printf("%c\n", wskaznik->znak);
    if(wskaznik->znak=='\0')                                                //przypadek gdy jest to wezel
        printf("%d\n", poziom);

    if(wskaznik->lewo!=NULL){                                               //przypadek gdy istnieje lewy potomek
        poziom=poziom+1;                                                    //zwiekszenie poziomu
        wizualizacja(wskaznik->lewo, poziom);                               //wejscie w lewego potomka
        poziom=poziom-1;                                                    //zmniejszego poziomu przed dalszymi operacjami
    }
    if(wskaznik->prawo!=NULL){                                              //przypadek gdy istnieje prawy potomek
        poziom=poziom+1;                                                    //zwiekszenie poziomu
        wizualizacja(wskaznik->prawo, poziom);                              //wejscie w prawego potomka
        poziom=poziom-1;                                                    //zmniejszenie poziomu przed dalszymi operacjami
    }
}

/** \brief znajdz_kod
* - Sluzy do znajdywania kodu pod konkretnym znakiem. Otrzymuje wskaznik na drzewo i wskaznik na zmienna ktorej zostanie przydzielony odpowiedni kod. Otrzymuje wskaznik na drzewo, szukany znak oraz wskaznik na zmienan w ktore zostanie zapisany kod.
*/
void znajdz_kod(drzewo *wskaznik, char znak, FILE *plik, int *kod)
{
    //funkcja rekurencyjna przyjmujaca znak i znajdujaca jego kod w drzewie

    if(wskaznik!=NULL){
        znajdz_kod(wskaznik->lewo, znak, plik, kod);
        znajdz_kod(wskaznik->prawo, znak, plik, kod);
        if(wskaznik->znak==znak){                                               //przypadek odnalezienia kodu
             *kod=(wskaznik->kod);
        }
    }
}

/** \brief kodowanie_po_cyfrze
* - Funkcja ktora kod binarny otrzymany po ulozeniu wszystkich kodow odpowiednich znakow obok siebie wysyla pojedynczo cyfry do funkcji "kodowanie_po_znaku" Otrzymuje argumenty:  zmienna z kodem jednego znaku (dla siebie samej), oraz wskaznik na plik, wskazniki na zmienne dotyczace litery, sumy i licznika dla funkcji (koduj_po_znaku).
*/
void kodowanie_po_cyfrze(FILE *plik, int *kod, char *litera, int *licznik, int *suma)
{
    //kod "binarny" stworzony z jedynek i dwojek jest tylko imitacja kodu binarnego
    //tak naprawde operacje przeprowadzane sa na zwyklych liczbach w systemie dziesietnym
    //czyli np zeby dodac cyfre na koncu, trzeba pomnozyc liczbe razy 10 i dodac taka ktora chcemy
    //stad tez w kodzie stosowane sa jedynki i dwojki zamiast zer i jedynek
    //gdyz liczb nie zapisuje sie z zerami na poczatku

    int pom=*kod;                                                               //zmienna ktora bedze przyjmowala wartosc pierwszej cyfry kodu znaku
    int licznik_bitow=10;                                                       //licznik w ktorej dziesiatce liczby (czyli cyfrze kodu) aktualnie sie znajdujemy
    while(pom!=1&&pom!=2){                                                      //petla konczaca sie kiedy w kodzie zostanie tylko ostatnia (pierwsza) cyfra
        pom=pom/10;                                                             //zmienna pom jest "obierana" z ostatnich cyfr az zostanie tylko pierwsza
        licznik_bitow=licznik_bitow*10;                                         //jednoczesnie liczone jest jej miejsce
    }
    printf("%d", pom);
    kodowanie_po_znaku(plik, litera, licznik, suma, pom);                       //otrzymana cyfra zostaje wyslana do funkcji ktora ja koduje
    *kod=*kod-pom*licznik_bitow/10;                                             //od oryginalnego kodu zostaje odcieta pierwsza cyfra, ktora program juz wykorzystal
}

/** \brief kodowanie_po_znaku
* - Funkcja grupuje otrzymany kod binarny po 8, a kazdy z segmentow zamienia na liczbe calkowita, by nadac wartosc zmienneej o typie char, ktora zostanie umieszczona w pliku. Innymi slowy zapisuje ona kod wlasciwy w pliku. Otrzymuje argumenty: wskaznik na plik, wskazniki na litere, sume, licznik.
*/
void kodowanie_po_znaku(FILE *plik, char *litera, int *licznik, int *suma, int pom)
{
    pom--;                                                                      //w tym miejscu kod z "1" i "2" zostaje zamieniony na "0" i "1"
    *suma=*suma+pom*pow(2,*licznik);                                            //do sumy dopisuje sie cyfre pomnozana przez odpowiednia potege dwojki (zalezna od osobnego licznika ktory okresla miejsca w porcji osmio bitowej)
                                                                                //innymi slowy wykorzystwany jest tutaj prosty konwerter z systemu dwojkowego na dziesietny
    if(*licznik==0){                                                            //przypadek w ktorym licznik porcjuacy po 8 bitow osiagnie ozstatni bit (w tym przypadku kiedy dojdzie do 0)
        *licznik=7;                                                             //licznik jest resetowany
        *litera=*suma;                                                          //znakowi przydzielamy wartosc otrzymanej liczby dziesietnej
        printf(" (%c) ", *litera);
        fprintf(plik, "%c", *litera);                                           //znak zapisujemy do pliku
        *suma=0;                                                                //zerowanie sumy
    }
    else{                                                                       //przypadek w ktorym nie zajmujemy sie ostatnim bitem
        *licznik=*licznik-1;                                                    //licznik przechodzi na kolejna cyfre
    }
}

/** \brief usun_strukture
* - Zwalnia pamiec po stworzonym drzewie. Otrzymuje wskaznik na drzewo.
*/
void usun_strukture(drzewo *wskaznik)
{
  if(wskaznik!=NULL)
  {
    usun_strukture(wskaznik->lewo);
    usun_strukture(wskaznik->prawo);
    free(wskaznik);
  }
}

/** \brief odczyt_klucza
* - Odczytuje kolejne znaki klucza oddzielane "\n" i tworzy odwrocona liste jak przy kodowaniu (tuz po wprowadzeniu ciagu). Otrzymuje wskaznik na plik z kluczem i wskaznik na nowo powstala liste.
*/
void odczyt_klucza(FILE *plik, drzewo **wskaznik)
{
    char a;                                                                 //robocza zmienna ktora sluzy przy pobieraniu znaku z pliku
    int i;
    char litera;
    int liczba;
    while(1){                                                               //petla pobierajaca kolejne znaki
        fscanf(plik,"%c", &a);                                              //pobranie litery z klucza
        if(a!='\n'){                                                        //przypadek w ktorej nie jest to koniec klucza
            litera=a;                                                       //zapisanie pobranej znaku do zmiennej litera
            fscanf(plik,"%c", &a);                                          //pobranie znaku nowej linii
            fscanf(plik,"%c", &a);                                          //pobranie pierwszej cyfry liczby dotyczacej ilosci wystapien znaku
            liczba=0;                                                       //wyzerowanie liczby
            while(a!='\n'){                                                 //petla pozwalajace pobieranie kolejnych cyfr liczby wystapien
                liczba=liczba*10+a-48;                                      //dodanie cyfry do zmiennej liczba
                fscanf(plik,"%c", &a);                                      //pobranie kolejnego znaku (jesli nie jest to znak nowej linii, petl wykonuje sie znowu
            }
            for(i=0; i<liczba; i++){                                        //petla pozwalajace na wlozenie litery do listy tyle razy ile wskazuje zmienna liczba
                *wskaznik=dodaj_do_listy(*wskaznik, litera);                //dodanie znaku do listy
            }
        }
        else{                                                               //przypadek w ktorej jest to koniec klucza
            break;                                                          //zakonczenie odczytywania klucza
        }
    }
}

/** \brief odwroc_liste
* - Funkcja odwracajac liste. Potrzebna jest dlatego ze przy odczytywaniue klucza lista tworzona jest wspak. Otrzymuje wskanzik na te liste.
*/
void odwroc_liste(drzewo **wskaznik)
{
    drzewo *pom1,*pom2,*pom3;                                               //wskazniki na trzy kolejne elementy listy
    pom1=*wskaznik;                                                         //poczynajac od wskaznika glownego
    pom2=pom1->nast;
    pom3=pom2->nast;
    pom1->nast=NULL;                                                        //wyzerowanie wskaznika w komorce na koncu listy (poniewaz bedzie pierwszym)
    while(1){                                                               //petla pozwalajace na przejscie do koncza listy odwracajac jej kolejnosc
        if(pom3==NULL){                                                     //przypadek w ktorym program dochodzi do konca listy
            pom2->nast=pom1;                                                //odwrocenie wskaznika w komorce
            *wskaznik=pom2;                                                 //ustawienie wskaznika glownego na ostatnim elemencie
            break;                                                          //wyjscie z funkcji
        }
        else{                                                               //przypadek w ktorym program nie jest jeszcze na koncu listy
            pom2->nast=pom1;                                                //odwrocenie wskaznika w komorce
            pom1=pom2;                                                      //przeniesienie wszystkich wskaznikow pomocniczych o jedna komorke do przodu
            pom2=pom3;
            pom3=pom3->nast;

        }
    }
}

/** \brief dodaj_do_kolejki
* - Dodaje do kolejki kolejne cyfry kodu binarnego utworzonego z kodu wlasciwego. Otrzymuje podwojny wskaznik na czolo i ogon do kolejki, a takze jedna cyfre.
*/
void dodaj_do_kolejki(kolejka **czolo, kolejka **ogon, int liczba)
{
    kolejka *ogon2=*ogon;                                                       //stworzenie kopii oryginalu wskaznika na ogon kolejki
    kolejka *pomocnik=(kolejka*)malloc(sizeof(kolejka));                        //stworzenie nowek komorki kolejki
    pomocnik->kod=liczba;                                                       //nadanie wartosci jej kodu
    pomocnik->nast=NULL;                                                        //wyzerowanie jej wskaznika
    if(*czolo==NULL){                                                           //przypadek w ktorym kolejka jest pusta
        *czolo=pomocnik;                                                        //czolo o ogon ustawiane sa na nowej komorce
        *ogon=pomocnik;
    }
    else{                                                                       //przypadek w ktorym kolejka nie jest pusta
        ogon2->nast=pomocnik;                                                   //wskaznik poprzedniej komorki ustawiany jest na nowa
        *ogon=pomocnik;                                                         //wskaznika ogon ustawiany jest na nowa komorke
    }
}

/** \brief usun_z_kolejki
* - Zabiera po jednej cyfrze z kolejki z kodem binarnym. Otrzymuje wskazniki na czolo i ogon, a zwraca zdjeta cyfre.
*/
int usun_z_kolejki(kolejka **czolo, kolejka **ogon)
{
    kolejka* czolo2=*czolo;                                                     //stworzenie kopii oryginalu wskaznika na czolo kolejki
    int liczba=czolo2->kod;                                                     //pobranie zawartosci usuwanej komorki
    if(czolo2->nast==NULL){                                                     //przypadek w ktorej pobierana komorka jest ostatnia w kolejkce
        free(*czolo);                                                           //zwolnienie pamieci
        *czolo=NULL;                                                            //zerowanie wskaznikow
        *ogon=NULL;
    }
    else{                                                                       //przypadek w ktorej pobierana komorka nie jest ostatnia w kolejce
        *czolo=czolo2->nast;                                                    //ustawienie czola na kolejny element
        free(czolo2);                                                           //zwolnienie pamieci
    }
    return liczba;                                                              //zwrocenie wartosci z pobranej komorki
}

/** \brief usun_z_kolejki
* - Na podstawie otrzymanego kodu binarnego szuka odpowiednich znakow, aby zapisac oryginalny ciag w pliku i wyswietlic go na ekranie. Otrzymuje wskaznik na drzewo w ktorym bedzie szukal, kod, wskaznik na plik oraz wskaznik na zmienna sygnalizujaca powodzenie operacji.
*/
void szukanie(drzewo *wskaznik, int kod, FILE *plik, int *znaleziony)
{
    if(wskaznik->lewo){                                                         //jesli istnieje lwey potomek
        szukanie(wskaznik->lewo, kod, plik, znaleziony);                        //wywolanie funkcji dla lewego potomku
    }
    if(wskaznik->prawo){                                                        //jesli istnieje prawy potomek
        szukanie(wskaznik->prawo, kod, plik, znaleziony);                       //wywolanie funkcji dla prawego potomka
    }
    if(wskaznik->kod==kod&&wskaznik->lewo==NULL&&wskaznik->prawo==NULL){        //przypadek w ktorym odnaleziono szukany kod oraz komorka nie ma potomkow (czyli zawiera znak)
        fprintf(plik, "%c",wskaznik->znak);                                     //zapisanie znaku do pliku "oryginal.txt"
        printf("%c", wskaznik->znak);
        *znaleziony=1;                                                          //zaznaczenie ze udalo sie znalezc znak
    }
}
