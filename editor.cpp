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

int main(int argc, char *argv[]) {

    init(win, txt);

    Editor *e = new Editor(64, 64);
    e->PrintHelp(help);

    string current_map = "";

    char key;
    bool loop = true;
    while (loop) {
        wclear(win);
        e->Print(win);
        e->PrintInfo(txt, current_map);
        if (e->mFillMode)
        {
            e->ModifyTile();
        }
        key = getch();
        switch (key) {
        case 'q': e->MoveCursor(-1, -1); break;
        case 'a': e->MoveCursor(-1, 0); break;
        case 'e': e->MoveCursor(1, -1); break;
        case 'w': e->MoveCursor(0, -1); break;
        case 'c': e->MoveCursor(1, 1); break;
        case 'd': e->MoveCursor(1, 0); break;
        case 'z': e->MoveCursor(-1, 1); break;
        case 'x': e->MoveCursor(0, 1); break;
        case 's': e->ModifyTile(); break;
        case 'Q': loop = false; break;
        case 'm': e->mFillMode = !e->mFillMode; break;
        case '.': e->mFillMode = false; e->NextTile(); break;
        case ',': e->mFillMode = false; e->PrevTile(); break;
        case 'F': e->SaveUndo(); e->FillArea(); break;
        case 'U': e->Undo(); break;
        case 'O': e->ManipulateObject(txt, true); break;
        case 'D': e->ManipulateObject(txt, false); break;
        case 'X': e->FindObject(); break;
        case 'R': e->RenameObject(txt); break;
        case '>':
        case '<':
            if (current_map != "")
            {
                current_map = e->SwitchMap(key, current_map);
            }
            break;
        case 'N':
        {
            int x, y;
            getarg(txt, "Size X: ", "%d", x);
            getarg(txt, "Size Y: ", "%d", y);
            delete e;
            e = new Editor(x, y);
            current_map = "";
            break;
        }
        case 'I':
        {
            e->SaveUndo();
            char filename[30];
            getarg(txt, "Type import file name: ", "%s", filename);
            fstream fs (filename, fstream::in);
            if (fs.is_open()) {
                e->Import(fs);
            }
            fs.close();
            break;
        }
        case 'J':
        {
            int x, y;
            getarg(txt, "Jump X: ", "%d", x);
            getarg(txt, "Jump Y: ", "%d", y);
            e->SetCursor(x, y);
            break;
        }
        case 'S':
        {
            char filename[30];
            getarg(txt, "Type file name: ", "%s", filename);
            fstream fs (filename, fstream::out);
            if (fs.is_open()) {
                e->Save(fs);
                string s(filename);
                current_map = s;
            }
            fs.close();
            break;
        }
        case 'L':
        {
            char filename[30];
            getarg(txt, "Type file name: ", "%s", filename);
            fstream fs (filename, fstream::in);
            if (fs.is_open()) {
                e->Load(fs);
                string s(filename);
                current_map = s;
            }
            fs.close();
            break;
        }
        default:
            break;
        }
    }
    delwin(help);
    delwin(win);
    delwin(txt);
    endwin();
    delete e;
    return 0;
}
