// TSP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <direct.h>
#include"_ClassHpp/bmpGraph.hpp"
int main(int argc, char* argv[])
{
#ifdef _DEBUG
    char cwd[256];
    _getcwd(cwd, 256);
    strcat_s(cwd, "\\test");
    _mkdir(cwd);
    CBMPgraph p(cwd);
#else
    CBMPgraph p;
#endif // _DEBUG
    setlocale(LC_ALL, "");
    CReadBmp b;
    const wchar_t* ws =  b.charTowchar(argv[1]);
    b.readFile(ws);
    p.parseBmp(&b);
    return 0;
}