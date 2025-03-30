# 五子棋大作业

计算概论（A）2024年秋大作业。使用minmax搜索和alpha-beta剪枝实现AI，并使用Win32编程实现图形化界面。

The project of Introduction to Computing (A) 2024 for freshman students at Peking University. Implement AI using minmax search and alpha beta pruning, and implement a graphical interface using Win32 programming.

## 文件说明

- game.cpp：源代码。若要在botzone上（Renju-Official）使用，需要加上
```cpp
#define BOTZONE
```
- jsonapp.cpp、jsoncpp：经botzone老师修改过的jsonapp，无需修改编译选项即可使用。
- judge.py：模拟botzone环境，将game.cpp编译后使用。
- game.exe：编译后的游戏，双击后即可食用。

## 总体框架
大体分为五个部分：

- Board 类：提供棋盘上的基本逻辑，例如判赢，判禁手
- AI实现：使用minmax搜索
- Botzone交互：实现botzone上的json交互
- Win32：提供图形化界面
- 文件操作：存盘


## Board类

### 数据结构
- Board[n][n]：存放棋盘盘面
- Stack：两个栈，分别存放黑白棋的放置历史记录

这里使用栈是写估价函数是做的优化。这样我们不需要枚举整个棋盘，只需要枚举所有棋子放置的位置即可。由于棋局大多在50回合内结束，这样做是有一定优化的。

### 基本函数

- 构造函数：初始化为-1，表示为空
- Copy：复制自身，创造新对象
- Board-remove：删除棋子（在搜索回溯时）
- Board-put：放置棋子
- Get-color：获取某个位置的棋子情况
- Canput：判断某个位置是否能放置棋子
- Get-stack：返回黑棋或白棋的栈
- Get-stack-top：返回黑棋或白棋的栈的栈顶索引
- Full：判断棋子是否铺满整个棋盘，即和局

### Find函数

- Find-next：从某个给定的位置出发，向某个给定的方向跳一格
- Find-adjacent：从某个给定的位置出发，向某个给定的方向一直跳到不同类的棋子，即【寻找相邻同色棋子】
- Find-jump：从某个给定的【空格】位置出发，向某个给定的方向找给定颜色的棋子。这是专门用来判断“跳三”“跳四”等棋形的。

### 判赢判禁手

- Checkwin：判赢，使用find-adjacent函数向各个方向寻找相邻同色棋子，然后判断其长度是否大于等于5即可。
- Checkban：判断黑棋禁手，综合使用find-adjacent和find-next判断棋形，计算活三和活四的数量，从而判断三三禁手和四四禁手

## AI实现

### minmax

- 用minmax搜索，枚举所有可放置的地方。

- 后来加入α-β剪枝，效率得到提升。

- 考虑到很多地方虽然可放置，但显然不利于赢。所以后来使用贪心策略，先枚举所有位置试下一棋，然后马上用估价函数估值，从中取出估值最大的若干（取了30）个下棋点，再往下搜索

- 由于botzone的时间限制，加入clock-overtine函数，超过0.9秒直接停止搜索。由于优先搜索估价高的盘面，所以这样截停影响不大。

### 估价函数

估价函数是重中之重，但是本人的实现并不理想。

- Evaluate函数：枚举所有棋子（根据stack），估计这个棋子的价值，然后求和

- Evaluate-element函数：对于一个棋子估价。具体地，对于四个方向，判断活三、跳三、眠三、跳四等棋形，对于每个棋形评分，然后相加

致命缺点：由于实现问题，没能对两边都可放置的**跳三**识别为理论上的**活三**。而这样的跳三也是必堵的。

## Botzone交互

- 主要从botzone.wiki上复制而来，为了适配自己的代码，做了一些改动。

- 为了测试，还写了一个python代码judge.py来模拟botzone平台的对战。

- 根据Botzone进行了一些估价函数的调参。

## Win32

由于Qt过于笨重，easyx又不兼容本人的mingw编译器，本人突发奇想，直接从底层写起。

从Microsoft learn上了解到win32编程。

使用window.h，windowx.h，d2d1.h，dwrite.h头文件中的内容。

### 重大困难

#### 困难1

在编译时遇到重大困难：
```
undefined reference to `D2D1CreateFactory'
```

经过大量的搜索，了解到这是d2d1头文件中定义的函数找不到源文件。

经过大量的搜索和试验，从mingw翻出了libd2d1.a文件，这是d2d1的静态库。

后来在使用dwrite库时遇到了同样的问题。

在vscode的编译选项中加入下面选项，成功编译。
```json
"D:/mingw64/x86_64-w64-mingw32/lib/libd2d1.a",
"D:/mingw64/x86_64-w64-mingw32/lib/libdwrite.a"
```


#### 困难2

邀请同学测试，给他发送了exe，但是他的电脑不能打开，显示缺少dll。

了解到dll是动态链接库，缺少的dll是libgcc和libstdc++。

经过大量搜索，得知在编译选项中加入下列选项，把动态链接库也放进exe里面就行。缺点是exe体积翻倍和编译变慢。

```json
"-static-libgcc",
"-static-libstdc++"
```

反思：或许需要对静态库和动态库有更多的了解

### 消息循环

- WM_CREATE：创建d2d1factory；读取存盘的数据；创建按钮子窗口（新游戏、继续游戏、退出、返回菜单、选择黑白棋子）
- WM_COMMAND：处理按钮消息
- WM_PAINT：绘画窗口
- WM_SIZE：响应窗口缩放，从而重新计算窗口布局
- WM_MOUSEMOVE：响应鼠标移动，用于指示半透明的下棋点
- WM_LBUTTONUP：下棋
- WM_DESTORY：释放内存

### 坐标转换
需要实现屏幕坐标和棋盘坐标的互相转化。

- Calculate-layout计算出棋盘的一下数据，例如左上角点的屏幕坐标，每行每列的间隔

- 棋盘到屏幕：
```cpp
x = gridStartX + x * gridGap;
y = gridStartY + y * gridGap;
```

- 屏幕到棋盘：
```cpp
x = 1.0 * (x - gridStartX) / gridGap + 0.5；
y = 1.0 * (y - gridStartY) / gridGap + 0.5;
```

### AI协程

由于本人AI效率低下，如果采用单线程，轮到机器下时窗口似乎会卡死，所以尝试使用了多线程。

- ComputerGoCoroutine函数里调用CreateThread创建线程，让机器开始跑。

- 跑完会直接修改board。

缺点&一些问题：直接修改board估计会造成一些问题。比如在ai线程还在跑时returnMenu并且快速地newGame，而newGame把board清空了，然后ai的线程去修改，大概率会有问题。但玩家（比如本人）一般不会有这样的手速。

### 窗口绘制

- 按钮直接使用createwindow创建子窗口，导致按钮极其丑陋。

- 在游戏界面，由于鼠标移动会修改半透明棋子的位置，从而需要重绘。然而重绘会覆盖按钮，因此也需要让按钮子窗口重绘，但消息循环不太同步，导致按钮会闪现。或许应该只对一小部分区域进行重绘。

- 坐标也没有经过精心计算，随意摆放，这是美观上的缺点。

- 游戏页面：使用drawLine画了n条竖线和n条横线，然后根据board类stack的数据用drawEllipse画出已有的棋子，再根据鼠标对应的坐标画出半透明的预测棋子。

反思：有一个界面设计辅助软件之类的东西是必要的

### 文件操作

使用fstream类。

- SaveData：等待isPlayerTurn为真时才继续执行，避免了ai协程的各种问题

- 为了写入board类，使用二进制读写

```cpp
fout.write((char *)&*board, sizeof(*board));
```

- 然后还要记录玩家颜色（黑or白），以及游戏是否结束等状态。

## 收获&亮点

- 使用了win32编程，自己动手丰衣足食。了解了windows系统的一些基础思想。

- 学习到了一种基本的ai算法，有一定棋力。

- 锻炼了代码能力。

## 总结&反思

- 全部代码写在一个cpp里，导致编译速度慢、查找困难等诸多问题。

- 估价函数设计不够好，导致无法很好地预测“活三+活四”这样的必胜局。

- 类使用不合理，例如盲目给board套上类。对面向对象编程的理解不够深入。

- 部分代码设计不合理。例如find函数的设计，其可读性和简洁程度远远不如直接把棋链整个取出然后配对。






