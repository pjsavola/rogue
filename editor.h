typedef pair<int,int> coords;
typedef map<coords, Object *> omap;

class Editor {
public:
    Editor(int x, int y);
    ~Editor();
    void InitMaps();
    void DeleteMaps();
    void ModifyTile();
    void FillArea();
    void Print(WINDOW *win);
    void PrintHelp(WINDOW *win);
    void PrintInfo(WINDOW *win, string current_map);
    void Save(fstream &fs);
    void Load(fstream &fs);
    void Import(fstream &fs);
    void MoveCursor(int dx, int dy);
    void SetCursor(int x, int y);
    void NextTile();
    void PrevTile();
    void SaveUndo();
    void Undo();
    void ManipulateObject(WINDOW *win, bool create);
    void RenameObject(WINDOW *win);
    void FindObject();
    string SwitchMap(char trigger, string old_map);
    bool mFillMode;

protected:
    int GetTile(coords p);
    Object *GetAttr(coords p);
    void SetTile(coords p);
    void SetAttr(coords p, Object *attr);
    bool WithinBounds(coords p);
    void LoadObjects(fstream &fs, int offset_x, int offset_y);

    omap::iterator mAttrBrowser;
    omap mObjectMap;
    int *mTileMap;
    Object **mAttrMap;
    TerrainManager *mTerrainManager;
    int mRows;
    int mCols;
    int mX;
    int mY;
    int mBrushTile;
};

Editor::Editor(int x, int y)
{
    mCols = x;
    mRows = y;
    mX = 0;
    mY = 0;
    mBrushTile = 0;
    mFillMode = false;
    mAttrBrowser = mObjectMap.begin();
    InitMaps();
    mTerrainManager = new TerrainManager();
    for (int i = 0; i < mCols; i++)
    {
        for (int j = 0; j < mRows; j++)
        {
            coords p = make_pair(i, j);
            SetTile(p);
            SetAttr(p, 0);
        }
    }
}

Editor::~Editor()
{
    delete mTerrainManager;
    DeleteMaps();
}

void Editor::InitMaps()
{
    mTileMap = new int[mCols * mRows];
    mAttrMap = new Object*[mCols * mRows];
}

void Editor::DeleteMaps()
{
    delete[] mTileMap;
    delete[] mAttrMap;
    omap::iterator it;
    for (it = mObjectMap.begin(); it != mObjectMap.end(); ++it)
    {
        delete it->second;
    }
}

void Editor::ModifyTile()
    {
        coords p = make_pair(mX, mY);
        if (WithinBounds(p))
        {
            SetTile(p);
        }
    }

void Editor::FillArea()
{
    int fill_type;
    deque<coords> worklist;
    coords p = make_pair(mX, mY);
    if (WithinBounds(p))
    {
        fill_type = GetTile(p);
        if (fill_type != mBrushTile)
        {
            worklist.push_back(p);
        }
    }
    while (!worklist.empty())
    {
        p = worklist.front();
        worklist.pop_front();
        if (WithinBounds(p))
        {
            // To guarantee linear running time
            if (GetTile(p) != fill_type)
            {
                continue;
            }
            SetTile(p);
            worklist.push_back(make_pair(p.first - 1, p.second));
            worklist.push_back(make_pair(p.first + 1, p.second));
            worklist.push_back(make_pair(p.first, p.second - 1));
            worklist.push_back(make_pair(p.first, p.second + 1));
        }
    }
}

void Editor::Print(WINDOW *win)
{
    for (int i = mY - 10; i <= mY + 10; i++)
    {
        for (int j = mX - 10; j <= mX + 10; j++)
        {
            coords p = make_pair(j, i);
            if (WithinBounds(p))
            {
                Object *attr = GetAttr(p);
                if (mX == j && mY == i)
                {
                    chcol(win, "white");
                    waddch(win, 'o' | A_BOLD);
                }
                else if (attr)
                {
                    attr->Print(win);
                }
                else
                {
                    mTerrainManager->GetTerrain(GetTile(p))->Print(win);
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

void Editor::PrintHelp(WINDOW *win)
{
    wclear(win);
    wprintw(win, "               --- HELP ---\n");
    wprintw(win, "(m)ode, previous(,), next(.), (F)ill, (Q)uit\n");
    wprintw(win, "(J)ump, (I)mport, (U)ndo, (S)ave, (L)oad\n");
    wprintw(win, "(New) Examine(X), (O)bject, (D)elete\n");
    wrefresh(win);
}

void Editor::PrintInfo(WINDOW *win, string current_map)
{
    wclear(win);
    waddstr(win, "Current brush tile: ");
    mTerrainManager->GetTerrain(mBrushTile)->Print(win);
    chcol(win, "gray");
    waddstr(win, "\nTile under the cursor: ");
    mTerrainManager->GetTerrain(GetTile(make_pair(mX, mY)))->Print(win);
    chcol(win, "gray");
    wprintw(win, "\nFill mode: %s", (mFillMode ? "on" : "off"));
    wprintw(win, "\nCoordinates: %d, %d (%s)\n", mX, mY, current_map.c_str());
    Object *o = GetAttr(make_pair(mX, mY));
    if (o)
    {
        o->PrintInfo(win);
    }
    wrefresh(win);
}

void Editor::Save(fstream &fs)
{
    fs << mCols << " ";
    fs << mRows << " ";
    for (int i = 0; i < mCols; i++)
    {
        for (int j = 0; j < mRows; j++)
        {
            fs << GetTile(make_pair(i, j)) << " ";
        }
    }
    fs << mX << " ";
    fs << mY << " ";
    fs << mObjectMap.size() << " ";
    omap::iterator it;
    for (it = mObjectMap.begin(); it != mObjectMap.end(); ++it)
    {
        fs << it->first.first << " "; // x-coordinate
        fs << it->first.second << " "; // y-coordinate
        it->second->Save(fs);
    }
}

void Editor::Load(fstream &fs)
{
    DeleteMaps();
    mObjectMap.clear();
    fs >> mCols;
    fs >> mRows;
    InitMaps();
    for (int i = 0; i < mCols; i++)
    {
        for (int j = 0; j < mRows; j++)
        {
            fs >> mTileMap[i + j * mCols];
            SetAttr(make_pair(i, j), 0);
        }
    }
    fs >> mX;
    fs >> mY;
    LoadObjects(fs, 0, 0);
}

void Editor::Import(fstream &fs)
{
    int x_imp;
    int y_imp;
    fs >> x_imp;
    fs >> y_imp;
    if (mRows < mX + x_imp || mCols < mY + y_imp) {
        return;
    }
    for (int i = 0; i < x_imp; i++)
    {
        for (int j = 0; j < y_imp; j++)
        {
            coords p = make_pair(i + mX, j + mY);
            fs >> mTileMap[i + mX + (j + mY) * mCols];
            omap::iterator it;
            it = mObjectMap.find(p);
            if (it != mObjectMap.end())
            {
                SetAttr(p, 0);
                delete it->second;
                mObjectMap.erase(it);
            }
        }
    }
    int x, y;
    fs >> x; // Ignored cursor position
    fs >> y; // Ignored cursor position
    LoadObjects(fs, mX, mY);
}

void Editor::MoveCursor(int dx, int dy)
{
    int x = mX + dx;
    int y = mY + dy;
    coords p = make_pair(x, y);
    if (WithinBounds(p))
    {
        mX = x;
        mY = y;
    }
}

void Editor::SetCursor(int x, int y)
{
    coords p = make_pair(x, y);
    if (WithinBounds(p))
    {
        mX = x;
        mY = y;
    }
}

void Editor::NextTile()
{
    mBrushTile = (mBrushTile + 1) % mTerrainManager->GetSize();
}

void Editor::PrevTile()
{
    int size = mTerrainManager->GetSize();
    mBrushTile = (mBrushTile + size - 1) % size;
}

void Editor::SaveUndo()
{
    fstream fs ("Undo.map", fstream::out);
    if (fs.is_open()) {
        Save(fs);
    }
    fs.close();
}

void Editor::Undo()
{
    fstream fs ("Undo.map", fstream::in);
    if (fs.is_open()) {
        Load(fs);
    }
    fs.close();
}

void Editor::ManipulateObject(WINDOW *win, bool create)
{
    coords p = make_pair(mX, mY);
    if (WithinBounds(p))
    {
        omap::iterator it = mObjectMap.find(p);
        if (it != mObjectMap.end())
        {
            delete it->second;
        }
        if (create) {
            int type;
            Object *attr;
            wprintw(win, "ITEM = 1, MONSTER = 2, HUMANOID = 3\n");
            wprintw(win, "DOOR = 4, TELEPORT = 5\n");
            getarg(win, "Choose type: ", "%d", type);
            switch (type) {
            case Object::ITEM: attr = new Item(win); break;
            case Object::MONSTER: attr = new Monster(win); break;
            case Object::HUMANOID: attr = new Humanoid(win); break;
            case Object::DOOR: attr = new Door(win); break;
            case Object::TELEPORT: attr = new Teleport(win); break;
            default:
                SetAttr(it->first, 0);
                mObjectMap.erase(p);
                return;
            }
            mObjectMap[p] = attr;
            SetAttr(p, attr);
        }
        else
        {
            SetAttr(it->first, 0);
            mObjectMap.erase(p);
        }
    }
}

void Editor::RenameObject(WINDOW *win)
{
    coords p = make_pair(mX, mY);
    if (!WithinBounds(p))
    {
        return;
    }
    Object *attr = GetAttr(p);
    if (!attr)
    {
        return;
    }
    while (true)
    {
        char id[30];
        NamedObject *attr;
        bool name_collision = false;
        getarg(win, "Name: ", "%s", id);
        string name(id);
        omap::iterator it;
        for (it = mObjectMap.begin(); it != mObjectMap.end(); ++it)
        {
            if (it->second == attr)
            {
                continue;
            }
            attr = dynamic_cast<NamedObject *>(it->second);
            if (!attr)
            {
                continue;
            }
            if (attr->mName == name)
            {
                name_collision = true;
                break;
            }
        }
        if (attr && !name_collision)
        {
            attr->mName = name;
            break;
        }
    }
}

void Editor::FindObject()
{
    if (mObjectMap.empty())
    {
        return;
    }
    if (mAttrBrowser == mObjectMap.end())
    {
        mAttrBrowser = mObjectMap.begin();
    }
    mX = mAttrBrowser->first.first;
    mY = mAttrBrowser->first.second;
    mAttrBrowser++;
}

int Editor::GetTile(coords p)
{
    return mTileMap[p.first + p.second * mCols];
}

Object* Editor::GetAttr(coords p)
{
    return mAttrMap[p.first + p.second * mCols];
}

void Editor::SetTile(coords p)
{
    mTileMap[p.first + p.second * mCols] = mBrushTile;
}

void Editor::SetAttr(coords p, Object *attr)
{
    mAttrMap[p.first + p.second * mCols] = attr;
}

bool Editor::WithinBounds(coords p)
{
    return (p.first >= 0 && p.first < mCols &&
            p.second >= 0 && p.second < mRows);
}

void Editor::LoadObjects(fstream &fs, int offset_x, int offset_y)
{
    int attr_map_size;
    fs >> attr_map_size;
    while (attr_map_size > 0) {
        int x, y;
        int type;
        Object *attr;
        fs >> x;
        fs >> y;
        fs >> type;
        switch (type) {
        case Object::ITEM: attr = new Item(fs); break;
        case Object::MONSTER: attr = new Monster(fs); break;
        case Object::HUMANOID: attr = new Humanoid(fs); break;
        case Object::DOOR: attr = new Door(fs); break;
        case Object::TELEPORT: attr = new Teleport(fs); break;
        default:
            break;
        }
        coords p = make_pair(x + offset_x, y + offset_y);
        mObjectMap[p] = attr;
        SetAttr(p, attr);
        attr_map_size--;
    }
}

string Editor::SwitchMap(char trigger, string old_map)
{
    string ret_str = old_map;
    Teleport *t = dynamic_cast<Teleport *>(GetAttr(make_pair(mX, mY)));
    if (t && t->mTrigger == trigger)
    {
        string s(t->mFile);
        if (s == old_map)
        {
            SetCursor(t->mX, t->mY);
            return ret_str;
        }
        fstream fs_out(old_map.c_str(), fstream::out);
        if (fs_out.is_open())
        {
            Save(fs_out);
            fstream fs_in(t->mFile, fstream::in);
            if (fs_in.is_open())
            {
                int x = t->mX;
                int y = t->mY;
                Load(fs_in);
                SetCursor(x, y);
                ret_str = s;
                fs_in.close();
            }
            fs_out.close();
        }
    }
    return ret_str;
}
