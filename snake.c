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

#define MINBOXsize 10           /*����ߴ�*/
#define NODE_num 100            /*��������*/

#define POS_ROW 16              /*ǽ��λ������*/
#define POS_COL 16
#define WALL_ROW 45             /*ǽ������*/
#define WALL_COL 40

enum {_RIGHT=1,_LEFT,_UP,_DOWN};            /*��������*/

struct FOOD {
    int x;                  /*ʳ������*/
    int y;
    int color;              /*ʳ����ɫ*/
    int flag;               /*�Ƿ���Ҫʳ��*/
} food;

struct SNAKE {
    int x[NODE_num];
    int y[NODE_num];
    int node;               /*�ߵĽ���*/
    int dir;                /*���ƶ��ķ���*/
    int color;
    int life;               /*�ߵ�����*/
} snake;

char *nm="\0\0\0\0";                /*score*/
char *lev="\0\0";                   /*level*/
char*Bye="GOODBYE!";
char*Over="GAMEOVER!";
int num=0;
int level=0;
int Ide=0;                          /*ѡ��*/
int Key=0;                          /*����*/
int speedlev[3]= {200,100,20};      /*���ٶȼ���*/

void init(void);                    /*����ͼ�λ�*/
void drawbkg(void);                 /*��������*/
void dwall(void);                   /*����ǽ*/
void wall(int, int);                /*����ש��*/
void dsidebar(void);                /*���Ʋ���*/
/*��Ϸ�и��²���*/
void updatesidebar(int new_scr,int new_lev);
void chlevel(void);                 /*ѡ�񼶱�*/
void menu(int n);                   /*ѡ���˵�*/
/*��ӡ����*/
void texts(int a,int b,char *texts,int font,int color);
/*����*/
void clscr(int a,int b,int c,int d,int color);
void foodpos(int*pa,int*pb);        /*ʳ��λ������*/
/*ʳ�����*/
void foodraw(int x,int y,int color);
void space(void);                   /*��ͣ��Ϸ*/
void menusp(int n);                 /*��ͣ�˵�*/
void play(void);                    /*��ʼ��Ϸ*/
void Exit(char*);                   /*�๦���˳�*/

int main(void)
{
    init();
    drawbkg();
    chlevel();
    play();                         /*������Ϸ*/
    Exit(Over);                     /*��Ϸ�����˳�*/
    cleardevice();                  /*����*/
    closegraph();                   /*ͼ�λ�����*/

    return 0;
}

void init(void)
{
    int gdriver=DETECT,gmode,errorcode;

    /*��½ͼ����������*/
    registerbgidriver(EGAVGA_driver);
    /*��ʼ��ͼ��ϵͳ*/
    initgraph(&gdriver,&gmode,"");

    /*ͼ�λ�ʧ����ʾ*/
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
    dwall();                    /*��ǽ*/
    dsidebar();                 /*������*/
}

void dwall(void)
{
    int i;

    /*����ǽ����*/
    for(i=POS_ROW; i<POS_ROW+MINBOXsize*WALL_COL; i+=MINBOXsize) {
        wall(i,POS_COL);
        wall(i,POS_COL+MINBOXsize*(WALL_ROW-1));
    }

    /*����ǽ����*/
    for(i=POS_COL+MINBOXsize; i<POS_COL+MINBOXsize*(WALL_ROW-1); i+=MINBOXsize) {
        wall(POS_ROW,i);
        wall(POS_ROW+MINBOXsize*(WALL_COL-1),i);
    }
}

void wall(int x,int y)
{
    int sizx=9;
    int sizy=9;

    setcolor(15);                                   /*��ɫ����*/
    line(x,y,x+sizx,y);                             /*��ש���ϱߺ����*/
    line(x,y+1,x+sizx-1,y+1);
    line(x,y,x,y+sizy);
    line(x+1,y,x+1,y+sizy-1);
    setcolor(8);                                    /*���ɫ����*/
    line(x+1,y+sizy,x+sizx,y+sizy);                 /*��ש���±ߺ��ұ�*/
    line(x+2,y+sizy-1,x+sizx,y+sizy-1);
    line(x+sizx-1,y+2,x+sizx-1,y+sizy-1);
    line(x+sizx,y+1,x+sizx,y+sizy);
    setfillstyle(1,7);                              /*����ɫʵ�����ש��*/
    bar(x+2,y+2,x+sizx-2,y+sizy-2);                 /*���ƶ�ά����ͼ�ĺ���*/
}

void dsidebar(void)
{
    int sidx=WALL_COL*10+30;
    texts(sidx+20,20,"SNAKE",4,2);                  /*��Logo*/
    texts(sidx+30,100,"LEVEL:",1,15);               /*�ȼ��ͷ���*/
    texts(sidx+50,120,lev,1,15);
    texts(sidx+30,160,"SCORE:",1,15);
    texts(sidx+50,180,nm,1,15);
    texts(sidx,250,"HELP:",1,15);                   /*������*/
    texts(sidx+20,270,"Arrow keys:",1,15);
    texts(sidx+60,290,"  Control snake",1,15);
    texts(sidx+20,310,"Esc:   Exit",1,15);
    texts(sidx+20,330,"Space: Suspend/resume",1,15);
}

void chlevel(void)
{
    /*ǽ������*/
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);

    delay(20);                  /*ѡ�������ӳٺ�ų�*/
    clscr(170,190,250,290,7);   /*����ѡ���򱳾�*/
    menu(Ide);                  /*����ѡ���Ŀ���*/
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
                Ide=(Ide+4)%4;  /*���⸺��*/
                menu(Ide);
                break;
            }
        }
    } while(Key!=ENTER);        /*���»س����˳�ѭ��*/

    if(Ide==3)                  /*��ѡ��Exit*/
        Exit(Bye);              /*δ�����Ϸ�˳�*/
    else
        level=Ide+1;            /*��Ϸ�ȼ�*/

}

void menu(int n)
{
    int i;
    char *item[4]= {"LEVEL 1","LEVEL 2","LEVEL 3","EXIT"};

    for(i=0; i<4; i++)
        texts(180,205+20*i,item[i],1,GREEN);
    /*��ǰ��Ϊ��ɫ*/
    texts(180,205+20*n,item[n],1,RED);
}

void texts(int a,int b,char *texts,int font,int color)
{
    setcolor(color);
    settextstyle(0,0,font);         /*���������С*/
    outtextxy(a,b,texts);
}

void clscr(int a,int b,int c,int d,int color)
{
    setfillstyle(SOLID_FILL,color); /*������䷽ʽ*/
    setcolor(color);
    bar(a,b,c,d);
}

void updatesidebar(int new_scr,int new_lev)
{
    int x=WALL_COL*10+30;
    sprintf(nm,"%d",new_scr);           /*������д���ַ���*/
    sprintf(lev,"%d",new_lev);
    clscr(x+50,110,x+200,135,0);        /*����ԭ��¼*/
    clscr(x+50,170,x+200,195,0);
    texts(x+50,120,lev,1,15);           /*���¼�¼*/
    texts(x+50,180,nm,1,15);
}

/*ʱ���ж϶���*/
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

/*ʱ���жϽ���*/
void KillTimer()
{
    disable();
    setvect(TIMER,oldhandler);
    enable();
}

void play(void)                             /*��Ϸʵ�ֵĺ���*/
{
    int i,key=0;
    int poly[8];                            /*�����ߵ������������*/

    updatesidebar(num,level);               /*��Ϸǰ���²����ȼ���������*/
    srand((unsigned long)time(0));
    /*ǽ������*/
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);
    food.flag=1;                            /*1��ʾ��Ҫ��ʳ��,0��ʾ�Ѵ���ʳ��*/
    food.color=GREEN;                       /*ʳ���ʼ��ɫΪ��ɫ*/
    snake.life=1;                           /*��ʾ̰���߻���*/
    snake.dir=_RIGHT;                       /*�߳�ʼ����Ϊ��*/
    snake.x[0]=100+POS_ROW;                 /*�ߵĳ�ʼλ��*/
    snake.y[0]=100+POS_COL;
    snake.x[1]=100+POS_ROW;
    snake.y[1]=100+POS_COL;
    snake.node=2;                           /*�ߵĳ�ʼ����Ϊ2*/
    snake.color=3;                          /*�ߵĳ�ʼ��ɫ*/
    do {
        while(!kbhit()) {                   /*��û�а�����������������ƶ�*/
            if(food.flag==1)                /*�����Ҫ��ʳ��,����������µ�ʳ������*/
                foodpos(&(food.x),&(food.y));
            if(food.flag==0)                /*�������ʳ������,�򻭳�ʳ��*/
                foodraw(food.x,food.y,food.color);
            for(i=snake.node-1; i>0; i--) { /*ʵ���ߵ��ƶ�*/
                snake.x[i]=snake.x[i-1];
                snake.y[i]=snake.y[i-1];
            }

            switch(snake.dir) {             /*��ͷ�������ƶ�*/
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
                /*����߳Ե��Լ�*/
                if(snake.x[i]==snake.x[0]&&snake.y[i]==snake.y[0]) {
                    Exit(Over);             /*��Ϸ����*/
                    snake.life=0;           /*����*/
                    break;
                }
            }

            /*�����ײ��ǽ��*/
            if(snake.x[0]<POS_ROW+MINBOXsize||snake.x[0]>POS_ROW+MINBOXsize*(WALL_COL-2)||snake.y[0]<POS_COL+MINBOXsize||
                    snake.y[0]>POS_COL+MINBOXsize*(WALL_ROW-2)) {
                Exit(Over);                 /*��Ϸ����*/
                snake.life=0;               /*����*/
                break;
            }

            /*����߳Ե�ʳ��*/
            if(snake.x[0]==food.x&&snake.y[0]==food.y) {
                setcolor(0);                            /*�ñ���ɫ�ڸǵ�ʳ��*/
                rectangle(food.x,food.y,food.x+10,food.y+10);
                snake.node++;                           /*�ߵ����峤һ��*/
                food.flag=1;                            /*��Ҫ�����µ�ʳ��*/
                snake.color=food.color;                 /*�ߵ���ɫ��Ϊ�Ե���ʳ�����ɫ*/
                food.color=rand()%6+9;                  /*������ʳ�����ɫ*/
                num+=10;                                /*�Ե�һ��ʳ���ʮ��*/
                updatesidebar(num,level);               /*���²�������*/
            }

            /*����*/
            setcolor(snake.color);                      /*����������ɫ*/
            setlinestyle(0,0,1);
            setfillstyle(1,snake.color-8);              /*�߱�Ե����ɫΪ������ɫ�ĵ�ɫ*/
            for(i=0; i<snake.node; i++) {
                poly[0]=snake.x[i];                     /*���߽������������*/
                poly[1]=snake.y[i];
                poly[2]=snake.x[i]+MINBOXsize;
                poly[3]=snake.y[i];
                poly[4]=snake.x[i]+MINBOXsize;
                poly[5]=snake.y[i]+MINBOXsize;
                poly[6]=snake.x[i];
                poly[7]=snake.y[i]+MINBOXsize;
                fillpoly(4,poly);                       /*���ߵ�һС��*/
            }

            delay(speedlev[level-1]);                   /*�ٶȿ���*/
            /*�ñ���ɫ��ȥ��β*/
            clscr(snake.x[snake.node-1],snake.y[snake.node-1],
                  snake.x[snake.node-1]+MINBOXsize,snake.y[snake.node-1]+MINBOXsize,0);
        }
        /*endwhile����kbhit��*/

        if(snake.life==0)                               /*�������������ѭ��*/
            break;
        key=bioskey(0);                                 /*���հ���*/
        if(key==UP&&snake.dir!=_DOWN)                   /*�ж��Ƿ����෴�ķ����ƶ�*/
            snake.dir=_UP;
        else if(key==DOWN&&snake.dir!=_UP)              /*�ж��Ƿ����෴�ķ����ƶ�*/
            snake.dir=_DOWN;
        else if(key==RIGHT&&snake.dir!=_LEFT)           /*�ж��Ƿ����෴�ķ����ƶ�*/
            snake.dir=_RIGHT;
        else if(key==LEFT&&snake.dir!=_RIGHT)           /*�ж��Ƿ����෴�ķ����ƶ�*/
            snake.dir=_LEFT;
        else if(key==SPACE)
            space();

    } while(key!=ESC);                                  /*ESC���˳���Ϸ*/
}
void foodpos(int*pa,int*pb)
{
    int i,jdg=1;
    do {
        /*��֤ʳ����ǽ��*/
        *pa=(rand()%(MINBOXsize*(WALL_COL-2)))/10*10+POS_ROW+MINBOXsize;
        *pb=(rand()%(MINBOXsize*(WALL_ROW-2)))/10*10+POS_COL+MINBOXsize;

        /*��֤ʳ�ﲻ������������*/
        for(i=snake.node-1; i>0; i--)
            if((*pa==snake.x[i])&&(*pb==snake.y[i]))
                jdg=0;
    } while(jdg==0);
    food.flag=0;
}

void foodraw(int x,int y,int color)
{
    setcolor(color);
    setlinestyle(3,0,3);                                /*�㻮��*/
    rectangle(x,y,x+MINBOXsize,y+MINBOXsize);           /*�����εĺ���*/
}

/*��Ϸ��ͣ*/
void space(void)
{
    int ide=0;
    int KEY=0;
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);

    delay(20);
    clscr(170,220,250,280,7);                   /*���Ƶ��򱳾�*/
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
        /*�����ͣѡ��*/
        clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
              POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
              ,0);
}
/*������ͣѡ��*/
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

    SetTimer(newhandler);                   /*ʱ���жϿ�ʼ*/
    clscr(POS_ROW+MINBOXsize,POS_COL+MINBOXsize,
          POS_ROW+MINBOXsize*(WALL_COL-1),POS_COL+MINBOXsize*(WALL_ROW-1)
          ,0);
    settextstyle(0,0,4);

    while(!kbhit()) {
        if(TimerCounter>50)                 /*�˳�����ά��һ��ʱ��*/
            break;
        setcolor(rand()%3+2);
        outtextxy(midx-120,210,str);

        delay(100);                         /*��ɫ���*/
    }
    KillTimer(newhandler);                  /*�ر�ʱ���ж�*/
    cleardevice();                          /*����*/
    closegraph();                           /*ͼ�λ���ֹ*/
    exit(0);
}
