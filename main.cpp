/*
*	Author: Xiang
*	Data: 2018/10/30
*	Description: 解决B2国王与骑士问题（即 经典问题：亚瑟王的宫殿）
*			基本思路:
*			要实现所有人集合到一点，只需要考虑两种情况：
*			1、国王非常"孤儿"地自己走：骑士和国王各走各的，就算有骑士半路遇到国王了也不带
*			2、国王有骑士接：找到距离国王最近的骑士，到接国王的地点接上国王，接到国王的地点为该骑士的新位置，然后以骑士的走法走到集合点
*			分别计算 上述两种情况下所需的最少步数，取其中较短者
*			经简单推演，国王如果要被骑士接，最多只需要走2步，因此接到国王的区域的最大范围就是以国王为中心的5×5的范围
*			经简单推演，最终的集合点必定在由最外围的骑士和国王所围成的多边形（也可能是线段或点）内
*	Others：为了便于程序实现，在此将多边形补全为四边形，图解详见文档
*	Version 3.0：修正了部分注释错误
*/

#include<iostream>

using namespace std;

#define INFTY 32767	//假装这个是无穷大


/*
* @brief	Chess类	生成一个8×8的棋盘，抽象化国王与骑士问题（即 经典问题：亚瑟王的宫殿）
* @definition		集合点assemblyPoint：国王和骑士最终走到一起的点
*					接送点pickPoint：骑士去接国王，然后从该点开始带着国王一起运动的点
*					御用骑士pickKnight：距离接送点步数最少，到达接送点后带着国王一起运动的骑士
*/
class Chess
{
public:
	Chess();

	int GetRow(int m);//通过一维坐标求得对应二维坐标中的行坐标
	int GetCol(int m);//通过一维坐标求得对应二维坐标中的列坐标
	void SetKing();//设置国王坐标（二维）
	void SetKnight();//设置骑士数量与骑士坐标（二维）
	
	int Choose(int *d, bool *s);//选取当前最短路径的下标
	int Dijkstra(int u, int v);//迪杰斯特拉算法，返回源点到终点的步数
	void NarrowTheRange();//剪枝函数，分别求出集合点的所在范围和接送点的所在范围
	void FindAssemblyPoint();//找到集合点
	
	void Output();//输出计算结果
private:
	int king[2];//国王坐标（行，列）
	int knightNum;//骑士数量
	int knight[63][2];//骑士坐标（行，列）,最多63个骑士
	int v[64][64];//邻接矩阵，对应的是以棋盘上64个格点为顶点、以骑士的运动方式生成的图
	int range[4];//存储可能出现集合点的坐标范围，依次为 xMax , xMin, yMax, yMin
	int pickArea[4];//以国王为中心最大可为5×5的方形区域，该区域内国王可以被骑士接上
	int pickKnight;//“御用骑士”，其值对应knight数组的行标（即第n个骑士）
	int pickPoint;//骑士接上国王位置的一维坐标
	int minPickStep;//骑士走到接送点的最小步数
	int assemblyStep;//到最终集合点的总步数
	int assemblyPoint[2];//集合点的坐标
};


/*
* @brief	构造函数	初始化骑士和国王坐标为-1
*						以骑士运动方式生成图及其邻接矩阵
*/
Chess::Chess()
{
	memset(king, -1, sizeof(king));//初始化骑士和国王坐标为-1
	memset(knight, -1, sizeof(knight));

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
	cout << "坐标范围为（0，0）到（7，7），请输入国王的坐标（行，列）：";
	cin >> king[0] >> king[1];
}


/*
* @brief	设置骑士数量与骑士坐标（二维）
*/
void Chess::SetKnight()
{
	int x, y;//骑士的行列坐标
	do {
		cout << "请输入在8×8棋盘中骑士的数量：";
		cin >> knightNum;
		if (knightNum < 0 || knightNum > 63)
			cout << "骑士数量必须在1到63之间！" << endl;
	} while (knightNum < 0 || knightNum > 63);

	cout << "坐标范围为（0，0）到（7，7），" << endl;
	for (int i = 0; i < knightNum; i++)
		do {
			cout << "请输入第" << i + 1 << "个骑士的坐标（行， 列）：";
			cin >> x >> y;
			if (x < 0 || x > 7 || y < 0 || y > 7)
				cout << "第" << i << "个骑士坐标有误！请重新输入！" << endl;
			else
			{
				knight[i][0] = x;
				knight[i][1] = y;
			}
		} while (x < 0 || x > 7 || y < 0 || y > 7);
		cout << "计算中..." << endl;
}


/*
* @brief	选取当前最短路径的下标
* @param	d	记录从源点u到顶点i的当前最短路径长度
* @param	s	记录从源点u到顶点i的路径是否已经确定
*/
int Chess::Choose(int *d, bool *s)
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
* @brief	迪杰斯特拉算法，返回源点到终点的步数
* @param	u	源点
* @param	v	终点
*/
int Chess::Dijkstra(int u, int v)
{
	int i, j, k;//计数标记
	bool s[64];//s[i]记录从源点u到顶点i的路径是否已经确定
	int d[64];//d[i]记录从源点u到顶点i的当前最短路径长度（步数）
	int path[64];//path[i]记录从源点u到顶点i的当前最短路径上顶点i的直接前驱顶点的序号
	memset(s, false, sizeof(s));

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
		for (j = 0; j < 64; j++)//更新d和path
			if (!s[j] && this->v[k][j] && d[k] + this->v[k][j] < d[j])
			{
				d[j] = d[k] + this->v[k][j];
				path[j] = k;
			}
	}
	return d[v];
}


/*
*@ brief	剪枝函数	求出集合点所在的范围,并存储到range数组中，仅保存最大和最小的行列值，同时求出以国王为中心最大为5×5的接送区域pickArea
*			核心思想	若国王 + 骑士 >= 4人，集合点必定出现在最外围的四个角色所围成的四边形中
*						若国王 + 骑士 < 4人，无法构成四边形，则以这3点或2点为基础构成一个四边形，极限情况是一个点
*						经简单推论，国王最多走两步就能被骑士接到，故接送地点即为以国王初始位置为中心5×5的范围。
*/
void Chess::NarrowTheRange() 
{
	//四边形的坐标范围，初始化为国王的位置
	int rowMax , rowMin, colMax, colMin;
		rowMax = rowMin = king[0]; colMax = colMin = king[1];

	//遍历骑士坐标，求得可能存在集合点的范围
	for (int i = 0; i < knightNum; i++)
	{
		rowMax = (rowMax > knight[i][0]) ? rowMax : knight[i][0];
		rowMin = (rowMin < knight[i][0]) ? rowMin : knight[i][0];
		colMax = (colMax > knight[i][1]) ? colMax : knight[i][1];
		colMin = (colMin < knight[i][1]) ? colMin : knight[i][1];
	}
	//将求得的范围存入range数组
	range[0] = rowMin; range[1] = rowMax;
	range[2] = colMin; range[3] = colMax;

	//求国王被接送的区域，只需判断坐标±2或±1后是否越界即可
	pickArea[0] = (king[0] - 2 > -1 ? king[0] - 2 : (king[0] - 1 > -1 ? king[0] - 1 : king[0]));//rowMin
	pickArea[1] = (king[0] + 2 < 8 ? king[0] + 2 : (king[0] + 1 < 8 ? king[0] + 1 : king[0]));//rowMax
	pickArea[2] = (king[1] - 2 > -1 ? king[1] - 2 : (king[1] - 1 > -1 ? king[1] - 1 : king[1]));//colMin
	pickArea[3] = (king[1] + 2 < 8 ? king[1] + 2 : (king[1] + 1 < 8 ? king[1] + 1 : king[1]));//colMax
}


/*
* @brief	找到集合点
*			核心思想	1、已通过剪枝函数求得集合点可能出现的范围，只需枚举出现在range数组界定的范围内的点
*						2、骑士不接国王：让所有角色都自己走，求得总路径最短的集合点（1）
*						3、骑士接国王：让离国王最近的骑士去接国王，以接到国王的地点作为该骑士的起始点，再求集合点（2）
*						4、取集合点（1）和（2）中较小者作为最终集合点，若二者相等，取（2），这样国王比较有牌面
*/
void Chess::FindAssemblyPoint()
{
	//取出range数组和pickArea内的坐标范围
	int rowMin = range[0], rowMax = range[1], colMin = range[2], colMax = range[3];
	int step = 0;//计步器，当前顶点的总步数
	int minStep1 = INFTY, minStep2 = INFTY;//记录最少步数，初始化为无穷大
	int point1 = 0, point2 = 0;//初始化两种方案的集合点为一维坐标0
	int pickStep = 0;//当前骑士去接国王所需的步数，初始化为0

	//用一维坐标遍历棋盘上的点
	//骑士不接国王时，直接寻找集合点（1）
	for (int i = 0; i < 64; i++)
	{
		//判断该点是否在剪枝函数限定的范围内，若是，则继续计算，否则跳过查询下一个点
		if (GetRow(i) > rowMin && GetRow(i) < rowMax && GetCol(i) > colMin && GetCol(i) < colMax)
		{
			for (int j = 0; j < knightNum; j++)
				step += Dijkstra(i, knight[j][0] * 8 + knight[j][1]);//用迪杰斯特拉算法求各个骑士到给定点的步数
			//国王可以斜着走，因此国王的步数 = max（ 源目两点行坐标的差值 , 源目两点列坐标的差值）
			step += abs(king[0] - GetRow(i)) > abs(king[1] - GetCol(i)) ? abs(king[0] - GetRow(i)) : abs(king[1] - GetCol(i));
			//更新当前最小步数与暂定集合点（1）
			if (step < minStep1)
			{
				minStep1 = step;
				point1 = i;
			}
		}
		step = 0;//计数器置零
	}

	//骑士接国王时
	int pickRowMin = pickArea[0], pickRowMax = pickArea[1], pickColMin = pickArea[2], pickColMax = pickArea[3];//取出pickArea内的范围
	minPickStep = INFTY; //骑士走到接送点的最小接送步数初始化为无穷大
	//在pickArea内挑选出“御用骑士”
	for (int i = pickRowMin; i <= pickRowMax; i++)
		for (int j = pickColMin; j <= pickColMax; j++)
		{
			for (int k = 0; k < knightNum; k++)//挑选接到国王需要的步数最少的骑士作为“御用骑士”
			{
				pickStep = Dijkstra(i * 8 + j, knight[k][0] * 8 + knight[k][1]);//求“御用骑士”到接送点的步数
				//更新骑士接送步数，将接送点作为“御用骑士”的新起始位置
				if (pickStep < minPickStep)
				{
					minPickStep = pickStep;
					pickKnight = k;
					pickPoint = i * 8 + j;
				}
			}
		}
	int newKnight[63][2];//复制一个knight数组，更新“御用骑士”的位置
	memcpy(newKnight, knight, sizeof(knight));
	newKnight[pickKnight][0] = GetRow(pickPoint); newKnight[pickKnight][1] = GetCol(pickPoint);
	//复制并更新完骑士数组后，正式开始寻找集合点（2）
	for (int i = 0; i < 64; i++)
	{
		//判断该点是否在剪枝函数限定的范围内，若是，则继续计算，否则跳过查询下一个点
		if (GetRow(i) > rowMin && GetRow(i) < rowMax && GetCol(i) > colMin && GetCol(i) < colMax)
		{
			for (int j = 0; j < knightNum; j++)
				step += Dijkstra(i, newKnight[j][0] * 8 + newKnight[j][1]);//用迪杰斯特拉算法求各个骑士到给定点的步数
			//更新当前最小步数与暂定集合点（2）
			if (step < minStep1)
			{
				minStep2 = step;
				point2 = i;
			}
		}
		step = 0;//计数器置零
	}
	//总步数还需包括 国王移动到接送点的步数
	minStep2 += abs(king[0] - GetRow(pickPoint)) > abs(king[1] - GetCol(pickPoint)) ? abs(king[0] - GetRow(pickPoint)) : abs(king[1] - GetCol(pickPoint));

	//取上述两种方法求得步数的较小者，更新集合点位置assemblyPoint和集合所需的总步数assemblyStep
	if (minStep1 < minStep2)
	{
		assemblyStep = minStep1;
		assemblyPoint[0] = GetRow(point1); assemblyPoint[1] = GetCol(point1);
	}
	else {
		assemblyStep = minStep2;
		assemblyPoint[0] = GetRow(point2); assemblyPoint[1] = GetCol(point2);
	}
}


/*
* @brief	输出计算结果
*/
void Chess::Output()
{
	cout << "最小步数为：" << assemblyStep << "步。" << endl;
	cout << "集合点为：（" << assemblyPoint[0] << "，" << assemblyPoint[1] << "）。" << endl;
	cout << "计算完成。" << endl;
}


int main()
{
	Chess c;//生成一个棋盘对象

	c.SetKing();//设置国王坐标
	c.SetKnight();//设置骑士数量与各个骑士的坐标
	c.NarrowTheRange();//剪枝函数，缩小枚举的范围
	c.FindAssemblyPoint();//根据剪枝函数划定的范围，利用迪杰斯特拉算法，寻找集合点
	c.Output();//输出
	getchar();
	getchar();
	return 0;
}