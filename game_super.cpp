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

void init_field(int field_log[][12], int field_play[][12]);  // ������������� ����������� � ����������� �����
void mine_generate(int field_log[][12]);                     // ��������� ��������� ���
void fill_field_log(int field_log[][12]);                    // ���������� ����������� ���� ������� � ����������� �� ������������ ���
void demining(int field_log[][12], int field_play[][12], int x, int y);  // ���������� ����������� ����� ����
void flag_state(int field_log[][12], int field_play[][12], int x, int y, int *mine); // ����������� �������� ������
int count_flag(int field_play[][12]);                                                // ������� ���������� �����
void win_field(int field_log[][12], int field_play[][12]);                           // ���������� ���� ��� ������

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

    init_field(field_log, field_play);                      // ������������� ����������� � ����������� �����
    mine_generate(field_log);                               // ��������� ��������� ���
    fill_field_log(field_log);                              // ���������� ����������� ���� ������� � ����������� �� ������������ ���

    clock_t start, end;                                     // ���������� �������� �������
    start = clock();
    int pass_time = 0;

    while (window.isOpen())
    {
       Vector2i pos = Mouse::getPosition(window);       // ��������� ��������� ��� ������� ������ �����
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
                    if( x > 0 && y > 0 && x <=10 && y <= 10)                // ���������� ����������� ����� ����
                        demining(field_log, field_play, x, y);
                    if( x >= 1 && x <= 2 && y >= 11 && y <= 12)             // ��� ������� ������� ������� � ������ ����
                        return 1;
                }
                if (event.key.code == Mouse::Right)                     // ����������� �������� ������
                    flag_state(field_log, field_play, x, y, &mine);     // � ������� ��������� ���������� �����
            }
        }
        window.clear();

        fild_sprite.setTextureRect(IntRect(0, 40, 480, 520));          // ���������� ������� ���� ����
        fild_sprite.setPosition(0, 0);
        window.draw(fild_sprite);

        restart_sprite.setTextureRect(IntRect(720, 50, 80, 80));       // ���������� ������ �������
        restart_sprite.setPosition(40, 440);
        window.draw(restart_sprite);

        if(mine == MINE)
           win_field(field_log, field_play);                    // ��� ���������� ���� ��� ����� ����

        for(int i = 1; i <= 10; i++)                            // ���������� �������� ����
             for(int j = 1; j <= 10; j++)
             {
                 pict_sprite.setTextureRect(IntRect(field_play[i][j]*size, 0, size, size));
                 pict_sprite.setPosition(i*size, j*size);
                 window.draw(pict_sprite);
             }

        if(mine == MINE)                                // ��� ���������� ���� ��� ���������� ����������� �� �������
        {
            win_sprite.setTextureRect(IntRect(500, 50, 200, 200));
            win_sprite.setPosition(140, 40);
            window.draw(win_sprite);
        }
        else
            end = clock();

        if ( ( end - start ) >= 1000 )
        {
            pass_time = pass_time + ( end - start );              // ������� ���������� �������
            start = end;
        }

        text1.setString(std::to_string(pass_time/1000));           // ����� ���������� �������
        text1.setPosition(225,0);
        window.draw(text1);

        text.setString(std::to_string(MINE - count_flag(field_play)));    // ������� ���������� �����
        text.setPosition(215,450);                                        // ����� ���������� ����������� �����
        window.draw(text);

        window.display();
    }

    return 0;
}

void init_field(int field_log[][12], int field_play[][12])         // ������������� ����������� � ����������� �����
{
    for(int i = 1; i <= 10; i++)
        for(int j = 1; j <= 10; j++)
        {
            field_log [i][j] = EMPTY;
            field_play[i][j] = 13 + rand()%13;
        }
}

void mine_generate(int field_log[][12])                         // ��������� ��������� ���
{
    int x, y, i=0;                                              // ��������� ����������� ���� ������
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

void fill_field_log(int field_log[][12])                // ���������� ����������� ���� �������
{
    int bomb = 0, x = 0, y = 0, i, j;
    for(x = 1; x <= 10; x++)                            // ������������ ���������� ��� ������ ������ ������,
    {                                                   // � ������� ��� ����, ���������� � ���������� �������
        for(y = 1; y <= 10; y++)                        // ���������� ��� ���� ������
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

void demining(int field_log[][12], int field_play[][12], int x, int y)        // ���������� ����������� ����� ����
{
    if(field_log[x][y] == BOMB)                                         // ���� ����������� ������ ����, �� ����� ����
    {
        for(int i = 1; i <= 10; i++)
            for(int j = 1; j <= 10; j++)
                field_play[i][j] = field_log[i][j];
    }
    if(field_log[x][y] > 0 && field_log[x][y] <= 8)                     // ���� ���������� ������ � ������ �� 1 �� 8
        field_play[x][y] = field_log[x][y];                             // ���������� ���� ������

    if(field_log[x][y] == 0 )                                           // ���� ������ ������ ���������� ������ ������,
   {                                                                    // ���������� ����� ������ �� � �����������
        field_play[x][y] = field_log[x][y];                             // ����� ����� ������ ������
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

void flag_state(int field_log[][12], int field_play[][12], int x, int y, int *mine) // ����������� �������� ������
{
    if (field_play[x][y] >=13 && field_play[x][y] <= 25)                        // � ������� ��������� ���������� �����
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

int count_flag(int field_play[][12])                            // ������� ���������� �����
{
    int mine = 0;
    for(int i = 1; i <= 10; i++)
        for(int j = 1; j <= 10; j++)
            if( field_play[i][j] == FLAG )
                mine++;
    return mine;
}

void win_field(int field_log[][12], int field_play[][12])       // ���������� ���� ��� ������
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
