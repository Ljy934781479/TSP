#include"../_ClassHpp/bmpGraph.hpp"
#include"../myDefine.h"
#include"../mathAlg.h"
#include"../tspAlgorithm.h"
#include<functional>
#include<direct.h>
#include<queue>
#include<map>
#include<set>

typedef unsigned int UINT;

CBMPgraph::CBMPgraph(char* outPath)
{
	_outPath = outPath;
}

CBMPgraph::~CBMPgraph()
{
	Release(_distanceArr);
}

int CBMPgraph::count_one_bits(unsigned int value)
{
	int count = 0;
	while (value != 0)
	{
		if (value % 2 == 1)
			count++;
		value /= 2;
	}
	return count;
}

int CBMPgraph::parseBmp(CReadBmp* bmp)
{
	_bmp = bmp;
	//默认黑色是图形,白色是背景.
	for (int i = 0; i < bmp->getHigh(); i++)
		for (int j = 0; j < bmp->getWidth(); j++)
		{
			if (bmp->getPixel(j, i) != COL_BLACK)
				continue;
			myPoint p(j, i);
			tagGraph t = findGraph(p);
			_vGraph.push_back(t);
		}
	calcuDistance();
#ifdef _DEBUG
	char buff[256];
	strcpy_s(buff, _outPath);
	strcat_s(buff, "\\out.bmp");
	_bmp->save24bit(buff);
#endif // !_DEBUG
	return 0;
}

tagGraph CBMPgraph::findGraph(myPoint& p)
{
	COLOR col = _vGraph.size() ? COL_GRAPH : COL_BLUE;
	tagGraph t;
	queue<myPoint> q;//队列
	t.upData(p);
	q.push(p);
	_bmp->setPixel(col,p.x, p.y);
	while (q.size())
	{
		myPoint& cur = q.front();
		for (int k = 0; k < 9; k++)
		{
			int x1 = cur.x + (k % 3) - 1;
			int y1 = cur.y + k / 3 - 1;
			if (_bmp->getPixel(x1, y1) != COL_BLACK)
				continue;
			_bmp->setPixel(col, x1, y1);
			myPoint add(x1, y1);
			t.upData(add);
			q.push(add);
		}
		q.pop();
	}
	t.upCtp();
	return t;
}

int CBMPgraph::calcuDistance()
{
	//开辟储存空间
	int n = _vGraph.size() - 1;
	int size = sumAp(1,n, 1);
	_distanceArr = new int[size];
	_distanceSize = size;
	int arrId = 0;
	for (int i = 0; i < n; i++)
		for (int j = i + 1; j < n + 1; j++)
		{
			double dist = _vGraph[i].distance(_vGraph[j]);
			_distanceArr[arrId++] = dist;
			//cout << i << "----" << j << "----" << dist << endl;
		}
	auto f = std::bind(&CBMPgraph::getDis, this, std::placeholders::_1, std::placeholders::_2);
	vector<short> vPath;
	vPath = Exhaustion(n + 1, f);
	//vPath.clear();
	//vPath = dpAlg();
	drawTsp(vPath);
	return 0;
}
 
int CBMPgraph::getDis(int a, int b)
{
	if(a==b)
		return 1000;
	int min = min(a, b);
	int max = max(a, b);
	if (max >= _vGraph.size())
		return 1000;
	int offset1 = _vGraph.size() - 1;
	int offset2 = max - min-1;
	//根据偏移找到数据
	int address = 0;
	int temp = min;
	while (temp--)
		address += (offset1--);
	address += offset2;
	if (address >= _distanceSize)
		int q = 0;
	int ret = _distanceArr[address];
	return ret;
}

vector<short> CBMPgraph::dpAlg()
{
	//UINT getDis[4][4] =
	//{
	//	{MAX_DISTANCE,3,6,7},
	//	{5,MAX_DISTANCE,2,3},
	//	{6,4,MAX_DISTANCE,2},
	//	{3,7,5,MAX_DISTANCE},
	//};

	/*设*/
	/*V是所有点的集合.   对于dp[i]{V'},存放着[从点i出发,经过且只过一次集合V'的每一个点,V'不包含点i]*/
	/*[Cik]表示点i到点k的距离.   最开始的起点为o.  */
	/*转移方程:         dp[i]{V-i} = min( Ci{V-i}[0] +  dp[{V-i}[0]][{V-i- {V-i}[0] }] , ....  )  其中 .... 为剩下的组合数的其他项  */
	/*列如: dp[0]{1,2} = min( C01 + dp[1]{2} ,  C02+ dp[2]{1} ) */
	/*其中:    C01+dp[1]{2} = C01 + C12 + dp[2]{} = C01 + C12 + C20 */
	/*当 V' 为空集时,dp[i]{V'} = Cio.  */

	struct taghuisu {
		UINT fromNode;
		UINT fromSet;
	};

	UINT maxInt = pow(2, sizeof(UINT) * 8) - 1;
	maxInt = 1000;
	vector<short> vPath;//保存路径顺序
	map<UINT, taghuisu> mhuisu;//集合点数量,所走的集合
	int n = _vGraph.size();
	for (int i = 0; i < n; i++)
	{
		taghuisu t;
		t.fromNode = -1;
		t.fromSet = -1;
		mhuisu[i] = t;
	}
	if (n > sizeof(UINT) * 8)
	{
		cout << "节点个数超出最大值!" << endl;
		return vPath;
	}
	//总的组合数符合二次项定理
	int m = pow(2, n - 1);
	UINT** dp = new UINT * [n];
	for (int i = 0; i < n; i++)
	{
		dp[i] = new UINT[m];
		for (int j = 0; j < m; j++)
			dp[i][j] = maxInt;
	}
	//初始化dp. 也就是第0列的数据是可以确认的
	for (UINT i = 0; i < n; i++)
		dp[i][0] = getDis(i, 0);
		//dp[i][0] = getDis[i][0];
	/*推导其他列数*/
	for (UINT j = 1; j < m; j++)
	{//数字0 - [m-1] 的二进制数,可以一对一的表示出各个子集顶点的存在情况，但是需要注意并不是那么的有序
		/*如: 3 = 011 = {1,2},  4 = 100 = {3}.*/
		UINT cntNodes = count_one_bits(j);
		if (cntNodes == 1)
			int z = 0;
		for (UINT i = 0; i < n; i++)
		{//如果这个集合的数字的二进制i位为1则不行,i到达不了这个集合
			if (((j >> (i - 1)) & 1) == 1)
				continue;
			//如果能到这个集合,就要遍历到达这个集合所有可能,取最短
			int minCost = 99999;
			for (int k = 1; k < n; k++)
			{/*这个位运算是很重点的！！！     假设现在i = 2, j = 5 = {1,3} = 101  */
				/*之后的过程就是求每一个     V = j, V' = V-k,  求每一个d(k,V')  =  C2K + d(k,V'') */
				/*也就是k必须在集合V中 */
				if (((j >> (k - 1)) & 1) == 0)
					continue;
				//这里dp[k][n], n就是利用下一个子集的数据了
				/*假设当前情况是求:  3->{1,2} , 当前j = 3,    */
				/*这个for循环就要求: c31+d(1, {2}),  c32+d(2,{1})     */
				/*重点就变成了:  怎么利用j得到{2}的对应位置*/
				/*这个子集2是不存在点k的,  所以只要把j的二进制第K位置为0，即可得到n;   */
				/*当前j的二进制: 011,   (1<<k-1) = 001, (011 ^ 001) = 010 = 2 */
				/*j==2 即对应着集合 {2}*/
				int cur = getDis(i, k) + dp[k][j ^ (1 << (k - 1))];
				//int cur = getDis[i][k] + dp[k][j ^ (1 << (k - 1))];
				if (cur < minCost)
				{
					mhuisu[j].fromNode = k;
					mhuisu[j].fromSet = j ^ (1 << (k - 1));
				}
				minCost = min(minCost, cur);
			}
			dp[i][j] = minCost;
		}
	}
	cout << "动态规划算法最小花费: " << dp[0][m - 1] << endl;
	for (int i = m - 1;;)
	{
		taghuisu t = mhuisu[i];
		vPath.push_back(t.fromNode);
		i = t.fromSet;
		if (i == 0)
			break;
	}
	return vPath;
}

int CBMPgraph::drawTsp(vector<short>& vPath)
{
	int n = _vGraph.size();
	int total = 0;
	for (int i = 0; i < vPath.size(); i++)
	{
		short nid = vPath[i];
		myPoint a;
		myPoint b;
		if (i == 0)
		{
			a = myPoint(_vGraph[0].cx, _vGraph[0].cy);
			b = myPoint(_vGraph[nid].cx, _vGraph[nid].cy);
			_bmp->lineto(a, b);
			total += getDis(0, nid);
			continue;
		}
		short pre = vPath[i-1];
		a = myPoint(_vGraph[pre].cx, _vGraph[pre].cy);
		b = myPoint(_vGraph[nid].cx, _vGraph[nid].cy);
		total += getDis(pre, nid);
		_bmp->lineto(a, b);
	}
	short tail = *vPath.rbegin();
	myPoint a(_vGraph[tail].cx, _vGraph[tail].cy);
	myPoint b(_vGraph[0].cx, _vGraph[0].cy);
	total += getDis(0, tail);
	_bmp->lineto(a, b);
	cout << "再次验证最小花费: " << total << endl;
	cout << "路径顺序为: -0-";
	for (short& a : vPath)
		cout << a << "-";
	cout << "0-" << endl;;
	return 0;
}

int CBMPgraph::getLowZero(UINT& n)
{
	for (int i = 0; i < sizeof(UINT) * 8; i++)
	{
		UINT temp = n >> i;
		if ( (temp & 1) == 1)
			continue;
		return i + 1;
	}
	return 0;
}

tagGraph::tagGraph()
{
	x0 = y0 = 99999;
	x1 = y1 = -9999;
	cx = cy = 0;
}
