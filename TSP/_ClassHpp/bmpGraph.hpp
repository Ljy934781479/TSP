
#pragma once
#include"../_ClassHpp/readBmp.hpp"
#include"struct.hpp"
#include<map>
#include<set>

#define MAX_DISTANCE 999999
struct tagGraph
{
	tagGraph();
	void upData(myPoint& p)
	{
		x0 = min(p.x, x0);
		x1 = max(p.x, x1);
		y0 = min(p.y, y0);
		y1 = max(p.y, y1);
	}
	void upCtp()
	{
		cx = (x0 + x1) / 2;
		cy = (y0 + y1) / 2;
	}
	//中心点
	int cx;
	int cy;
	//最小外接矩形
	int x0,x1,y0,y1;
	double distance(tagGraph& b)
	{
		return sqrt(pow(abs(cx - b.cx), 2) + pow(abs(cy - b.cy), 2));
	}
};

//用来读取一些bmp内的图形
class CBMPgraph
{
public:
	CBMPgraph(char* outPath = nullptr);
	~CBMPgraph();
	int  count_one_bits(unsigned int value);
	int parseBmp(CReadBmp* bmp);//解析工作
	tagGraph findGraph(myPoint& p);//查找单个图形
	int calcuDistance();//完成各个点之间的距离计算
	int getDis(int a,int b);//得到两个点的距离. 参数是点的标号. 没有0点！！！从1开始!!习惯了....
	vector<short> dpAlg();//创建压缩dp对应的指针.
	vector<short> dpAl1g();
	int drawTsp(vector<short>& vPath);//绘出最优结果图
	int getLowZero(UINT& n);//得到最低位的0
private:
	map<short, set<short> > _mSet;//debug用,用来检查数字与集合是否出错
	char* _outPath;//dbg输出路径
	CReadBmp* _bmp;//图片源文件
	vector<tagGraph> _vGraph;//读取到的所有图形,图形编号优先级是从上到下从左到右的
	int* _distanceArr;//各个点之间的距离,动态创建,并且只考虑点对点是对称距离的.也就是: n个点需要,  ∑[1,n-1] 个数据.
	int _distanceSize;//为了节省空间但是好像写错了debug用
};

