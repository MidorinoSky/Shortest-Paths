#include "stdio.h"
#include<stdlib.h>
#include<vector>
#include <iostream>
#include<string>
#include<map>
#include<conio.h>
#pragma warning(disable:4996)

using namespace std;

#define M 69866  	//无向图文件的行数
#define L 3			//用于读取数据表的三列
#define Z 800		//图的点数
#define INF 32767   //看作无限值

int dist[Z];		//每个点与源点的距离
int dist2[Z];		//次短路每个点与源点的距离
int G[Z][Z];		//邻接矩阵
int point_num = Z;

bool hasPath[Z];		//hasPath[i]为true时表示确定有最短路径通往节点i
bool has2ndPath[Z];

vector<int> P[Z];	//若P[3]包含1，则至少存在一条最短路，使得经过1才能到达3。
vector<int> P2[Z];

int v_start;
int v_end;

void Dijkstra(int v0) {//对源点用Dijkstra算法求最短路
	int a, b;
	for (a = 0; a < Z; a++) {
		dist[a] = G[v0][a];
		P[a].clear();
		P[a].push_back(v0); //v0能直接到达a，则v0加入P[a]中
		P2[a].clear();
		P2[a].push_back(v0);
		hasPath[a] = false;
		has2ndPath[a] = false;
	}

	hasPath[v0] = true; has2ndPath[v0] = true;
	dist[v0] = 0;     dist2[v0] = 0;
	P[v0].clear();    P2[v0].clear();

	for (int i = 0; i < Z; i++) {
		int min = INF;
		for (b = 0; b < Z; b++) {
			if (hasPath[b] == false && dist[b] < min) {
				a = b;
				dist2[b] = min;
				min = dist[b]; //min为当前未访问的最近节点的距离
			}
		}
		hasPath[a] = true; //当前最近结点加入集合。
		has2ndPath[a] = true;
		for (b = 0; b < Z; b++) {
			if (hasPath[b] == false && min + G[a][b] < dist[b]) {// 更新最短路径
				dist2[b] = dist[b]; //同时更新次短路            
				dist[b] = min + G[a][b];
				P[b].clear();
				P[b].push_back(a);
				P2[b].clear();
				P2[b].push_back(a);
			}
			else if (hasPath[b] == false && min + G[a][b] == dist[b]) {
				dist[b] = min + G[a][b]; //更新最短路
				P[b].push_back(a);
			}

			if (has2ndPath[b] == false && (min + G[a][b] > dist[b] && min + G[a][b] < dist2[b])) {
				dist2[b] = min + G[a][b];  //更新次短路
				P2[b].clear();
				P2[b].push_back(a);
			}
			else if (has2ndPath[b] == false && (min + G[a][b] > dist[b] && min + G[a][b] == dist2[b])) {
				dist2[b] = min + G[a][b];
				P2[b].push_back(a);
			}
		}
	}
}

int getPathCount(int v0, int v1) { //最短路条数
	if (v1 == v0)
		return 1;
	int num = 0;
	for (int i = 0; i < P[v1].size(); ++i)
		num += getPathCount(v0, P[v1][i]);
	return num;
}

int get2ndPathCount(int v0, int v1) { //次短路条数
	if (v1 == v0)
		return 1;
	int num = 0;
	for (int i = 0; i < P2[v1].size(); ++i)
		num += get2ndPathCount(v0, P2[v1][i]);
	return num;
}

int flag = 0, flag2 = 0;

void printSinglePath(int v0, int v1, FILE* fp) { //输出两点之间的最短路
	if (v1 == v0) {
		return;
	}
	for (int i = 0; i < P[v1].size(); ++i) {
		if (P[v1][i] == v0 && flag != 0) {
			fprintf(fp, "%d\n", P[v1][i]);
			flag = 0;
		}
		else if (P[v1][i] == v0 && flag == 0) {
			fprintf(fp, "  %d - ", v_start);
			fprintf(fp, "%d\n", P[v1][i]);
			flag = 0;
		}
		else {
			if (flag == 0) {
				fprintf(fp, "  %d - ", v_start);
				flag++;
			}
			fprintf(fp, "%d - ", P[v1][i]);
		}
		printSinglePath(v0, P[v1][i], fp);
	}
}

void printSinglePath2nd(int v0, int v1, FILE* fp) { //输出两点之间的次短路
	if (v1 == v0) {
		return;
	}
	for (int i = 0; i < P2[v1].size(); ++i) {
		if (P2[v1][i] == v0 && flag != 0) {
			fprintf(fp, "%d\n", P2[v1][i]);
			flag = 0;
		}
		else if (P2[v1][i] == v0 && flag == 0) {
			fprintf(fp, "  %d - ", v_start);
			fprintf(fp, "%d\n", P2[v1][i]);
			flag = 0;
		}
		else {
			if (flag == 0) {
				fprintf(fp, "  %d - ", v_start);
				flag++;
			}
			fprintf(fp, "%d - ", P2[v1][i]);
		}
		printSinglePath2nd(v0, P2[v1][i], fp);
	}
}

void printPathResult(int start, int end, FILE* fp) {
	flag = 0;
	flag2 = 0;
	Dijkstra(end);
	fprintf(fp, "\nv%d - v%d:\n", start, end);
	fprintf(fp, "最短路条数: %d\n", getPathCount(end, start));
	fprintf(fp, "最短路有:\n");
	printSinglePath(end, start, fp);
	fprintf(fp, "最短路长度: %d km\n\n", dist[start]);

	if (dist2[start] < INF) {
		fprintf(fp, "次短路条数: %d\n", get2ndPathCount(end, start));
		fprintf(fp, "次短路有:\n");
		printSinglePath2nd(end, start, fp);
		fprintf(fp, "次短路长度: %d km\n", dist2[start]);
	}
	else {
		fprintf(fp, "无次短路\n");
	}

	fprintf(fp, "\n---------------------------------\n");
}

/*vector<string> V;
// 函数功能：打印并保存 v0->o 的所有最短路径
// cur字符串作递归辅助用，首次调用务必使cur为空串
void Print_Path(int start, int end, string cur = "") {
	cur += end + '0';
	if (end == start) { //退出递归的条件：起点==终点，输出路径，保存至V数组
		reverse(cur.begin(), cur.end());
		cout << cur << endl;
		V.push_back(cur);
		return;
	}
	for (int i = 0; i < P[end].size(); i++)
		Print_Path(start, P[end][i], cur);
}
*/

void menu() {
	printf("\n----------------------------------------------\n");
	printf("|                                            |\n");
	printf("|       最短路径工具 v1.4  by Midoric        |\n");
	printf("|                                            |\n");
	printf("---------------------功能---------------------\n");
	printf("|                                            |\n");
	printf("|  1.统计任意两点间的最短路径条数            |\n");
	printf("|  2.统计任意两点间的次短路径条数            |\n");
	printf("|  3.统计任意两点间的最短路长度和次短路长度  |\n");
	printf("|  4.计算一点到其他所有点最短路径和次短路径  |\n");
	printf("|  5.计算指定两点间的最短路径和次短路径      |\n");
	printf("|                                            |");
	printf("\n----------------------------------------------\n");
	printf("\n此程序功能用于计算无向图的最短路径。\n用例文档Graph.txt为含有%d个点的无向连通图，距离单位假设为km。\n", point_num);
	printf("\n输入序号进入功能：");
}

void pause() {
	printf("\n按Enter键继续\n");
	system("notepad.exe Result.txt");
	//D:\\Documents\\Coding\\ShortestPath\\800points\\Result.txt
	getchar();
	getchar();
}

void initGraph() { //初始化无向图
	int i, j;
	for (i = 0; i < Z; i++) {
		dist[i] = INF;
		dist2[i] = INF;
		for (j = 0; j < Z; j++) {
			G[i][j] = INF;
		}
	}
	FILE* fp;
	if ((fp = fopen("Graph_800.txt", "rt")) == NULL) {
		printf("Cannot open this file!\n");
		exit(0);
	}

	int arr[M][L];
	for (i = 0; i < M; i++) {
		for (j = 0; j < L; j++)
			fscanf(fp, "%d", &arr[i][j]); //将文件的数据存入数组
		fscanf(fp, "\n");
	}
	fclose(fp);

	int x, y;
	for (i = 0; i < M; i++) {
		x = arr[i][0];
		y = arr[i][1];
		G[x][y] = arr[i][2]; //存储成邻接矩阵
	}
}

void selection() {
	int i, start, end, select, mincount, count, mindist, mindist2;
	bool hasResult;
	scanf("%d", &select);
	hasResult = false;
	if (select == 1) {
		system("cls");
		printf("| 统计任意两点之间的最短路径条数 |\n\n设置最少的最短路径条数，即只显示大于等于这个条数的结果：");
		scanf("%d", &mincount);
		printf("\n\n运行中......\n");

		FILE* fp;
		if ((fp = fopen("Result.txt", "wt")) == NULL) {
			printf("Cannot open this file!\n");
			exit(0);
		}
		for (start = 0; start < Z; start++) {
			Dijkstra(start);
			for (end = 0; end < Z; end++) {
				if (end != start) {
					count = getPathCount(start, end);
					if (count >= mincount) {
						hasResult = true;
						fprintf(fp, "\nv%d - v%d:", start, end);
						fprintf(fp, "\n最短路条数: %d\n", count);
					}
				}
			}
		}
		if (!hasResult)
			printf("\n无结果\n");
		else
			printf("\n运行完毕，可在Result.txt中查看结果");
		fclose(fp);
		pause();
	}
	else if (select == 2) {
		system("cls");
		printf("| 统计任意两点之间的次短路径条数 |\n\n设置最少的次短路径条数，即只显示大于等于这个条数的结果：");
		scanf("%d", &mincount);
		printf("\n\n运行中......\n");

		FILE* fp;
		if ((fp = fopen("Result.txt", "wt")) == NULL) {
			printf("Cannot open this file!\n");
			exit(0);
		}
		for (start = 0; start < Z; start++) {
			Dijkstra(start);
			for (end = 0; end < Z; end++) {
				if (end != start) {
					count = get2ndPathCount(start, end);
					if (count >= mincount) {
						hasResult = true;
						fprintf(fp, "\nv%d - v%d:", start, end);
						fprintf(fp, "\n次短路条数: %d\n", count);
					}
				}
			}
		}
		if (!hasResult)
			printf("\n无结果\n");
		else
			printf("\n运行完毕，可在Result.txt中查看结果");
		fclose(fp);
		pause();
	}
	else if (select == 3) {
		system("cls");
		printf("| 统计任意两点间最短路径长度和次短路径长度 |\n\n设置最短路最小值：");
		scanf("%d", &mindist);
		printf("\n设置次短路最小值：");
		scanf("%d", &mindist2);
		printf("\n\n运行中......\n");

		FILE* fp;
		if ((fp = fopen("Result.txt", "wt")) == NULL) {
			printf("Cannot open this file!\n");
			exit(0);
		}
		for (start = 0; start < Z; start++) {
			Dijkstra(start);
			for (end = 0; end < Z; end++) {
				if (end != start && mindist <= dist[end] && mindist2 <= dist2[end] && dist2[end] < INF) {
					fprintf(fp, "\nv%d - v%d:\n", start, end);
					fprintf(fp, "最短路长度: %d km\n", dist[end]);
					fprintf(fp, "次短路长度: %d km\n", dist2[end]);
					hasResult = true;
				}
			}
		}
		if (!hasResult)
			printf("\n无结果\n");
		else
			printf("\n运行完毕，可在Result.txt中查看结果");
		fclose(fp);
		pause();
	}
	else if (select == 4) {
		system("cls");
		printf("| 计算一点到其他所有点最短路径和次短路径 |\n");
		printf("输入起始点：（0-%d）,之后计算该点到其他所有点的最短路径及次短路径\n"
			, point_num - 1);
		scanf("%d", &start);
		printf("\n\n运行中......\n");

		FILE* fp;
		if ((fp = fopen("Result.txt", "wt")) == NULL) {
			printf("Cannot open this file!\n");
			exit(0);
		}
		for (i = 0; i < Z; i++) {
			if (i != start) {
				end = i;
				v_start = start;
				printPathResult(start, end, fp);
			}
		}
		fclose(fp);
		printf("\n运行完毕，可在Result.txt中查看结果");
		pause();
	}
	else if (select == 5) {
		system("cls");
		printf("| 计算指定两点间的最短路径和次短路径 |\n");
		printf("输入起始点：（0-%d）\n", point_num - 1);
		scanf("%d", &start);
		v_start = start;
		printf("输入终点：（0-%d）\n", point_num - 1);
		scanf("%d", &end);
		v_end = end;
		printf("\n\n运行中......\n");

		FILE* fp;
		if ((fp = fopen("Result.txt", "wt")) == NULL) {
			printf("Cannot open this file!\n");
			exit(0);
		}
		printPathResult(start, end, fp);

		fclose(fp);
		printf("\n运行完毕，可在Result.txt中查看结果");
		pause();
	}
	else {
		return;
	}
}

int main()
{
	while (1) {
		system("cls");
		initGraph();
		menu();
		selection();
	}
	return 0;
}