#pragma once

//释放动态内存函数模板
template<class T>
void Release(T t)
{
	if (t != NULL) {
		delete t;
		t = NULL;
	}
}
//释放动态内存函数模板
template<class T>
void ReleaseVec(std::vector<T>& v)
{
	for (T& o : v)
		Release(o);
	v.clear();
}



#define COL_BLACK RGB(0,0,0)
#define COL_WHITE RGB(255,255,255)
#define COL_BLUE RGB(0,0,255)
#define COL_GRAPH RGB(255,0,0)
#define COL_LINETO RGB(165,165,165)