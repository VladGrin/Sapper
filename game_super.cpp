#include "game_sapper.h"
#include <SFML/Graphics.hpp>
#include <time.h>

using namespace sf;

#define MINE 20

typedef enum
{
     EMPTY      = 0
    ,DEACT_MINE = 9
    ,BOMB
    ,FLAG
    ,QUESTION
}eFieldInfo;

void init_field(int field_log[][12], int field_play[][12]);  // инициалицация логического и визуального полей
void mine_generate(int field_log[][12]);                     // рандомная генерация мин
void fill_field_log(int field_log[][12]);                    // заполнение логического поля числами в зависимости от расположения мин
void demining(int field_log[][12], int field_play[][12], int x, int y);  // прорисовка открываемых ячеек поля
void flag_state(int field_log[][12], int field_play[][12], int x, int y, int *mine); // возможность помячать ячейки
int count_flag(int field_play[][12]);                                                // подсчет помеченных ячеек
void win_field(int field_log[][12], int field_play[][12]);                           // прорисовка поля при победе

int game_realize()
{
    srand(time(NULL));
    RenderWindow window(VideoMode(480, 520), "SUPER");

    int size = 40;
    int field_log[13][12] = {0};
    int field_play[13][12] = {0};
    int mine = 0;

    Font font, font1;
    font.loadFromFile("aBosaNova Bold.ttf");
    font1.loadFromFile("aBosaNova Bold.ttf");
    Text text, text1;
    text.setFont(font);
    text.setCharacterSize(40);
    text.setColor(Color::Red);
    text.setStyle(Text::Bold);
    text1.setFont(font1);
    text1.setCharacterSize(25);
    text1.setColor(Color::Black);
    text1.setStyle(Text::Bold);

    Texture pict_texture;
    pict_texture.loadFromFile("image.jpg");

    Sprite pict_sprite, fild_sprite, win_sprite, restart_sprite;
    pict_sprite.setTexture(pict_texture);
    fild_sprite.setTexture(pict_texture);
    win_sprite.setTexture (pict_texture);
    restart_sprite.setTexture (pict_texture);

    init_field(field_log, field_play);                      // инициалицация логического и визуального полей
    mine_generate(field_log);                               // рандомная генерация мин
    fill_field_log(field_log);                              // заполнение логического поля числами в зависимости от расположения мин

    clock_t start, end;                                     // реализация подсчета времени
    start = clock();
    int pass_time = 0;

    while (window.isOpen())
    {
       Vector2i pos = Mouse::getPosition(window);       // получение координат при нажатии кнопок мышки
        int x = pos.x/size;
        int y = pos.y/size;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::MouseButtonPressed){
                if(event.key.code == Mouse::Left)
                {
                    if( x > 0 && y > 0 && x <=10 && y <= 10)                // прорисовка открываемых ячеек поля
                        demining(field_log, field_play, x, y);
                    if( x >= 1 && x <= 2 && y >= 11 && y <= 12)             // при нажатии рестарт возврат к началу игры
                        return 1;
                }
                if (event.key.code == Mouse::Right)                     // возможность помячать ячейки
                    flag_state(field_log, field_play, x, y, &mine);     // и подсчет правильно отмеченных ячеек
            }
        }
        window.clear();

        fild_sprite.setTextureRect(IntRect(0, 40, 480, 520));          // прорисовка заднего вида поля
        fild_sprite.setPosition(0, 0);
        window.draw(fild_sprite);

        restart_sprite.setTextureRect(IntRect(720, 50, 80, 80));       // прорисовка кнопки рестарт
        restart_sprite.setPosition(40, 440);
        window.draw(restart_sprite);

        if(mine == MINE)
           win_field(field_log, field_play);                    // при нахождении всех мин конец игры

        for(int i = 1; i <= 10; i++)                            // прорисовка игрового поля
             for(int j = 1; j <= 10; j++)
             {
                 pict_sprite.setTextureRect(IntRect(field_play[i][j]*size, 0, size, size));
                 pict_sprite.setPosition(i*size, j*size);
                 window.draw(pict_sprite);
             }

        if(mine == MINE)                                // при нахождении всех мин прорисовка уведомления ты победил
        {
            win_sprite.setTextureRect(IntRect(500, 50, 200, 200));
            win_sprite.setPosition(140, 40);
            window.draw(win_sprite);
        }
        else
            end = clock();

        if ( ( end - start ) >= 1000 )
        {
            pass_time = pass_time + ( end - start );              // подсчет прошедшего времени
            start = end;
        }

        text1.setString(std::to_string(pass_time/1000));           // вывод прошедшего времени
        text1.setPosition(225,0);
        window.draw(text1);

        text.setString(std::to_string(MINE - count_flag(field_play)));    // подсчет помеченных ячеек
        text.setPosition(215,450);                                        // вывод количества неотмеченых ячеек
        window.draw(text);

        window.display();
    }

    return 0;
}

void init_field(int field_log[][12], int field_play[][12])         // инициалицация логического и визуального полей
{
    for(int i = 1; i <= 10; i++)
        for(int j = 1; j <= 10; j++)
        {
            field_log [i][j] = EMPTY;
            field_play[i][j] = 13 + rand()%13;
        }
}

void mine_generate(int field_log[][12])                         // рандомная генерация мин
{
    int x, y, i=0;                                              // заполнени логического поля минами
    while(i < MINE)
    {
        x = 1 + rand()%10;
        y = 1 + rand()%10;
        if(field_log[x][y] != BOMB)
        {
            field_log[x][y] = BOMB;
            i++;
        }
    }
}

void fill_field_log(int field_log[][12])                // заполнение логического поля числами
{
    int bomb = 0, x = 0, y = 0, i, j;
    for(x = 1; x <= 10; x++)                            // подсчитываем количество мин вокруг каждой ячейки,
    {                                                   // в которой нет мины, присвоение в логическом массиве
        for(y = 1; y <= 10; y++)                        // количества мин этой ячейке
        {
            bomb = 0;
            if(field_log[x][y] != BOMB)
            {
                for(i = x-1; (i <= x+1) && (i<=10) ; i++ )
                    for(j = y-1; (j <= y+1) && (j<=10) ; j++ )
                        if( ( field_log[i][j] == BOMB) && i>=1 && j>=1)
                            bomb++;
            }
            if(bomb > 0)
                field_log[x][y] = bomb;
        }
    }
}

void demining(int field_log[][12], int field_play[][12], int x, int y)        // прорисовка открываемых ячеек поля
{
    if(field_log[x][y] == BOMB)                                         // усли открываемая ячейка мина, то конец игры
    {
        for(int i = 1; i <= 10; i++)
            for(int j = 1; j <= 10; j++)
                field_play[i][j] = field_log[i][j];
    }
    if(field_log[x][y] > 0 && field_log[x][y] <= 8)                     // если открывемая ячейка с числом от 1 до 8
        field_play[x][y] = field_log[x][y];                             // прорисовка этой ячейки

    if(field_log[x][y] == 0 )                                           // если ячейка пустая прорисовка пустой ячейки,
   {                                                                    // прорисовка ячеек вогруг неё и рекурсивный
        field_play[x][y] = field_log[x][y];                             // поиск пусты хячеек вокруг
        for(int i = x-1; (i <= x+1) && (i<=10) ; i++ )
            for(int j = y-1; (j <= y+1) && (j<=10) ; j++ )
            {
               if( field_play[i][j] >=13 && field_play[i][j] <= 25 &&
                   field_log[i][j] != EMPTY && i>=1 && j>=1)
                        field_play[i][j] = field_log[i][j];
               if( field_play[i][j] >=13 && field_play[i][j] <= 25 &&
                   field_log[i][j] == EMPTY && i>=1 && j>=1)
                        demining(field_log, field_play, i, j);
            }
    }
}

void flag_state(int field_log[][12], int field_play[][12], int x, int y, int *mine) // возможность помячать ячейки
{
    if (field_play[x][y] >=13 && field_play[x][y] <= 25)                        // и подсчет правильно отмеченных ячеек
    {
        field_play[x][y] = FLAG;
        if(field_log[x][y] == BOMB)
            (*mine)++;
        return;
    }
    if (field_play[x][y] == FLAG)
    {
        field_play[x][y] = QUESTION;
        if(field_log[x][y] == BOMB)
            (*mine)--;
        return;
    }
    if (field_play[x][y] == QUESTION)
        field_play[x][y] = 13 + rand()%13;
}

int count_flag(int field_play[][12])                            // подсчет помеченных ячеек
{
    int mine = 0;
    for(int i = 1; i <= 10; i++)
        for(int j = 1; j <= 10; j++)
            if( field_play[i][j] == FLAG )
                mine++;
    return mine;
}

void win_field(int field_log[][12], int field_play[][12])       // прорисовка поля при победе
{
    for(int i = 1; i <= 10; i++)
        for(int j = 1; j <= 10; j++)
        {
            if(field_log[i][j] == BOMB)
                field_play[i][j] = DEACT_MINE;
            else
                field_play[i][j] = field_log[i][j];
        }
}
