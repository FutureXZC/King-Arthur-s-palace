/*
	Author: Xiang
	Data: 2018/10/30
	Description: ���B2��������ʿ���⣨�� �������⣺��ɪ���Ĺ��
*/

/*
	�����Ƶ����������Ҫ����ʿ�ӣ����ֻ��Ҫ��2��
	�ҵ���������������ʿ�����ӹ����ĵص���Ϲ������ӵ������ĵص�Ϊ����ʿ����λ��
	�ֱ���� ��ʿ�ӹ��� �� �����Լ��� ���������·����
	ȡ���������϶���
	������ϸ��Ϣ������ĵ��鿴
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
#define INFTY 32767	//��װ����������

/*
* @brief	Chess��	ʵ�ֹ�������ʿ���⣨�� �������⣺��ɪ���Ĺ��
* @definition		���ϵ㣺��������ʿ�����ߵ�һ��ĵ�
*					���͵㣺��ʿȥ�ӹ�����Ȼ��Ӹõ㿪ʼ���Ź���һ���˶��ĵ�
*/
class Chess
{
public:
	Chess();

	int GetRow(int m);//ͨ��һά������ö�Ӧ��ά�����е�������
	int GetCol(int m);//ͨ��һά������ö�Ӧ��ά�����е�������
	void SetKing();//���ù������꣨��ά��
	void SetKnight();//������ʿ��������ʿ����
	
	int Choose(vector<int> d, vector<bool> s);//ѡȡ��ǰ���·�����±�
	int Dijkstra(int u, int v);//�Ͻ�˹�����㷨������Դ�㵽�յ�Ĳ���
	void NarrowTheRange();//��֦������������ϵ�ķ�Χ
	void FindAssemblyPoint();//�ҵ����ϵ�
	
	void Output();//�������
private:
	int king[2];//�������꣨x��y��
	int knightNum;//��ʿ����
	int knight[63][2];//��ʿ���꣨x��y��,���63����ʿ
	int m[8][8][8][8];// m[x][y][u][v], ��ʾ�ӵ㣨x��y������u��v�������·��
	int v[64][64];//�ڽӾ��󣬶�Ӧ������������64�����Ϊ���㡢����ʿ���˶���ʽ���ɵ�ͼ
	int range[4];//�洢���ܳ��ּ��ϵ�����귶Χ������Ϊ xMax , xMin, yMax, yMin
	int pickArea[4];//�ѹ���Ϊ��������Ϊ5��5�ķ������򣬸������ڹ������Ա���ʿ����
	int pickKnight;//�ӹ����ġ������������ֵ��Ӧknight������б꣨����n����ʿ��
	int pickPoint;//��ʿ���Ϲ���λ�õ�һά����
	int minPickStep;//��ʿ�ߵ����͵����С����
	int assemblyStep;//�����ռ��ϵ���ܲ���
	int assemblyPoint[2];//���ϵ������
};

/*
* @brief	���캯��	��ʼ����ʿ�͹�������Ϊ-1
*						��ʼ�����·������m
*						����ʿ�˶���ʽ����ͼ�����ڽӾ���
*/
Chess::Chess()
{
	memset(king, -1, sizeof(king));//��ʼ����ʿ�͹�������Ϊ-1
	memset(knight, -1, sizeof(knight));

	memset(m, INFTY, sizeof(m));//��ʼ��������������·��Ϊ�����
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{ 
			m[i][j][i][j];//�Լ����Լ���·��Ϊ0
			//8���������������λ�ɴ�
			if (i - 2 > -1 && j - 1 >- 1) m[i][j][i - 2][j - 1] = 1;
			if (i - 1 > -1 && j - 2 > -1) m[i][j][i - 1][j - 2] = 1;
			if (i + 1 < 8 && j - 2 > -1) m[i][j][i + 1][j - 2] = 1;
			if (i + 2 < 8 && j - 1 > -1) m[i][j][i + 2][j - 1] = 1;
			if (i + 2 < 8 && j + 1 < 8) m[i][j][i + 2][j + 1] = 1;
			if (i + 1 < 8 && j + 2 < 8) m[i][j][i + 1][j + 2] = 1;
			if (i - 1 > -1 && j + 2 < 8) m[i][j][i - 1][j + 2] = 1;
			if (i - 2 > -1 && j + 1 < 8) m[i][j][i - 2][j + 1] = 1;
		}

	memset(v, 0, sizeof(v));//�ڽӾ�������0��������ʿ�˶���ʽ����ͼ����������껻��
	for (int k = 0; k < 64; k++)
	{
		//��ʿ������8�����򣬼�һ���������������8������ֱ���������轫һά�������λ�����໥ת��
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
* @brief	ͨ��һά������ö�Ӧ��ά�����е�������
* @param	m	�����һά���� 
* @return	row	һά���껻��Ϊ��ά�����������
*/
int Chess::GetRow(int m)
{
	int row = m/8;
	return row;
}

/*
* @brief	ͨ��һά������ö�Ӧ��ά�����е�������
* @param	m	�����һά����
* @return	col	һά���껻��Ϊ��ά�����������
*/
int Chess::GetCol(int m)
{
	int col = m % 8;
	return col;
}

/*
* @brief	���ù�������(��ά)
*/
void Chess::SetKing()
{
	cout << "���귶ΧΪ��0��0������7��7������������������꣨�У��У���";
	cin >> king[0] >> king[1];
}

/*
* @brief	������ʿ��������ʿ����
*/
void Chess::SetKnight()
{
	int x, y;//��ʿ��������
	do {
		cout << "��������8��8��������ʿ��������";
		cin >> knightNum;
		if (knightNum < 0 || knightNum > 63)
			cout << "��ʿ����������1��63֮�䣡" << endl;
	} while (knightNum < 0 || knightNum > 63);

	cout << "���귶ΧΪ��0��0������7��7����" << endl;
	for (int i = 0; i < knightNum; i++)
		do {
			cout << "�������" << i + 1 << "����ʿ�����꣨�У� �У���";
			cin >> x >> y;
			if (x < 0 || x > 7 || y < 0 || y > 7)
				cout << "��" << i << "����ʿ�����������������룡" << endl;
			else
			{
				knight[i][0] = x;
				knight[i][1] = y;
			}
		} while (x < 0 || x > 7 || y < 0 || y > 7);
		cout << "������..." << endl;
}

/*
* @brief	ѡȡ��ǰ���·�����±�
* @param	d	��¼��Դ��u������i�ĵ�ǰ���·������
* @param	s	��¼��Դ��u������i��·���Ƿ��Ѿ�ȷ��
*/
int Chess::Choose(vector<int> d, vector<bool> s)
{
	int i, minpos;//d�����·�����±�
	int min;
	min = INFTY;
	minpos = -1;//��ʼ���±�Ϊ-1
	for (i = 1; i < 64; i++)
		if (d[i] < min && !s[i])
		{
			min = d[i];
			minpos = i;
		}
	return minpos;//�����±�λ��
}

/*
* @brief	�Ͻ�˹�����㷨������Դ�㵽�յ�Ĳ���
* @param	u	Դ��
* @param	v	�յ�
*/
int Chess::Dijkstra(int u, int v)
{
	int i, j, k;//�������
	vector<bool> s;//s[i]��¼��Դ��u������i��·���Ƿ��Ѿ�ȷ��
	vector<int> d;//d[i]��¼��Դ��u������i�ĵ�ǰ���·�����ȣ�������
	vector<int> path;//path[i]��¼��Դ��u������i�ĵ�ǰ���·���϶���i��ֱ��ǰ����������
	s.resize(64, false);//s����
	d.resize(64);
	path.resize(64);

	for (i = 0; i < 64; i++)//��ʼ��
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
	s[u] = true;  d[u] = 0;//����uΪԴ��

	for (i = 1; i < 63; i++)
	{
		k = Choose(d, s);
		s[k] = true;//k���뵽s��
		//cout << "�Ѳ�ѯ����" << k << endl;
		for (j = 0; j < 64; j++)//����d��path
			if (!s[j] && this->v[k][j] && d[k] + this->v[k][j] < d[j])
			{
				d[j] = d[k] + this->v[k][j];
				path[j] = k;
			}
	}
	return d[v];
	/*cout << endl;*/
	/*for(i = 0; i < 64; i++)*/
	/*cout << "����" << u << "��" << v << "�����·��Ϊ" << d[v] << endl;
	for (int i = 0; i < 64; i++)
		cout << "path[" << i << "] = " << path[i] << endl;*/
}

/*
*@ brief	��֦����	������ϵ����ڵķ�Χ,���洢��range�����У�������������С������ֵ��ͬʱ����Թ���Ϊ�������Ϊ5��5�Ľ�������pickArea
*			����˼��	������ + ��ʿ >= 4�ˣ����ϵ�ض�����������Χ���ĸ���ɫ��Χ�ɵ��ı�����
*						������ + ��ʿ < 4�ˣ��޷������ı��Σ�������3���2��Ϊ��������һ���ı��Σ����������һ����
*						�������ۣ�����������������ܱ���ʿ�ӵ����ʽ��͵ص㼴Ϊ�Թ�����ʼλ��Ϊ����5��5�ķ�Χ��
*/
void Chess::NarrowTheRange() 
{
	int rowMax , rowMin, colMax, colMin;//�ı��ε����귶Χ����ʼ��Ϊ�����ĵ�
		rowMax = rowMin = king[0]; colMax = colMin = king[1];

	//������ʿ���꣬��ÿ��ܴ��ڼ��ϵ�ķ�Χ
	for (int i = 0; i < knightNum; i++)
	{
		rowMax = (rowMax > knight[i][0]) ? rowMax : knight[i][0];
		rowMin = (rowMin < knight[i][0]) ? rowMin : knight[i][0];
		colMax = (colMax > knight[i][1]) ? colMax : knight[i][1];
		colMin = (colMin < knight[i][1]) ? colMin : knight[i][1];
	}
	//����õķ�Χ����range����
	range[0] = rowMin; range[1] = rowMax;
	range[2] = colMin; range[3] = colMax;

	//����������͵�����ֻ���ж������2���1���Ƿ�Խ�缴��
	pickArea[0] = (king[0] - 2 > -1 ? king[0] - 2 : (king[0] - 1 > -1 ? king[0] - 1 : king[0]));//rowMin
	pickArea[1] = (king[0] + 2 < 8 ? king[0] + 2 : (king[0] + 1 < 8 ? king[0] + 1 : king[0]));//rowMax
	pickArea[2] = (king[1] - 2 > -1 ? king[1] - 2 : (king[1] - 1 > -1 ? king[1] - 1 : king[1]));//colMin
	pickArea[3] = (king[1] + 2 < 8 ? king[1] + 2 : (king[1] + 1 < 8 ? king[1] + 1 : king[1]));//colMax
}

/*
* @brief	�ҵ����ϵ�
*			����˼��	1����ͨ����֦������ü��ϵ���ܳ��ֵķ�Χ��ֻ��ö�ٳ�����range����綨�ķ�Χ�ڵĵ�
*						2����ʿ���ӹ����������н�ɫ���Լ��ߣ������·����̵ļ��ϵ㣨1��
*						3����ʿ�ӹ�������������������ʿȥ�ӹ������Խӵ������ĵص���Ϊ����ʿ����ʼ�㣬���󼯺ϵ㣨2��
*						4��ȡ���ϵ㣨1���ͣ�2���н�С����Ϊ���ռ��ϵ㣬��������ȣ�ȡ��2�������������Ƚ�������
*/
void Chess::FindAssemblyPoint()
{
	//ȡ��range�����pickArea�ڵ����귶Χ
	int rowMin = range[0], rowMax = range[1], colMin = range[2], colMax = range[3];
	int step = 0;//�Ʋ�������ǰ������ܲ���
	int minStep1 = INFTY, minStep2 = INFTY;//��¼���ٲ�������ʼ��Ϊ�����
	int point1 = 0, point2 = 0;//��ʼ�����ַ����ļ��ϵ�Ϊһά����0
	int pickStep = 0;//��ǰ��ʿȥ�ӹ�������Ĳ�������ʼ��Ϊ0

	//��һά������������ϵĵ�
	//��ʿ���ӹ���ʱ��ֱ��Ѱ�Ҽ��ϵ�1
	for (int i = 0; i < 64; i++)
	{
		//�жϸõ��Ƿ��ڼ�֦�����޶��ķ�Χ�ڣ����ǣ���������㣬����������ѯ��һ����
		if (GetRow(i) > rowMin && GetRow(i) < rowMax && GetCol(i) > colMin && GetCol(i) < colMax)
		{
			for (int j = 0; j < knightNum; j++)
				step += Dijkstra(i, knight[j][0] * 8 + knight[j][1]);//�õϽ�˹�����㷨�������ʿ��������Ĳ���
			//��������б���ߣ���˹����Ĳ��� = max�� ԴĿ����������Ĳ�ֵ , ԴĿ����������Ĳ�ֵ��
			step += abs(king[0] - GetRow(i)) > abs(king[1] - GetCol(i)) ? abs(king[0] - GetRow(i)) : abs(king[1] - GetCol(i));
			//���µ�ǰ��С�������ݶ����ϵ�1
			if (step < minStep1)
			{
				minStep1 = step;
				point1 = i;
			}
		}
		step = 0;//����������
	}

	//��ʿ�ӹ���ʱ
	int pickRowMin = pickArea[0], pickRowMax = pickArea[1], pickColMin = pickArea[2], pickColMax = pickArea[3];//ȡ��pickArea�ڵķ�Χ
	minPickStep = INFTY; //��ʿ�ߵ����͵����С���Ͳ�����ʼ��Ϊ�����
	//��pickArea����ѡ��������������ʿ
	for (int i = pickRowMin; i <= pickRowMax; i++)
		for (int j = pickColMin; j <= pickColMax; j++)
		{
			for (int k = 0; k < knightNum; k++)//��ѡ��ʿ
			{
				pickStep = Dijkstra(i * 8 + j, knight[k][0] * 8 + knight[k][1]);//�����ʿ�����͵�Ĳ���
				//������ʿ���Ͳ���������������±�
				if (pickStep < minPickStep)
				{
					minPickStep = pickStep;
					pickKnight = k;
					pickPoint = i * 8 + j;
				}
			}
		}
	int newKnight[63][2];//����һ��knight���飬�������������λ��
	memcpy(newKnight, knight, sizeof(knight));
	newKnight[pickKnight][0] = GetRow(pickPoint); newKnight[pickKnight][1] = GetCol(pickPoint);
	//Ѱ�Ҽ��ϵ㣨2��
	for (int i = 0; i < 64; i++)
	{
		//�жϸõ��Ƿ��ڼ�֦�����޶��ķ�Χ�ڣ����ǣ���������㣬����������ѯ��һ����
		if (GetRow(i) > rowMin && GetRow(i) < rowMax && GetCol(i) > colMin && GetCol(i) < colMax)
		{
			for (int j = 0; j < knightNum; j++)
				step += Dijkstra(i, newKnight[j][0] * 8 + newKnight[j][1]);//�õϽ�˹�����㷨�������ʿ��������Ĳ���
			//���µ�ǰ��С�������ݶ����ϵ�1
			if (step < minStep1)
			{
				minStep2 = step;
				point2 = i;
			}
		}
		step = 0;//����������
	}
	//�ܲ���������� �����ƶ������͵�Ĳ���
	minStep2 += abs(king[0] - GetRow(pickPoint)) > abs(king[1] - GetCol(pickPoint)) ? abs(king[0] - GetRow(pickPoint)) : abs(king[1] - GetCol(pickPoint));

	//ȡ�������ַ�����ò����Ľ�С�ߣ�����assemblyPoint��assemblyStep
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

//�������
void Chess::Output()
{
	/*for (int i = 0; i < knightNum; i++)
		cout << "��ʿ" << i+1 << "��������" << knight[i][0] << ", " << knight[i][1] << endl;
	cout << "����" << king[0] << ", " << king[1] << endl;*/
	cout << "��С����Ϊ��" << assemblyStep << "����" << endl;
	cout << "���ϵ�Ϊ����" << assemblyPoint[0] << "��" << assemblyPoint[1] << "����" << endl;
	cout << "������ɡ�" << endl;
}

int main()
{
	Chess c;

	c.SetKing();
	c.SetKnight();
	c.NarrowTheRange();
	c.FindAssemblyPoint();
	c.Output();
	getchar();
	getchar();
	return 0;
}