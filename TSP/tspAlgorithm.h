#pragma once
#include"../TSP/_ClassHpp/bmpGraph.hpp"
#include<functional>

typedef int (CBMPgraph::*disFn)(int a, int b);
//������� (�ڵ�����,�����ڵ����ĺ���ָ��)
vector<short> Exhaustion(int n, std::function<int(int, int)> fn);//����㷨�����������,��Щ��������
