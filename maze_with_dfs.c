
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 1000
#define CLEAR "clear"

typedef struct node
{
    int y;
    int x;
    struct node *next;
} node;

struct Graph
{
    int sizeY;
    int sizeX;
    int hit;
    int apple;
    int visit[MAX][MAX];
    node *adjacents[MAX][MAX];
};

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

void randomAppleSeeding(int maze[MAX][MAX], int y, int x)
{
    int i, k, a;
    for (i = 0; i < y; i++)
    {
        for (k = 0; k < x; k++)
        {
            if (i % 2 == 1 && k % 2 == 1 && maze[i][k] == 1)
            {
                a = rand() % 9;
                if (a == 0)
                {
                    a = 4;
                }
                else
                {
                    a = 1;
                }
                maze[i][k] = a;
            }
        }
    }
}

node *createNode(int y, int x)
{
    node *new = malloc(sizeof(node));
    new->y = y;
    new->x = x;
    new->next = NULL;
    return new;
}

void addEdge(struct Graph *graph, int srcY, int srcX, int destY, int destX)
{
    int notFound = 1;
    node *ptr = graph->adjacents[srcY][srcX];
    while (ptr != NULL && notFound)
    {
        if (ptr->y == destY && ptr->x == destX)
        {
            notFound = 0;
        }
        ptr = ptr->next;
    }

    if (notFound)
    {
        // edge src -> dest
        node *newNode = createNode(destY, destX);
        newNode->next = graph->adjacents[srcY][srcX];
        graph->adjacents[srcY][srcX] = newNode;

        // edge dest -> src
        newNode = createNode(srcY, srcX);
        newNode->next = graph->adjacents[destY][destX];
        graph->adjacents[destY][destX] = newNode;
    }
}

struct Graph *createGraph(int maze[MAX][MAX], int y, int x)
{
    struct Graph *graph = malloc(sizeof(struct Graph));
    graph->sizeY = y;
    graph->sizeX = x;
    graph->hit = 0;
    graph->apple = 0;

    int i, k;
    for (i = 0; i < y + 1; i++)
    {
        for (k = 0; k < x + 1; k++)
        {
            graph->visit[i][k] = 0;
            graph->adjacents[i][k] = NULL;
        }
    }

    for (i = 1; i < y; i = i + 2)
    {
        for (k = 1; k < x; k = k + 2)
        {
            if (maze[i][k] != 0)
            {
                if (maze[i - 1][k] != 0)
                {
                    addEdge(graph, i, k, i - 2, k);
                }
                if (maze[i + 1][k] != 0)
                {
                    addEdge(graph, i, k, i + 2, k);
                }
                if (maze[i][k - 1] != 0)
                {
                    addEdge(graph, i, k, i, k - 2);
                }
                if (maze[i][k + 1] != 0)
                {
                    addEdge(graph, i, k, i, k + 2);
                }
            }
        }
    }

    return graph;
}

void printGraph(struct Graph *graph)
{
    int y, x;
    for (y = 1; y < graph->sizeY + 1; y++)
    {
        for (x = 1; x < graph->sizeX + 1; x++)
        {
            node *temp = graph->adjacents[y][x];
            if (temp)
            {
                printf("\n Adjacency list of vertex %i, %i\n ", y, x);
                while (temp)
                {
                    printf("%d, %d -> ", temp->y, temp->x);
                    temp = temp->next;
                }
                printf("\n");
            }
        }
    }
}

int dfsPathFinder(int maze[MAX][MAX], int sizeY, int sizeX, struct Graph *graph, int startY, int startX, int finishY, int finishX)
{
    node *adjacent = graph->adjacents[startY][startX];

    int stackInd = 0;
    node *stack[MAX];

    int result = 0;
    while (adjacent && result == 0)
    {
        int adjacentY = adjacent->y;
        int adjacentX = adjacent->x;

        if (adjacentY == finishY && adjacentX == finishX)
        {
            result = 1;
        }
        else
        {
            if (maze[adjacentY][adjacentX] != 5)
            {
                maze[adjacentY][adjacentX] = 5;

                node *newItem = malloc(sizeof(node));
                newItem->y = adjacentY;
                newItem->x = adjacentX;
                stack[stackInd] = newItem;
                stackInd++;
            }

            maze[adjacentY][adjacentX] = 5;
            if (graph->visit[adjacentY][adjacentX] == 0)
            {
                graph->visit[startY][startX] = 1;
                result = dfsPathFinder(maze, sizeY, sizeX, graph, adjacentY, adjacentX, finishY, finishX);
                if (result == 0)
                {
                    int index = 0;
                    for (index = stackInd; index > 0; index--)
                    {
                        node *getNode = stack[index - 1];

                        if (maze[getNode->y][getNode->x] != 1)
                        {
                            maze[getNode->y][getNode->x] = 1;
                        }
                    }
                }
            }

            adjacent = adjacent->next;
        }
    }

    return result;
}

int main(void)
{
    system(CLEAR);
    srand(time(0));

    int mazeSizeY, mazeSizeX;
    int startY, startX, finishY, finishX;
    int maze[MAX][MAX];

    mazeLoadFromFile(maze, &mazeSizeY, &mazeSizeX, &startY, &startX, &finishY, &finishX);
    randomAppleSeeding(maze, mazeSizeY, mazeSizeX);

    struct Graph *graph = createGraph(maze, mazeSizeY, mazeSizeX);
    
    dfsPathFinder(maze, mazeSizeY, mazeSizeX, graph, startY, startX, finishY, finishX);
    printMaze(maze, mazeSizeY, mazeSizeX);

    return 0;
}