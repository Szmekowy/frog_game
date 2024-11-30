#include <iostream>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

#define ESC 27
#define WYS 30
#define SZER 300
#define K_CZEROWNY 1

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
    timeout(100);
    int ch = ERR;
    while (ch != ESC)
    {
        ch = getch();
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
    clear();
    refresh();
}
void podczasgry(stan_gry gra)
{
    czerwone_pole();
}

void ustawienia()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    start_color();
    init_pair(K_CZEROWNY, COLOR_RED, COLOR_RED);
}
int main()
{

    ustawienia();
    attron(COLOR_PAIR(K_CZEROWNY));
    stan_gry gra;
    FILE *plik;
    plik = fopen("struktura.txt", "r+");
    podczasgry(gra);
    fclose(plik);
    wypisztest(gra);
    endwin(); // Kończy pracę z ncurses
    return 0;
}