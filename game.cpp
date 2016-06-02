#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <curses.h>
#include "misc.h"
#include "terrain.h"
#include "editor.h"
using namespace std;

class Game : public Editor {
public:
    Game(fstream &fs) : Editor(0, 0)
    {
        Load(fs);
        mVisibilityMap = new bool[mCols * mRows];
    }
    ~Game()
    {
        delete[] mVisibilityMap;
    }
    void ExpandVision(int x, int y, int dir, int sub_dir,
                      int max, int maxc, double p)
    {
        if (p < 0)
        {
            return;
        }
        coords c = make_pair(x, y);
        if (!WithinBounds(c))
        {
            return;
        }
        else
        {
            mVisibilityMap[x + y * mCols] = true;
        }
        if (dir == 0)
        {
            ExpandVision(x - 1, y + 1, 1, 0, 1, 1, p - 1.414);
            ExpandVision(x, y + 1, 2, 0, 1, 1, p - 1);
            ExpandVision(x + 1, y + 1, 3, 0, 1, 1, p - 1.414);
            ExpandVision(x - 1, y, 4, 0, 1, 1, p - 1);
            ExpandVision(x + 1, y, 6, 0, 1, 1, p - 1);
            ExpandVision(x - 1, y - 1, 7, 0, 1, 1, p - 1.414);
            ExpandVision(x, y - 1, 8, 0, 1, 1, p - 1);
            ExpandVision(x + 1, y - 1, 9, 0, 1, 1, p - 1.414);
            return;
        }
        if (sub_dir == 0)
        {
            max++;
        }
        maxc++;
        int tr = 0;
        Object *attr = GetAttr(c);
        if (attr)
        {
            tr = attr->GetTransparency();
        }
        int tr2 = mTerrainManager->GetTerrain(GetTile(c))->GetTransparency();
        tr = (tr > tr2) ? tr : tr2;
        if (dir == 1)
        {
            ExpandVision(x - 1, y + 1, dir, sub_dir,
                         max, maxc, p - tr * 1.414);
            if (max == maxc)
            {
                if (sub_dir != 2)
                {
                    ExpandVision(x - 1, y, dir, 4, max, 0, p - tr * 1);
                }
                if (sub_dir != 4)
                {
                    ExpandVision(x, y + 1, dir, 2, max, 0, p - tr * 1);
                }
            }
        }
        if (dir == 2)
        {
            ExpandVision(x, y + 1, dir, sub_dir, max, maxc, p - tr * 1);
            if (max == maxc)
            {
                if (sub_dir != 1)
                {
                    ExpandVision(x + 1, y + 1, dir, 3, max, 0, p - tr * 1.414);
                }
                if (sub_dir != 3)
                {
                    ExpandVision(x - 1, y + 1, dir, 1, max, 0, p - tr * 1.414);
                }
            }
        }
        if (dir == 3)
        {
            ExpandVision(x + 1, y + 1, dir, sub_dir,
                         max, maxc, p - tr * 1.414);
            if (max == maxc)
            {
                if (sub_dir != 2)
                {
                    ExpandVision(x + 1, y, dir, 6, max, 0, p - tr * 1);
                }
                if (sub_dir != 6)
                {
                    ExpandVision(x, y + 1, dir, 2, max, 0, p - tr * 1);
                }
            }
        }
        if (dir == 4)
        {
            ExpandVision(x - 1, y, dir, sub_dir, max, maxc, p - tr * 1);
            if (max == maxc)
            {
                if (sub_dir != 1)
                {
                    ExpandVision(x - 1, y - 1, dir, 7, max, 0, p - tr * 1.414);
                }
                if (sub_dir != 7)
                {
                    ExpandVision(x - 1, y + 1, dir, 1, max, 0, p - tr * 1.414);
                }
            }
        }
        if (dir == 6)
        {
            ExpandVision(x + 1, y, dir, sub_dir, max, maxc, p - tr * 1);
            if (max == maxc)
            {
                if (sub_dir != 3)
                {
                    ExpandVision(x + 1, y - 1, dir, 9, max, 0, p - tr * 1.414);
                }
                if (sub_dir != 9)
                {
                    ExpandVision(x + 1, y + 1, dir, 3, max, 0, p - tr * 1.414);
                }
            }
        }
        if (dir == 7)
        {
            ExpandVision(x - 1, y - 1, dir, sub_dir,
                         max, maxc, p - tr * 1.414);
            if (max == maxc)
            {
                if (sub_dir != 4)
                {
                    ExpandVision(x, y - 1, dir, 8, max, 0, p - tr * 1);
                }
                if (sub_dir != 8)
                {
                    ExpandVision(x - 1, y, dir, 4, max, 0, p - tr * 1);
                }
            }
        }
        if (dir == 8)
        {
            ExpandVision(x, y - 1, dir, sub_dir, max, maxc, p - tr * 1);
            if (max == maxc)
            {
                if (sub_dir != 7)
                {
                    ExpandVision(x + 1, y - 1, dir, 9, max, 0, p - tr * 1.414);
                }
                if (sub_dir != 9)
                {
                    ExpandVision(x - 1, y - 1, dir, 7, max, 0, p - tr * 1.414);
                }
            }
        }
        if (dir == 9)
        {
            ExpandVision(x + 1, y - 1, dir, sub_dir,
                         max, maxc, p - tr * 1.414);
            if (max == maxc)
            {
                if (sub_dir != 6)
                {
                    ExpandVision(x, y - 1, dir, 8, max, 0, p - tr * 1);
                }
                if (sub_dir != 8)
                {
                    ExpandVision(x + 1, y, dir, 6, max, 0, p - tr * 1);
                }
            }
        }
    }

    void PrintVision(double perception, WINDOW *win)
    {
        for (int i = mY - 10; i <= mY + 10; i++)
        {
            for (int j = mX - 10; j <= mX + 10; j++)
            {
                coords p = make_pair(j, i);
                if (WithinBounds(p))
                {
                    mVisibilityMap[j + i * mCols] = false;
                }
            }
        }
        ExpandVision(mX, mY, 0, 0, 0, 0, perception);
        Print(win);
    }

    void Print(WINDOW *win)
    {
        for (int i = mY - 10; i <= mY + 10; i++)
        {
            for (int j = mX - 10; j <= mX + 10; j++)
            {
                coords p = make_pair(j, i);
                if (WithinBounds(p))
                {
                    if (i == mY && j == mX)
                    {
                        chcol(win, "white");
                        waddch(win, '@' | A_BOLD);
                    }
                    else if (mVisibilityMap[j + i * mRows])
                    {
                        Object *attr = GetAttr(p);
                        if (attr)
                        {
                            attr->Print(win);
                        }
                        else
                        {
                            mTerrainManager->GetTerrain(
                                GetTile(p))->Print(win);
                        }
                    }
                    else
                    {
                        waddch(win, ' ');
                    }
                }
                else
                {
                    waddch(win, ' ');
                }
            }
        }
        wrefresh(win);
    }
    void Examine(int dx, int dy)
    {
        dx = mX + dx;
        dy = mY + dy;
        coords p = make_pair(dx, dy);
        Object *o = GetAttr(p);
        if (o)
        {
            wprintw(mTextWindow, "You examine an object\n");
            wrefresh(mTextWindow);
        }
    }

    void Move(int dx, int dy)
    {
        dx = mX + dx;
        dy = mY + dy;
        coords p = make_pair(dx, dy);
        Object *attr = GetAttr(p);
        if (attr && attr->IsSolid())
        {
            Door *d = dynamic_cast<Door *>(attr);
            if (d && !d->mLocked)
            {
                d->mClosed = !d->mClosed;
            }
            return;
        }
        Terrain *t = mTerrainManager->GetTerrain(GetTile(p));
        if (t->IsSolid())
        {
            return;
        }

        mX = dx;
        mY = dy;
    }

    void SetTextWindow(WINDOW *win)
    {
        mTextWindow = win;
    }

private:
    bool *mVisibilityMap;
    WINDOW *mTextWindow;
};

#define keypad(action)                        \
case 'q': g->action(-1, -1); break;           \
case 'a': g->action(-1, 0); break;            \
case 'e': g->action(1, -1); break;            \
case 'w': g->action(0, -1); break;            \
case 'c': g->action(1, 1); break;             \
case 'd': g->action(1, 0); break;             \
case 'z': g->action(-1, 1); break;            \
case 'x': g->action(0, 1); break;

int main(int argc, char *argv[])
{
    if (argv[1] == NULL)
    {
        return 0;
    }

    Game *g;
    fstream fs (argv[1], fstream::in);
    if (fs.is_open())
    {
        g = new Game(fs);
    }
    fs.close();
    char key;
    bool loop = true;
    double perception = 9.5;

    init(win, txt);
    g->SetTextWindow(txt);

    while (loop)
    {
        wclear(win);
        g->PrintVision(perception, win);
        key = getch();
        switch (key) {
            keypad(Move);
        case 'E': // EXAMINE
            key = getch();
            switch (key) {
                keypad(Examine);
            default:
                break;

            }
            break;
        case '.': perception += 0.2; break;
        case ',': perception -= 0.2; break;
        case 'Q': loop = false; break;
        default:
            break;
        }
    }
    delwin(win);
    delwin(txt);
    endwin();
    delete g;
    return 0;
}
