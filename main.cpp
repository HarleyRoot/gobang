#include<graphics.h>
#include<string.h>
#include<stdio.h>
#include<conio.h>
#include<iostream>
#include<math.h>
#include<fstream>
#define MAXIMUS 13
#define ll long long
using namespace std;
IMAGE start;
IMAGE bgp;
IMAGE ep;
int num = -1;//-1表示黑子下，1表示白子下
int piece[13][13];
ll weight[13][13];
int mode;
int Mx;
int My;
int a;
int temp;
int counts;
int FileNum = 1;
char back[5] = ".txt";

void DrawPiece(int m, int n);
void PutBackground();
void DrawLine();
void DrawPoint();
int ChangePiece(int x, int y);
void explain();
void Game();
void InitPiece();
void DrawMap();
void Welcome();
void CountWeight();
void FindMaxWeight();
void InitWeight();
void Count(int Cx, int Cy);
int Check(int Cx, int Cy);
void EVEGame();
int Judge(int x, int y);
int ChooseFile(int m,int n);
void OpenFile(int n);

struct Pie {
	int x;
	int y;
};
struct Pie chess[1000];
void Stay(){	
	char whiteName[10];
	if (FileNum < 10) {
		whiteName[0] = '0';
		whiteName[1] = FileNum + '0';
	}
	else {
		whiteName[0] = FileNum / 10+'0';
		whiteName[1] = FileNum % 10+'0';
	}
	whiteName[2] = '\0';
	strcat_s(whiteName, back);
	ofstream fout(whiteName);
	int i;
	fout << temp << "\t";
	for (i = 0; i < temp; i++) {
		fout << chess[i].x << " " << chess[i].y << " \t" ;
	}
	fout.close();
	FileNum++;
}
//对局回放函数
void Show() {
	initgraph(390, 390);
	loadimage(&bgp, _T("images\\棋盘.png"));//加载图片
	PutBackground();//输出背景图片
	settextcolor(WHITE);
	settextstyle(20, 10, _T("宋体"));
	setbkmode(TRANSPARENT);
	outtextxy(30, 10,_T("鼠标点击选择回放的对局"));
	int col = 30;
	for (int i = 0; i < FileNum-1; i++) {
		if (i % 8 == 0)col += 30;
		TCHAR Name[5];
		_stprintf_s(Name, _T("%d"), i+1);
		settextcolor(WHITE);
		settextstyle(20, 10, _T("宋体"));
		setbkmode(TRANSPARENT);
		outtextxy( 40 * (i % 8 +1), col, Name);
	}
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();
		int File;
		if (m.uMsg == WM_LBUTTONDOWN) {
			File=ChooseFile(m.x,m.y);
			if (!File) continue;
			else OpenFile(File);
		}
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
}
//打开所选择的文件
void OpenFile(int n) {
	DrawMap();
	char readName[10];
	if (n < 10) {
		readName[0] = '0';
		readName[1] = n + '0';
	}
	else {
		readName[0] = n / 10 + '0';
		readName[1] = n % 10 + '0';
	}
	readName[2] = '\0';
	strcat_s(readName, back);
	ifstream fin(readName);
	int i = 0;
	int tempx;
	fin >> tempx;
	while (i<tempx){
		struct Pie p;
		fin >> p.x >> p.y;
		DrawPiece(30 * p.x, 30 * p.y);
		Judge(30 * p.x, 30 * p.y);
		Sleep(500);
		i++;
	}
	fin.close();
}
//判断游戏是否结束
int Judge(int x, int y) {
	int ans=0;//初始化答案
	//如果下棋次数为169,则平局
	if (counts == 169) {
		ans = MessageBox(NULL, TEXT("游戏结束 平局\n是否返回主界面"), TEXT("提示"), MB_YESNO);
		//保存对局
		if(mode!=5)Stay();
		if (ans == IDYES) Welcome();
		else if (ans == IDNO)MessageBox(NULL, TEXT("鼠标右键返回主界面"), TEXT("提示"), MB_OK);
		return 1;
	}
	//未平局,判断是否有一方获胜
	if (Check(x / 30, y / 30)) {
		 if (num == -1) ans=MessageBox(NULL, TEXT("游戏结束 白方获胜\n是否返回主界面"),TEXT("提示"), MB_YESNO);
		else if (num == 1) ans=MessageBox(NULL, TEXT("游戏结束 黑方获胜\n是否返回主界面"), TEXT("提示"), MB_YESNO);
		//保存对局
		if(mode!=5)Stay();
		if (ans == IDYES) Welcome();
		else if (ans == IDNO)MessageBox(NULL, TEXT("鼠标右键返回主界面"), TEXT("提示"), MB_OK);
		return 1;
	}
}
//机器下棋函数
void AI() {
	MOUSEMSG m;
	//初始化权重
	InitWeight();
	num *= -1;
	//计算权重
	CountWeight();
	num *= -1;
	//计算权重
	CountWeight();
	//寻找最大权重
	FindMaxWeight();
	//落下权重最大的棋子
	DrawPiece(30 * Mx, 30 * My);
}
//弹出窗口,选择开始游戏
void Countdown() {
	DrawMap();
	MessageBox(NULL, _T("点击确定开始游戏"), _T("提示"), MB_OK);
	DrawMap();
}
//对每个位置计算权重
void CountWeight() {
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++) {
			if (piece[i][j] == 0)
				Count(i, j);//计算该位置权重
		}
}
//计算权重
void Count(int Cx, int Cy) {
	int w = 1, x = 1, y = 1, z = 1, i,j;//初始化
	for (i = 1; i < 5; i++) {
		if (Cy + i < MAXIMUS && piece[Cx][Cy + i] == -1 * num) w++;
		else break;//向下检查
	}
	for (j = 1; j < 5; j++) {
		if (Cy - j >= 0 && piece[Cx][Cy - j] == -1 * num)	w++;
		else break;//向上检查
	}
	//如果连成四字
	if (w >= 5)w*=2;
	//如果为死棋,数量值减半
	if (Cy + i == MAXIMUS || Cy - j == -1 || piece[Cx][Cy + i] == num || piece[Cx][Cy - j] == num)w/=2;
	//赋值
	weight[Cx][Cy] += (ll)pow(10, w);
	for (i = 1; i < 5; i++) {
		if (Cx + i < MAXIMUS && piece[Cx + i][Cy] == -1 * num)	x++;
		else break;//向右检查
	}
	for (j = 1; j < 5; j++) {
		if (Cx - j >= 0 && piece[Cx - j][Cy] == -1 * num)	x++;
		else break;//向左检查
	}
	if (x >= 5)x *=2;
	if (Cx + i == MAXIMUS || Cx - j == -1 || piece[Cx + i][Cy] == num || piece[Cx-j][Cy] == num)x/=2;
	weight[Cx][Cy] += (ll)pow(10, x);
	for (i = 1; i < 5; i++) {
		if (Cx + i < MAXIMUS && Cy + i < MAXIMUS && piece[Cx + i][Cy + i] == -1 * num)	y++;
		else break;//向右下检查
	}
	for (j = 1; j < 5; j++) {
		if (Cx - j >= 0 && Cy - j >= 0 && piece[Cx - j][Cy - j] == -1 * num)	y++;
		else break;//向左上检查
	}
	if (y >= 5)y *=2;
	if (Cx + i == MAXIMUS || Cy + i == MAXIMUS || Cx - j == -1 || Cy - j == -1 || piece[Cx + i][Cy + i] == num || piece[Cx - j][Cy - j] == -1 * num)y/=2;
	weight[Cx][Cy] += (ll)pow(10, y);
	for (i = 1; i < 5; i++) {
		if (Cx + i < MAXIMUS && Cy - i >= 0 && piece[Cx + i][Cy - i] == -1 * num)	z++;
		else break;//向右上检查
	}
	for (j = 1; j < 5; j++) {
		if (Cx - j >= 0 && Cy + j < MAXIMUS && piece[Cx - j][Cy + j] == -1 * num)	z++;
		else break;//向左下检查 
	}
	if (z >= 5)z *=2;
	if (Cx + i == MAXIMUS || Cy - i == -1 || Cx - j == -1 || Cy + j == MAXIMUS || piece[Cx + i][Cy - i] == num || piece[Cx - j][Cy + j] == num)z /=2;
	weight[Cx][Cy] += (ll)pow(10, z);
}
//判断是否有棋子
int ChangePiece(int x, int y) {
	if (piece[x][y] != 0) {
		return 0;
	}
	else {
		piece[x][y] = num;
		return num;
	}
}
//检查是否连成五子
int Check(int Cx, int Cy) {
	int w = 1, x = 1, y = 1, z = 1, i;//累计横竖正斜反邪四个方向的连续相同棋子数目
	for (i = 1; i < 5; i++)
		if (Cy + i < MAXIMUS && piece[Cx][Cy + i] == -1 * num)
			w++;
		else
			break;//向下检查
	for (i = 1; i < 5; i++)
		if (Cy - i >= 0 && piece[Cx][Cy - i] == -1 * num)
			w++;
		else
			break;//向上检查
	if (w >= 5)
		return 1;//若果达到5个则判断当前走子玩家为赢家
	for (i = 1; i < 5; i++)
		if (Cx + i < MAXIMUS && piece[Cx + i][Cy] == -1 * num)
			x++;
		else
			break;//向右检查
	for (i = 1; i < 5; i++)
		if (Cx - i > 0 && piece[Cx - i][Cy] == -1 * num)
			x++;
		else
			break;//向左检查
	if (x >= 5)
		return 1;//若果达到5个则判断当前走子玩家为赢家
	for (i = 1; i < 5; i++)
		if (Cx + i < MAXIMUS && Cy + i < MAXIMUS && piece[Cx + i][Cy + i] == -1 * num)
			y++;
		else
			break;//向右下检查
	for (i = 1; i < 5; i++)
		if (Cx - i > 0 && Cy - i > 0 && piece[Cx - i][Cy - i] == -1 * num)
			y++;
		else
			break;//向左上检查
	if (y >= 5)
		return 1;//若果达到5个则判断当前走子玩家为赢家
	for (i = 1; i < 5; i++)
		if (Cx + i < MAXIMUS && Cy - i>0 && piece[Cx + i][Cy - i] == -1 * num)
			z++;
		else
			break;//向右上检查
	for (i = 1; i < 5; i++)
		if (Cx - i > 0 && Cy + i < MAXIMUS && piece[Cx - i][Cy + i] == -1 * num)
			z++;
		else
			break;//向左下检查 if(z>=5)return Now;//若果达到5个则判断当前走子玩家为赢家
	if (z >= 5) {
		return 1;
	}
	return 0;//若没有检查到五连珠，则返回0表示还没有玩家达成胜利
}
//判断玩家选择的是哪个对局
int ChooseFile(int m, int n) {
	for (int i = 0; 8*i<FileNum-1; i++) {
		for (int j = 1; j <= 8; j++) {
			if (8 * i + j == FileNum) break;
			if (m > 40 * j && m < 40 * j + 10 && n>30 * i + 60 && n < 30 * i + 80) return 8 * i + j;
		}
	}
	return 0;
}
//通过鼠标位置而判断所选择的模式
int ChooseMode(int m, int n) {
	if (m >= 250 && m <= 305 && n > 211 && n <= 241) return 1;
	if (m >= 250 && m <= 305 && n > 241 && n <= 271) return 2;
	if (m >= 250 && m <= 305 && n > 271 && n <= 301) return 3;
	if (m >= 250 && m <= 357 && n > 301 && n <= 331) return 4;
	if (m >= 250 && m <= 357 && n > 331 && n <= 361) return 5;
	else return 0;//未点中模式则返回0
}
//画红点
void DrawSign(int x, int y) {
	setfillcolor(RED);
	fillcircle(30 * x + 15, 30 * y + 15, 2);
}
//画棋函数
void DrawPiece(int m, int n) {
	putimage(0, 0, &bgp);
	int x, y;
	x = m / 30;
	y = n / 30;
	if (mode != 5) {
		chess[temp].x = x;
		chess[temp].y = y;//记录坐标
		temp++;
	}
	a = ChangePiece(x, y);
	getimage(&bgp, 0, 0, 390, 390);
	if (a == -1) setfillcolor(BLACK);
	if (a == 1) setfillcolor(WHITE);
	if (a == 0) {
		int ans = MessageBox(NULL, TEXT("此位置已有棋子\n请重新选择"), TEXT("提示"), MB_OK);
		return;
	}
	counts++;
	fillcircle((x) * 30 + 15, (y) * 30 + 15, 13);
	getimage(&bgp, 0, 0, 390, 390);
	//画红点
	DrawSign(x, y);
	//变换棋子颜色
	num *= -1;
}
//画棋盘的线
void DrawLine() {
	setlinecolor(BLACK);
	for (int x = 15; x < 390; x += 30)
		line(x, 15, x, 375);
	for (int y = 15; y < 390; y += 30)
		line(15, y, 375, y);
}
//画出棋盘中的定位点
void DrawPoint() {
	setfillcolor(BLACK);
	fillcircle(4 * 30 - 15, 4 * 30 - 15, 3);
	fillcircle(4 * 30 - 15, 10 * 30 - 15, 3);
	fillcircle(10 * 30 - 15, 4 * 30 - 15, 3);
	fillcircle(10 * 30 - 15, 10 * 30 - 15, 3);
	fillcircle(7 * 30 - 15, 7 * 30 - 15, 3);
}
//画背景和棋盘
void DrawMap() {
	initgraph(390, 390);
	loadimage(&bgp, _T("images\\棋盘.png"));//加载图片
	PutBackground();//输出背景图片
	DrawLine();//画线
	DrawPoint();//画中间黑点
	getimage(&bgp, 0, 0, 390, 390);
}
//说明界面
void explain() {
	//加载、输出说明图片
	loadimage(&ep, _T("images\\说明.png"));
	putimage(0, 0, 390, 390, &ep, 0, 0);
	MOUSEMSG m;
	//右键返回开始界面
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) {
			Welcome();
		}
	}
	Welcome();
}
//寻找权重最大的位置
void FindMaxWeight() {
	Mx = 0, My = 0;
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++) {
			if (weight[i][j] > weight[Mx][My]) {
				Mx = i;
				My = j;
			}
		}
}
//将每个位置初始化
void InitPiece() {
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++) piece[i][j] = 0;
}
//初始化权重
void InitWeight() {
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
			weight[i][j] = 0;
}
//输出背景图片
void PutBackground() {
	putimage(0, 0, 390, 390, &bgp, 0, 0);
}
//模式一:PVP
void Game() {
	num = -1;//初始化
	DrawMap();//调用画图函数
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();//获取鼠标信息
		if (m.uMsg == WM_LBUTTONDOWN) {//如果是左键
			DrawPiece(m.x, m.y);//调用画棋函数
			if (Judge(m.x, m.y)) break;//判断游戏是否结束
		}
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();//如果按下右键,返回开始界面
	}
	//游戏结束后,获取到右键按下的鼠标信息后,返回开始界面
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
}
//模式二:PVE
void AIGame() {
	num = -1;//初始化
	DrawMap();//画棋盘和背景
	//弹出窗口,选择下棋顺序
	int ans=MessageBox(NULL, TEXT("请问是否选择先手"),TEXT("title"), MB_YESNO);
	//如果选择先手
	if (ans == IDYES) {
		DrawMap();
	}
	//如果选择后手
	else {
		DrawMap();
		//随机下第一个棋
		srand((unsigned)time(NULL));
		int x = rand() % 210 + 90;
		srand(2*(unsigned)time(NULL));
		int y = rand() % 210 + 90;
		DrawPiece(x, y);
	}
	MOUSEMSG m;
	//依次下棋
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {//左键玩家下棋
			DrawPiece(m.x, m.y);
			if (Judge(m.x, m.y)) break;//判断游戏是否结束
			Sleep(500);
			if (a != 0) AI();//机器下棋
			if (Judge(30*Mx, 30*My)) break;//判断游戏是否结束
		}
		//如果右键,返回开始界面
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
	//如果右键按下,返回开始界面
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
}
//模式三: EVE
void EVEGame() {
	num = -1;//初始化
	Countdown();//选择开始游戏
	//随机落下第一个棋
	int x,y;
	srand((unsigned)time(NULL));
	x = rand() % 210 + 90;
	srand(2*(unsigned)time(NULL));
	y = rand() % 210 + 90;
	DrawMap();
	DrawPiece(x,y);
	//机器依次下棋
	while (1) {
			AI();
			Sleep(300);
			if (Judge(30*Mx, 30*My)) break;
			AI();
			Sleep(300);
			if (Judge(30*Mx, 30*My)) break;
		}
	//游戏结束后,右键返回开始界面
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
}
//欢迎界面
void Welcome() {
	initgraph(390, 390);
	num = -1;//初始化
	temp = 0;//初始化
	counts = 0;//初始化
	loadimage(&start, _T("images\\开始.png"));//加载开始图片
	putimage(0, 0, 390, 390, &start, 0, 0);//输出开始图片
	InitPiece();//位置初始化
	MOUSEMSG m;//声明鼠标信息
	while (1) {//获取鼠标信息,左键按下选择模式
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			mode = ChooseMode(m.x,m.y);
			break;
		}
	}
	//选择模式
	switch (mode) {
	case 0://如果为0,则刷新界面,重新获取鼠标信息
		Welcome();
	case 1://如果判断为模式一,则调用Game函数
		Game();
		break;
	case 2://如果判断为模式二,则调用AIGame函数
		AIGame();
		break;
	case 3://如果判断为模式三,则调用EVEGame函数
		EVEGame();
		break;
	case 4://如果判断为模式四,则调用explain函数
		explain();
		break;
	case 5://如果判断为模式五,则调用Show函数
		Show();
		break;
	}
}
int main() {
	Welcome();
	return 0;
}