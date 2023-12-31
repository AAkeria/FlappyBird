#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <graphics.h>

// 初始背景图 白天、黑夜
IMAGE BK[2];

// 全局画板
IMAGE bk;

// 飞鸟资源 橘、蓝、红
IMAGE BIRD[3][3];

// 管道资源 绿、红
IMAGE PIPE[2][2];

// 三种数字字体
IMAGE NUMBER[3][10];

// 奖牌资源
IMAGE MEDAL[4];

// 地面资源
IMAGE LAND;

// 开始游戏按钮
IMAGE PLAY;

// 得分面板
IMAGE PANEL;

// 以下为五种游戏文字
// 游戏结束 游戏预备 游戏标题 游戏提示 最高分
IMAGE OVER;
IMAGE READY;
IMAGE TITLE;
IMAGE TUTORIAL;
IMAGE NEW;

// 结束时状态
IMAGE OVEIMG;

// 是否为白天
int isDay = 0;

// 鸟的颜色
int birdColor = 0;

// 最高分
int best = 0;

// 游戏初始时间
long startTime = 0;

// 游戏基本属性

// 重力加速度
const double g = 6.5;
// 管道的移动速度
double vx = 11;
// 鸟的下落速度
double vy = 10;
// 鸟的当前位置
double y = 220;
const double x = 288 / 2 - (41 + 8) / 2 - 80;
// 管道的横向坐标
double pipeX[2];
// 管道的纵向坐标
int pipeY[2];
// 当前得分
int score;
// 鸟的矩形判断区域
const int top = 12;
const int left = 10;
const int right = 37;
const int buttom = 33;

// 奖牌显示位置
const int medalXY[2] = { 0, 0 };

// 陆地坐标
const double landY = 420;
double landX = -20;

// 飞鸟姿态
int pose = 0;

// 飞行间隔
const int diff = 110;

// 管道上补偿
const int pipeUp = -280;
// 管道下补偿
const int pipeDown = 140;

// 游戏预处理
void start();
// 游戏函数
void game();
// 结束游戏函数
void end();
// 移动函数
void move(long time);
// 绘制函数
void draw();
// 重置画布
void reset();
// 绘制画布
void put();
// 加载资源
void loadRes();
// 根据透明度绘图
void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg);
// 绘制分数
void drawScore(int y, int sc, int dif, int se, int st, int type);
// 绘制管道
void drawPipe();
// 判断鸟死亡
bool isDie();
// 初始化游戏资源
void init();


int main()
{
	// 游戏开局页面
	init();
	while (true)
	{
		// 游戏初始化
		start();
		// 游戏进行
		game();
		// 显示得分
		end();
	}
	return 0;
}


bool GetControl() 
{
	bool res = false;

	if (_kbhit()) 
	{
		char ch = _getch();
		if (ch == ' ') 
		{
			res = true;
		}
	}

	MOUSEMSG msg;
	while (MouseHit()) 
	{
		msg = GetMouseMsg();
		if (msg.mkLButton) 
		{
			res = true;
		}
	}

	return res;
}


// 游戏预处理
void start()
{
	// 初始化数据
	isDay = rand() % 2;
	birdColor = rand() % 3;
	pipeX[0] = 288 + 30;
	pipeX[1] = 288 + 30 + 190;
	pipeY[0] = rand() % 250;
	pipeY[1] = rand() % 250;
	pose = 0;
	landX = 0;
	score = 0;
	y = 220;
	vy = 0;

	// 游戏初始时间
	clock_t time = clock();
	// 开场动画
	clock_t t = clock();
	while (true)
	{
		reset();
		drawScore(60, score, 13, 26, 144, 0);
		drawAlpha(&bk, 50, 120, &READY);
		drawAlpha(&bk, (int)x, (int)y, &BIRD[birdColor][pose]);
		drawAlpha(&bk, 90, 220, &TUTORIAL);
		drawAlpha(&bk, (int)landX, (int)landY, &LAND);
		landX -= (clock() - t) * vx / 100;
		t = clock();
		pose = ((clock() - time) / diff) % 3;
		put();
		if (landX < -44)
		{
			landX = -20;
		}
		if (GetControl())
		{
			break;
		}
		Sleep(10);
	}
}


// 游戏函数
void game()
{
	// 根据毫秒
	startTime = clock();
	long time = clock();
	while (!isDie())
	{
		// 移动
		move(clock() - time);
		time = clock();
		// 控制
		if (GetControl())
		{
			vy = -26;
		}
		// 绘制
		draw();
		Sleep(10);	
	}

	startTime = clock();
	while (clock() - startTime < 1000);
	vy = -30;
	time = clock();
	while (y < 520)
	{
		y += (clock() - time) * vy / 100;
		vy += g * (clock() - time) / 100;
		time = clock();
		draw();
		Sleep(10);
	}

	// 备份此时图片
	drawAlpha(&OVEIMG, 0, 0, &bk);
}


void flush()
{
	while (MouseHit())
		GetMouseMsg();

	while (_kbhit())
		_getch();
}


// 结束函数
void end()
{
	// 将面板移出
	startTime = clock();
	// 重置画板
	reset();
	// 面板从下方移出
	drawPipe();
	drawAlpha(&bk, (int)landX, (int)landY, &LAND);
	drawAlpha(&bk, 0, 0, &OVEIMG);
	drawAlpha(&bk, 25, 150, &PANEL);
	// 根据评分打印奖牌
	int r = (score - 50) / 50;
	if (score > 50)
	{
		if (r > 3)
		{
			r = 3;
		}
		drawAlpha(&bk, 57, 195, &MEDAL[r]);
	}
	// 打印当前分
	drawScore(189, score, 16, 16, 240, 2);
	// 写最高分
	int tmp = best;
	if (best < score)
	{
		best = score;
	}
	// 打印最高分
	drawScore(231, best, 16, 16, 240, 2);

	// 如果当前分超过最高分 则显示新分数图标
	if (tmp < score)
	{
		drawAlpha(&bk, 165, 210, &NEW);
	}
	flush();
	put();
	while (true)
	{
		if (GetControl())
		{
			break;
		}
		Sleep(10);
	}
}


void drawScore(int y, int sc, int dif, int se, int st, int type)
{
	// 将分数居中显示
	int t = sc;
	int num[8];
	int length = 0;
	do
	{
		num[length] = t % 10;
		length++;
		t /= 10;
	} while (t != 0);

	// 计算出起始坐标
	int s = st - dif * length;
	for (int i = length - 1; i >= 0; i--)
	{
		drawAlpha(&bk, s, y, &NUMBER[type][num[i]]);
		s += se;
	}
}


// 管道、主角、地面移动
void move(long time)
{
	y += time * vy / 100;
	vy += g * time / 100;
	pipeX[0] -= time * vx / 100;
	pipeX[1] -= time * vx / 100;
	landX -= time * vx / 100;
	pose = ((clock() - startTime) / diff) % 3;
	if (landX < -44)
	{
		landX = -20;
	}
	if (pipeX[0] < -52)
	{
		pipeX[0] = pipeX[1] + 190;
		pipeY[0] = rand() % 250;
	}
	if (pipeX[1] < -52)
	{
		pipeX[1] = pipeX[0] + 190;
		pipeY[1] = rand() % 250;
	}
	if (y < 0 - top)
	{
		y = -top;
	}
	score = (int)(((clock() - startTime) * vx / 100 - (288 - x + 30)) / 190 + 1);
	if (score < 0)
	{
		score = 0;
	}
}


void draw()
{
	// 初始化背景图
	drawAlpha(&bk, 0, 0, &BK[isDay]);
	// 画管道
	drawPipe();
	// 画陆地
	drawAlpha(&bk, (int)landX, (int)landY, &LAND);
	// 画鸟
	drawAlpha(&bk, (int)x, (int)y, &BIRD[birdColor][pose]);
	// 画分数
	drawScore(60, score, 13, 26, 144, 0);
	// 将背景画到窗口上
	put();
}


// 根据透明度绘图
void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	if (dstimg == NULL)
	{
		return;
	}
	// 变量初始化
	DWORD *dst = GetImageBuffer(dstimg);
	DWORD *src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = dstimg->getwidth();
	int dst_height = dstimg->getheight();

	// 实现透明贴图  可优化
	for (int iy = 0; iy < src_height; iy++)
	{
		for (int ix = 0; ix < src_width; ix++)
		{
			int srcX = ix + iy * src_width;
			int sa = ((src[srcX] & 0xff000000) >> 24);
			int sr = ((src[srcX] & 0xff0000) >> 16);
			int sg = ((src[srcX] & 0xff00) >> 8);
			int sb = src[srcX] & 0xff;
			if (x + ix >= 0 && x + ix < dst_width
				&& y + iy >= 0 && y + iy < dst_height)
			{
				int dstX = (x + ix) + (y + iy) * dst_width;
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				dst[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
						  | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
						  |  (sb * sa / 255 + db * (255 - sa) / 255);
			}
		}
	}
}


void drawPipe()
{
	// 画管道
	drawAlpha(&bk, (int)pipeX[0], pipeY[0] + pipeUp, &PIPE[isDay][0]);
	drawAlpha(&bk, (int)pipeX[0], pipeY[0] + pipeDown, &PIPE[isDay][1]);
	// 画管道
	drawAlpha(&bk, (int)pipeX[1], pipeY[1] + pipeUp, &PIPE[isDay][0]);
	drawAlpha(&bk, (int)pipeX[1], pipeY[1] + pipeDown, &PIPE[isDay][1]);
}


// 判断鸟死亡
bool isDie()
{
	if (y + buttom > landY)
		return true;

	if (x + right > pipeX[0] && x + left < pipeX[0] + 52)
	{
		if (y + top < pipeY[0] + 40 || y + buttom > pipeY[0] + 140)
			return true;
	}

	if (x + right > pipeX[1] && x + left < pipeX[1] + 52)
	{
		if (y + top < pipeY[1] + 40 || y + buttom > pipeY[1] + 140)
			return true;
	}

	return false;
}


void reset()
{
	drawAlpha(&bk, 0, 0, &BK[isDay]);
}


void put()
{
	putimage(0, 0, &bk);
}


// 初始化游戏资源
void init()
{
	// 加载图形资源
	loadRes();
	// 初始化图形界面
	initgraph(288, 512);
	// 初始化随机数种子
	srand((unsigned int)time(NULL));
	// 初始化变量
	best = 0;
	isDay = rand() % 2;
	birdColor = rand() % 3;
	// 游戏初始时间
	clock_t time = clock();
	// 开场动画
	while (true)
	{
		reset();
		drawAlpha(&bk, 60, 120, &TITLE);
		drawAlpha(&bk, 125, 200, &BIRD[birdColor][pose]);
		drawAlpha(&bk, 90, 270, &PLAY);
		pose = ((clock() - time) / diff) % 3;
		put();
		if (GetControl())
		{
			break;
		}
		Sleep(10);
	}
}


// 加载图片资源
void loadRes()
{
	loadimage(&BK[0], _T("res\\bg_day.png"));
	loadimage(&BK[1], _T("res\\bg_night.png"));

	loadimage(&bk, _T("res\\bg_day.png"));

	loadimage(&OVEIMG, _T("res\\bg_day.png"));

	loadimage(&BIRD[0][0], _T("res\\bird0_0.png"));
	loadimage(&BIRD[0][1], _T("res\\bird0_1.png"));
	loadimage(&BIRD[0][2], _T("res\\bird0_2.png"));
	loadimage(&BIRD[1][0], _T("res\\bird1_0.png"));
	loadimage(&BIRD[1][1], _T("res\\bird1_1.png"));
	loadimage(&BIRD[1][2], _T("res\\bird1_2.png"));
	loadimage(&BIRD[2][0], _T("res\\bird2_0.png"));
	loadimage(&BIRD[2][1], _T("res\\bird2_1.png"));
	loadimage(&BIRD[2][2], _T("res\\bird2_2.png"));

	loadimage(&PIPE[0][0], _T("res\\pipe_down.png"));
	loadimage(&PIPE[0][1], _T("res\\pipe_up.png"));
	loadimage(&PIPE[1][0], _T("res\\pipe2_down.png"));
	loadimage(&PIPE[1][1], _T("res\\pipe2_up.png"));

	loadimage(&NUMBER[0][0], _T("res\\font_048.png"));
	loadimage(&NUMBER[0][1], _T("res\\font_049.png"));
	loadimage(&NUMBER[0][2], _T("res\\font_050.png"));
	loadimage(&NUMBER[0][3], _T("res\\font_051.png"));
	loadimage(&NUMBER[0][4], _T("res\\font_052.png"));
	loadimage(&NUMBER[0][5], _T("res\\font_053.png"));
	loadimage(&NUMBER[0][6], _T("res\\font_054.png"));
	loadimage(&NUMBER[0][7], _T("res\\font_055.png"));
	loadimage(&NUMBER[0][8], _T("res\\font_056.png"));
	loadimage(&NUMBER[0][9], _T("res\\font_057.png"));

	loadimage(&NUMBER[1][0], _T("res\\number_context_00.png"));
	loadimage(&NUMBER[1][1], _T("res\\number_context_01.png"));
	loadimage(&NUMBER[1][2], _T("res\\number_context_02.png"));
	loadimage(&NUMBER[1][3], _T("res\\number_context_03.png"));
	loadimage(&NUMBER[1][4], _T("res\\number_context_04.png"));
	loadimage(&NUMBER[1][5], _T("res\\number_context_05.png"));
	loadimage(&NUMBER[1][6], _T("res\\number_context_06.png"));
	loadimage(&NUMBER[1][7], _T("res\\number_context_07.png"));
	loadimage(&NUMBER[1][8], _T("res\\number_context_08.png"));
	loadimage(&NUMBER[1][9], _T("res\\number_context_09.png"));

	loadimage(&NUMBER[2][0], _T("res\\number_score_00.png"));
	loadimage(&NUMBER[2][1], _T("res\\number_score_01.png"));
	loadimage(&NUMBER[2][2], _T("res\\number_score_02.png"));
	loadimage(&NUMBER[2][3], _T("res\\number_score_03.png"));
	loadimage(&NUMBER[2][4], _T("res\\number_score_04.png"));
	loadimage(&NUMBER[2][5], _T("res\\number_score_05.png"));
	loadimage(&NUMBER[2][6], _T("res\\number_score_06.png"));
	loadimage(&NUMBER[2][7], _T("res\\number_score_07.png"));
	loadimage(&NUMBER[2][8], _T("res\\number_score_08.png"));
	loadimage(&NUMBER[2][9], _T("res\\number_score_09.png"));

	loadimage(&MEDAL[0], _T("res\\medals_3.png"));
	loadimage(&MEDAL[1], _T("res\\medals_2.png"));
	loadimage(&MEDAL[2], _T("res\\medals_1.png"));
	loadimage(&MEDAL[3], _T("res\\medals_0.png"));

	loadimage(&LAND, _T("res\\land.png"));

	loadimage(&PLAY, _T("res\\button_play.png"));

	loadimage(&OVER, _T("res\\text_game_over.png"));

	loadimage(&READY, _T("res\\text_ready.png"));

	loadimage(&TITLE, _T("res\\title.png"));

	loadimage(&TUTORIAL, _T("res\\tutorial.png"));

	loadimage(&PANEL, _T("res\\score_panel.png"));

	loadimage(&NEW, _T("res\\new.png"));
}