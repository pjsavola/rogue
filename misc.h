using namespace std;

#ifndef MISC_DEFINED
#define MISC_DEFINED

#define init(win, txt)                          \
    initscr();                                  \
    WINDOW* win = newwin(21, 21, 0, 0);         \
    WINDOW* help = newwin(5, 50, 0, 25);        \
    WINDOW* txt = newwin(16, 50, 6, 25);        \
    start_color();                              \
    curs_set(0);                                \
    cbreak();                                   \
    noecho();                                   \
    clear();                                    \
    refresh();                                  \
    init_pair(1, COLOR_RED, COLOR_BLACK);       \
    init_pair(2, COLOR_GREEN, COLOR_BLACK);     \
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);    \
    init_pair(4, COLOR_BLUE, COLOR_BLACK);      \
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);   \
    init_pair(6, COLOR_CYAN, COLOR_BLACK);      \
    init_pair(7, COLOR_WHITE, COLOR_BLACK);

bool chcol(WINDOW *win, string s) {
    stringstream ss;
    ss << s;
    ss >> s;
    bool light = false;
    if (s == "light") {
        light = true;
        ss >> s;
    }
    if (s == "red") wattrset(win, COLOR_PAIR(1));
    if (s == "green") wattrset(win, COLOR_PAIR(2));
    if (s == "brown") wattrset(win, COLOR_PAIR(3));
    if (s == "yellow") { wattrset(win, COLOR_PAIR(3)); return true; }
    if (s == "blue") wattrset(win, COLOR_PAIR(4));
    if (s == "magenta") wattrset(win, COLOR_PAIR(5));
    if (s == "cyan") wattrset(win, COLOR_PAIR(6));
    if (s == "gray") wattrset(win, COLOR_PAIR(7));
    if (s == "white") { wattrset(win, COLOR_PAIR(7)); return true; }
    return light;
}

void getarg(WINDOW *win, const char *txt, const char *type, int &x)
{
    echo();
    wprintw(win, txt);
    wrefresh(win);
    if (wscanw(win, (char *) type, &x) == EOF)
    {
        // Default int value
        x = 0;
    }
    noecho();
}
void getarg(WINDOW *win, const char *txt, const char *type, char *x)
{
    echo();
    wprintw(win, txt);
    wrefresh(win);
    if (wscanw(win, (char *) type, x) == EOF)
    {
        // Default *char value
        x[0] = '\0';
    }
    noecho();
}

#endif
