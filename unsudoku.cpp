#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <map>
#include <chrono>

using std::cout;
using std::string;
using std::vector;
using std::map;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;

/**
 * 这是一个数独游戏的解算程序。
 *
 * 用法：在dimTpl数组中定义好待解算的数独矩阵，0表示空白；
 * 然后定义好数独的X/Y维度、每个空白的最大数值、每个宫的维度
 *
 * DIM_X: 数独矩阵横向的维度
 * DIM_Y：数独矩阵纵向的维度
 * DIM_N_MAX：数独矩阵每个空白中可填写的最大数字
 * SUB_DIM_X：每个“宫”的横向维度
 * SUB_DIM_Y：每个“宫”的纵向维度
 *
 *
 * 算法概要：
 *
 * <阶段一>
 * 先搜索数独矩阵中，所有“有唯一解”的空白单元格。然后将“唯一解”的空白单元格填回到数独矩阵中，
 * 然后再次重新搜索有“有唯一解”的空白单元格，以此类推，直到找到所有的解。
 *
 * 对于一般难度的数独矩阵，用此方法就可以解开了。
 *
 * 对于较高难度的数独矩阵，用此方法解不开时，则进入到下一阶段，
 *
 * <阶段二>
 * 此阶段基本上是“穷举法”。 把每一个单元格可填写的数字，逐一枚举一遍，直到找到正确的解。
 *
 * 这里使用的是简单的“直接穷举”，理论上可以进一步优化，例如：
 * 将“解”最少的单元格作为最后穷举的对象，而将“解”最多的单元格，作为最先穷举的对象。
 * 这样可以加快穷举的速度。（其实人脑在解题时，应该也是这个思路）。
 *
 * 这里并没有对穷举法进行进一步的优化，因为，即便是那些号称“骨灰级”难度的数独矩阵，辅以最烂的穷举算法，
 * 在现代的计算机面前，也是渣渣----解开它只需要不到30秒。
 *
 * 如果是优化后的穷举法，猜测“骨灰级难度”的数独矩阵，解算时间或许只需要10秒。
 *
 * 第二阶段算法，如果你有更好的想法，欢迎与我交流：48746190@qq.com
 */

/*
 //这是一个4x4数独游戏的例子，宫的维度是2x2
 const int DIM_X = 4, DIM_Y = 4, DIM_N_MAX = 4, SUB_DIM_X = 2, SUB_DIM_Y = 2;
 int dimTpl[DIM_X][DIM_Y] = { { 2, 0, 0, 3, }, { 3, 0, 2, 1 }, { 0, 2, 3, 0 }, { 0, 0, 0, 2 }, };
 */

/*
 //这是一个6x6数独游戏的例子，宫的维度是3x2
 const int DIM_X = 6, DIM_Y = 6, DIM_N_MAX = 6, SUB_DIM_X = 3, SUB_DIM_Y = 2;
 int dimTpl[DIM_X][DIM_Y] = { { 5, 0, 6, 0, 3, 0, }, { 2, 0, 1, 0, 0, 0, }, { 0, 0, 0, 2, 0, 3, }, { 3, 0, 4, 0, 6, 0, },
 { 4, 0, 0, 6, 0, 5, }, { 0, 5, 0, 3, 0, 4, }, };*/

//这是这是一个9x9数独游戏的例子，宫的维度是3x3。这个矩阵属于“一般”难度。
const int DIM_X = 9, DIM_Y = 9, DIM_N_MAX = 9, SUB_DIM_X = 3, SUB_DIM_Y = 3;
int dimTpl[DIM_X][DIM_Y] = { { 0, 0, 1, 0, 0, 4, 0, 0, 0 }, { 0, 0, 7, 0, 0, 9, 0, 0, 0 }, { 0, 0, 6, 0, 7, 0, 1, 5, 8 }, { 7, 5, 0, 9, 0,
		0, 0, 0, 0 }, { 0, 0, 9, 0, 2, 0, 5, 0, 0 }, { 0, 0, 0, 0, 0, 1, 0, 9, 6 }, { 1, 3, 5, 0, 9, 0, 6, 0, 0 }, { 0, 0, 0, 8, 0, 0, 4, 0,
		0 }, { 0, 0, 0, 3, 0, 0, 2, 0, 0 }, };

/*//这是一个9x9数独游戏的例子，宫的维度是3x3。（网上搜的号称“骨灰级”难度的数独矩阵）
 const int DIM_X = 9, DIM_Y = 9, DIM_N_MAX = 9, SUB_DIM_X = 3, SUB_DIM_Y = 3;
 int dimTpl[DIM_X][DIM_Y] = { { 0, 3, 0, 0, 4, 5, 0, 0, 0 }, { 8, 2, 0, 0, 0, 0, 0, 0, 9 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 7, 0, 0, 8, 0,
 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 3, 0, 0 }, { 0, 0, 4, 0, 0, 0, 6, 5, 0 }, { 0, 0, 0, 2, 0, 0, 0, 0, 8 }, { 1, 0, 0, 0, 0, 0, 0, 0,
 0 }, { 0, 0, 5, 0, 0, 3, 0, 0, 0 }, };*/

shared_ptr<vector<int>> genV()
{
	auto v = make_shared<vector<int>>();
	for (int n = 0; n <= DIM_N_MAX; n++)
	{
		v->push_back(n);
	}
	return v;
}

void reallocV(shared_ptr<vector<int>> v)
{
	for (auto iter = v->begin(); iter != v->end();)
	{
		if (*iter == 0)
			iter = v->erase(iter);
		else
			iter++;
	}
}

void printV(shared_ptr<vector<int>> v)
{
	for (size_t n = 0; n < v->size(); n++)
	{
		cout << v->at(n) << " ";
	}
	cout << "\n" << std::flush;
}

void calcX(int dy, shared_ptr<vector<int>> v, int dim[][DIM_X])
{
	for (int x = 0; x < DIM_X; x++)
	{
		if (dim[dy][x] > 0)
			v->at(dim[dy][x]) = 0;
	}
}

void calcY(int dx, shared_ptr<vector<int>> v, int dim[][DIM_X])
{
	for (int y = 0; y < DIM_Y; y++)
	{
		if (dim[y][dx] > 0)
			v->at(dim[y][dx]) = 0;
	}
}

/**
 * 计算 dx,dy 所指定的数独矩阵单元格，隶属于哪个“宫”
 */
void calcRScope(int dx, int dy, int &scpX, int &scpY)
{
	scpX = (std::ceil((float) (dx + 1) / SUB_DIM_X) - 1) * SUB_DIM_X;
	scpY = (std::ceil((float) (dy + 1) / SUB_DIM_Y) - 1) * SUB_DIM_Y;
}

void calcR(int dx, int dy, shared_ptr<vector<int>> v, int dim[][DIM_X])
{
	int scpX, scpY;

	calcRScope(dx, dy, scpX, scpY); //计算单元格所属的“宫”

	for (int y = scpY; y < (scpY + SUB_DIM_Y); y++)
	{
		for (int x = scpX; x < (scpX + SUB_DIM_X); x++)
		{
			if (dim[y][x] > 0)
			{
				v->at(dim[y][x]) = 0;
			}
		}
	}
}

/**
 * 计算 dx,dy 指定的数独矩阵单元格，此单元格可以填写的有效的数字列表
 * 函数将dx,dy指定的单元格可填写的数字列表，保存于v变量中。
 *
 * 通过计算 横坐标、纵坐标、宫 三者的最小并集，确定dx,dy单元格可填写的有效数字集合。
 */
void calcXYR(int dx, int dy, shared_ptr<vector<int>> v, int dim[][DIM_X])
{
	calcX(dy, v, dim); //计算横向坐标，可填写的有效数字
	calcY(dx, v, dim); //计算纵向坐标，可填写的有效数字
	calcR(dx, dy, v, dim); //计算所属“宫”的，可填写的有效数字
}

#define KEY(x,y) ((x + 1) * 10 + y + 1)

/**
 * 计算每一个空白单元格的可填写的有效数字集合，并汇总到f变量中保存
 * 注：这个方法，只用于数据分析，对解算没有意义。
 */
shared_ptr<map<int, shared_ptr<vector<int>>>> totalUnfilled(int dim[][DIM_X])
{
	shared_ptr<map<int, shared_ptr<vector<int>>>> f = make_shared<map<int, shared_ptr<vector<int>>>>();

	for (int y = 0; y < DIM_Y; y++)
	{
		for (int x = 0; x < DIM_X; x++)
		{
			if (dim[y][x] == 0)
			{
				std::pair<int, shared_ptr<vector<int>>> kv(KEY(x, y), make_shared<vector<int>>());
				f->insert(kv);
			}
		}
	}
	return f;
}

/**
 * 打印指定的数独矩阵
 */
void printDim(int dim[][DIM_X], int ax = -1, int ay = -1)
{
//打印结果
	for (int y = 0; y <= DIM_Y; y++)
	{
		for (int x = 0; x <= DIM_X; x++)
		{
			if (x == 0 && y == 0)
			{
				cout << "  +  ";
				continue;
			}
			if (y == 0)
			{
				cout << "(x" << x << ") ";
				continue;
			}

			if (x == 0)
			{
				cout << "(y" << y << ")";
				continue;
			}

			int di = dim[y - 1][x - 1];
			if (ax >= 0 && ay >= 0 && ax == x && ay == y)
				cout << " [" << (di == 0 ? "*" : std::to_string(di)) << "] ";
			else
				cout << "  " << (di == 0 ? "*" : std::to_string(di)) << "  ";
		}
		cout << "\n";
	}
	cout << "\n" << std::flush;
}

bool testX(int dim[][DIM_X])
{
	for (int y = 0; y < DIM_Y; y++)
	{
		int t[DIM_X + 1] = { 0 };
		for (int x = 0; x < DIM_X; x++)
		{
			if (dim[y][x] > 0 && t[dim[y][x]] == 0)
			{
				t[dim[y][x]] += 1;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool testY(int dim[][DIM_X])
{
	for (int x = 0; x < DIM_X; x++)
	{
		int t[DIM_Y + 1] = { 0 };
		for (int y = 0; y < DIM_Y; y++)
		{
			if (dim[y][x] > 0 && t[dim[y][x]] == 0)
			{
				t[dim[y][x]] += 1;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool testR(int sdx, int sdy, int dim[][DIM_X])
{
	for (int sy = 0; sy < DIM_Y; sy += sdy)
	{
		for (int sx = 0; sx < DIM_X; sx += sdx)
		{
			int t[(sdx * sdy) + 1] = { 0 };

			for (int y = sy; y < (sy + sdy); y++)
			{
				for (int x = sx; x < (sx + sdx); x++)
				{
					if (dim[y][x] > 0 && t[dim[y][x]] == 0)
					{
						t[dim[y][x]] += 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

/**
 * 测算指定的数独矩阵，是不是有一个有效的解。
 *
 * 如果当前数独矩阵为有效解，则返回true
 */

inline bool testXYR(int sdx, int sdy, int dim[][DIM_X])
{
	return testX(dim) && testY(dim) && testR(sdx, sdy, dim);
}

int main()
{
	cout << "初始数独矩阵：" << "\n" << std::flush;

	int dimVal[DIM_Y][DIM_X];
	std::copy(&dimTpl[0][0], &dimTpl[0][0] + DIM_X * DIM_Y, &dimVal[0][0]);

	printDim(dimVal);

	auto uf = totalUnfilled(dimVal);
	cout << "当前数独矩阵中共有" << uf->size() << "个空白，需要计算。\n\n"
			"每个空白单元格，可填写的合法数据，分析如下：\n" << std::flush;

//分析出矩阵中每一个“空白”，允许填写哪些数字
	shared_ptr<map<int, shared_ptr<vector<int>>>> mapUniq = make_shared<map<int, shared_ptr<vector<int>>>>();
	bool hasUniq = true;

	int uCnt = 0;
	while (true)
	{
		cout << "\n正在进行第 " << ++uCnt << " 轮唯一解分析..\n" << std::flush;

		for (int y = 0; y < DIM_Y; y++)
		{
			for (int x = 0; x < DIM_X; x++)
			{
				if (dimTpl[y][x] == 0)
				{
					auto v = genV();
					calcXYR(x, y, v, dimTpl);

					reallocV(v);

					int key = KEY(x, y);

					if (v->size() == 1)
					{
						std::pair<int, shared_ptr<vector<int>>> kv(key, make_shared<vector<int>>());
						mapUniq->insert(kv);
						mapUniq->at(key)->push_back(v->front());
					}

					uf->at(key)->assign(v->begin(), v->end());

					cout << "x" << (x + 1) << ",y" << (y + 1) << " 的解为=>  ";
					printV(uf->at(key));
				}
			}
		}

		if (mapUniq->size() > 0)
		{
			cout << "\n本轮共找到" << mapUniq->size() << "个，有唯一解的单元格，分别是：\n" << std::flush;

			for (auto iter = mapUniq->begin(); iter != mapUniq->end(); iter++)
			{
				int key = iter->first;
				int val = iter->second->front();
				int tx = key / 10 - 1;
				int ty = (key % 10) - 1;
				dimTpl[ty][tx] = val;
				cout << "x" << tx << ",y" << ty << " 解为=>  " << val << "\n" << std::flush;
			}
			mapUniq->clear();

			cout << "\n将所有唯一解的单元格，填写回数据矩阵后，结果为：\n" << std::flush;
			printDim(dimTpl);
		}
		else
			break;
	}

	if (testXYR(SUB_DIM_X, SUB_DIM_Y, dimTpl))
	{
		cout << "数独矩阵求解完成。";
		return 0;
	}

	bool dir = true;
	int pos = 0, calcCnt = 0, cc = 0;

	cout << "按 回车键 开始计算...\n" << std::flush;
	std::cin.get();

	auto startTime = std::chrono::high_resolution_clock::now();

	while (testXYR(SUB_DIM_X, SUB_DIM_Y, dimVal) == false)
	{
		if (pos == DIM_X * DIM_Y)
		{
			pos -= 1;
			dir = false;
		}

		if (*((&dimTpl[0][0]) + pos) == 0) //空白
		{
			int x = 0, y = 0;
			if (pos > 0)
			{
				x = pos % DIM_X;
				y = std::floor((float) pos / DIM_X);
			}

			int bk = *((&dimVal[0][0]) + pos);
			*((&dimVal[0][0]) + pos) = 0;

			auto v = genV();
			calcXYR(x, y, v, dimVal);
			reallocV(v);

			*((&dimVal[0][0]) + pos) = bk;

			if (v->size() <= 0)
			{
				dir = false;
			}
			else
			{
				int iv = *((&dimVal[0][0]) + pos);
				if (iv == 0)
				{
					*((&dimVal[0][0]) + pos) = v->front();
					dir = true;
				}
				else
				{
					dir = false;
					auto ie = std::find(v->begin(), v->end(), iv);

					if (ie != v->end())
					{
						if (++ie != v->end())
						{
							dir = true;
							*((&dimVal[0][0]) + pos) = *ie;
						}
					}
				}
			}

			if (dir == false)
			{
				*((&dimVal[0][0]) + pos) = 0;
			}

			if (++calcCnt % 99999 == 0)
			{
				//cout << "cnt:" << calcCnt << "\n" << std::flush;
				//printDim(dimVal, x, y);
				cout << "." << std::flush;
			}

			if (cc-- <= 0 || (pos + 1) == DIM_X * DIM_Y)
			{
				cout << "第" << calcCnt << "步  " << (dir ? "[->]" : "[<-]") << "  单元格:(xy" << KEY(x, y) << ") 可填写数字 => ";
				printV(v);
				printDim(dimVal, x, y);

				cout << "按 回车键 执行下一步...\n" << std::flush;
				std::cin.get();

				//cout << "\n输入继续执行的步数：" << std::flush;
				//std::cin >> cc;
			}
		}

		pos += (dir ? 1 : -1);

	}

	cout << "本次计算共执行了 " << calcCnt << " 步\n当前数独矩阵的解是：\n";
	printDim(dimVal);

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;

	std::cout << "计算共花费了 " << fp_ms.count() / 1000 << " 秒" << std::endl;

	return 0;
}
