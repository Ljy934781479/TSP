#include"tspAlgorithm.h"
#include"mathAlg.h"
#include<vector>
#include<queue>

vector<short> Exhaustion(int n, std::function<int(int, int)> fn)
{//好像算法还是有问题.... 
	int minCost = 0;//最小花费
	vector<short> minPath;//最小路径顺序
	vector<short> v;//用来保证能做完所有全排列
	vector<short> v1;//当前路径顺序
	for (int i = 1; i < n; i++)
	{
		minCost += fn(i-1, i);
		v.push_back(i);
	}
	minCost += fn(0, n-1);
	int curCost = minCost;
	if (n < 4)
	{
		cout << "暴力算法最小花费： " << minCost << endl;
		return v;
	}
	v1 = v;
	int p = v1.size() - 1;//指向最后一位
	while (1)
	{
		//交换之前考虑距离的变换                  设当前路径:    0   --- 1--2--3--4 ---  0
		if (p == v1.size() - 1)
		{
			curCost -= fn(0, v1[p]);              // -=  (4--0)的距离
			curCost += fn(0, v1[p - 1]);          // +=  (3--0)的距离
		}
		else
		{
			curCost -= fn(v1[p + 1], v1[p]);
			curCost += fn(v1[p + 1], v1[p - 1]);
		}
		curCost -= fn(v1[p - 2], v1[p - 1]);    // -=  (2--3)的距离
		curCost += fn(v1[p - 2], v1[p]);        // +=  (2--4)的距离,  最后路径  0  ------ 1--2--4--3--- 0
		//交换
		short temp = v1[p];
		v1[p] = v1[p - 1];
		v1[p - 1] = temp;
		p--;
		//每交换一次都要考虑当前是不是最优解
		if (curCost <= minCost)
		{
			minCost = curCost;
			minPath = v1;
		}
		if (v1 == v)//如果交换玩之后回到原来的样子则表明做完了一轮排列
		{//说明已经做完了所有的排列
			if (*v.begin() == n - 1)
				break;
			short head = *v.begin();//像队列一样操作一下
			v.erase(v.begin());
			v.push_back(head);
			//!!!!这里很重要,debug半天没注意. 既然v1变了,也要跟着维护curCost这个变量
			/* 假设原本 v1{1,2,3,4,5}   --->   {2,3,4,5,1}*/
			curCost -= fn(0, v1[0]);    // -(0->1)
			curCost -= fn(v1[0], v1[1]);// -(1->2)
			curCost += fn(0, v1[1]);    // +(0->2)
			curCost -= fn(*v1.rbegin(), 0);//-(5->0)
			curCost += fn(*v1.rbegin(), v1[0]);//+(5->1)
			curCost += fn(v1[0], 0);//+(1->0)
			v1 = v;//!!一定要先算好再替换排列
			p = v1.size() - 1;//指向新排列的最后位置
			//这样子写好恶心啊..... 之后再看看能不能优化.
		}
		if (p - 2 < 0)//一直交换
			p = v1.size() - 1;
	}
	cout << "暴力算法最小花费： " << minCost << endl;
	return minPath;
}

