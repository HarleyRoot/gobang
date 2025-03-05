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
int num = -1;//-1��ʾ�����£�1��ʾ������
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
//�Ծֻطź���
void Show() {
	initgraph(390, 390);
	loadimage(&bgp, _T("images\\����.png"));//����ͼƬ
	PutBackground();//�������ͼƬ
	settextcolor(WHITE);
	settextstyle(20, 10, _T("����"));
	setbkmode(TRANSPARENT);
	outtextxy(30, 10,_T("�����ѡ��طŵĶԾ�"));
	int col = 30;
	for (int i = 0; i < FileNum-1; i++) {
		if (i % 8 == 0)col += 30;
		TCHAR Name[5];
		_stprintf_s(Name, _T("%d"), i+1);
		settextcolor(WHITE);
		settextstyle(20, 10, _T("����"));
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
//����ѡ����ļ�
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
//�ж���Ϸ�Ƿ����
int Judge(int x, int y) {
	int ans=0;//��ʼ����
	//����������Ϊ169,��ƽ��
	if (counts == 169) {
		ans = MessageBox(NULL, TEXT("��Ϸ���� ƽ��\n�Ƿ񷵻�������"), TEXT("��ʾ"), MB_YESNO);
		//����Ծ�
		if(mode!=5)Stay();
		if (ans == IDYES) Welcome();
		else if (ans == IDNO)MessageBox(NULL, TEXT("����Ҽ�����������"), TEXT("��ʾ"), MB_OK);
		return 1;
	}
	//δƽ��,�ж��Ƿ���һ����ʤ
	if (Check(x / 30, y / 30)) {
		 if (num == -1) ans=MessageBox(NULL, TEXT("��Ϸ���� �׷���ʤ\n�Ƿ񷵻�������"),TEXT("��ʾ"), MB_YESNO);
		else if (num == 1) ans=MessageBox(NULL, TEXT("��Ϸ���� �ڷ���ʤ\n�Ƿ񷵻�������"), TEXT("��ʾ"), MB_YESNO);
		//����Ծ�
		if(mode!=5)Stay();
		if (ans == IDYES) Welcome();
		else if (ans == IDNO)MessageBox(NULL, TEXT("����Ҽ�����������"), TEXT("��ʾ"), MB_OK);
		return 1;
	}
}
//�������庯��
void AI() {
	MOUSEMSG m;
	//��ʼ��Ȩ��
	InitWeight();
	num *= -1;
	//����Ȩ��
	CountWeight();
	num *= -1;
	//����Ȩ��
	CountWeight();
	//Ѱ�����Ȩ��
	FindMaxWeight();
	//����Ȩ����������
	DrawPiece(30 * Mx, 30 * My);
}
//��������,ѡ��ʼ��Ϸ
void Countdown() {
	DrawMap();
	MessageBox(NULL, _T("���ȷ����ʼ��Ϸ"), _T("��ʾ"), MB_OK);
	DrawMap();
}
//��ÿ��λ�ü���Ȩ��
void CountWeight() {
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++) {
			if (piece[i][j] == 0)
				Count(i, j);//�����λ��Ȩ��
		}
}
//����Ȩ��
void Count(int Cx, int Cy) {
	int w = 1, x = 1, y = 1, z = 1, i,j;//��ʼ��
	for (i = 1; i < 5; i++) {
		if (Cy + i < MAXIMUS && piece[Cx][Cy + i] == -1 * num) w++;
		else break;//���¼��
	}
	for (j = 1; j < 5; j++) {
		if (Cy - j >= 0 && piece[Cx][Cy - j] == -1 * num)	w++;
		else break;//���ϼ��
	}
	//�����������
	if (w >= 5)w*=2;
	//���Ϊ����,����ֵ����
	if (Cy + i == MAXIMUS || Cy - j == -1 || piece[Cx][Cy + i] == num || piece[Cx][Cy - j] == num)w/=2;
	//��ֵ
	weight[Cx][Cy] += (ll)pow(10, w);
	for (i = 1; i < 5; i++) {
		if (Cx + i < MAXIMUS && piece[Cx + i][Cy] == -1 * num)	x++;
		else break;//���Ҽ��
	}
	for (j = 1; j < 5; j++) {
		if (Cx - j >= 0 && piece[Cx - j][Cy] == -1 * num)	x++;
		else break;//������
	}
	if (x >= 5)x *=2;
	if (Cx + i == MAXIMUS || Cx - j == -1 || piece[Cx + i][Cy] == num || piece[Cx-j][Cy] == num)x/=2;
	weight[Cx][Cy] += (ll)pow(10, x);
	for (i = 1; i < 5; i++) {
		if (Cx + i < MAXIMUS && Cy + i < MAXIMUS && piece[Cx + i][Cy + i] == -1 * num)	y++;
		else break;//�����¼��
	}
	for (j = 1; j < 5; j++) {
		if (Cx - j >= 0 && Cy - j >= 0 && piece[Cx - j][Cy - j] == -1 * num)	y++;
		else break;//�����ϼ��
	}
	if (y >= 5)y *=2;
	if (Cx + i == MAXIMUS || Cy + i == MAXIMUS || Cx - j == -1 || Cy - j == -1 || piece[Cx + i][Cy + i] == num || piece[Cx - j][Cy - j] == -1 * num)y/=2;
	weight[Cx][Cy] += (ll)pow(10, y);
	for (i = 1; i < 5; i++) {
		if (Cx + i < MAXIMUS && Cy - i >= 0 && piece[Cx + i][Cy - i] == -1 * num)	z++;
		else break;//�����ϼ��
	}
	for (j = 1; j < 5; j++) {
		if (Cx - j >= 0 && Cy + j < MAXIMUS && piece[Cx - j][Cy + j] == -1 * num)	z++;
		else break;//�����¼�� 
	}
	if (z >= 5)z *=2;
	if (Cx + i == MAXIMUS || Cy - i == -1 || Cx - j == -1 || Cy + j == MAXIMUS || piece[Cx + i][Cy - i] == num || piece[Cx - j][Cy + j] == num)z /=2;
	weight[Cx][Cy] += (ll)pow(10, z);
}
//�ж��Ƿ�������
int ChangePiece(int x, int y) {
	if (piece[x][y] != 0) {
		return 0;
	}
	else {
		piece[x][y] = num;
		return num;
	}
}
//����Ƿ���������
int Check(int Cx, int Cy) {
	int w = 1, x = 1, y = 1, z = 1, i;//�ۼƺ�����б��а�ĸ������������ͬ������Ŀ
	for (i = 1; i < 5; i++)
		if (Cy + i < MAXIMUS && piece[Cx][Cy + i] == -1 * num)
			w++;
		else
			break;//���¼��
	for (i = 1; i < 5; i++)
		if (Cy - i >= 0 && piece[Cx][Cy - i] == -1 * num)
			w++;
		else
			break;//���ϼ��
	if (w >= 5)
		return 1;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
	for (i = 1; i < 5; i++)
		if (Cx + i < MAXIMUS && piece[Cx + i][Cy] == -1 * num)
			x++;
		else
			break;//���Ҽ��
	for (i = 1; i < 5; i++)
		if (Cx - i > 0 && piece[Cx - i][Cy] == -1 * num)
			x++;
		else
			break;//������
	if (x >= 5)
		return 1;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
	for (i = 1; i < 5; i++)
		if (Cx + i < MAXIMUS && Cy + i < MAXIMUS && piece[Cx + i][Cy + i] == -1 * num)
			y++;
		else
			break;//�����¼��
	for (i = 1; i < 5; i++)
		if (Cx - i > 0 && Cy - i > 0 && piece[Cx - i][Cy - i] == -1 * num)
			y++;
		else
			break;//�����ϼ��
	if (y >= 5)
		return 1;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
	for (i = 1; i < 5; i++)
		if (Cx + i < MAXIMUS && Cy - i>0 && piece[Cx + i][Cy - i] == -1 * num)
			z++;
		else
			break;//�����ϼ��
	for (i = 1; i < 5; i++)
		if (Cx - i > 0 && Cy + i < MAXIMUS && piece[Cx - i][Cy + i] == -1 * num)
			z++;
		else
			break;//�����¼�� if(z>=5)return Now;//�����ﵽ5�����жϵ�ǰ�������ΪӮ��
	if (z >= 5) {
		return 1;
	}
	return 0;//��û�м�鵽�����飬�򷵻�0��ʾ��û����Ҵ��ʤ��
}
//�ж����ѡ������ĸ��Ծ�
int ChooseFile(int m, int n) {
	for (int i = 0; 8*i<FileNum-1; i++) {
		for (int j = 1; j <= 8; j++) {
			if (8 * i + j == FileNum) break;
			if (m > 40 * j && m < 40 * j + 10 && n>30 * i + 60 && n < 30 * i + 80) return 8 * i + j;
		}
	}
	return 0;
}
//ͨ�����λ�ö��ж���ѡ���ģʽ
int ChooseMode(int m, int n) {
	if (m >= 250 && m <= 305 && n > 211 && n <= 241) return 1;
	if (m >= 250 && m <= 305 && n > 241 && n <= 271) return 2;
	if (m >= 250 && m <= 305 && n > 271 && n <= 301) return 3;
	if (m >= 250 && m <= 357 && n > 301 && n <= 331) return 4;
	if (m >= 250 && m <= 357 && n > 331 && n <= 361) return 5;
	else return 0;//δ����ģʽ�򷵻�0
}
//�����
void DrawSign(int x, int y) {
	setfillcolor(RED);
	fillcircle(30 * x + 15, 30 * y + 15, 2);
}
//���庯��
void DrawPiece(int m, int n) {
	putimage(0, 0, &bgp);
	int x, y;
	x = m / 30;
	y = n / 30;
	if (mode != 5) {
		chess[temp].x = x;
		chess[temp].y = y;//��¼����
		temp++;
	}
	a = ChangePiece(x, y);
	getimage(&bgp, 0, 0, 390, 390);
	if (a == -1) setfillcolor(BLACK);
	if (a == 1) setfillcolor(WHITE);
	if (a == 0) {
		int ans = MessageBox(NULL, TEXT("��λ����������\n������ѡ��"), TEXT("��ʾ"), MB_OK);
		return;
	}
	counts++;
	fillcircle((x) * 30 + 15, (y) * 30 + 15, 13);
	getimage(&bgp, 0, 0, 390, 390);
	//�����
	DrawSign(x, y);
	//�任������ɫ
	num *= -1;
}
//�����̵���
void DrawLine() {
	setlinecolor(BLACK);
	for (int x = 15; x < 390; x += 30)
		line(x, 15, x, 375);
	for (int y = 15; y < 390; y += 30)
		line(15, y, 375, y);
}
//���������еĶ�λ��
void DrawPoint() {
	setfillcolor(BLACK);
	fillcircle(4 * 30 - 15, 4 * 30 - 15, 3);
	fillcircle(4 * 30 - 15, 10 * 30 - 15, 3);
	fillcircle(10 * 30 - 15, 4 * 30 - 15, 3);
	fillcircle(10 * 30 - 15, 10 * 30 - 15, 3);
	fillcircle(7 * 30 - 15, 7 * 30 - 15, 3);
}
//������������
void DrawMap() {
	initgraph(390, 390);
	loadimage(&bgp, _T("images\\����.png"));//����ͼƬ
	PutBackground();//�������ͼƬ
	DrawLine();//����
	DrawPoint();//���м�ڵ�
	getimage(&bgp, 0, 0, 390, 390);
}
//˵������
void explain() {
	//���ء����˵��ͼƬ
	loadimage(&ep, _T("images\\˵��.png"));
	putimage(0, 0, 390, 390, &ep, 0, 0);
	MOUSEMSG m;
	//�Ҽ����ؿ�ʼ����
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) {
			Welcome();
		}
	}
	Welcome();
}
//Ѱ��Ȩ������λ��
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
//��ÿ��λ�ó�ʼ��
void InitPiece() {
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++) piece[i][j] = 0;
}
//��ʼ��Ȩ��
void InitWeight() {
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
			weight[i][j] = 0;
}
//�������ͼƬ
void PutBackground() {
	putimage(0, 0, 390, 390, &bgp, 0, 0);
}
//ģʽһ:PVP
void Game() {
	num = -1;//��ʼ��
	DrawMap();//���û�ͼ����
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();//��ȡ�����Ϣ
		if (m.uMsg == WM_LBUTTONDOWN) {//��������
			DrawPiece(m.x, m.y);//���û��庯��
			if (Judge(m.x, m.y)) break;//�ж���Ϸ�Ƿ����
		}
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();//��������Ҽ�,���ؿ�ʼ����
	}
	//��Ϸ������,��ȡ���Ҽ����µ������Ϣ��,���ؿ�ʼ����
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
}
//ģʽ��:PVE
void AIGame() {
	num = -1;//��ʼ��
	DrawMap();//�����̺ͱ���
	//��������,ѡ������˳��
	int ans=MessageBox(NULL, TEXT("�����Ƿ�ѡ������"),TEXT("title"), MB_YESNO);
	//���ѡ������
	if (ans == IDYES) {
		DrawMap();
	}
	//���ѡ�����
	else {
		DrawMap();
		//����µ�һ����
		srand((unsigned)time(NULL));
		int x = rand() % 210 + 90;
		srand(2*(unsigned)time(NULL));
		int y = rand() % 210 + 90;
		DrawPiece(x, y);
	}
	MOUSEMSG m;
	//��������
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {//����������
			DrawPiece(m.x, m.y);
			if (Judge(m.x, m.y)) break;//�ж���Ϸ�Ƿ����
			Sleep(500);
			if (a != 0) AI();//��������
			if (Judge(30*Mx, 30*My)) break;//�ж���Ϸ�Ƿ����
		}
		//����Ҽ�,���ؿ�ʼ����
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
	//����Ҽ�����,���ؿ�ʼ����
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
}
//ģʽ��: EVE
void EVEGame() {
	num = -1;//��ʼ��
	Countdown();//ѡ��ʼ��Ϸ
	//������µ�һ����
	int x,y;
	srand((unsigned)time(NULL));
	x = rand() % 210 + 90;
	srand(2*(unsigned)time(NULL));
	y = rand() % 210 + 90;
	DrawMap();
	DrawPiece(x,y);
	//������������
	while (1) {
			AI();
			Sleep(300);
			if (Judge(30*Mx, 30*My)) break;
			AI();
			Sleep(300);
			if (Judge(30*Mx, 30*My)) break;
		}
	//��Ϸ������,�Ҽ����ؿ�ʼ����
	MOUSEMSG m;
	while (1) {
		m = GetMouseMsg();
		if (m.uMsg == WM_RBUTTONDOWN) Welcome();
	}
}
//��ӭ����
void Welcome() {
	initgraph(390, 390);
	num = -1;//��ʼ��
	temp = 0;//��ʼ��
	counts = 0;//��ʼ��
	loadimage(&start, _T("images\\��ʼ.png"));//���ؿ�ʼͼƬ
	putimage(0, 0, 390, 390, &start, 0, 0);//�����ʼͼƬ
	InitPiece();//λ�ó�ʼ��
	MOUSEMSG m;//���������Ϣ
	while (1) {//��ȡ�����Ϣ,�������ѡ��ģʽ
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			mode = ChooseMode(m.x,m.y);
			break;
		}
	}
	//ѡ��ģʽ
	switch (mode) {
	case 0://���Ϊ0,��ˢ�½���,���»�ȡ�����Ϣ
		Welcome();
	case 1://����ж�Ϊģʽһ,�����Game����
		Game();
		break;
	case 2://����ж�Ϊģʽ��,�����AIGame����
		AIGame();
		break;
	case 3://����ж�Ϊģʽ��,�����EVEGame����
		EVEGame();
		break;
	case 4://����ж�Ϊģʽ��,�����explain����
		explain();
		break;
	case 5://����ж�Ϊģʽ��,�����Show����
		Show();
		break;
	}
}
int main() {
	Welcome();
	return 0;
}