/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <ncurses.h> 

#define WIDTH 50       
#define HEIGHT 20      
#define MAX_LENGTH 400 

// 定義食物的結構
struct {
    int x;
    int y;
} food;

// 定義蛇的結構
struct {
    int x[MAX_LENGTH];
    int y[MAX_LENGTH];
    int len;
    int score;
    int speed;
} snake;

int update_food = 0; 
int key = 'w';       // 初始化方向鍵

// 畫地圖邊界
void Drawmap(void) {
    for (int x = 0; x < WIDTH; x++) {
        mvprintw(0, x, "#");           // 畫出上邊界
        mvprintw(HEIGHT - 1, x, "#");  // 畫出下邊界
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        mvprintw(y, 0, "#");           // 畫出左邊界
        mvprintw(y, WIDTH - 1, "#");   // 畫出右邊界
    }
    refresh(); 
}

// 創建蛇
void CreateSnake(void) {
    int origin_x = WIDTH / 2;
    int origin_y = HEIGHT / 2;

    snake.len = 3; 
    snake.x[0] = origin_x;
    snake.y[0] = origin_y;
    snake.x[1] = origin_x;
    snake.y[1] = origin_y + 1;
    snake.x[2] = origin_x;
    snake.y[2] = origin_y + 2;

    snake.score = -1; 
    snake.speed = 10; 

    for (int i = 0; i < snake.len; i++) {
        mvprintw(snake.y[i], snake.x[i], "O"); // 畫出蛇的身體
    }
    refresh(); 
}

// 創建食物
void CreateFood(void) {
    if (update_food == 0) { 
        int flag;
        do {
            food.x = rand() % (WIDTH - 2) + 1; // 隨機生成食物的x座標
            food.y = rand() % (HEIGHT - 2) + 1; // 隨機生成食物的y座標

            flag = 0;
            for (int j = 0; j < snake.len; j++) {
                if (snake.x[j] == food.x && snake.y[j] == food.y) { // 食物位置和蛇重疊，則重新生成
                    flag = 1; 
                    break;
                }
            }
        } while (flag);
        mvprintw(food.y, food.x, "*"); // 畫出食物

        snake.score++; 
        update_food = 1; 
    }
    refresh(); 
}

// 移動蛇
void SnakeMove(int x, int y) {
    if (!update_food)
        snake.len++; 
    else {
        mvprintw(snake.y[snake.len - 1], snake.x[snake.len - 1], " "); // 清除蛇尾的位置
    }

    for (int i = snake.len - 1; i > 0; i--) { // 更新蛇身的位置
        snake.x[i] = snake.x[i - 1];
        snake.y[i] = snake.y[i - 1];
    }
    snake.x[0] = x; // 更新蛇頭位置
    snake.y[0] = y;
    mvprintw(snake.y[0], snake.x[0], "O"); // 畫出蛇頭
    refresh(); 
}

// 控制蛇的移動方向
void Move() {
    int pre_key = key, x, y;
    if (kbhit()) { // 檢查是否有按鍵按下
        pre_key = key;
        key = getch();
    }

    // 阻止蛇往相反方向移動
    if (pre_key == 'w' && key == 's') key = 'w';
    if (pre_key == 's' && key == 'w') key = 's';
    if (pre_key == 'a' && key == 'd') key = 'a';
    if (pre_key == 'd' && key == 'a') key = 'd';

    // 根據指定按鍵更新蛇頭的位置
    switch (key) {
        case 'a': 
            x = snake.x[0] - 1;
            y = snake.y[0];
            break;
        case 'd': 
            x = snake.x[0] + 1;
            y = snake.y[0];
            break;
        case 'w': 
            x = snake.x[0];
            y = snake.y[0] - 1;
            break;
        case 's': 
            x = snake.x[0];
            y = snake.y[0] + 1;
            break;
        default:
            x = snake.x[0];
            y = snake.y[0];
            break;
    }
    if (x == food.x && y == food.y) update_food = 0; // 蛇吃到食物，重新生成食物
    SnakeMove(x, y); 
}

// 檢查蛇是否存活
void alive(void) {
    // 檢查蛇是否碰到邊界
    if (snake.x[0] == 0 || snake.x[0] == WIDTH - 1 || snake.y[0] == 0 || snake.y[0] == HEIGHT - 1) {
        mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "Game Over!"); 
        refresh();
        endwin(); // 結束ncurses模式
        exit(0);
    }
    // 檢查蛇是否碰到自己
    for (int i = 1; i < snake.len; i++) {
        if (snake.x[0] == snake.x[i] && snake.y[0] == snake.y[i]) {
            mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "Game Over!"); 
            refresh();
            endwin(); 
            exit(0);
        }
    }

    // 檢查蛇是否達到最大長度
    if (snake.len == MAX_LENGTH) {
        mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "You Win!"); 
        refresh();
        endwin(); 
        exit(0);
    }

    mvprintw(HEIGHT, 0, "Your score: %d", snake.score); 
    refresh(); 
}

// 檢查鍵盤是否有按鍵被按下
int kbhit(void) {
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch); // 把按鍵重新放回緩衝區
        return 1; // 有按鍵被按下
    } else {
        return 0; 
    }
}

int main(void) {
    srand((unsigned int)time(NULL)); // 初始化隨機亂數種子

    initscr(); // 初始化ncurses
    noecho(); // 禁止按鍵顯示
    curs_set(0); // 隱藏游標
    timeout(0); // 非阻塞模式

    Drawmap(); 
    CreateSnake(); 
    while (1) {
        CreateFood(); 
        Move(); 
        usleep(100000 - (snake.len * 200)); // 控制蛇的速度
        alive(); 
    }

    endwin();
    return 0;
}


    
    
    
    






