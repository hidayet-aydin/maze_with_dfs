
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1000
#define CLEAR "clear"

int mazeLoadFromFile(int maze[MAX][MAX], int *sizeY, int *sizeX, int *startY, int *startX, int *finishY, int *finishX)
{
    FILE *fp;
    char *line;
    size_t len;
    ssize_t read;

    fp = fopen("sample.txt", "r");
    if (fp == NULL)
    {
        printf("file error!");
        return 0;
    }

    int i = 0;
    char ch[MAX];
    while (fgets(ch, MAX, fp) != NULL)
    {
        int k;
        for (k = 0; k < strlen(ch) - 2; k++)
        {
            if (ch[k] == '+' || ch[k] == '-' || ch[k] == '|')
            {
                maze[i][k] = 0;
            }
            else if (ch[k] == ' ')
            {
                maze[i][k] = 1;
            }
            else if (ch[k] == 'b')
            {
                maze[i][k] = 2;
                *startY = i;
                *startX = k;
            }
            else if (ch[k] == 'c')
            {
                maze[i][k] = 3;
                *finishY = i;
                *finishX = k;
            }
            else
            {
                maze[i][k] = 1;
            }
        }
        i++;
    }
    *sizeX = strlen(ch);
    *sizeY = i;

    fclose(fp);
    return 1;
}

void printMaze(int maze[MAX][MAX], int y, int x)
{
    int i, k;
    for (i = 0; i < y; i++)
    {
        for (k = 0; k < x; k++)
        {
            if (i % 2 == 0 && k % 2 == 0 && maze[i][k] == 0)
            {
                printf("+");
            }
            else if (i % 2 == 0 && maze[i][k] == 0)
            {
                printf("-");
            }
            else if (k % 2 == 0 && maze[i][k] == 0)
            {
                printf("|");
            }
            else if (maze[i][k] == 1)
            {
                printf(" ");
            }
            else if (maze[i][k] == 2)
            {
                printf("b");
            }
            else if (maze[i][k] == 3)
            {
                printf("c");
            }
            else if (maze[i][k] == 4)
            {
                printf("O");
            }
            else if (maze[i][k] == 5)
            {
                printf("*");
            }
            else
            {
                printf("%i", maze[i][k]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int main(void)
{
    system(CLEAR);

    int mazeSizeY, mazeSizeX;
    int startY, startX, finishY, finishX;
    int maze[MAX][MAX];

    mazeLoadFromFile(maze, &mazeSizeY, &mazeSizeX, &startY, &startX, &finishY, &finishX);
    printMaze(maze, mazeSizeY, mazeSizeX);

    return 0;
}