using namespace std;

class Terrain {
  public:
    Terrain() {}
  Terrain(int t, bool i, string c, char s) :
    mTransparency(t), mSolid(i), mColor(c), mSymbol(s) {}
    int GetTransparency() { return mTransparency; }
    bool IsSolid() { return mSolid; }
    string GetColor() { return mColor; }
    char GetSymbol() { return mSymbol; }
    virtual void Print(WINDOW *win) {
        if (chcol(win, mColor))
            waddch(win, mSymbol | A_BOLD);
        else
            waddch(win, mSymbol);
    }
  protected:
    int mTransparency;
    bool mSolid;
    string mColor;
    char mSymbol;
};

class Object {
  public:
    enum Type {ITEM = 1, MONSTER, HUMANOID, DOOR, TELEPORT};
    Object() {}
    explicit Object(fstream &fs)
    {
        fs >> mColor;
        fs >> mSymbol;
        fs >> mTransparency;
    }
    virtual ~Object() {}
    virtual void Save(fstream &fs) = 0;
    virtual void PrintInfo(WINDOW *win) = 0;
    virtual bool IsSolid() { return true; }
    virtual void Print(WINDOW *win)
    {
        if (chcol(win, mColor))
        {
            waddch(win, mSymbol | A_BOLD);
        }
        else
        {
            waddch(win, mSymbol);
        }
    }
    string GetColor(fstream &fs)
    {
        string color;
        fs >> color;
        if (color == "light")
        {
            string s;
            fs >> s;
            color = color + " " + s;
        }
        return color;
    }
    virtual int GetTransparency() { return mTransparency; }

  protected:
    void ChangeColor(WINDOW *win)
    {
        char str[30];
        getarg(win, "Color: ", "%s", str);
        if (str[0] == 'l')
        {
            string color(str + 1);
            color = "light " + color;
            mColor = color;
        }
        else
        {
            string color(str);
            mColor = color;
        }
    }
    void ChangeSymbol(WINDOW *win)
    {
        wprintw(win, "Symbol?\n");
        wrefresh(win);
        mSymbol = getch();
    }
    void ChangeTransparency(WINDOW *win)
    {
        getarg(win, "Transparency: ", "%d", mTransparency);
    }
    void SaveObject(fstream &fs)
    {
        fs << mType << " ";
        fs << mColor << " ";
        fs << mSymbol << " ";
        fs << mTransparency << " ";
    }
    void PrintObject(WINDOW *win)
    {
        wprintw(win, "Type: %d\n", mType);
        wprintw(win, "Color: %s\n", mColor.c_str());
        wprintw(win, "Symbol: %c\n", mSymbol);
        wprintw(win, "Transparency: %d\n", mTransparency);
    }
    Type mType;
    string mColor;
    char mSymbol;
    int mTransparency;
};

class NamedObject : public Object {
  public:
    NamedObject() {}
    explicit NamedObject(fstream &fs) : Object(fs)
    {
        fs >> mName;
    }
    virtual ~NamedObject() {}
    virtual void Save(fstream &fs) = 0;
    virtual void PrintInfo(WINDOW *win) = 0;
    void ChangeName(WINDOW *win)
    {
        char str[30];
        getarg(win, "Name: ", "%s", str);
        string name(str);
        mName = name;
    }
    void SaveNamedObject(fstream &fs)
    {
        SaveObject(fs);
        fs << mName << " ";
    }
    void PrintNamedObject(WINDOW *win)
    {
        PrintObject(win);
        wprintw(win, "Name: %s\n", mName.c_str());
    }
    string mName;
};

class Item : public NamedObject {
  public:
    explicit Item(WINDOW *win)
    {
        mType = ITEM;
        ChangeColor(win);
        ChangeSymbol(win);
        ChangeTransparency(win);
        ChangeName(win);
        mSolid = false;
        wprintw(win, "Solid (y/N)\n");
        wrefresh(win);
        if (getch() == 'y')
        {
            mSolid = true;
        }
    }
    explicit Item(fstream &fs) : NamedObject(fs)
    {
        mType = ITEM;
        fs >> mSolid;
    }
    void Save(fstream &fs)
    {
        SaveNamedObject(fs);
        fs << mSolid << " ";
    }
    void PrintInfo(WINDOW *win)
    {
        PrintNamedObject(win);
        wprintw(win, (mSolid ? "Solid\n" : "Not solid\n"));
    }
    bool IsSolid() { return mSolid; }

  private:
    bool mSolid;
};

class Monster : public NamedObject {
  public:
    explicit Monster(WINDOW *win)
    {
        mType = MONSTER;
        ChangeColor(win);
        ChangeSymbol(win);
        ChangeTransparency(win);
        ChangeName(win);
        mHostile = true;
        wprintw(win, "Hostile (Y/n)\n");
        wrefresh(win);
        if (getch() == 'n')
        {
            mHostile = false;
        }
    }
    explicit Monster(fstream &fs) : NamedObject(fs)
    {
        mType = MONSTER;
        fs >> mHostile;
    }
    void Save(fstream &fs)
    {
        SaveNamedObject(fs);
        fs << mHostile << " ";
    }
    void PrintInfo(WINDOW *win)
    {
        PrintNamedObject(win);
        wprintw(win, (mHostile ? "Hostile\n" : "Friendly\n"));
    }
    bool mHostile;
};

class Humanoid : public NamedObject {
  public:
    explicit Humanoid(WINDOW *win)
    {
        mType = HUMANOID;
        ChangeColor(win);
        mSymbol = '@';
        mTransparency = 2;
        ChangeName(win);
        mHostile = false;
        wprintw(win, "Hostile (y/N)\n");
        wrefresh(win);
        if (getch() == 'y')
        {
            mHostile = true;
        }
    }
    explicit Humanoid(fstream &fs) : NamedObject(fs)
    {
        mType = HUMANOID;
        fs >> mHostile;
    }
    void Save(fstream &fs)
    {
        SaveNamedObject(fs);
        fs << mHostile << " ";
    }
    void PrintInfo(WINDOW *win)
    {
        PrintNamedObject(win);
        wprintw(win, (mHostile ? "Hostile\n" : "Friendly\n"));
    }
    bool mHostile;
};

class Door : public Object {
  public:
    explicit Door(WINDOW *win)
    {
        mType = DOOR;
        ChangeColor(win);
        mClosed = false;
        mLocked = false;
        mTrapped = false;
        mLock = 0;
        mLockComplexity = 0;
        mTrapLevel = 0;
        wprintw(win, "Closed (Y/n)\n");
        wrefresh(win);
        if (getch() != 'n')
        {
            mClosed = true;
            wprintw(win, "Locked (y/N)\n");
            wrefresh(win);
            if (getch() == 'y')
            {
                mLocked = true;
                getarg(win, "Lock number: ", "%d", mLock);
                getarg(win, "Lock complexity: ", "%d", mLockComplexity);
            }
            wprintw(win, "Trapped (y/N)\n");
            wrefresh(win);
            if (getch() == 'y')
            {
                mTrapped = true;
                getarg(win, "Trap level: ", "%d", mTrapLevel);
            }
        }
    }
    explicit Door(fstream &fs)
    {
        mType = DOOR;
        fs >> mColor;
        fs >> mClosed;
        fs >> mLocked;
        fs >> mTrapped;
        fs >> mLock;
        fs >> mLockComplexity;
        fs >> mTrapLevel;
    }
    void Save(fstream &fs)
    {
        fs << mType << " ";
        fs << mColor << " ";
        fs << mClosed << " ";
        fs << mLocked << " ";
        fs << mTrapped << " ";
        fs << mLock << " ";
        fs << mLockComplexity << " ";
        fs << mTrapLevel << " ";
    }
    void PrintInfo(WINDOW *win)
    {
        wprintw(win, "Color: %s\n", mColor.c_str());
        wprintw(win, (mClosed ? "Closed\n" : "Open\n"));
        wprintw(win, (mLocked ? "Locked\n" : "Not locked\n"));
        wprintw(win, (mTrapped ? "Trapped\n" : "Not trapped\n"));
        wprintw(win, "Lock number: %d\n", mLock);
        wprintw(win, "Lock complexity: %d\n", mLockComplexity);
        wprintw(win, "Trap level: %d\n", mTrapLevel);
    }
    void Print(WINDOW *win)
    {
        if (chcol(win, mColor))
        {
            waddch(win, (mClosed ? '+' : '-') | A_BOLD);
        }
        else
        {
            waddch(win, (mClosed ? '+' : '-'));
        }
    }
    bool IsSolid() { return mClosed; }
    int GetTransparency() { return (mClosed ? 20 : 2); }
    bool mClosed;
    bool mLocked;
    bool mTrapped;
    int mLock;
    int mLockComplexity;
    int mTrapLevel;
};

class Teleport : public Object {
  public:
    explicit Teleport(WINDOW *win)
    {
        mType = TELEPORT;
        ChangeColor(win);
        ChangeSymbol(win);
        mTransparency = 1;
        wprintw(win, "Trigger?\n");
        wrefresh(win);
        mTrigger = getch();
        getarg(win, "Destination map: ", "%s", mFile);
        getarg(win, "Destination X: ", "%d", mX);
        getarg(win, "Destination Y: ", "%d", mY);
    }
    explicit Teleport(fstream &fs) : Object(fs)
    {
        mType = TELEPORT;
        fs >> mTrigger;
        fs >> mFile;
        fs >> mX;
        fs >> mY;
    }
    void Save(fstream &fs)
    {
        SaveObject(fs);
        fs << mTrigger << " ";
        fs << mFile << " ";
        fs << mX << " ";
        fs << mY << " ";
    }
    void PrintInfo(WINDOW *win)
    {
        PrintObject(win);
        wprintw(win, "Trigger: %c\n", mTrigger);
        wprintw(win, "Map file: %s\n", mFile);
        wprintw(win, "Destination X: %d\n", mX);
        wprintw(win, "Destination Y: %d\n", mY);
    }
    bool IsSolid() { return false; }
    char mTrigger;
    char mFile[30];
    int mX;
    int mY;
};

#define T(name, a1, a2, a3, a4)                \
    class name : public Terrain {              \
      public:                                  \
      name() : Terrain(a1, a2, a3, a4) {}      \
    };

T(Grass, 1, false, "light green", '.');
T(Snow, 1, false, "white", '.');
T(Sand, 1, false, "yellow", '.');
T(Ice, 1, false, "light blue", '.');
T(Road, 1, false, "brown", '.');
T(RoughGrass, 1, false, "light green", ',');
T(Herb, 2, false, "green", '"');
T(Grain, 2, false, "yellow", '"');
T(Forest, 3, false, "green", 'T');
T(SnowForest, 3, false, "white", 'T');
T(PlainForest, 3, false, "brown", 'T');
T(Swamp, 2, false, "green", '~');
T(Hill, 5, false, "gray", '^');
T(Mountain, 10, true, "white", '^');
T(Bush, 3, false, "green", '*');
T(Water, 1, true, "light blue", '~');
T(DeepWater, 1, true, "blue", '~');
T(Bridge, 1, false, "brown", '=');
T(Wall, 20, true, "gray", '#');
T(BrickWall, 20, true, "red", '#');
T(MarbleWall, 20, true, "white", '#');
T(WoodenWall, 20, true, "brown", '#');
T(SandWall, 20, true, "yellow", '#');
T(VerticalFence, 2, true, "brown", '|');
T(HorizontalFence, 2, true, "brown", '-');
T(RedFloor, 1, false, "light red", '.');
T(Floor, 1, false, "gray", '.');

class TerrainManager {
  public:
    TerrainManager() {
        t.push_back(new Grass);
        t.push_back(new Snow);
        t.push_back(new Sand);
        t.push_back(new Ice);
        t.push_back(new Road);
        t.push_back(new RoughGrass);
        t.push_back(new Herb);
        t.push_back(new Grain);
        t.push_back(new Forest);
        t.push_back(new SnowForest);
        t.push_back(new PlainForest);
        t.push_back(new Swamp);
        t.push_back(new Hill);
        t.push_back(new Mountain);
        t.push_back(new Bush);
        t.push_back(new Water);
        t.push_back(new DeepWater);
        t.push_back(new Bridge);
        t.push_back(new Wall);
        t.push_back(new BrickWall);
        t.push_back(new MarbleWall);
        t.push_back(new WoodenWall);
        t.push_back(new SandWall);
        t.push_back(new VerticalFence);
        t.push_back(new HorizontalFence);
        t.push_back(new RedFloor);
        t.push_back(new Floor);
    }
    ~TerrainManager()
    {
        while (!t.empty())
        {
            delete t.back();
            t.pop_back();
        }
    }
    Terrain* GetTerrain(int id)
    {
        return t[id % t.size()];
    }
    int GetSize()
    {
        return t.size();
    }

  private:
    vector<Terrain *> t;
};
