#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <dos.h>
#include <conio.h>

#ifdef __cplusplus
#define __CPPARGS ...
#else
#define __CPPARGS
#endif

#define UP 0x4800
#define DOWN 0x5000
#define LEFT 0x4b00
#define RIGHT 0x4d00
#define ESC 0x011b
#define ENTER 0x1c0d
#define SPACE 0x3920

#define MINBOXsize 10           /*基块尺寸*/
#define NODE_num 100            /*蛇最大节数*/

#define POS_ROW 16              /*墙的位置坐标*/
#define POS_COL 16
#define WALL_ROW 45             /*墙行列数*/
#define WALL_COL 40

enum {_RIGHT=1,_LEFT,_UP,_DOWN};            /*上下左右*/

struct FOOD {
    int x;                  /*食物坐标*/
    int y;
    int color;              /*食物颜色*/
    int flag;               /*是否需要食物*/
} food;

struct SNAKE {
    int x[NODE_num];
    int y[NODE_num];
    int node;               /*蛇的节数*/
    int dir;                /*蛇移动的方向*/
    int color;
    int life;               /*蛇的死活*/
} snake;

char *nm="\0\0\0\0";                /*score*/
char *lev="\0\0";                   /*level*/
char*Bye="GOODBYE!";
char*Over="GAMEOVER!";
int num=0;
int level=0;
int Ide=0;                          /*选项*/
int Key=0;                          /*按键*/
int speedlev[3]= {200,100,20};      /*蛇速度级别*/

void init(void);                    /*界面图形化*/
void drawbkg(void);                 /*背景绘制*/
void dwall(void);                   /*绘制墙*/
void wall(int, int);                /*绘制砖块*/
void dsidebar(void);                /*绘制侧栏*/
/*游戏中更新侧栏*/
void updatesidebar(int new_scr,int new_lev);
void chlevel(void);                 /*选择级别*/
void menu(int n);                   /*选级菜单*/
/*打印文字*/
void texts(int a,int b,char *texts,int font,int color);
/*清屏*/
void clscr(int a,int b,int c,int d,int color);
void foodpos(int*pa,int*pb);        /*食物位置生成*/
/*食物绘制*/
void foodraw(int x,int y,int color);
void space(void);                   /*暂停游戏*/
void menusp(int n);                 /*暂停菜单*/
void play(void);                    /*开始游戏*/
void Exit(char*);                   /*多功能退出*/

int main(void)
{
    init();
    drawbkg();
    chlevel();
    play();                         /*进入游戏*/
    Exit(Over);                     /*游戏结束退出*/
    cleardevice();                  /*清屏*/
    closegraph();                   /*图形化结束*/

    return 0;
}

void init(void)
{
    int gdriver=DETECT,gmode,errorcode;

    /*登陆图形驱动程序*/
    registerbgidriver(EGAVGA_driver);
    /*初始化图形系统*/
    initgraph(&gdriver,&gmode,"");

    /*图形化失败提示*/
    errorcode=graphresult();
    if(errorcode!=grOk) {
        printf("Graphics error: %s\n", grapherrormsg(errorcode));
        printf("Press any key to halt:");
        getch();
        exit(1);
    }
}

void drawbkg(void)
{
    cleardevice();
    dwall();                    /*画墙*/
    dsidebar();                 /*画侧栏*/
}

void dwall(void)
{
    int i;

    /*上下墙绘制*/
    for(i=POS_ROW; i<POS_ROW+MINBOXsize*WALL_COL; i+=MINBOXsize) {
        wall(i,POS_COL);
        wall(i,POS_COL+MINBOXsize*(WALL_ROW-1));
    }

    /*左右墙绘制*/
    for(i=POS_COL+MINBOXsize; i<POS_COL+MINBOXsize*(WALL_ROW-1); i+=MINBOXsize) {
        wall(POS_ROW,i);
        wall(POS_ROW+MINBOXsize*(WALL_COL-1),i);
    }
}

void wall(int x,int y)
{
    int sizx=9;
    int sizy=9;

    setcolor(15);                                   /*白色画笔*/
    line(x,y,x+sizx,y);                             /*画砖的上边和左边*/
    line(x,y+1,x+sizx-1,y+1);
    line(x,y,x,y+sizy);
    line(x+1,y,x+1,y+sizy-1);
    setcolor(8);                                    /*深灰色画笔*/
    line(x+1,y+sizy,x+sizx,y+sizy);                 /*画砖的下边和右边*/
    line(x+2,y+sizy-1,x+sizx,y+sizy-1);
    line(x+sizx-1,y+2,x+sizx-1,y+sizy-1);
    line(x+sizx,y+1,x+sizx,y+sizy);
    setfillstyle(1,7);                              /*淡灰色实线填充砖面*/
    bar(x+2,y+2,x+sizx-2,y+sizy-2);                 /*绘制二维条形图的函数*/
}

void dsidebar(void)
{
    int sidx=WALL_COL*10+30;
    texts(sidx+20,20,"SNAKE",4,2);                  /*大Logo*/
    texts(sidx+30,100,"LEVEL:",1,15);               /*等级和分数*/
    texts(sidx+50,120,lev,1,15);
    texts(sidx+30,160,"SCORE:",1,15);
    texts(sidx+50,180,nm,1,15);
    texts(sidx,250,"HELP:",1,15);                   /*帮助栏*/
    texts(sidx+20,270,"Arrow keys:",1,15);
    texts(sidx+60,290,"  Control snake",1,15);
    texts(sidx+20,310,"Esc:   Exit",1,15);
    texts(sidx+20,330,"Space: Suspend/resume",1,15);
}

void chlevel(void)
{
    /*墙内清屏*/
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);

    delay(20);                  /*选级框稍延迟后放出*/
    clscr(170,190,250,290,7);   /*绘制选级框背景*/
    menu(Ide);                  /*上下选级的控制*/
    do {
        if(bioskey(1)) {
            Key=bioskey(0);
            switch(Key) {
            case DOWN:
                Ide++;
                Ide%=4;
                menu(Ide);
                break;
            case UP:
                Ide--;
                Ide=(Ide+4)%4;  /*避免负数*/
                menu(Ide);
                break;
            }
        }
    } while(Key!=ENTER);        /*按下回车即退出循环*/

    if(Ide==3)                  /*若选择Exit*/
        Exit(Bye);              /*未完成游戏退出*/
    else
        level=Ide+1;            /*游戏等级*/

}

void menu(int n)
{
    int i;
    char *item[4]= {"LEVEL 1","LEVEL 2","LEVEL 3","EXIT"};

    for(i=0; i<4; i++)
        texts(180,205+20*i,item[i],1,GREEN);
    /*当前项为红色*/
    texts(180,205+20*n,item[n],1,RED);
}

void texts(int a,int b,char *texts,int font,int color)
{
    setcolor(color);
    settextstyle(0,0,font);         /*设置字体大小*/
    outtextxy(a,b,texts);
}

void clscr(int a,int b,int c,int d,int color)
{
    setfillstyle(SOLID_FILL,color); /*设置填充方式*/
    setcolor(color);
    bar(a,b,c,d);
}

void updatesidebar(int new_scr,int new_lev)
{
    int x=WALL_COL*10+30;
    sprintf(nm,"%d",new_scr);           /*把数字写入字符串*/
    sprintf(lev,"%d",new_lev);
    clscr(x+50,110,x+200,135,0);        /*擦除原纪录*/
    clscr(x+50,170,x+200,195,0);
    texts(x+50,120,lev,1,15);           /*更新记录*/
    texts(x+50,180,nm,1,15);
}

/*时间中断定义*/
#define TIMER 0x1c
int TimerCounter=0;
void interrupt( *oldhandler)(__CPPARGS);

void interrupt newhandler(__CPPARGS)
{
    TimerCounter++;
    oldhandler();
}
void SetTimer(void interrupt (*IntProc)(__CPPARGS))
{
    oldhandler=getvect(TIMER);
    disable();
    setvect(TIMER,IntProc);
    enable();
}

/*时间中断结束*/
void KillTimer()
{
    disable();
    setvect(TIMER,oldhandler);
    enable();
}

void play(void)                             /*游戏实现的函数*/
{
    int i,key=0;
    int poly[8];                            /*储存蛇单节坐标的数组*/

    updatesidebar(num,level);               /*游戏前更新侧栏等级分数数据*/
    srand((unsigned long)time(0));
    /*墙内清屏*/
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);
    food.flag=1;                            /*1表示需要新食物,0表示已存在食物*/
    food.color=GREEN;                       /*食物初始颜色为绿色*/
    snake.life=1;                           /*标示贪吃蛇活着*/
    snake.dir=_RIGHT;                       /*蛇初始方向为右*/
    snake.x[0]=100+POS_ROW;                 /*蛇的初始位置*/
    snake.y[0]=100+POS_COL;
    snake.x[1]=100+POS_ROW;
    snake.y[1]=100+POS_COL;
    snake.node=2;                           /*蛇的初始节数为2*/
    snake.color=3;                          /*蛇的初始颜色*/
    do {
        while(!kbhit()) {                   /*在没有按键的情况下蛇自行移动*/
            if(food.flag==1)                /*如果需要新食物,则随机产生新的食物数据*/
                foodpos(&(food.x),&(food.y));
            if(food.flag==0)                /*如果已有食物数据,则画出食物*/
                foodraw(food.x,food.y,food.color);
            for(i=snake.node-1; i>0; i--) { /*实现蛇的移动*/
                snake.x[i]=snake.x[i-1];
                snake.y[i]=snake.y[i-1];
            }

            switch(snake.dir) {             /*蛇头按方向移动*/
            case _RIGHT:
                snake.x[0]+=MINBOXsize;
                break;
            case _LEFT:
                snake.x[0]-=MINBOXsize;
                break;
            case _UP:
                snake.y[0]-=MINBOXsize;
                break;
            case _DOWN:
                snake.y[0]+=MINBOXsize;
                break;
            }

            for(i=3; i<snake.node; i++) {
                /*如果蛇吃到自己*/
                if(snake.x[i]==snake.x[0]&&snake.y[i]==snake.y[0]) {
                    Exit(Over);             /*游戏结束*/
                    snake.life=0;           /*蛇死*/
                    break;
                }
            }

            /*如果蛇撞到墙壁*/
            if(snake.x[0]<POS_ROW+MINBOXsize||snake.x[0]>POS_ROW+MINBOXsize*(WALL_COL-2)||snake.y[0]<POS_COL+MINBOXsize||
                    snake.y[0]>POS_COL+MINBOXsize*(WALL_ROW-2)) {
                Exit(Over);                 /*游戏结束*/
                snake.life=0;               /*蛇死*/
                break;
            }

            /*如果蛇吃到食物*/
            if(snake.x[0]==food.x&&snake.y[0]==food.y) {
                setcolor(0);                            /*用背景色遮盖调食物*/
                rectangle(food.x,food.y,food.x+10,food.y+10);
                snake.node++;                           /*蛇的身体长一节*/
                food.flag=1;                            /*需要出现新的食物*/
                snake.color=food.color;                 /*蛇的颜色变为吃掉的食物的颜色*/
                food.color=rand()%6+9;                  /*产生新食物的颜色*/
                num+=10;                                /*吃掉一个食物得十分*/
                updatesidebar(num,level);               /*更新侧栏数据*/
            }

            /*画蛇*/
            setcolor(snake.color);                      /*设置蛇身颜色*/
            setlinestyle(0,0,1);
            setfillstyle(1,snake.color-8);              /*蛇边缘的颜色为蛇身颜色的淡色*/
            for(i=0; i<snake.node; i++) {
                poly[0]=snake.x[i];                     /*将蛇节坐标存入数组*/
                poly[1]=snake.y[i];
                poly[2]=snake.x[i]+MINBOXsize;
                poly[3]=snake.y[i];
                poly[4]=snake.x[i]+MINBOXsize;
                poly[5]=snake.y[i]+MINBOXsize;
                poly[6]=snake.x[i];
                poly[7]=snake.y[i]+MINBOXsize;
                fillpoly(4,poly);                       /*画蛇的一小节*/
            }

            delay(speedlev[level-1]);                   /*速度控制*/
            /*用背景色擦去蛇尾*/
            clscr(snake.x[snake.node-1],snake.y[snake.node-1],
                  snake.x[snake.node-1]+MINBOXsize,snake.y[snake.node-1]+MINBOXsize,0);
        }
        /*endwhile（！kbhit）*/

        if(snake.life==0)                               /*如果蛇死就跳出循环*/
            break;
        key=bioskey(0);                                 /*接收按键*/
        if(key==UP&&snake.dir!=_DOWN)                   /*判断是否往相反的方向移动*/
            snake.dir=_UP;
        else if(key==DOWN&&snake.dir!=_UP)              /*判断是否往相反的方向移动*/
            snake.dir=_DOWN;
        else if(key==RIGHT&&snake.dir!=_LEFT)           /*判断是否往相反的方向移动*/
            snake.dir=_RIGHT;
        else if(key==LEFT&&snake.dir!=_RIGHT)           /*判断是否往相反的方向移动*/
            snake.dir=_LEFT;
        else if(key==SPACE)
            space();

    } while(key!=ESC);                                  /*ESC键退出游戏*/
}
void foodpos(int*pa,int*pb)
{
    int i,jdg=1;
    do {
        /*保证食物在墙内*/
        *pa=(rand()%(MINBOXsize*(WALL_COL-2)))/10*10+POS_ROW+MINBOXsize;
        *pb=(rand()%(MINBOXsize*(WALL_ROW-2)))/10*10+POS_COL+MINBOXsize;

        /*保证食物不出现在蛇身上*/
        for(i=snake.node-1; i>0; i--)
            if((*pa==snake.x[i])&&(*pb==snake.y[i]))
                jdg=0;
    } while(jdg==0);
    food.flag=0;
}

void foodraw(int x,int y,int color)
{
    setcolor(color);
    setlinestyle(3,0,3);                                /*点划线*/
    rectangle(x,y,x+MINBOXsize,y+MINBOXsize);           /*画矩形的函数*/
}

/*游戏暂停*/
void space(void)
{
    int ide=0;
    int KEY=0;
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);

    delay(20);
    clscr(170,220,250,280,7);                   /*绘制弹框背景*/
    menusp(ide);
    do {
        if(bioskey(1)) {
            KEY=bioskey(0);
            switch(KEY) {
            case DOWN:
                ide++;
                ide%=2;
                menusp(ide);
                break;
            case UP:
                ide--;
                ide=(ide+2)%2;
                menusp(ide);
                break;
            }
        }
    } while(KEY!=ENTER);

    if(ide==1)
        Exit(Bye);
    else
        /*清楚暂停选框*/
        clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
              POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
              ,0);
}
/*绘制暂停选框*/
void menusp(int n)
{
    int i;
    char *item[2]= {"Continue","Exit"};

    for(i=0; i<2; i++)
        texts(180,235+20*i,item[i],1,GREEN);
    texts(180,235+20*n,item[n],1,RED);
}

void Exit(char*str)
{
    int midx=(2*POS_ROW+MINBOXsize*WALL_COL)/2;

    SetTimer(newhandler);                   /*时间中断开始*/
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);
    settextstyle(0,0,4);

    while(!kbhit()) {
        if(TimerCounter>50)                 /*退出界面维持一定时间*/
            break;
        setcolor(rand()%3+2);
        outtextxy(midx-120,210,str);

        delay(100);                         /*变色间隔*/
    }
    KillTimer(newhandler);                  /*关闭时间中断*/
    cleardevice();                          /*清屏*/
    closegraph();                           /*图形化终止*/
    exit(0);
}
