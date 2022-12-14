
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
	//���ĵ�
	int cx;
	int cy;
	//��С��Ӿ���
	int x0,x1,y0,y1;
	double distance(tagGraph& b)
	{
		return sqrt(pow(abs(cx - b.cx), 2) + pow(abs(cy - b.cy), 2));
	}
};

//������ȡһЩbmp�ڵ�ͼ��
class CBMPgraph
{
public:
	CBMPgraph(char* outPath = nullptr);
	~CBMPgraph();
	int  count_one_bits(unsigned int value);
	int parseBmp(CReadBmp* bmp);//��������
	tagGraph findGraph(myPoint& p);//���ҵ���ͼ��
	int calcuDistance();//��ɸ�����֮��ľ������
	int getDis(int a,int b);//�õ�������ľ���. �����ǵ�ı��. û��0�㣡������1��ʼ!!ϰ����....
	vector<short> dpAlg();//����ѹ��dp��Ӧ��ָ��.
	vector<short> dpAl1g();
	int drawTsp(vector<short>& vPath);//������Ž��ͼ
	int getLowZero(UINT& n);//�õ����λ��0
private:
	map<short, set<short> > _mSet;//debug��,������������뼯���Ƿ����
	char* _outPath;//dbg���·��
	CReadBmp* _bmp;//ͼƬԴ�ļ�
	vector<tagGraph> _vGraph;//��ȡ��������ͼ��,ͼ�α�����ȼ��Ǵ��ϵ��´����ҵ�
	int* _distanceArr;//������֮��ľ���,��̬����,����ֻ���ǵ�Ե��ǶԳƾ����.Ҳ����: n������Ҫ,  ��[1,n-1] ������.
	int _distanceSize;//Ϊ�˽�ʡ�ռ䵫�Ǻ���д����debug��
};

