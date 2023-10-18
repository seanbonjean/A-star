/**
 * @file Astar.c
 * @author Sean Bonjean (sean-bonjean@outlook.com)
 * @brief A*算法 C语言实现
 * @version 0.1
 * @date 2023-04-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

// 地图尺寸
#define MAP_WIDTH 51
#define MAP_HEIGHT 31
// 起始节点坐标
#define START_X 5
#define START_Y 6
// 目标节点坐标
#define TARGET_X 45
#define TARGET_Y 25

typedef struct node
{
    int x;
    int y;
    struct node *parent;
    char action;
    int pathCost;
    int distance;
    int evolution;
    struct node *next;
} Node;

Node *open = NULL;
Node *closed = NULL;

char map[MAP_HEIGHT][MAP_WIDTH] = {0};

int createNode(Node **, int, int, Node *, char);
int insNode(Node *, Node *);
int popNode(Node *, Node **);

int buildMap(void);
int expand(Node *);

void FullScreen(void);
void moveCursor(int, int);
int printPoint(int, int, char);
int printMap(void);

int main(void)
{
    Node *p = NULL;

    printf("Make sure this window is fullscreened. \n");
    FullScreen();
    system("pause");
    system("cls");

    buildMap();
    printMap();
    printPoint(START_X, START_Y, '&');
    printPoint(TARGET_X, TARGET_Y, '$');

    // 算法开始
    // 创建open表和closed表
    createNode(&open, 0, 0, NULL, 0);
    createNode(&closed, 0, 0, NULL, 0);

    // open表插入起始节点
    createNode(&p, START_X, START_Y, NULL, 0);
    insNode(open, p);

    // open表为空则搜索失败
    while (open->next != NULL)
    {
        popNode(open, &p);
        insNode(closed, p); // 其实没必要按估价函数值降序插入，但因为我懒得单独写一个函数了，就这样用吧

        // 若为目标节点
        if (0 == p->distance)
        {
            while (p->pathCost != 0)
            {
                p = p->parent;
                printPoint(p->x, p->y, '@');
                Sleep(10);
            }
            printPoint(START_X, START_Y, '&');
            printPoint(TARGET_X, TARGET_Y, '$');
            moveCursor(0, MAP_HEIGHT);
            printf("search success\n");
            break;
        }
        else
        {
            expand(p);
            printPoint(p->x, p->y, '*');
            printPoint(START_X, START_Y, '&');
            printPoint(TARGET_X, TARGET_Y, '$');
            Sleep(20);
        }
    }

    system("pause");
    return 0;
}

/**
 * @brief 创建节点
 *
 * @param p 创建的节点指针
 * @param x 坐标x
 * @param y 坐标y
 * @param parent 父节点指针
 * @param action 父节点经过何种操作达到本节点状态
 * @return int 0表示正常退出
 */
int createNode(Node **p, int x, int y, Node *parent, char action)
{
    *p = malloc(sizeof(Node));
    if (NULL == *p)
    {
        system("cls");
        printf("No enough memory. exiting~\n");
        system("pause");
        exit(1);
    }
    (*p)->x = x;
    (*p)->y = y;
    (*p)->parent = parent;
    (*p)->action = action;
    if (parent == NULL)
        (*p)->pathCost = 0;
    else
        (*p)->pathCost = parent->pathCost + 1;
    (*p)->distance = abs(TARGET_X - x) + abs(TARGET_Y - y);
    (*p)->evolution = (*p)->pathCost + (*p)->distance;
    (*p)->next = NULL;

    return 0;
}

/**
 * @brief 按估价函数值降序插入节点
 *
 * @param head 头节点指针
 * @param ins 待插入节点指针
 * @return int 0表示正常退出
 */
int insNode(Node *head, Node *ins)
{
    Node *p1, *p2;

    p1 = head;
    p2 = p1->next;
    while (p2 != NULL)
    {
        if (p2->evolution < ins->evolution)
            break;
        p1 = p2;
        p2 = p2->next;
    }

    ins->next = p2;
    p1->next = ins;

    return 0;
}

/**
 * @brief 获取并删除链表尾节点
 *
 * @param head 头节点指针
 * @param pop 获得的节点
 * @return int 0表示正常退出，1表示链表为空无法获取
 */
int popNode(Node *head, Node **pop)
{
    Node *p1, *p2;

    p1 = head;
    p2 = p1->next;
    if (NULL == p2)
        return 1;

    while (p2->next != NULL)
    {
        p1 = p2;
        p2 = p2->next;
    }

    p1->next = NULL;
    *pop = p2;

    return 0;
}

int buildMap(void)
{
    // 生成地图轮廓
    for (char i = 0; i < MAP_WIDTH; i++)
    {
        map[0][i] = 1;
        map[MAP_HEIGHT - 1][i] = 1;
    }
    for (char i = 0; i < MAP_HEIGHT; i++)
    {
        map[i][0] = 1;
        map[i][MAP_WIDTH - 1] = 1;
    }
    // 生成地形
    for (char i = 1; i < 16; i++)
    {
        map[i][20] = 1;
        map[i][40] = 1;
    }
    for (char i = 10; i < 20; i++)
        map[15][i] = 1;
    for (char i = 15; i < MAP_HEIGHT; i++)
        map[i][30] = 1;

    return 0;
}

int expand(Node *n)
{
    int x, y;
    int count = 0;
    Node *p1 = NULL, *p2 = NULL;
    char needAdd = 0;

    for (char action = 1; action < 5; action++)
    {
        count = 0;
        needAdd = 0;

        switch (action)
        {
        case 1:
            x = n->x;
            y = n->y + 1;
            break;

        case 2:
            x = n->x + 1;
            y = n->y;
            break;

        case 3:
            x = n->x;
            y = n->y - 1;
            break;

        case 4:
            x = n->x - 1;
            y = n->y;
            break;
        }

        if (!map[y][x])
        {
            // 搜索open表
            p1 = open;
            p2 = p1->next;
            while (p2 != NULL)
            {
                if (p2->x == x && p2->y == y)
                {
                    // 如果新节点代价更小
                    if (p2->pathCost > n->pathCost + 1)
                    {
                        p1->next = p2->next;
                        needAdd = 1;
                    }
                    break;
                }
                p1 = p2;
                p2 = p2->next;
            }
            if (NULL == p2)
                count++;

            // 搜索closed表
            p1 = closed;
            p2 = p1->next;
            while (p2 != NULL)
            {
                if (p2->x == x && p2->y == y)
                {
                    // 如果新节点代价更小
                    if (p2->pathCost > n->pathCost + 1)
                    {
                        p1->next = p2->next;
                        needAdd = 1;
                    }
                    break;
                }
                p1 = p2;
                p2 = p2->next;
            }
            if (NULL == p2)
                count++;

            if (2 == count)
                needAdd = 1;
        }

        if (needAdd)
        {
            createNode(&p1, x, y, n, action);
            insNode(open, p1);
            printPoint(x, y, '^');
        }
    }
}

// 控制台全屏
void FullScreen(void)
{
    HWND hwnd = GetForegroundWindow();
    int x = GetSystemMetrics(SM_CXSCREEN) + 300;
    int y = GetSystemMetrics(SM_CYSCREEN) + 300;
    char setting[30];
    sprintf(setting, "mode con:cols=%d lines=%d", x, y);
    system(setting);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, x + 300, y + 300, 0);
    MoveWindow(hwnd, -10, -40, x + 300, y + 300, 1);
    printf("\n\n");
}

// 光标移动到坐标处
void moveCursor(int x, int y)
{
    HANDLE hOut;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(hOut, pos);
}

// 按坐标绘制点
int printPoint(int x, int y, char ch)
{
    moveCursor(x * 2, MAP_HEIGHT - y - 1);
    printf("%c ", ch);
    return 0;
}

int printMap(void)
{
    for (char i = MAP_HEIGHT - 1; i >= 0; i--)
    {
        for (char j = 0; j < MAP_WIDTH; j++)
        {
            if (map[i][j])
                printf("# ");
            else
                printf("  ");
        }
        putchar('\n');
    }

    return 0;
}