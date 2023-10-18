import ctypes
import time
import os

# 地图尺寸
MAP_WIDTH = 51
MAP_HEIGHT = 31
# 起始节点坐标
START_X = 5
START_Y = 6
# 目标节点坐标
TARGET_X = 45
TARGET_Y = 25


class Node:
    '''
    一个节点类
    coord：坐标状态
    parent：父节点
    action：父节点根据该算符将状态转变为本节点，1为向上一步，1~4按顺时针方向定义
    pathCost：代价
    distance：与目标节点的曼哈顿距离
    evolution：估价函数
    新实例化的Node对象无父节点，action=0，通过调用方法来设定其属性
    '''

    def __init__(self, coord: tuple = (1, 1)) -> None:
        '''根据节点坐标初始化节点'''
        self.coord = coord
        self.parent = None
        self.action = 0
        self.pathCost = 0
        self.distance = abs(
            TARGET_X - self.coord[0]) + abs(TARGET_Y - self.coord[1])
        self.evolution = self.pathCost + self.distance

    def setParent(self, parent, action: int) -> None:
        '''根据父节点和算符，设置节点属性'''
        if action == 1:
            self.coord = (parent.coord[0], parent.coord[1] + 1)
        elif action == 2:
            self.coord = (parent.coord[0] + 1, parent.coord[1])
        elif action == 3:
            self.coord = (parent.coord[0], parent.coord[1] - 1)
        elif action == 4:
            self.coord = (parent.coord[0] - 1, parent.coord[1])

        self.parent = parent
        self.action = action
        self.pathCost = parent.pathCost + 1
        self.distance = abs(
            TARGET_X - self.coord[0]) + abs(TARGET_Y - self.coord[1])
        self.evolution = self.pathCost + self.distance


def expand(node) -> None:
    for action in range(1, 5):  # 扩展四个方向
        if action == 1:
            coord = (node.coord[0], node.coord[1] + 1)
        elif action == 2:
            coord = (node.coord[0] + 1, node.coord[1])
        elif action == 3:
            coord = (node.coord[0], node.coord[1] - 1)
        elif action == 4:
            coord = (node.coord[0] - 1, node.coord[1])

        needAdd = False
        if not map[coord[1]][coord[0]]:  # 通过地图判断能否拓展
            for i in open:
                if i.coord == coord:  # 如果open表中有该节点
                    if i.pathCost > node.pathCost + 1:  # 如果新节点代价比open表中的节点小
                        open.remove(i)  # 从open表中移除节点，并添加新节点（在后面添加，通过needAdd指示）
                        needAdd = True
                    break
            else:  # open表里没有就去closed表里找
                for i in closed:
                    if i.coord == coord:  # 操作同open表
                        if i.pathCost > node.pathCost + 1:
                            closed.remove(i)
                            needAdd = True
                        break
                else:  # 如果两个表里都没有
                    needAdd = True  # 那也得新增节点

        if needAdd:  # 将后继节点添加到open表中
            new = Node()
            new.setParent(node, action)
            if len(open) == 0:
                open.append(new)
            else:
                for i in range(len(open)):  # 按照估价函数值降序插入
                    if open[i].evolution < new.evolution:
                        # if open[i].pathCost < new.pathCost:  # 退化为深度优先算法
                        # if open[i].distance < new.distance:  # 退化为贪心算法
                        open.insert(i, new)
                        break
                else:
                    open.append(new)
            printPoint(new.coord, '^')  # 在地图上记录待扩展节点（open表中节点）


class COORD(ctypes.Structure):
    '''调整光标位置用'''
    _fields_ = [("X", ctypes.c_short), ("Y", ctypes.c_short)]

    def __init__(self, x, y):
        self.X = x
        self.Y = y


def moveCursor(coord: tuple) -> None:
    '''调整光标位置，(x, y)对应横纵坐标'''
    ctypes.windll.kernel32.SetConsoleCursorPosition(
        ctypes.windll.kernel32.GetStdHandle(-11), COORD(coord[0], coord[1]))


def printMap() -> None:
    '''打印地图'''
    for i in map[::-1]:
        for j in i:
            if j:
                print('#', end=' ')
            else:
                print(' ', end=' ')
        print()


def printPoint(coord: tuple, format: str) -> None:
    '''在地图上标记点'''
    moveCursor((coord[0] * 2, MAP_HEIGHT - 1 - coord[1]))
    print(format, end='')


# 地图的生成
# 1.生成轮廓
map = list()
map.append([1 for i in range(MAP_WIDTH)])
for i in range(MAP_HEIGHT-2):
    map.append([0 for i in range(MAP_WIDTH)])
    map[i+1][0] = 1
    map[i+1][MAP_WIDTH-1] = 1
map.append([1 for i in range(MAP_WIDTH)])
# 2.生成地形
for i in range(1, 16):
    map[i][20] = 1
    map[i][40] = 1
for i in range(10, 20):
    map[15][i] = 1
for i in range(15, MAP_HEIGHT):
    map[i][30] = 1

os.system("cls")
printMap()
printPoint((START_X, START_Y), '&')
printPoint((TARGET_X, TARGET_Y), '$')

# 算法开始
open = list()
closed = list()

open.append(Node((START_X, START_Y)))
while (len(open) != 0):
    # 将open表evo最小的节点移入closed表
    n = open.pop()  # 因为open表已经按估价函数值降序排列
    closed.append(n)
    if n.distance == 0:  # 判断是否为目标节点
        # 由父节点指针回溯得到路径
        this = n
        while this.pathCost != 0:
            this = this.parent
            printPoint(this.coord, '@')
            time.sleep(0.01)
        printPoint((START_X, START_Y), '&')
        printPoint((TARGET_X, TARGET_Y), '$')
        moveCursor((0, MAP_HEIGHT))
        print("search success!")
        break
    else:
        expand(n)  # 拓展后继节点
        printPoint(n.coord, '*')  # 在地图上记录已扩展节点（closed表中节点）
        printPoint((START_X, START_Y), '&')
        printPoint((TARGET_X, TARGET_Y), '$')
        time.sleep(0.02)
else:
    moveCursor((0, MAP_HEIGHT))
    print("search failed!")
