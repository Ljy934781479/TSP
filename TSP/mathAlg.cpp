#include"mathAlg.h"

int sumAp(int low,int n,int space = 1)
{
	int hig = low + (n - 1) * space;
	int ret = (low + hig) * (n/2);
	if (n % 2)
		ret += low + (n / 2) * space;
	return ret;
}

int CnmAlg(int n, int m)
{
	double numerator = 1;//分子
	double denominator = 1;//分母
	double a = 1;//(n-m)的阶乘
	for (int i = 1; i < n + 1; i++)
		numerator *= i;
	//分母先算m阶乘
	for (int i = 1; i < m + 1; i++)
		denominator *= i;
	for (int i = 1; i < n - m + 1; i++)
		a *= i;
	denominator *= a;
	return numerator / denominator;
}
