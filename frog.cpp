#include <iostream>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <cstdlib>
using namespace std;

#define WYS 20
#define SZER 75
#define K_CZEROWNY 1
#define K_NIEBIESKI 2
#define K_ZOLTY 3
#define K_ZIELONY 4
#define AUTA 10
struct okno
{
    WINDOW *win;
    int color;
    int a;
    int b;
};

struct stan_gry
{
    int punkty = 123;
    int highscore = 1243;
    int timer = 1274;
    struct
    {
        int wys = WYS;
        int szer = SZER;
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
        okno *win;
    } car[AUTA];
};

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

void wypisztest(stan_gry &gra)
{
    clear();
    mvprintw(0, 0, "%d", gra.highscore);
    mvprintw(1, 0, "%d", gra.plansza.szer);
    refresh();
}

void czerwone_pole()
{
    attron(COLOR_PAIR(K_CZEROWNY));
    for (int i = 0; i < SZER; i++)
    {
        mvprintw(0, i, " ");
        refresh();
    }
    for (int i = 0; i < SZER; i++)
    {
        mvprintw(WYS, i, " ");
        refresh();
    }
}
void zielone_pole()
{
    attron(COLOR_PAIR(K_ZIELONY));
    for (int i = 1; i < WYS; i++)
    {
        mvprintw(i, SZER - 1, " ");
        refresh();
    }
    attroff(COLOR_PAIR(K_ZIELONY));
}

// Funkcja do poruszania żabą
void frog_jump(int ch, stan_gry &gra)
{
    wclear(gra.frog.win->win);
    wrefresh(gra.frog.win->win);
    if (ch == 's')
        gra.frog.poz_pion++;
    else if (ch == 'w')
        gra.frog.poz_pion--;
    else if (ch == 'd')
        gra.frog.poz_poziom++;
    else if (ch == 'a')
        gra.frog.poz_poziom--;
    mvwin(gra.frog.win->win, gra.frog.poz_pion, gra.frog.poz_poziom);
    wattron(gra.frog.win->win, COLOR_PAIR(K_ZOLTY));
    mvwprintw(gra.frog.win->win, 0, 0, " ");
    wrefresh(gra.frog.win->win);
}
void startowe_predkosci_aut(stan_gry &gra, int i);
void odmierz_czas(int &czas, int &licznik_iteracji, int limit_iteracji)
{
    // Zwiększamy licznik iteracji
    licznik_iteracji++;

    // Jeżeli licznik iteracji osiągnie próg (np. 10 iteracji = 1 sekunda)
    if (licznik_iteracji >= limit_iteracji)
    {
        czas++;               // Zwiększamy licznik czasu
        licznik_iteracji = 0; // Resetujemy licznik iteracji
    }
}
void car_go(stan_gry &gra, int i)
{
    nodelay(stdscr, TRUE);
    wclear(gra.car[i].win->win);
    wrefresh(gra.car[i].win->win);
    mvwin(gra.car[i].win->win, gra.car[i].poz_pion, gra.car[i].poz_poziom);
    wattron(gra.car[i].win->win, COLOR_PAIR(K_NIEBIESKI)); // Aktywacja niebieskiej pary kolorów
    mvwprintw(gra.car[i].win->win, 0, 0, "#");
    mvwprintw(gra.car[i].win->win, 1, 0, "#");
    mvwprintw(gra.car[i].win->win, 0, 1, "#");
    mvwprintw(gra.car[i].win->win, 1, 1, "#");
    mvwprintw(gra.car[i].win->win, 0, 2, "#");
    mvwprintw(gra.car[i].win->win, 1, 2, "#");
    gra.car[i].poz_pion += 1;
    wrefresh(gra.car[i].win->win);
    if (gra.car[i].poz_pion >= WYS - 1)
    {
        startowe_predkosci_aut(gra, i);
        gra.car[i].poz_pion = 1;
    }
    usleep(7000);
}
int rozmiar_aut() // losowanie rozmiaru auta
{
    return 2; // rand() % 7 + 1;
}
void startowe_pozycje_aut(stan_gry &gra)
{
    int a = 5;
    for (int i = 0; i < AUTA; i++)
    {
        gra.car[i].poz_poziom = a + 1;
        a += rozmiar_aut() + 2;
    }
}
void startowe_predkosci_aut(stan_gry &gra, int i)
{
    gra.car[i].speed = rand() % 5 + 1;
}
void tworzenie_okien_wyswietlania(stan_gry &gra, int i)
{
    okno *car_window = new okno;
    car_window->win = newwin(2, 3, gra.car[i].poz_pion, gra.car[i].poz_poziom);
    gra.car[i].win = car_window;
}
int czy_kolicja(stan_gry &gra)
{
    for (int i = 0; i < AUTA; i++)
    {
        if (gra.frog.poz_pion == gra.car[i].poz_pion && gra.frog.poz_poziom == gra.car[i].poz_poziom || gra.frog.poz_pion == gra.car[i].poz_pion - 1 && gra.frog.poz_poziom == gra.car[i].poz_poziom)
        {
            return 1;
            break;
        }
    }
    return 0;
}
int wygrana(stan_gry gra)
{
    if (gra.frog.poz_poziom == SZER - 1)
        return 1;
    else
        return 0;
}
void podczasgry(stan_gry &gra)
{
    int czas = 0;             // Zmienna przechowująca upływający czas (np. w sekundach)
    int licznik_iteracji = 0; // Licznik iteracji pętli
    int limit_iteracji = 10;
    startowe_pozycje_aut(gra);
    for (int i = 0; i < AUTA; i++)
    {
        startowe_predkosci_aut(gra, i);
        tworzenie_okien_wyswietlania(gra, i);
    }
    timeout(100);
    int ch = ERR;
    czerwone_pole();
    zielone_pole();
    int kolizja = 0;
    while (ch != 'p' || kolizja == 1)
    {
        for (int i = 0; i < AUTA; i++)
        {
            odmierz_czas(czas, licznik_iteracji, limit_iteracji);
            usleep(6000);
            if (czas % gra.car[i].speed == 0 || czas % gra.car[i].speed == 1)
                car_go(gra, i);
            ch = getch();
            kolizja = czy_kolicja(gra);
            if (kolizja == 1)
            {
                clear();
                mvprintw(10, 10, "KONIEC GRY");
                refresh();
                break;
            }
            // timeout(10000);
            frog_jump(ch, gra);
            flushinp();
            if (wygrana(gra))
            {
            }
            kolizja = czy_kolicja(gra);
            if (kolizja == 1)
            {
                clear();
                mvprintw(10, 10, "KONIEC GRY");
                refresh();
                break;
            }
            if (ch == 'p')
                break;
        }
        if (kolizja == 1 || wygrana(gra))
            break;
    }
    if (kolizja == 1)
    {
        clear();
        mvprintw(10, 10, "KONIEC GRY");
        mvprintw(11, 10, "NACISNIJ P ABY ZAKONCZYC");
        refresh();
    }
    else
    {
        clear();
        mvprintw(10, 10, "KONIEC GRY -> WYGRALES");
        mvprintw(11, 10, "NACISNIJ P ABY ZAKONCZYC");
        refresh();
    }

    while (ch != 'p')
        ch = getch();
}

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
}

int main()
{
    ustawienia();
    stan_gry gra;
    FILE *plik;
    plik = fopen("struktura.txt", "r+");

    // Tworzymy okno o rozmiarze 2x2 dla żaby
    okno *frog_window = new okno;
    frog_window->win = newwin(1, 1, gra.frog.poz_pion, gra.frog.poz_poziom);
    gra.frog.win = frog_window;
    podczasgry(gra);
    fclose(plik);
    endwin(); // Kończy pracę z ncurses
    return 0;
}
