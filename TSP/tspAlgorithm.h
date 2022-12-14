#pragma once
#include"../TSP/_ClassHpp/bmpGraph.hpp"
#include<functional>

typedef int (CBMPgraph::*disFn)(int a, int b);
//暴力穷举 (节点总数,求两节点距离的函数指针)
vector<short> Exhaustion(int n, std::function<int(int, int)> fn);//这个算法还是有问题的,有些情况会算错
