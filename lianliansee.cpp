#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <mmsystem.h>
#include <winuser.h>
#pragma comment(lib, "WINMM.LIB")
#define null 0
#define winmain -1
#define rk -2
#define gm -3
int now;//现在的界面
bool first;
int tip = 0, score = 0, shf = 0, n;//剩余tip次提示，shf次洗牌，龙虎榜上有几位大侠
int nx = -1, ny = -1;//当前选定的格子，（-1，-1）表示没选
double k1, k2;//这次和上次的消去的时间
IMAGE empty;//这只是一张空图，用来覆盖别的
MOUSEMSG k;//鼠标信息
TCHAR name[100] = { 'C','o','m','r','a','d','e',0 };//默认名字

struct node { int x1, y1; };
struct pairnode { int x1, y1, x2, y2; };
struct rank
{
	int day = 0, s = 0;
	TCHAR c[100] = { 0 };
}a[10];//排行榜
struct block
{
	int num = 0, t = 0, show = 0, clk = 0;
	int x = 0, y = 0, left = 0, right = 0, top = 0, bottom = 0;
	IMAGE img;

}b[15][20], test;//方块
void firstload()
{
	for (int i = 0; i <= 14; i++)
		for (int j = 0; j <= 19; j++)
		{
			b[i][j].x = 50 * j + 30;
			b[i][j].y = 50 * i + 45;
			b[i][j].left = b[i][j].x - 25;
			b[i][j].top = b[i][j].y - 25;
			b[i][j].right = b[i][j].x + 25;
			b[i][j].bottom = b[i][j].y + 25;
			b[i][j].show = 0;
			b[i][j].t = 0;
			if (i == 0)b[i][j].y = 55;
		}
	loadimage(&empty, _T("0.jpg"), 50, 50);
}   //初始化每个格子的基本信息


void init_main();//游戏界面
int getname(TCHAR c[]);//获取玩家名字
void getrankings();//获取排行榜
void showrankings();//显示排行榜
void add(TCHAR name[], int score);//欢迎新人上榜
void initial();//初始化游戏
void shuffle_first(int n);//第一次洗牌，至少要n对相邻格子
void shuffle();//洗牌
void showgame();//显示格子
void update_tip(int tip);//更新提示次数
void update_shuffle(int shf);//更新洗牌次数
void undate_score(int score);//更新分数
void update_time(double t, int t1);//更新时间
int findtip(int k);//寻找可以消去的格子，k=1代表需要显示
int legal(int x1, int y1, int x2, int y2);//判定是否可消去，返回0不可，1可以直接连，2折一次，3折两次
bool con1(int x1, int y1, int x2, int y2);//是否直接连
node con2(int x1, int y1, int x2, int y2);//折一次
pairnode con3(int x1, int y1, int x2, int y2);//折两次
void write();//写入排行榜

int main()
{
	srand((unsigned)time(NULL));
	initgraph(800, 600);
	setbkcolor(WHITE);
	cleardevice();
	setcolor(BLACK);
	PlaySound(TEXT("start.wav"), NULL, SND_FILENAME | SND_ASYNC);
	getrankings();
	//mciSendString(TEXT("open korobeiniki.wav alias mainmusic"), NULL, 0, NULL);

	firstload();
	now = null;
	while (1)
	{
		//mciSendString(TEXT("play mainmusic"), NULL, 0, NULL);
		MOUSEMSG k;
		while (1)
		{
			if (now != winmain)
			{
				cleardevice();
				rectangle(200, 70, 600, 120);
				outtextxy(370, 90, _T("Begin"));
				rectangle(200, 170, 600, 220);
				outtextxy(370, 190, _T("Settings"));
				rectangle(200, 270, 600, 320);
				outtextxy(360, 290, _T("Rankings"));
				rectangle(200, 370, 600, 420);
				outtextxy(370, 390, _T("About"));
				rectangle(200, 470, 600, 520);
				outtextxy(380, 490, _T("Exit"));
				now = winmain;
			}
			k = GetMouseMsg();
			if (k.x >= 200 && k.x <= 600 && k.y >= 70 && k.y <= 120)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
				if (k.uMsg == WM_LBUTTONDOWN)
				{
					PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);

					init_main();

				}

			}
			else if (k.x >= 200 && k.x <= 600 && k.y >= 170 && k.y <= 220)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
				if (k.uMsg == WM_LBUTTONDOWN)
				{
					SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
					PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
					now = 0; cleardevice();
					outtextxy(100, 100, TEXT("Please input your English name (No longer than 50 characters,press enter to confirm or go back.) :"));
					if (!getname(name))break;
					outtextxy(100, 500, TEXT("Processing"));
					Sleep(600);
					outtextxy(100, 500, TEXT("You have set your name successfully!"));
					rectangle(500, 480, 650, 530);
					outtextxy(560, 500, TEXT("Back"));
					while (1)
					{
						k = GetMouseMsg();
						SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
						if (k.x >= 500 && k.x <= 650 && k.y >= 480 && k.y <= 530)
						{

							SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
							if (k.uMsg == WM_LBUTTONDOWN)
							{
								PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
								break;
							}
						}
					}
				}
			}
			else if (k.x >= 200 && k.x <= 600 && k.y >= 270 && k.y <= 320)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
				if (k.uMsg == WM_LBUTTONDOWN)
				{
					PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
					now = 0;
					cleardevice();
					showrankings();
					while (1)
					{
						k = GetMouseMsg();
						if (k.uMsg == WM_LBUTTONDOWN)break;
					}
				}

			}
			else if (k.x >= 200 && k.x <= 600 && k.y >= 370 && k.y <= 420)
			{

				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
				if (k.uMsg == WM_LBUTTONDOWN)
				{
					PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
					now = 0;
					cleardevice();
					TCHAR s[20];
					swprintf_s(s, _T("Hello,%s!"), name);
					outtextxy(20, 15, s);
					outtextxy(20, 45, _T("This game is made by HuangYifei,2019013349 from Xinya College or Department of Mathematical Science."));
					outtextxy(20, 75, _T("Beg for a high score"));
					outtextxy(20, 105, _T("Click anywhere to return"));
					while (1)
					{
						k = GetMouseMsg();
						if (k.uMsg == WM_LBUTTONDOWN)break;
					}
				}

			}
			else if (k.x >= 200 && k.x <= 600 && k.y >= 470 && k.y <= 520)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
				if (k.uMsg == WM_LBUTTONDOWN)
				{
					write();
					closegraph();
					return 0;

				}
			}
			else
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));

		}
		//getch();
	}
	while (1);
	closegraph();
	return 0;
}

void init_main()
{
	cleardevice();
	SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
	clock_t start, end;
	const int T = 360;

	while (1)
	{
		if (now != gm)
		{
			cleardevice();
			TCHAR s1[20], s2[20];
			swprintf_s(s1, _T("Tip:%2d"), tip);
			swprintf_s(s2, _T("Shuffle:%2d"), shf);
			line(0, 50, 800, 50);
			outtextxy(20, 15, TEXT("Score:   0"));
			outtextxy(450, 15, _T("  Time:  "));
			rectangle(110, 10, 190, 40);
			outtextxy(135, 15, _T("Start"));
			rectangle(210, 10, 290, 40);
			outtextxy(220, 15, s1);
			rectangle(310, 10, 390, 40);
			outtextxy(320, 15, s2);
			rectangle(600, 10, 700, 40);
			outtextxy(630, 15, _T("Exit"));
			now = gm;
		}
		MOUSEMSG k;
		while (1)
		{
			k = GetMouseMsg();
			if (k.x >= 110 && k.x <= 190 && k.y >= 10 && k.y <= 40)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
				if (k.uMsg == WM_LBUTTONDOWN)			//begin to play
				{
					k1 = k2 = score = 0;
					int win = 0;
					PlaySound(TEXT("start.wav"), NULL, SND_FILENAME | SND_ASYNC);
					mciSendString(TEXT("open korobeiniki.wav alias bkmusic"), NULL, 0, NULL);
					mciSendString(TEXT("play bkmusic"), NULL, 0, NULL);

					undate_score(score);
					update_time(0, 0);
					outtextxy(135, 15, _T(" Stop"));

					initial();
					SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
					start = clock();
					while (1)
					{
						end = clock();
						double t = T - (double)(end - start) / CLK_TCK;
						int t1 = ceil(t);
						update_time(t, t1);
						if (end - k1 >= 2 * CLK_TCK || k1 - end >= 2 * CLK_TCK)outtextxy(730, 15, TEXT("             "));
						if (t > 0 && !MouseHit())continue;
						k = GetMouseMsg();
						if (k.x >= 110 && k.x <= 190 && k.y >= 10 && k.y <= 40)//stop
						{
							SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
							if (k.uMsg == WM_LBUTTONDOWN)t = 0;
						}
						else if (k.x >= 210 && k.x <= 290 && k.y >= 10 && k.y <= 40)//tip
						{
							if (tip > 0)
							{
								SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
								if (k.uMsg == WM_LBUTTONDOWN)
								{
									PlaySound(TEXT("hint.wav"), NULL, SND_FILENAME | SND_ASYNC);
									int k = findtip(1);
									tip -= 1;
									update_tip(tip);
								}
							}
							else SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_NO));

						}
						else if (k.x >= 310 && k.x <= 390 && k.y >= 10 && k.y <= 40)//shuffle
						{
							if (shf > 0)
							{
								SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
								if (k.uMsg == WM_LBUTTONDOWN)
								{
									PlaySound(TEXT("shuffle.wav"), NULL, SND_FILENAME | SND_ASYNC);
									shuffle(); showgame();
									shf -= 1;
									update_shuffle(shf);
								}
							}
							else SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_NO));
						}
						else if (k.x >= 600 && k.x <= 700 && k.y >= 10 && k.y <= 40)//exit
						{
							SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
							if (k.uMsg == WM_LBUTTONDOWN)
							{
								PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
								mciSendString(TEXT("close bkmusic"), NULL, 0, NULL);
								return;
							}
						}
						else
						{
							int flag = 0;
							for (int i = 1; i <= 10; i++)
								for (int j = 1; j <= 14; j++)
									if (b[i][j].show && k.x >= b[i][j].left && k.x <= b[i][j].right && k.y >= b[i][j].top && k.y <= b[i][j].bottom)
									{
										SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
										flag = 1;
										if (k.uMsg == WM_LBUTTONDOWN)
										{
											if (nx == -1 && ny == -1)
											{
												PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
												nx = i, ny = j;
												rectangle(b[nx][ny].left, b[nx][ny].top, b[nx][ny].right, b[nx][ny].bottom);
											}
											else if (nx == i && ny == j)
											{
												PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
												setcolor(RGB(220, 220, 220));
												rectangle(b[nx][ny].left, b[nx][ny].top, b[nx][ny].right, b[nx][ny].bottom);
												setcolor(BLACK);
												nx = -1; ny = -1;
											}
											else
											{
												int sig = legal(nx, ny, i, j);
												if (sig != 0)//如果可以消，那就找一条消的路线，画出来然后再用新的图覆盖掉
												{
													PlaySound(TEXT("right.wav"), NULL, SND_FILENAME | SND_ASYNC);
													k2 = clock();
													if (k2 - k1 <= 2 * CLK_TCK && k1 - k2 <= 2 * CLK_TCK)
													{
														score += 4;
														outtextxy(730, 15, TEXT("Combo"));
													}
													else score += 2;
													k1 = k2;
													undate_score(score);
													rectangle(b[i][j].left, b[i][j].top, b[i][j].right, b[i][j].bottom);
													node a1; pairnode a2;
													if (sig == 1)line(b[nx][ny].x, b[nx][ny].y, b[i][j].x, b[i][j].y);
													else if (sig == 2)
													{
														a1 = con2(nx, ny, i, j);
														line(b[nx][ny].x, b[nx][ny].y, b[a1.x1][a1.y1].x, b[a1.x1][a1.y1].y);
														line(b[i][j].x, b[i][j].y, b[a1.x1][a1.y1].x, b[a1.x1][a1.y1].y);
													}
													else if (sig == 3)
													{
														a2 = con3(nx, ny, i, j);
														line(b[nx][ny].x, b[nx][ny].y, b[a2.x1][a2.y1].x, b[a2.x1][a2.y1].y);
														line(b[a2.x2][a2.y2].x, b[a2.x2][a2.y2].y, b[a2.x1][a2.y1].x, b[a2.x1][a2.y1].y);
														line(b[i][j].x, b[i][j].y, b[a2.x2][a2.y2].x, b[a2.x2][a2.y2].y);
													}
													Sleep(100);
													setcolor(WHITE);
													//rectangle(b[i][j].left, b[i][j].top, b[i][j].right, b[i][j].bottom);
													if (sig == 1)line(b[nx][ny].x, b[nx][ny].y, b[i][j].x, b[i][j].y);
													else if (sig == 2)
													{
														line(b[nx][ny].x, b[nx][ny].y, b[a1.x1][a1.y1].x, b[a1.x1][a1.y1].y);
														line(b[i][j].x, b[i][j].y, b[a1.x1][a1.y1].x, b[a1.x1][a1.y1].y);
													}
													else if (sig == 3)
													{
														line(b[nx][ny].x, b[nx][ny].y, b[a2.x1][a2.y1].x, b[a2.x1][a2.y1].y);
														line(b[a2.x2][a2.y2].x, b[a2.x2][a2.y2].y, b[a2.x1][a2.y1].x, b[a2.x1][a2.y1].y);
														line(b[i][j].x, b[i][j].y, b[a2.x2][a2.y2].x, b[a2.x2][a2.y2].y);
													}
													setcolor(BLACK);
													b[nx][ny].show = b[i][j].show = 0;
													win = 1;
													for (int i = 1; i <= 10; i++)
														for (int j = 1; j <= 14; j++)
															if (b[i][j].show == 1)win = 0;
													if (win == 1)t = -1;
													else while (!findtip(0))shuffle();
													showgame();
													nx = ny = -1;
												}
												else
												{
													if (b[nx][ny].t == b[i][j].t)
													{
														PlaySound(TEXT("wrong.wav"), NULL, SND_FILENAME | SND_ASYNC);
														setcolor(RGB(220, 220, 220));
														rectangle(b[nx][ny].left, b[nx][ny].top, b[nx][ny].right, b[nx][ny].bottom);
														setcolor(BLACK);
														nx = -1, ny = -1;
													}
													else
													{
														PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
														setcolor(RGB(220, 220, 220));
														rectangle(b[nx][ny].left, b[nx][ny].top, b[nx][ny].right, b[nx][ny].bottom);
														setcolor(BLACK);
														nx = i, ny = j;
														rectangle(b[nx][ny].left, b[nx][ny].top, b[nx][ny].right, b[nx][ny].bottom);

													}
												}
											}
										}
									}

							if (flag == 0)SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
						}


						if (t <= 0.0)//胜利的话也是直接把时间跳到0的，所以要分情况
						{
							mciSendString(TEXT("close bkmusic"), NULL, 0, NULL);
							if (win)
							{
								outtextxy(450, 15, TEXT("    You Win!    "));
								PlaySound(TEXT("win.wav"), NULL, SND_FILENAME | SND_ASYNC);
								now = 0;
								outtextxy(100, 100, TEXT("Do you want to enter your name? Y / N"));
								char ch = _getch();
								while (ch != 'n' && ch != 'N' && ch != 'y' && ch != 'Y')ch = _getch();
								if (ch == 'n' || ch == 'N')
								{
									cleardevice();
									add(name, score);
									return;
								}
								outtextxy(80, 100, TEXT("Please input. Press enter to confirm or go back."));
								if (!getname(name)) { add(name, score); return; }
								add(name, score);
								outtextxy(100, 500, TEXT("Processing"));
								Sleep(600);
								outtextxy(100, 500, TEXT("You have set your name successfully!"));
								rectangle(500, 480, 650, 530);
								outtextxy(560, 500, TEXT("Back"));
								while (1)
								{
									k = GetMouseMsg();
									SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
									if (k.x >= 500 && k.x <= 650 && k.y >= 480 && k.y <= 530)
									{

										SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
										if (k.uMsg == WM_LBUTTONDOWN)
										{
											PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
											break;
										}
									}
								}
								cleardevice();
								return;
							}
							else outtextxy(450, 15, TEXT("    Game Over    "));
							Sleep(2000); return;
							outtextxy(135, 15, _T("Start  "));
							outtextxy(730, 15, TEXT("             "));
						}


					}
				}

			}
			else if (k.x >= 600 && k.x <= 700 && k.y >= 10 && k.y <= 40)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_CROSS));
				if (k.uMsg == WM_LBUTTONDOWN)
				{
					PlaySound(TEXT("click.wav"), NULL, SND_FILENAME | SND_ASYNC);
					return;
				}
			}
			else if (k.x >= 210 && k.x <= 290 && k.y >= 10 && k.y <= 40)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_NO));
			}
			else if (k.x >= 310 && k.x <= 390 && k.y >= 10 && k.y <= 40)
			{
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_NO));
			}
			else
				SetClassLong(GetHWnd(), GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
		}
	}

}
int getname(TCHAR n[])
{
	TCHAR c[100] = { 0 };
	int i = 0;
	char j = _getch();
	if (j == 13)return 0;
	while (j != 13) //13是回车的ascii码
	{
		if (j == 8 && i > 0)c[--i] = 0;
		else if (j != 8)
		{
			if (i >= 50)
			{
				PlaySound(TEXT("wrong.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else
			{
				if (j >= 32) c[i++] = j;
			}
		}
		outtextxy(100, 300, TEXT("                                                                                                          "));
		outtextxy(100, 300, c);
		j = _getch();
	}
	for (int i = 0; i < 100; i++)n[i] = c[i];
	return 1;
}
void getrankings()
{
	FILE* stream1;
	for (int i = 0; i < 10; i++)
	{
		a[i].day = a[i].s = 0;
		for (int j = 0; j < 50; j++)a[i].c[j] = '\0';
	}
	freopen_s(&stream1, "rankings.txt", "r", stdin);
	scanf_s("%d", &n);
	for (int i = 0; i < n; i++)
	{
		scanf("%ls", &a[i].c);
		scanf_s("%d%d", &a[i].day, &a[i].s);
	}
	fclose(stdin);
}
void showrankings()
{
	outtextxy(300, 40, TEXT("Rankings"));
	for (int i = 0; i < 10; i++)
	{
		TCHAR s[100];
		if (a[i].s != 0)
		{
			swprintf_s(s, _T("No.%2d      name: %ls    day: %d    score: %d"), i + 1, a[i].c, a[i].day, a[i].s);
			outtextxy(150, (i + 2) * 40, s);
		}
		else outtextxy(150, (i + 2) * 40, TEXT("N/A"));
	}
	outtextxy(300, 12 * 40, TEXT("Click anywhere to return"));
}
void add(TCHAR name[], int score)
{
	rank g;
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	int year, month, day;
	year = 1900 + timeinfo->tm_year;
	month = 1 + timeinfo->tm_mon;
	day = timeinfo->tm_mday;
	g.day = 10000 * year + 100 * month + day; g.s = score;
	for (int i = 0; i < 100; i++)g.c[i] = name[i];

	int k = 0;
	while (k < 10 && g.s < a[k].s)k++;
	if (k < 10)
	{
		a[9] = g;
		for (int i = 0; i < 9; i++)
			for (int j = i + 1; j < 10; j++)
				if (a[i].s < a[j].s)
				{
					rank temp = a[i];
					a[i] = a[j]; a[j] = temp;
				}
	}
	write();
}
void initial()
{
	shf = 1; tip = 2;
	update_tip(tip); update_shuffle(shf);
	shuffle_first(10);
	showgame();
}
void update_tip(int tip)
{
	TCHAR s1[20];
	swprintf_s(s1, _T("Tip:%2d"), tip);
	outtextxy(220, 15, s1);
}
void update_shuffle(int shf)
{
	TCHAR s2[20];
	swprintf_s(s2, _T("Shuffle:%2d"), shf);
	outtextxy(320, 15, s2);
}
void update_time(double t, int t1)
{
	TCHAR s[20];
	swprintf_s(s, _T("%3ds   "), t1);
	if (t - t1<0.1 && t - t1>-0.1)outtextxy(500, 15, _T("       "));
	outtextxy(500, 15, s);
}
void undate_score(int score)
{
	TCHAR sc[20];
	swprintf_s(sc, _T("Score:%5d"), score);
	outtextxy(20, 15, TEXT("                   "));
	outtextxy(20, 15, sc);
}
void shuffle_first(int n)
{
	for (int i = 0; i <= 14; i++)
		for (int j = 0; j <= 19; j++)
		{
			b[i][j].t = b[i][j].show = 0;
			if (1 <= i && i <= 10 && 1 <= j && j <= 14)b[i][j].show = 1;
		}

	int k[15][20], p = 0, c[15][20];
	const int dx[4] = { 1,0,-1,0 }, dy[4] = { 0,-1,0,1 };

	for (int i = 1; i <= 10; i++)
		for (int j = 1; j <= 13; j++)k[i][j] = j, c[i][j] = 0;
	for (int i = 1; i <= 10; i++)k[i][14] = i % 5, c[i][14] = 0;

	while (p <= n)
	{
		int x, y, h;
		x = rand() % 10 + 1; y = rand() % 14 + 1; h = rand() % 4;
		if (1 <= x + dx[h] && x + dx[h] <= 10 && 1 <= y + dy[h] && y + dy[h] <= 14)
		{
			if (c[x][y] == 0 && c[x + dx[h]][y + dy[h]] == 0)
			{
				c[x][y] = 1; c[x + dx[h]][y + dy[h]] = 1;
				for (int i = 1; i <= 10; i++)
					for (int j = 1; j <= 14; j++)
						if (c[i][j] == 0 && k[i][j] == k[x][y])
						{
							int temp = k[x + dx[h]][y + dy[h]];
							k[x + dx[h]][y + dy[h]] = k[i][j];
							k[i][j] = temp;
							p++;
						}
			}
		}
	}
	p = 0;
	while (p <= 100)
	{
		int x1, y1, x2, y2;
		x1 = rand() % 10 + 1; y1 = rand() % 14 + 1;
		x2 = rand() % 10 + 1; y2 = rand() % 14 + 1;
		if (c[x1][y1] == 0 && c[x2][y2] == 0 && k[x1][y1] != k[x2][y2])
		{
			int temp = k[x1][y1];
			k[x1][y1] = k[x2][y2];
			k[x2][y2] = temp;
			p++;
		}
	}
	for (int i = 1; i <= 10; i++)
		for (int j = 1; j <= 14; j++)
		{
			TCHAR s[20];
			swprintf_s(s, _T("%d.jpg"), k[i][j] + 1);
			b[i][j].t = k[i][j] + 1;
			loadimage(&b[i][j].img, s, 50, 50);
		}
}
void shuffle()
{
	int p = 0;
	for (int i = 0; i < 200; i++)
	{
		int x1 = rand() % 10 + 1, y1 = rand() % 14 + 1, x2 = rand() % 10 + 1, y2 = rand() % 14 + 1;
		while (b[x1][y1].show == 0)x1 = rand() % 10 + 1, y1 = rand() % 14 + 1;
		int k = 0;
		while (k<100 && (b[x2][y2].show == 0 || b[x1][y1].t == b[x2][y2].t))
		{
			x2 = rand() % 10 + 1, y2 = rand() % 14 + 1;
			k++;
		}
		int temp = b[x1][y1].t;
		b[x1][y1].t = b[x2][y2].t;
		b[x2][y2].t = temp;
	}
	for (int i = 1; i <= 10; i++)
		for (int j = 1; j <= 14; j++)
		{
			TCHAR s[20];
			swprintf_s(s, _T("%d.jpg"), b[i][j].t);
			loadimage(&b[i][j].img, s, 50, 50);
		}
}
void showgame()
{
	setcolor(WHITE);
	rectangle(b[1][1].left, b[1][1].top, b[10][14].right, b[10][14].bottom);

	for (int i = 1; i <= 10; i++)
		for (int j = 1; j <= 14; j++)
			if (b[i][j].show == 1)putimage(b[i][j].left, b[i][j].top, &b[i][j].img);
			else putimage(b[i][j].left, b[i][j].top, &empty);

	setcolor(RGB(220, 220, 220));
	for (int i = 1; i <= 10; i++)
		for (int j = 1; j <= 14; j++)
			if (b[i][j].show == 1)rectangle(b[i][j].left, b[i][j].top, b[i][j].right, b[i][j].bottom);
	setcolor(BLACK);
}
int findtip(int k)
{
	if (nx != -1 && ny != -1)
	{
		setcolor(RGB(220, 220, 220));
		rectangle(b[nx][ny].left, b[nx][ny].top, b[nx][ny].right, b[nx][ny].bottom);
		setcolor(BLACK);
		nx = ny = -1;
	}
	for (int x1 = 1; x1 <= 10; x1++)for (int y1 = 1; y1 <= 14; y1++)if (b[x1][y1].show)
		for (int x2 = 1; x2 <= 10; x2++)for (int y2 = 1; y2 <= 14; y2++)if ((x2 != x1 || y2 != y1) && b[x2][y2].show)
			if (legal(x1, y1, x2, y2) != 0)
			{
				if (k == 1)
				{
					setcolor(RED);
					rectangle(b[x1][y1].left, b[x1][y1].top, b[x1][y1].right, b[x1][y1].bottom);
					rectangle(b[x2][y2].left, b[x2][y2].top, b[x2][y2].right, b[x2][y2].bottom);
					/*
					Sleep(200);
					setcolor(RGB(220, 220, 220));
					rectangle(b[x1][y1].left, b[x1][y1].top, b[x1][y1].right, b[x1][y1].bottom);
					rectangle(b[x2][y2].left, b[x2][y2].top, b[x2][y2].right, b[x2][y2].bottom);
					*/
					setcolor(BLACK);

				}
				return 1;
			}
	return 0;
}
int legal(int x1, int y1, int x2, int y2)
{
	if (b[x1][y1].t == b[x2][y2].t)
	{
		if (con1(x1, y1, x2, y2))return 1;
		node a1 = con2(x1, y1, x2, y2);
		if (a1.x1 != -1 && a1.y1 != -1)return 2;
		pairnode a2 = con3(x1, y1, x2, y2);
		if (a2.x1 != -1 && a2.y1 != -1 && a2.x2 != -1 && a2.y2 != -1)return 3;
	}
	return 0;
}
bool con1(int x1, int y1, int x2, int y2)
{
	if (x1 == x2)
	{
		if (y1 > y2) { int temp = y1; y1 = y2; y2 = temp; }
		if (y1 + 1 == y2)return 1;
		for (int i = y1 + 1; i <= y2 - 1; i++)if (b[x1][i].show == 1)return 0;
		return 1;
	}
	else if (y1 == y2)
	{
		if (x1 > x2) { int temp = x1; x1 = x2; x2 = temp; }
		if (x1 + 1 == x2)return 1;
		for (int i = x1 + 1; i <= x2 - 1; i++)if (b[i][y1].show == 1)return 0;
		return 1;
	}
	return 0;
}
node con2(int x1, int y1, int x2, int y2)
{
	node re;
	re.x1 = re.y1 = -1;
	//if (b[x1][y1].show == 0 || b[x2][y2].show == 0)return re;
	if (b[x1][y2].show == 0 && con1(x1, y1, x1, y2) && con1(x1, y2, x2, y2)) { re.x1 = x1; re.y1 = y2; }
	else if (b[x2][y1].show == 0 && con1(x1, y1, x2, y1) && con1(x2, y1, x2, y2)) { re.x1 = x2; re.y1 = y1; }
	return re;
}
pairnode con3(int x1, int y1, int x2, int y2)
{
	pairnode re;
	re.x1 = re.y1 = re.x2 = re.y2 = -1;
	for (int i = 0; i <= 11; i++)
		if (i != x1 && b[i][y1].show == 0 && con1(i, y1, x1, y1))
		{
			node f = con2(i, y1, x2, y2);
			if (f.x1 != -1 && f.y1 != -1)
			{
				re.x1 = i; re.y1 = y1;
				re.x2 = f.x1; re.y2 = f.y1;
				return re;
			}
		}
	for (int i = 0; i <= 15; i++)
		if (i != y1 && b[x1][i].show == 0 && con1(x1, i, x1, y1))
		{
			node f = con2(x1, i, x2, y2);
			if (f.x1 != -1 && f.y1 != -1)
			{
				re.x1 = x1; re.y1 = i;
				re.x2 = f.x1; re.y2 = f.y1;
				return re;
			}
		}

	return re;
}
void write()
{
	FILE* stream2;
	freopen_s(&stream2, "rankings.txt", "w+", stdout);
	int n = 0;
	for (int i = 0; i < 10; i++)if (a[i].s == 0) { n = i; break; }
	printf("%d\n", n);
	for (int i = 0; i < n; i++)
	{
		int j = 0;
		while (a[i].c[j] != 0)printf("%c", a[i].c[j++]);
		printf(" %d %d\n", a[i].day, a[i].s);
	}
	fclose(stdout);
}