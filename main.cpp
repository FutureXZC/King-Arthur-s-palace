/*
	Author: Xiang
	Data: 2018/10/30
	Description: 解决B2国王与骑士问题（即 经典问题：亚瑟王的宫殿）
*/

/*
	经简单推导，国王如果要被骑士接，最多只需要走2步
	找到距离国王最近的骑士，到接国王的地点接上国王，接到国王的地点为该骑士的新位置
	分别计算 骑士接国王 或 国王自己走 两种情况的路径和
	取上述方法较短者
	其余详细信息请配合文档查看
*/

#include<iostream>
#include<vector>

using namespace std;
#define Error 0
#define Ok 1
#define Overflow 2
#define Underflow 3
#define Notpresent 4
#define Duplicate 5
#define INFTY 32767

//将国王与骑士问题放在Chess类里实现
class Chess
{
public:
	Chess();

	int GetRow(int m);//通过一维坐标求得对应二维坐标中的行坐标
	int GetCol(int m);//通过一维坐标求得对应二维坐标中的列坐标
	void SetKing();//设置国王坐标（二维）
	void SetKnight();//设置骑士数量与骑士坐标

	void Dijkstra(int u, int v);//迪杰斯特拉算法
	int Choose(vector<int> d, vector<bool> s);//选取当前最短路径的下标
	void NarrowTheRange();//剪枝函数，求出集合点的范围
	int AssemblyPoint();//计算集合点
	
	void Output();//测试输出
private:
	int king[2];//国王坐标（x，y）
	int knightNum;//骑士数量
	int knight[63][2];//骑士坐标（x，y）,最多63个骑士
	int m[8][8][8][8];// m[x][y][u][v], 表示从点（x，y）到（u，v）的最短路径
	int v[64][64];//邻接矩阵，对应的是以棋盘上64个格点为顶点、以骑士的运动方式生成的图
	int finalRang[4][2];//存储可能出现最短路径点的范围
};

/*
* @brief	构造函数	初始化骑士和国王坐标为-1
*						初始化最短路径矩阵m
*						以骑士运动方式生成图及其邻接矩阵
*/
Chess::Chess()
{
	memset(king, -1, sizeof(king));//初始化骑士和国王坐标为-1
	memset(knight, -1, sizeof(knight));

	memset(m, INFTY, sizeof(m));//初始化任意两点间最短路程为无穷大
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{ 
			m[i][j][i][j];//自己到自己的路程为0
			//8个方向的日字形走位可达
			if (i - 2 > -1 && j - 1 >- 1) m[i][j][i - 2][j - 1] = 1;
			if (i - 1 > -1 && j - 2 > -1) m[i][j][i - 1][j - 2] = 1;
			if (i + 1 < 8 && j - 2 > -1) m[i][j][i + 1][j - 2] = 1;
			if (i + 2 < 8 && j - 1 > -1) m[i][j][i + 2][j - 1] = 1;
			if (i + 2 < 8 && j + 1 < 8) m[i][j][i + 2][j + 1] = 1;
			if (i + 1 < 8 && j + 2 < 8) m[i][j][i + 1][j + 2] = 1;
			if (i - 1 > -1 && j + 2 < 8) m[i][j][i - 1][j + 2] = 1;
			if (i - 2 > -1 && j + 1 < 8) m[i][j][i - 2][j + 1] = 1;
		}

	memset(v, 0, sizeof(v));//邻接矩阵先置0，后以骑士运动方式生成图，需进行坐标换算
	for (int k = 0; k < 64; k++)
	{
		//骑士最多可走8个方向，即一个顶点最多与其他8个顶点直接相连，需将一维坐标与二位坐标相互转换
		if (GetRow(k) - 2 > -1 && GetCol(k) - 1 > -1)	v[k][k-17] = 1;
		if (GetRow(k) - 1 > -1 && GetCol(k) - 2 > -1)	v[k][k - 10] = 1;
		if (GetRow(k) + 1 < 8 && GetCol(k) - 2 > -1)	v[k][k + 6] = 1;
		if (GetRow(k) + 2 < 8 && GetCol(k) - 1 > -1)	v[k][k + 15] = 1;
		if (GetRow(k) + 2 < 8 && GetCol(k) + 1 < 8)		v[k][k + 17] = 1;
		if (GetRow(k) + 1 < 8 && GetCol(k) + 2 < 8)		v[k][k + 10] = 1;
		if (GetRow(k) - 1 > -1 && GetCol(k) + 2 < 8)	v[k][k - 6] = 1;
		if (GetRow(k) -2 > -1 && GetCol(k) + 1 < 8)		v[k][k - 15] = 1;
	}
}

/*
* @brief	通过一维坐标求得对应二维坐标中的行坐标
* @param	m	顶点的一维坐标 
* @return	row	一维坐标换算为二维坐标的行坐标
*/
int Chess::GetRow(int m)
{
	int row = m/8;
	return row;
}

/*
* @brief	通过一维坐标求得对应二维坐标中的列坐标
* @param	m	顶点的一维坐标
* @return	col	一维坐标换算为二维坐标的列坐标
*/
int Chess::GetCol(int m)
{
	int col = m % 8;
	return col;
}

/*
* @brief	设置国王坐标(二维)
*/
void Chess::SetKing()
{
	cout << "坐标范围为（0，0）到（7，7），请输入国王的坐标（x，y）：";
	cin >> king[0] >> king[1];
}

/*
* @brief	设置骑士数量与骑士坐标
*/
void Chess::SetKnight()
{
	int x, y;//骑士横纵坐标
	do {
		cout << "请输入在8×8棋盘中骑士的数量：";
		cin >> knightNum;
		if (knightNum < 0 || knightNum > 63)
			cout << "骑士数量必须在1到63之间！" << endl;
	} while (knightNum < 0 || knightNum > 63);

	cout << "坐标范围为（0，0）到（7，7），" << endl;
	for (int i = 0; i < knightNum; i++)
		do {
			cout << "请输入第" << i + 1 << "个骑士的坐标（x， y）：";
			cin >> x >> y;
			if (x < 0 || x > 7 || y < 0 || y > 7)
				cout << "第" << i << "个骑士坐标有误！请重新输入！" << endl;
			else
			{
				knight[i][0] = x;
				knight[i][1] = y;
			}
		} while (x < 0 || x > 7 || y < 0 || y > 7);
}

/*
* @brief	选取当前最短路径的下标
* @param	d	记录从源点u到顶点i的当前最短路径长度
* @param	s	记录从源点u到顶点i的路径是否已经确定
*/
int Chess::Choose(vector<int> d, vector<bool> s)
{
	int i, minpos;//d内最短路径的下标
	int min;
	min = INFTY;
	minpos = -1;//初始化下标为-1
	for (i = 1; i < 64; i++)
		if (d[i] < min && !s[i])
		{
			min = d[i];
			minpos = i;
		}
	return minpos;//返回下标位置
}

/*
* @brief	迪杰斯特拉算法
* @param	u	源点
* @param	v	终点
*/
void Chess::Dijkstra(int u, int v)
{
	int i, j, k;//计数标记
	vector<bool> s;//s[i]记录从源点u到顶点i的路径是否已经确定
	vector<int> d;//d[i]记录从源点u到顶点i的当前最短路径长度
	vector<int> path;//path[i]记录从源点u到顶点i的当前最短路径上顶点i的直接前驱顶点的序号
	s.resize(64, false);//s置零
	d.resize(64);
	path.resize(64);

	for (i = 0; i < 64; i++)//初始化
	{
		if (this->v[u][i])
			d[i] = this->v[u][i];
		else
			d[i] = INFTY;
		if (i != u && d[i] < INFTY)
			path[i] = u;
		else
			path[i] = -1;
	}
	s[u] = true;  d[u] = 0;//顶点u为源点

	for (i = 1; i < 63; i++)
	{
		k = Choose(d, s);
		s[k] = true;//k加入到s中
		//cout << "已查询顶点" << k << endl;
		for (j = 0; j < 64; j++)//更新d和path
			if (!s[j] && this->v[k][j] && d[k] + this->v[k][j] < d[j])
			{
				d[j] = d[k] + this->v[k][j];
				path[j] = k;
			}
	}

	cout << endl;
	/*for(i = 0; i < 64; i++)*/
	cout << "顶点" << u << "到" << v << "的最短路径为" << d[v] << endl;
	for (int i = 0; i < 64; i++)
		cout << "path[" << i << "] = " << path[i] << endl;
}

/*
*@ brief	剪枝函数	求出集合点所在的范围,并存储到finalRange数组中，仅保存四个顶点的二维坐标
*			核心思想	若国王 + 骑士 >= 4人，集合点必定出现在最外围的四个角色所围成的四边形中
*						若国王 + 骑士 < 4人，无法构成四边形，则以这3点或2点为基础构成一个四边形
*/
void Chess::NarrowTheRange() 
{
	//当国王与骑士不少于4人时
	if (knightNum + 1 >= 4)
	{

	}
	//当国王与骑士合计3人时
	else if(knightNum == 3){

	}
	//当国王与骑士合计2人时
	else {

	}
}

//测试输出
void Chess::Output()
{
	for (int i = 0; i < knightNum; i++)
		cout << "骑士" << i+1 << "的坐标是" << knight[i][0] << ", " << knight[i][1] << endl;
	cout << "国王" << king[0] << ", " << king[1] << endl;
}

int main()
{
	Chess c;

	c.Dijkstra(0, 19);//测试从顶点0到顶点19
	//c.SetKing();
	//c.SetKnight();
	//c.Output();
	getchar();
	getchar();
	return 0;
}