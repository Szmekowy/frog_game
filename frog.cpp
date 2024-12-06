#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <curses.h>
#include <stdio.h>
#include <time.h>
#include <cstdlib>

using namespace std;

#define K_CZEROWNY 1
#define K_NIEBIESKI 2
#define K_ZOLTY 3
#define K_ZIELONY 4
#define K_BIALY 5
#define K_COIN 6
#define K_COINA 7

//------------------------------------------------
//-------------  STRUKTURY -----------------------
//------------------------------------------------
struct okno
{
    WINDOW *win;
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
    const int wyswietlanie_coinow = 1000;
    int licznik_wyswietlania_coinow = 1000;
    const int opoznienie_poruszania_bociana = 100;
    int licznik_poruszanie_bociana = 0;
};
struct przeszkoda
{
    int poz_pion;
    int poz_poz;
    okno *win;
};
struct coiny
{
    int poz_pion;
    int poz_poz;
    okno *win;
};
struct stan_gry
{
    int poziom;
    timer *czas_gry;
    struct
    {
        int wys;
        int szer;
        int auta;
        int ilosc_przeszkod;
        przeszkoda *pozyjce_przeszkod;
        int ilosc_coin;
        coiny *pozyjce_coinow;
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
        int speed;
        int dlugosc;
        int szerokosc;
        int zatrzymanie_szansa;
        int przyjacielski_szansa;
        int postoj = 0;
        int przyjaciel = 0;
        okno *win = NULL;
    } car[40];
    struct
    {
        int wynik = 0;
        int poz_pion = 21;
        int poz_poziom = 10;
        okno *win;
    } status;
    struct
    {
        int poz_pion = 0;
        int poz_poziom = 0;
        okno *win;
    } bocian;
};
//------------------------------------------------
//-------------  STRUKTURY -koniec -----------------------
//------------------------------------------------

//------------------------------------------------
//-------------  ZAKONCZENIE PODEJSCIA -----
//------------------------------------------------
void usuwanie_pamieci(stan_gry &gra)
{
    delete gra.czas_gry;
    delete gra.status.win;
    delete gra.frog.win;
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        if (gra.car[i].win != NULL)
            delete gra.car[i].win;
    }
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
        delete gra.plansza.pozyjce_coinow[i].win;
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
        delete gra.plansza.pozyjce_przeszkod[i].win;
    delete[] gra.plansza.pozyjce_coinow;
    delete[] gra.plansza.pozyjce_przeszkod;
}
//------------------------------------------------
//-------------  ZAKONCZENIE PODEJSCIA -koniec-----
//------------------------------------------------
int ekran_poziomu()
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
        napms(1);
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
//------------------------------------------------
//-   FUNKCJE ODCZYTU I ZAPISU PLIKI  -----
//------------------------------------------------
void odczyt_rozgrywki(stan_gry &gra, int i) // przypisanie grze początkowego stanu
{

    FILE *plik_z_poziomami;
    plik_z_poziomami = fopen("poziom.txt", "r");
    int wczytany_poziom;
    if (i)
    {
        wczytany_poziom = ekran_poziomu();
    }
    else
    {
        wczytany_poziom = gra.poziom;
    }
    int pom_dlug = 0;
    int pom_szer = 0;
    int pom_z_sz = 0;
    int pom_p_sz = 0;
    while (wczytany_poziom--)
    {
        fscanf_s(plik_z_poziomami, "%d", &gra.poziom);
        fscanf_s(plik_z_poziomami, "%d", &gra.plansza.wys);
        fscanf_s(plik_z_poziomami, "%d", &gra.plansza.szer);
        fscanf_s(plik_z_poziomami, "%d", &gra.plansza.auta);
        fscanf_s(plik_z_poziomami, "%d", &pom_dlug);
        fscanf_s(plik_z_poziomami, "%d", &pom_szer);
        fscanf_s(plik_z_poziomami, "%d", &gra.plansza.ilosc_coin);
        fscanf_s(plik_z_poziomami, "%d", &gra.plansza.ilosc_przeszkod);
        fscanf_s(plik_z_poziomami, "%d", &pom_z_sz);
        fscanf_s(plik_z_poziomami, "%d", &pom_p_sz);
        for (int i = 0; i < gra.plansza.auta; i++)
        {
            gra.car[i].dlugosc = pom_dlug;
            gra.car[i].szerokosc = pom_szer;
            gra.car[i].zatrzymanie_szansa = pom_z_sz;
            gra.car[i].przyjacielski_szansa = pom_p_sz;
        }
    }
    fclose(plik_z_poziomami);
}
void zapis1(stan_gry &gra, FILE *plik) // zapis timera i planszy
{

    fprintf(plik, "%d \n", gra.poziom);
    fprintf(plik, "%d \n", gra.czas_gry->czas);
    fprintf(plik, "%d \n", gra.czas_gry->iteracja);
    fprintf(plik, "%d \n", gra.czas_gry->licznik);
    fprintf(plik, "%d \n", gra.czas_gry->licznik_czasu);
    fprintf(plik, "%d \n", gra.czas_gry->aktualny_czas);
    fprintf(plik, "%d \n", gra.czas_gry->licznik_wyswietlania_coinow);
    fprintf(plik, "%d \n", gra.czas_gry->licznik_poruszanie_bociana);
}
void zapis2(stan_gry &gra, FILE *plik)
{
    fprintf(plik, "%d \n", gra.plansza.wys);
    fprintf(plik, "%d \n", gra.plansza.szer);
    fprintf(plik, "%d \n", gra.plansza.auta);
    fprintf(plik, "%d \n", gra.plansza.ilosc_przeszkod);
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
    {
        fprintf(plik, "%d \n", gra.plansza.pozyjce_przeszkod[i].poz_pion);
        fprintf(plik, "%d \n", gra.plansza.pozyjce_przeszkod[i].poz_poz);
    }
    fprintf(plik, "%d \n", gra.plansza.ilosc_coin);
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
    {
        fprintf(plik, "%d \n", gra.plansza.pozyjce_coinow[i].poz_pion);
        fprintf(plik, "%d \n", gra.plansza.pozyjce_coinow[i].poz_poz);
    }
}
void zapis3(stan_gry &gra, FILE *plik) // zapis zaby aut i bociana
{
    fprintf(plik, "%d \n", gra.frog.poz_pion);
    fprintf(plik, "%d \n", gra.frog.poz_poziom);
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        fprintf(plik, "%d \n", gra.car[i].poz_pion);
        fprintf(plik, "%d \n", gra.car[i].poz_poziom);
        fprintf(plik, "%d \n", gra.car[i].speed);
        fprintf(plik, "%d \n", gra.car[i].dlugosc);
        fprintf(plik, "%d \n", gra.car[i].szerokosc);
        fprintf(plik, "%d \n", gra.car[i].zatrzymanie_szansa);
        fprintf(plik, "%d \n", gra.car[i].przyjacielski_szansa);
        fprintf(plik, "%d \n", gra.car[i].postoj);
        fprintf(plik, "%d \n", gra.car[i].przyjaciel);
    }
    fprintf(plik, "%d \n", gra.status.wynik);
    fprintf(plik, "%d \n", gra.bocian.poz_pion);
    fprintf(plik, "%d \n", gra.bocian.poz_poziom);
}

void odczyt1(stan_gry &gra, FILE *plik) // odczyt timera
{
    fscanf_s(plik, "%d ", &gra.czas_gry->czas);
    fscanf_s(plik, "%d ", &gra.czas_gry->iteracja);
    fscanf_s(plik, "%d ", &gra.czas_gry->licznik);
    fscanf_s(plik, "%d ", &gra.czas_gry->licznik_czasu);
    fscanf_s(plik, "%d ", &gra.czas_gry->aktualny_czas);
    fscanf_s(plik, "%d ", &gra.czas_gry->licznik_wyswietlania_coinow);
    fscanf_s(plik, "%d ", &gra.czas_gry->licznik_poruszanie_bociana);
}
void odczyt2(stan_gry &gra, FILE *plik) // odczyt planszy
{
    fscanf_s(plik, "%d ", &gra.plansza.wys);
    fscanf_s(plik, "%d ", &gra.plansza.szer);
    fscanf_s(plik, "%d ", &gra.plansza.auta);
    fscanf_s(plik, "%d ", &gra.plansza.ilosc_przeszkod);
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
    {
        fscanf_s(plik, "%d ", &gra.plansza.pozyjce_przeszkod[i].poz_pion);
        fscanf_s(plik, "%d ", &gra.plansza.pozyjce_przeszkod[i].poz_poz);
    }
    fscanf_s(plik, "%d ", &gra.plansza.ilosc_coin);
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
    {
        fscanf_s(plik, "%d ", &gra.plansza.pozyjce_coinow[i].poz_pion);
        fscanf_s(plik, "%d ", &gra.plansza.pozyjce_coinow[i].poz_poz);
    }
}
void odczyt3(stan_gry &gra, FILE *plik) // odczyt zaby aut i bociana
{
    fscanf_s(plik, "%d ", &gra.frog.poz_pion);
    fscanf_s(plik, "%d ", &gra.frog.poz_poziom);
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        fscanf_s(plik, "%d ", &gra.car[i].poz_pion);
        fscanf_s(plik, "%d ", &gra.car[i].poz_poziom);
        fscanf_s(plik, "%d ", &gra.car[i].speed);
        fscanf_s(plik, "%d ", &gra.car[i].dlugosc);
        fscanf_s(plik, "%d ", &gra.car[i].szerokosc);
        fscanf_s(plik, "%d ", &gra.car[i].zatrzymanie_szansa);
        fscanf_s(plik, "%d ", &gra.car[i].przyjacielski_szansa);
        fscanf_s(plik, "%d ", &gra.car[i].postoj);
        fscanf_s(plik, "%d ", &gra.car[i].przyjaciel);
    }
    fscanf_s(plik, "%d ", &gra.status.wynik);
    fscanf_s(plik, "%d ", &gra.bocian.poz_pion);
    fscanf_s(plik, "%d ", &gra.bocian.poz_poziom);
}
//------------------------------------------------
//-   FUNKCJE ODCZYTU I ZAPISU PLIKI -koniec -----
//------------------------------------------------

//------------------------------------------------
//-------------   FUNKCJE ZWIAZANE Z COINAMI -----
//------------------------------------------------
void nowe_coiny(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
    {
        int linia = rand() % (gra.plansza.szer + 1) - 2;
        int wys = rand() % (gra.plansza.wys - 1) + 1;
        while ((linia - 3) % 3 == 0) ////asdhasfi
            linia = rand() % (gra.plansza.szer + 1) - 2;
        gra.plansza.pozyjce_coinow[i].poz_pion = wys;
        gra.plansza.pozyjce_coinow[i].poz_poz = linia;
    }
}
void ekrany_dla_coiny(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
    {
        okno *coin = new okno;
        gra.plansza.pozyjce_coinow[i].win = coin;
        gra.plansza.pozyjce_coinow[i].win->win = newwin(1, 1, 1, 1);
    }
}
void odswierz_coiny(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
    {
        if (gra.plansza.pozyjce_coinow[i].poz_pion != -1)
        {
            mvwin(gra.plansza.pozyjce_coinow[i].win->win, gra.plansza.pozyjce_coinow[i].poz_pion, gra.plansza.pozyjce_coinow[i].poz_poz);
            if (gra.frog.poz_pion == gra.plansza.pozyjce_coinow[i].poz_pion && gra.frog.poz_poziom == gra.plansza.pozyjce_coinow[i].poz_poz)
                wattron(gra.plansza.pozyjce_coinow[i].win->win, COLOR_PAIR(K_COINA));
            else
                wattron(gra.plansza.pozyjce_coinow[i].win->win, COLOR_PAIR(K_COIN));
            mvwprintw(gra.plansza.pozyjce_coinow[i].win->win, 0, 0, "o");
            wrefresh(gra.plansza.pozyjce_coinow[i].win->win);
        }
    }
}
void usun_poprzednie_coiny(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
    {
        if (gra.plansza.pozyjce_coinow[i].poz_pion != -1)
        {
            mvwin(gra.plansza.pozyjce_coinow[i].win->win, gra.plansza.pozyjce_coinow[i].poz_pion, gra.plansza.pozyjce_coinow[i].poz_poz);
            wattroff(gra.plansza.pozyjce_coinow[i].win->win, COLOR_PAIR(K_COIN));
            mvwprintw(gra.plansza.pozyjce_coinow[i].win->win, 0, 0, " ");
            wrefresh(gra.plansza.pozyjce_coinow[i].win->win);
        }
    }
}
void coiny_gra(stan_gry &gra)
{
    if (gra.czas_gry->licznik_wyswietlania_coinow == gra.czas_gry->wyswietlanie_coinow)
    {
        usun_poprzednie_coiny(gra);
        nowe_coiny(gra);
    }
    odswierz_coiny(gra);
}
//------------------------------------------------
//------  FUNKCJE ZWIAZANE Z COINAMI -koniec -----
//------------------------------------------------

//------------------------------------------------
//----------------  POCZATKOWE PARAMETRY GRY -----
//------------------------------------------------
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
void przyznanie_pamieci_dla_tablic(stan_gry &gra)
{
    gra.plansza.pozyjce_przeszkod = new przeszkoda[gra.plansza.ilosc_przeszkod];
    gra.plansza.pozyjce_coinow = new coiny[gra.plansza.ilosc_coin];
}
//------------------------------------------------
//--------  POCZATKOWE PARAMETRY GRY -koniec -----
//------------------------------------------------

//------------------------------------------------
//----------------  TIMER ----------------
//------------------------------------------------
void odmierz_czas(stan_gry &gra)
{
    gra.czas_gry->iteracja++;
    gra.czas_gry->licznik_czasu++;
    gra.czas_gry->licznik_wyswietlania_coinow++;
    gra.czas_gry->licznik_poruszanie_bociana++;
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
    if (gra.czas_gry->licznik_wyswietlania_coinow > gra.czas_gry->wyswietlanie_coinow)
    {
        gra.czas_gry->licznik_wyswietlania_coinow = 0;
    }
    if (gra.czas_gry->licznik_poruszanie_bociana > gra.czas_gry->opoznienie_poruszania_bociana)
    {
        gra.czas_gry->licznik_poruszanie_bociana = 0;
    }
    gra.czas_gry->licznik++;
}
//------------------------------------------------
//----------------  TIMER-koniec ----------------
//------------------------------------------------
//
//------------------------------------------------
//----------------  BOCIAN-----------------------
//------------------------------------------------
void przegrana(stan_gry &gra);
void ekran_dla_bocian(stan_gry &gra)
{
    okno *ekran_boc = new okno;
    ekran_boc->win = newwin(1, 2, 2, 2);
    gra.bocian.win = ekran_boc;
}
void ruch_bociana(stan_gry &gra)
{
    wattron(gra.bocian.win->win, COLOR_PAIR(K_BIALY));
    mvwin(gra.bocian.win->win, gra.bocian.poz_pion, gra.bocian.poz_poziom);
    mvwprintw(gra.bocian.win->win, 0, 0, "#");
    mvwprintw(gra.bocian.win->win, 0, 1, "#");
    wrefresh(gra.bocian.win->win);
    if (gra.czas_gry->licznik_poruszanie_bociana == gra.czas_gry->opoznienie_poruszania_bociana)
    {

        wclear(gra.bocian.win->win);
        wrefresh(gra.bocian.win->win);
        int poz1, poz2;
        poz1 = gra.frog.poz_pion - gra.bocian.poz_pion;
        poz2 = gra.frog.poz_poziom - gra.bocian.poz_poziom - 1;
        if (abs(poz1) > abs(poz2))
        {
            if (poz1 > 0)
            {
                gra.bocian.poz_pion++;
            }
            else
            {
                gra.bocian.poz_pion--;
            }
        }
        else
        {
            if (poz2 > 0)
            {
                gra.bocian.poz_poziom++;
            }
            else
            {
                gra.bocian.poz_poziom--;
            }
        }
        wattron(gra.bocian.win->win, COLOR_PAIR(K_BIALY));
        mvwin(gra.bocian.win->win, gra.bocian.poz_pion, gra.bocian.poz_poziom);
        mvwprintw(gra.bocian.win->win, 0, 0, "#");
        mvwprintw(gra.bocian.win->win, 0, 1, "#");
        wrefresh(gra.bocian.win->win);
    }
    else
        return;
}
int czy_bocian(stan_gry &gra)
{
    int poz1, poz2;
    poz1 = gra.frog.poz_pion - gra.bocian.poz_pion;
    poz2 = gra.frog.poz_poziom - gra.bocian.poz_poziom - 1;
    if (poz1 == 0 && poz2 == 0)
        return 1;
    if (poz1 == 0 && poz2 == -1)
        return 1;
    return 0;
}

//------------------------------------------------
//----------------  BOCIAN-koniec ----------------
//------------------------------------------------

//------------------------------------------------
//----------------  SAMOCHODY ----------------
//------------------------------------------------

/// ROZNE ZACHOWANIA SAMOCHODU ///
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
void czy_koniec_postoju(stan_gry &gra, int i)
{
    if (gra.frog.poz_poziom - gra.car[i].szerokosc > gra.car[i].poz_poziom || gra.frog.poz_poziom + gra.car[i].szerokosc - 2 < gra.car[i].poz_poziom) // czy auto może ruszyć ponownie
        gra.car[i].postoj = 0;
}
void powrot_na_gore(stan_gry &gra, int i)
{
    if (gra.car[i].poz_pion >= gra.plansza.wys - 1) // powrot na góre planszy
    {
        startowe_predkosci_aut(gra, i);
        gra.car[i].poz_pion = 1;
        gra.car[i].przyjaciel = 0;
        if (przyjacielski(gra, i))
            gra.car[i].przyjaciel = 1;
    }
}
/// ROZNE ZACHOWANIA SAMOCHODU -koniec ///

/// FUNKCJA MALOWANIA AUT ///
void car_go(stan_gry &gra, int i)
{
    nodelay(stdscr, TRUE);
    wclear(gra.car[i].win->win);
    wrefresh(gra.car[i].win->win);

    if (gra.car[i].poz_pion + gra.car[i].dlugosc <= gra.plansza.wys) // zmiana pozycji okna
        mvwin(gra.car[i].win->win, gra.car[i].poz_pion, gra.car[i].poz_poziom);
    if (gra.car[i].przyjaciel) // auta przyjazne
        wattron(gra.car[i].win->win, COLOR_PAIR(K_ZIELONY));
    else
    {
        wattron(gra.car[i].win->win, COLOR_PAIR(K_NIEBIESKI));
    }
    if (czy_zatrzymanie(gra, i))
        gra.car[i].postoj = 1;
    czy_koniec_postoju(gra, i);
    if (gra.car[i].poz_pion + gra.car[i].dlugosc <= gra.plansza.wys)
        for (int a = 0; a < gra.car[i].dlugosc; a++)
        {
            for (int j = 0; j < gra.car[i].szerokosc; j++)
            {
                mvwprintw(gra.car[i].win->win, a, j, "#");
            }
        }
    wrefresh(gra.car[i].win->win);

    if (gra.car[i].postoj == 0)
    {
        gra.car[i].poz_pion += 1;
    }
    powrot_na_gore(gra, i);
    napms(7);
}
/// FUNKCJA MALOWANIA AUT -koniec ///

//------------------------------------------------
//----------------  SAMOCHODY-koniec ----------------
//------------------------------------------------

//------------------------------------------------
//----------------  ZABA ----------------
//------------------------------------------------

/// SPRAWDZANIE CZY ZABA ODZIALUJE Z ELEMENTAMI GRY ///
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
    for (int i = 0; i < gra.plansza.auta; i++)
    {

        if ((gra.frog.poz_poziom + 1 == gra.car[i].poz_poziom) && (gra.frog.poz_pion >= gra.car[i].poz_pion) && (gra.car[i].przyjaciel == 1) && (gra.frog.poz_pion <= gra.car[i].poz_pion + gra.car[i].dlugosc - 1))
            return true;
    }
    return false;
}
int czy_coin(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_coin; i++)
    {
        if ((gra.frog.poz_poziom == gra.plansza.pozyjce_coinow[i].poz_poz) && (gra.frog.poz_pion == gra.plansza.pozyjce_coinow[i].poz_pion))
        {
            gra.plansza.pozyjce_coinow[i].poz_poz = -1;
            gra.plansza.pozyjce_coinow[i].poz_pion = -1;
            return true;
        }
    }
    return false;
}
int czy_kolicja(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        int zakres[4] = {0};
        zakres[0] = gra.car[i].poz_pion - 1;
        zakres[1] = gra.car[i].poz_pion + gra.car[i].dlugosc - 2;
        if (czy_zatrzymanie(gra, i))
        {
            zakres[1]++;
            zakres[0]++;
        }
        zakres[2] = gra.car[i].poz_poziom;
        zakres[3] = gra.car[i].poz_poziom + gra.car[i].szerokosc - 1;
        if (gra.frog.poz_pion >= zakres[0] && gra.frog.poz_pion <= zakres[1] && gra.frog.poz_poziom >= zakres[2] && gra.frog.poz_poziom <= zakres[3])
        {
            return 1;
        }
    }
    return 0;
}
/// SPRAWDZANIE CZY ZABA ODZIALUJE Z ELEMENTAMI GRY -koniec///

/// FUNCKJA DO PORUSZANIA I MALOWANIA ZABY ///
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
        {
            gra.frog.poz_poziom += 10;
            gra.status.wynik += 2;
        }
    }
    else if (ch == 'q')
    {
        if (czy_coin(gra))
            gra.status.wynik++;
    }
    mvwin(gra.frog.win->win, gra.frog.poz_pion, gra.frog.poz_poziom);
    wattron(gra.frog.win->win, COLOR_PAIR(K_ZIELONY));
    mvwprintw(gra.frog.win->win, 0, 0, " ");
    wrefresh(gra.frog.win->win);
}
/// FUNCKJA DO PORUSZANIA I MALOWANIA ZABY -koniec ///

//------------------------------------------------
//----------------  ZABA -koniec ----------------
//------------------------------------------------

//------------------------------------------------
//--  WYGRANA LUB PRZEGRANA ----------------------
//------------------------------------------------
int czy_wygrana(stan_gry &gra)
{
    if (gra.frog.poz_poziom >= gra.plansza.szer - 1)
    {
        return 1;
    }
    else
        return 0;
}
void wygrana(stan_gry &gra)
{
    clear();
    mvprintw(10, 10, "KONIEC GRY -> wygrales WYNIK =: %d", gra.status.wynik);
    mvprintw(11, 10, "NACISNIJ P ABY ZAKONCZYC");
    refresh();
    usuwanie_pamieci(gra);
}
void przegrana(stan_gry &gra)
{
    clear();
    mvprintw(10, 10, "KONIEC GRY");
    mvprintw(11, 10, "NACISNIJ P ABY ZAKONCZYC");
    refresh();
    usuwanie_pamieci(gra);
}
//------------------------------------------------
//--  WYGRANA LUB PRZEGRANA -koniec --------------
//------------------------------------------------

//------------------------------------------------
//--  RYSOWANIE PLANSZY I ELEMENTOW PLANSZY ------
//------------------------------------------------

void ulica(stan_gry &gra) /// malowanie ulicy
{
    for (int i = 3; i < gra.plansza.szer; i += gra.car->szerokosc + 3)
    {
        for (int j = 1; j < gra.plansza.wys; j++)
            mvprintw(j, i, "|");
    }
}
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
    attroff(COLOR_PAIR(K_CZEROWNY));
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

void status_gry(stan_gry &gra) /// wyswietlenie statusu gry ///
{
    wattron(gra.status.win->win, COLOR_PAIR(K_BIALY));
    mvwprintw(gra.status.win->win, 0, 0, "Aktualny czas gry: %d ", gra.czas_gry->aktualny_czas);
    mvwprintw(gra.status.win->win, 0, 25, "Aktualny wynik: %d ", gra.status.wynik);
    mvwprintw(gra.status.win->win, 1, 10, "Szymon Drywa 203668 Politechnika Gdanska ");
    wrefresh(gra.status.win->win);
}

/// LOSWOANIE POZYCJI ///
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
/// LOSOWANIE POZYCJI -koniec ///

void ekrany_dla_przeszkod(stan_gry &gra, int i)
{
    okno *przeszkoda = new okno;
    gra.plansza.pozyjce_przeszkod[i].win = przeszkoda;
}
void ekran_dla_statusu(stan_gry &gra)
{
    okno *status_window = new okno;
    status_window->win = newwin(2, 100, gra.plansza.wys + 2, gra.status.poz_poziom);
    gra.status.win = status_window;
}
void przeszkody(int linia, int wys, stan_gry &gra, int i)
{
    linia = gra.car[linia].poz_poziom + gra.car[linia].szerokosc + 1;
    gra.plansza.pozyjce_przeszkod[i].win->win = newwin(1, 1, wys, linia);
    gra.plansza.pozyjce_przeszkod[i].poz_pion = wys;
    gra.plansza.pozyjce_przeszkod[i].poz_poz = linia;
    wattron(gra.plansza.pozyjce_przeszkod[i].win->win, COLOR_PAIR(K_CZEROWNY));
    mvwprintw(gra.plansza.pozyjce_przeszkod[i].win->win, 0, 0, "#");
    wrefresh(gra.plansza.pozyjce_przeszkod[i].win->win);
}
void odswierz_przeszkody(stan_gry &gra)
{
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
    {
        werase(gra.plansza.pozyjce_przeszkod[i].win->win);
        wattron(gra.plansza.pozyjce_przeszkod[i].win->win, COLOR_PAIR(K_CZEROWNY));
        mvwprintw(gra.plansza.pozyjce_przeszkod[i].win->win, 0, 0, "#");
        wrefresh(gra.plansza.pozyjce_przeszkod[i].win->win);
    }
}

//------------------------------------------------
//--  RYSOWANIE PLANSZY I ELEMENTOW PLANSZY -koniec ------
//------------------------------------------------

//------------------------------------------------
//--  INICJALIZAJA POCZATKOWEGO STANU GRY ------
//------------------------------------------------

void start_gry(stan_gry &gra, int tryb)
{
    ekran_dla_bocian(gra);
    frog_jump(ERR, gra); // pokazanie zaby
    przyznanie_pamieci_dla_tablic(gra);
    ekran_dla_statusu(gra);
    if (gra.poziom != 3)
        ulica(gra);
    czerwone_pole(gra);
    if (tryb == 1)
        startowe_pozycje_aut(gra); //
    ekrany_dla_coiny(gra);
    for (int i = 0; i < gra.plansza.ilosc_przeszkod; i++)
    {
        ekrany_dla_przeszkod(gra, i);
        przeszkody(losuj_l(gra), losuj_w(gra), gra, i); //
    }
    zielone_pole(gra);
    for (int i = 0; i < gra.plansza.auta; i++)
    {
        if (tryb == 1)
            startowe_predkosci_aut(gra, i); //
        tworzenie_okien_wyswietlania(gra, i);
    }
    gra.czas_gry = new timer();
}
//------------------------------------------------
//-- INICJALIZAJA POCZATKOWEGO STANU GRY -koniec -
//------------------------------------------------

//------------------------------------------------
//--------- GŁÓWNA PETLA GRY  --------------------
//------------------------------------------------
void podczasgry(stan_gry &gra, FILE *plik)
{
    int ch = ERR;
    int kolizja = 0;
    int pom_akt = 0;
    frog_jump(ch, gra);
    while (ch != 'p' || kolizja == 1 || czy_wygrana(gra))
    {
        for (int i = 0; i < gra.plansza.auta; i++)
        {

            coiny_gra(gra);
            if (gra.poziom == 3)
            {
                odswierz_przeszkody(gra);
                ruch_bociana(gra);
            }
            odmierz_czas(gra);

            if (gra.czas_gry->aktualny_czas >= 60)
            {
                kolizja = 1;
                break;
            }
            status_gry(gra);
            if (gra.czas_gry->czas % gra.car[i].speed == 0 || gra.czas_gry->czas % gra.car[i].speed == 1)
                car_go(gra, i);
            ch = getch();
            if (ch == 'u')
            {
                zapis1(gra, plik);
                zapis2(gra, plik);
                zapis3(gra, plik);
            }
            kolizja = czy_kolicja(gra);
            if (kolizja == 1 || czy_wygrana(gra))
                break;
            if (gra.czas_gry->licznik >= gra.czas_gry->limit && ch != ERR)
                frog_jump(ch, gra);
            flushinp();
            kolizja = czy_kolicja(gra);
            if (gra.frog.poz_pion == 0 || gra.frog.poz_pion >= gra.plansza.wys)
                kolizja = 1;
            if (gra.poziom == 3 && czy_bocian(gra))
                kolizja = 1;
            if (kolizja == 1 || czy_wygrana(gra))
                break;
            if (ch == 'p')
                break;
        }
        if (kolizja == 1 || czy_wygrana(gra))
            break;
    }
    fclose(plik);
    if (kolizja == 1)
        przegrana(gra);
    else
        wygrana(gra);
    while (ch != 'p')
        ch = getch();
}
//------------------------------------------------
//-- GŁÓWNA PETLA GRY -koniec --------------------
//------------------------------------------------

//------------------------------------------------
//-- USTAWIENIA NCURSES --------------------------
//------------------------------------------------
void ustawienia()
{
    nodelay(stdscr, TRUE);
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
    init_pair(K_COIN, COLOR_YELLOW, COLOR_BLACK);
    init_pair(K_COINA, COLOR_YELLOW, COLOR_GREEN);
}
//------------------------------------------------
//-- USTAWIENIA NCURSES -koniec ------------------
//------------------------------------------------
void nowa_gra(stan_gry &gra);
void jak_grac(stan_gry &gra)
{
    clear();
    refresh();
    mvprintw(13, 15, "Poruszasz się  WSAD ");
    mvprintw(15, 15, "Aby zakonczyc gre wcisnij P ");
    mvprintw(17, 15, "Kliknij K aby zapisac gre ");
    mvprintw(19, 15, "Wcisnij R aby wrocic: ");
    refresh();
    napms(1);
    char ch = ERR;
    while (ch != 'r')
    {
        cin >> ch;
        mvprintw(20, 15, "%d ", ch);
    }
    nowa_gra(gra);
}
void nowa_gra(stan_gry &gra)
{
    FILE *plik;
    plik = fopen("zapisgry.txt", "r+");
    if (plik == NULL)
    {
        clear();
        mvprintw(0, 0, "Nie znaleziono pliku");
        refresh();
    }
    fscanf_s(plik, "%d ", &gra.poziom);
    clear();
    refresh();
    keypad(stdscr, TRUE);
    int pozycja_kropki = 13;
    int ch = ERR;
    while (ch != 10)
    {
        attron(COLOR_PAIR(K_BIALY));
        mvprintw(pozycja_kropki, 8, " ");
        refresh();
        attroff(COLOR_PAIR(K_BIALY));
        mvprintw(13, 15, "NOWA GRA: ");
        mvprintw(15, 15, "WCZYTAJ GRE: ");
        mvprintw(17, 15, "JAK GRAC: ");
        mvprintw(19, 15, "HIGHSCORE: ");
        refresh();
        napms(1);
        ch = getch();
        if (ch == 's')
        {
            if (pozycja_kropki == 19)
                pozycja_kropki = 13;
            else
                pozycja_kropki += 2;
        }
        else if (ch == 'w')
        {
            if (pozycja_kropki == 13)
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
    flushinp();
    if (pozycja_kropki == 19)
        ;
    if (pozycja_kropki == 17)
        jak_grac(gra);
    if (pozycja_kropki == 15)
    {
        odczyt_rozgrywki(gra, 0);
        start_gry(gra, 0);
        odczyt1(gra, plik);
        odczyt2(gra, plik);
        odczyt3(gra, plik);
    }
    if (pozycja_kropki == 13)
    {
        odczyt_rozgrywki(gra, 1);
        start_gry(gra, 1);
    }

    fclose(plik);
}

//------------------------------------------------
//-------------------MAIN  -----------------------
//------------------------------------------------
int main()
{
    FILE *plik;
    plik = fopen("zapisgry.txt", "r+");
    if (plik == NULL)
    {
        clear();
        mvprintw(0, 0, "Nie znaleziono pliku");
        refresh();
    }
    ustawienia();
    stan_gry gra;
    okno *frog_window = new okno;
    frog_window->win = newwin(1, 1, gra.frog.poz_pion, gra.frog.poz_poziom);
    gra.frog.win = frog_window;
    nowa_gra(gra);
    podczasgry(gra, plik);
    endwin();
    return 0;
}
//------------------------------------------------
//-------------------MAIN  -koniec ---------------
//------------------------------------------------

/// ekran odczyt -> 1 nowa gra 2 wcztaj gre 3 jak grac 4 higscore 5 wpisz swoje imie
/// ekran zamknicia -> 1 zakoncz 2 nowa gra 3 wcztaj gre
/// odczyt z pliku higscore, odzcyt z pliku stan gry, przycisk wroc,
/// po zakonczeniu nowa gra na nowo uruchamia funckje odpowiedzialne za gre i zeruje niektóre wartośći
/// timer=0; status=0;
/// bocian -> 3 poziom usunąć ulice i usunąć