#include <iostream>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <cstdlib>
using namespace std;

#define K_CZEROWNY 1
#define K_NIEBIESKI 2
#define K_ZOLTY 3
#define K_ZIELONY 4
#define K_BIALY 5

struct okno
{
    WINDOW *win;
    int color;
    int a;
    int b;
};
struct timer
{
    int czas = 0;
    int iteracja = 0;
    const int limit_iteracji = 10;
    int licznik = 0;
    const int limit = 7;
    int licznik_czasu = 0;
    int aktualny_czas = 0;
    const int kontrolne = 100;
};
struct przeszkoda
{
    int poz_pion;
    int poz_poz;
};
struct stan_gry
{
    timer *czas_gry;
    struct
    {
        int wys;
        int szer;
        int auta;
        int ilosc_przeszkod = 100;
        przeszkoda pozyjce_przeszkod[100];
    } plansza;
    struct
    {
        int poz_pion = 10;
        int poz_poziom = 0;
        okno *win;
    } frog;

    struct
    {
        int poz_pion = 1;
        int poz_poziom = 5;
        int speed = 3;
        int dlugosc = 2;
        int szerokosc = 3;
        int zatrzymanie_szansa = 80;
        int przyjacielski_szansa = 0;
        int postoj = 0;
        int przyjaciel = 0;
        okno *win;
    } car[40];
    struct
    {
        int poz_pion = 21;
        int poz_poziom = 10;
        okno *win;
    } status;
};
int ekran_startowy();
void startowe_predkosci_aut(stan_gry &gra, int i);
/// zapis i odczyty z pliku ///
void odczyt_rozgrywki(stan_gry &gra) // przypisanie grze początkowego stanu
{
    FILE *plik_z_poziomami;
    plik_z_poziomami = fopen("poziom.txt", "r");
    int wczytany_poziom = ekran_startowy();
    int poziom;
    int pom_dlug = 0;
    int pom_szer = 0;
    while (wczytany_poziom--)
    {
        fscanf(plik_z_poziomami, "%d", &poziom);
        fscanf(plik_z_poziomami, "%d", &gra.plansza.wys);
        fscanf(plik_z_poziomami, "%d", &gra.plansza.szer);
        fscanf(plik_z_poziomami, "%d", &gra.plansza.auta);
        fscanf(plik_z_poziomami, "%d", &pom_dlug);
        fscanf(plik_z_poziomami, "%d", &pom_szer);
        for (int i = 0; i < gra.plansza.auta; i++)
        {
            gra.car[i].dlugosc = pom_dlug;
            gra.car[i].szerokosc = pom_szer;
        }
    }
}
void zapis(stan_gry &gra, FILE *plik)
{
    if (plik == NULL)
    {
        clear();
        mvprintw(0, 0, "Nie znaleziono pliku");
        refresh();
        return;
    }
    fseek(plik, 0, SEEK_SET);
    fwrite(&gra, sizeof(struct stan_gry), 1, plik);
}

void odczyt(stan_gry &gra, FILE *plik)
{
    if (plik == NULL)
    {
        clear();
        mvprintw(0, 0, "Nie znaleziono pliku");
        refresh();
    }
    fseek(plik, 0, SEEK_SET);
    fread(&gra, sizeof(struct stan_gry), 1, plik);
}
/// zapis i odczyt z pliku ///

/// funkcja timera ///
void odmierz_czas(stan_gry &gra)
{
    usleep(6000);
    gra.czas_gry->iteracja++;
    gra.czas_gry->licznik_czasu++;
    if (gra.czas_gry->iteracja >= gra.czas_gry->limit_iteracji)
    {
        gra.czas_gry->czas++;
        gra.czas_gry->iteracja = 0;
    }
    if (gra.czas_gry->licznik_czasu >= gra.czas_gry->kontrolne)
    {
        gra.czas_gry->aktualny_czas++;
        gra.czas_gry->licznik_czasu = 0;
    }

    gra.czas_gry->licznik++;
}
/// funkcja timera ///

/// Funkcja do poruszania żabą  ///
int czy_przeszkoda1(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
        if ((gra.frog.poz_pion + 1 == gra.plansza.pozyjce_przeszkod[i].poz_pion) && (gra.frog.poz_poziom == gra.plansza.pozyjce_przeszkod[i].poz_poz))
            return false;
    return true;
}
int czy_przeszkoda2(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
        if ((gra.frog.poz_pion - 1 == gra.plansza.pozyjce_przeszkod[i].poz_pion) && (gra.frog.poz_poziom == gra.plansza.pozyjce_przeszkod[i].poz_poz))
            return false;
    return true;
}
int czy_przeszkoda3(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
        if ((gra.frog.poz_poziom + 1 == gra.plansza.pozyjce_przeszkod[i].poz_poz) && (gra.frog.poz_pion == gra.plansza.pozyjce_przeszkod[i].poz_pion))
            return false;
    return true;
}
int czy_przeszkoda4(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
        if ((gra.frog.poz_poziom - 1 == gra.plansza.pozyjce_przeszkod[i].poz_poz) && (gra.frog.poz_pion == gra.plansza.pozyjce_przeszkod[i].poz_pion))
            return false;
    return true;
}
int czy_podwozka(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
    {
        if ((gra.frog.poz_poziom + 2 == gra.car[i].poz_poziom) && (gra.frog.poz_pion == gra.car[i].poz_pion))
            return true;
        if ((gra.frog.poz_poziom + 1 == gra.car[i].poz_poziom) && (gra.frog.poz_pion == gra.car[i].poz_pion))
            return true;
        if ((gra.frog.poz_poziom + 2 == gra.car[i].poz_poziom) && (gra.frog.poz_pion + 1 == gra.car[i].poz_pion))
            return true;
        if ((gra.frog.poz_poziom + 1 == gra.car[i].poz_poziom) && (gra.frog.poz_pion + 1 == gra.car[i].poz_pion))
            return true;
    }
    return false;
}
void frog_jump(int ch, stan_gry &gra)
{
    if (ch == 's' || ch == 'd' || ch == 'w' || ch == 'a')
        gra.czas_gry->licznik = 0;
    wclear(gra.frog.win->win);
    wrefresh(gra.frog.win->win);
    if (ch == 's')
    {
        if (czy_przeszkoda1(gra))
            gra.frog.poz_pion++;
    }
    else if (ch == 'w')
    {
        if (czy_przeszkoda2(gra))
            gra.frog.poz_pion--;
    }
    else if (ch == 'd')
    {
        if (czy_przeszkoda3(gra))
            gra.frog.poz_poziom++;
    }
    else if (ch == 'a')
    {
        if (czy_przeszkoda4(gra))
            gra.frog.poz_poziom--;
    }
    else if (ch == 'e')
    {
        if (czy_podwozka(gra))
            gra.frog.poz_poziom += 10;
    }
    mvwin(gra.frog.win->win, gra.frog.poz_pion, gra.frog.poz_poziom);
    wattron(gra.frog.win->win, COLOR_PAIR(K_ZOLTY));
    mvwprintw(gra.frog.win->win, 0, 0, " ");
    wrefresh(gra.frog.win->win);
}
/// Funkcja do poruszania żabą ///
int czy_zatrzymanie(stan_gry &gra, int i)
{
    int a = rand() % 100;
    if (a >= gra.car[i].zatrzymanie_szansa)
    {

        int zakres[4] = {0};
        zakres[0] = gra.car[i].poz_pion - 1;
        zakres[1] = gra.car[i].poz_pion + gra.car[i].dlugosc + 1;
        zakres[2] = gra.car[i].poz_poziom;
        zakres[3] = gra.car[i].poz_poziom + gra.car[i].szerokosc - 1;
        if (gra.frog.poz_pion >= zakres[0] && gra.frog.poz_pion <= zakres[1] && gra.frog.poz_poziom >= zakres[2] && gra.frog.poz_poziom <= zakres[3])
        {
            return true;
        }
    }
    return false;
}
int przyjacielski(stan_gry &gra, int i)
{
    int a = rand() % 100;
    if (a >= gra.car[i].przyjacielski_szansa)
    {
        return true;
    }
    return false;
}
/// Funkcja malowania aut ///
void car_go(stan_gry &gra, int i)
{
    nodelay(stdscr, TRUE);
    wclear(gra.car[i].win->win);
    wrefresh(gra.car[i].win->win);
    mvwin(gra.car[i].win->win, gra.car[i].poz_pion, gra.car[i].poz_poziom);
    if (gra.car[i].przyjaciel)
        wattron(gra.car[i].win->win, COLOR_PAIR(K_ZIELONY));
    else
    {
        wattron(gra.car[i].win->win, COLOR_PAIR(K_NIEBIESKI));
        if (czy_zatrzymanie(gra, i))
            gra.car[i].postoj = 1;

        if (gra.frog.poz_poziom - 3 > gra.car[i].poz_poziom)
            gra.car[i].postoj = 0;
    }
    for (int a = 0; a < gra.car[i].dlugosc; a++)
    {
        for (int j = 0; j < gra.car[i].szerokosc; j++)
        {
            mvwprintw(gra.car[i].win->win, a, j, "#");
        }
    }

    if (gra.car[i].postoj == 0)
    {
        gra.car[i].poz_pion += 1;
    }
    wrefresh(gra.car[i].win->win);
    if (gra.car[i].poz_pion >= gra.plansza.wys - 1)
    {
        startowe_predkosci_aut(gra, i);
        gra.car[i].poz_pion = 1;
        gra.car[i].przyjaciel = 0;
        if (przyjacielski(gra, i))
            gra.car[i].przyjaciel = 1;
    }

    usleep(7000);
}
/// Funkcja malowania aut ///

/// sprawdzanie kolizji zaby z autem ///
int czy_kolicja(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        int zakres[4] = {0};
        zakres[0] = gra.car[i].poz_pion - 1;
        zakres[1] = gra.car[i].poz_pion + gra.car[i].dlugosc - 2;
        if (czy_zatrzymanie(gra, i))
            zakres[1]++;
        zakres[2] = gra.car[i].poz_poziom;
        zakres[3] = gra.car[i].poz_poziom + gra.car[i].szerokosc - 1;
        if (gra.frog.poz_pion >= zakres[0] && gra.frog.poz_pion <= zakres[1] && gra.frog.poz_poziom >= zakres[2] && gra.frog.poz_poziom <= zakres[3])
        {
            return 1;
        }
    }
    return 0;
}
/// sprawdzanie kolizji zaby z autem ///

/// sprawdzanie warunku wygranej i przegranej///
int wygrana(stan_gry gra)
{
    if (gra.frog.poz_poziom == gra.plansza.szer - 1)
    {
        clear();
        mvprintw(10, 10, "KONIEC GRY -> WYGRALES");
        mvprintw(11, 10, "NACISNIJ P ABY ZAKONCZYC");
        refresh();
        delete gra.czas_gry;
        return 1;
    }
    else
        return 0;
}
void przegrana(stan_gry gra)
{
    clear();
    mvprintw(10, 10, "KONIEC GRY");
    mvprintw(11, 10, "NACISNIJ P ABY ZAKONCZYC");
    refresh();
    delete gra.czas_gry;
}
/// sprawdzanie warunku wygranej i przegranej///

/// poczatkowe rysowanie planszy ///
void czerwone_pole(stan_gry gra) // rysowanie czerwonej obwodki
{
    attron(COLOR_PAIR(K_CZEROWNY));
    for (int i = 0; i < gra.plansza.szer; i++)
    {
        mvprintw(0, i, " ");
        refresh();
    }
    for (int i = 0; i < gra.plansza.szer; i++)
    {
        mvprintw(gra.plansza.wys, i, " ");
        refresh();
    }
}
void zielone_pole(stan_gry gra) // rysowanie mety
{
    attron(COLOR_PAIR(K_ZIELONY));
    for (int i = 1; i < gra.plansza.wys; i++)
    {
        mvprintw(i, gra.plansza.szer - 1, " ");
        refresh();
    }
    attroff(COLOR_PAIR(K_ZIELONY));
}
/// poczatkowe rysowanie planszy ///

/// wyswietlenie statusu gry ///
void status_gry(stan_gry &gra)
{
    // pamieć dla okna statusu
    okno *status_window = new okno;
    status_window->win = newwin(2, 100, gra.plansza.wys + 2, gra.status.poz_poziom);
    gra.status.win = status_window;
    // pamieć dla okna statusu
    wattron(gra.status.win->win, COLOR_PAIR(K_BIALY));
    mvwprintw(gra.status.win->win, 0, 0, "Aktualny czas gry: %d ", gra.czas_gry->aktualny_czas);
    mvwprintw(gra.status.win->win, 1, 20, "Szymon Drywa 203668 Politechnika ");
    wrefresh(gra.status.win->win);
}
/// wyswietlenie statusu gry ///

/// losowe powstawanie elementow ///
int losuj_l(stan_gry &gra)
{
    int losuj_linie;
    losuj_linie = rand() % gra.plansza.auta;
    return losuj_linie;
}
int losuj_w(stan_gry &gra)
{
    int losuj_wys;
    losuj_wys = rand() % (gra.plansza.wys - 1) + 1;
    return losuj_wys;
}
/// losowe powstawanie elementow ///

/// malowanie dziur ///

/// malowanie dziur ///
void maluj_dziury(int linia, int wys, stan_gry &gra)
{
    linia = gra.car[linia].poz_poziom + 1;
    okno *dziura = new okno;
    dziura->win = newwin(1, 1, wys, linia);
    mvwprintw(dziura->win, 0, 0, "*");
    wrefresh(dziura->win);
}
void przeszkody(int linia, int wys, stan_gry &gra, int k)
{
    linia = gra.car[linia].poz_poziom + gra.car[linia].szerokosc + 1;
    okno *przeszkoda = new okno;
    przeszkoda->win = newwin(1, 1, wys, linia);
    gra.plansza.pozyjce_przeszkod[k].poz_pion = wys;
    gra.plansza.pozyjce_przeszkod[k].poz_poz = linia;
    mvwprintw(przeszkoda->win, 0, 0, "#");
    wrefresh(przeszkoda->win);
}
/// nadanie wartosci elementą gry ///
void startowe_pozycje_aut(stan_gry &gra) // poczatkowe pozycje
{
    int a = 5;
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        gra.car[i].poz_poziom = a;
        a += gra.car[i].szerokosc + 3;
    }
}
void startowe_predkosci_aut(stan_gry &gra, int i) // poczatkowa predkosc
{
    gra.car[i].speed = rand() % 5 + 1;
}
void tworzenie_okien_wyswietlania(stan_gry &gra, int i) // tworzenie okien dla aut
{
    okno *car_window = new okno;
    car_window->win = newwin(gra.car[i].dlugosc, gra.car[i].szerokosc, gra.car[i].poz_pion, gra.car[i].poz_poziom);
    gra.car[i].win = car_window;
}
int ekran_startowy()
{
    keypad(stdscr, TRUE);
    int pozycja_kropki = 15;
    int ch = ERR;
    while (ch != 10)
    {
        attron(COLOR_PAIR(K_BIALY));
        mvprintw(pozycja_kropki, 8, " ");
        refresh();
        attroff(COLOR_PAIR(K_BIALY));
        mvprintw(13, 10, "Wybierz poziom: ");
        mvprintw(15, 15, "Poziom pierwszy: ");
        mvprintw(17, 15, "Poziom drugi: ");
        mvprintw(19, 15, "Poziom trzeci: ");
        refresh();
        usleep(1000);
        ch = getch();
        if (ch == 's')
        {
            if (pozycja_kropki == 19)
                pozycja_kropki = 15;
            else
                pozycja_kropki += 2;
        }
        else if (ch == 'w')
        {
            if (pozycja_kropki == 15)
                pozycja_kropki = 19;
            else
                pozycja_kropki -= 2;
        }
        clear();
        refresh();
    }
    attroff(COLOR_PAIR(K_BIALY));
    clear();
    refresh();
    if (pozycja_kropki == 19)
        return 3;
    if (pozycja_kropki == 17)
        return 2;
    if (pozycja_kropki == 15)
        return 1;
    return -1;
}
void start_gry(stan_gry &gra) // inicjalizacja początkowego stanu gry
{
    czerwone_pole(gra);
    startowe_pozycje_aut(gra);
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
        przeszkody(losuj_l(gra), losuj_w(gra), gra, i);
    zielone_pole(gra);
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        startowe_predkosci_aut(gra, i);
        tworzenie_okien_wyswietlania(gra, i);
    }
    gra.czas_gry = new timer();
}
/// nadanie wartosci elementą gry ///

/// główna pętla gry ///
void podczasgry(stan_gry &gra)
{
    start_gry(gra);
    startowe_pozycje_aut(gra);
    timeout(100);
    int ch = ERR;
    int kolizja = 0;
    int pom_akt = 0;
    while (ch != 'p' || kolizja == 1 || wygrana(gra))
    {
        for (int i = 0; i < gra.plansza.auta; i++)
        {
            odmierz_czas(gra);
            status_gry(gra);
            if (gra.czas_gry->czas % gra.car[i].speed == 0 || gra.czas_gry->czas % gra.car[i].speed == 1)
                car_go(gra, i);
            ch = getch();
            kolizja = czy_kolicja(gra);
            if (kolizja == 1 || wygrana(gra))
                break;
            if (gra.czas_gry->licznik >= gra.czas_gry->limit && ch != ERR)
                frog_jump(ch, gra);
            flushinp();
            kolizja = czy_kolicja(gra);
            if (kolizja == 1 || wygrana(gra))
                break;
            if (ch == 'p')
                break;
        }
        if (kolizja == 1 || wygrana(gra))
            break;
        /*
    if ((gra.czas_gry->aktualny_czas) % 2 == 0&&gra.czas_gry->aktualny_czas!=pom_akt)
        {
            maluj_dziury(losuj_l(gra), losuj_w(gra), gra);
            pom_akt=gra.czas_gry->aktualny_czas;
        }
        */
    }
    if (kolizja == 1)
        przegrana(gra);
    else
        wygrana(gra);
    while (ch != 'p')
        ch = getch();
}
/// główna pętla gry ///

/// inicjalizacja ncurses itp ///
void ustawienia()
{
    srand(time(NULL));
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    start_color();
    init_pair(K_CZEROWNY, COLOR_RED, COLOR_RED);
    init_pair(K_NIEBIESKI, COLOR_BLUE, COLOR_BLUE);
    init_pair(K_ZOLTY, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(K_ZIELONY, COLOR_GREEN, COLOR_GREEN);
    init_pair(K_BIALY, COLOR_RED, COLOR_WHITE);
}
/// inicjalizacja ncurses itp ///

int main()
{
    ustawienia();
    stan_gry gra;
    odczyt_rozgrywki(gra);
    FILE *plik;
    plik = fopen("struktura.txt", "r+");
    okno *frog_window = new okno;
    frog_window->win = newwin(1, 1, gra.frog.poz_pion, gra.frog.poz_poziom);
    gra.frog.win = frog_window;
    podczasgry(gra);
    fclose(plik);
    endwin();
    return 0;
}
