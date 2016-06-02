typedef pair<int,int> coords;
typedef map<coords, Object *> omap;

coords LoadCoords(fstream &fs)
{
    int x, y;
    fs >> x;
    fs >> y;
    return make_pair(x, y);
}
int LoadTransparency(fstream &fs)
{
    int tr;
    fs >> tr;
    return tr;
}
bool LoadSolidity(fstream &fs)
{
    bool s;
    fs >> s;
    return s;
}
string LoadColor(fstream &fs)
{
    string c1;
    fs >> c1;
    if (c1 == "light")
    {
        string c2;
        fs >> c2;
        c1 = c1 + " " + c2;
    }
    return c1;
}
char LoadSymbol(fstream &fs)
{
    char s;
    fs >> s;
    return s;
}

void LoadObject(fstream &fs, omap &objectMap)
{
    coords p = LoadCoords(fs);
    int tr = LoadTransparency(fs);
    bool so = LoadSolidity(fs);
    string co = LoadColor(fs);
    char sy = LoadSymbol(fs);
    objectMap[p] = new Object(tr, so, co, sy);
}
