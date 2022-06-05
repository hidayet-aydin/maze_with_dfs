
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#define CLEAR "cls"
#else
#include <unistd.h>
#define CLEAR "clear"
#endif

#define MAX 1000

typedef struct node
{
    int y;
    int x;
    struct node *next;
} node;

typedef struct Graph
{
    int sizeY;
    int sizeX;
    int hit;
    int apple;
    int visit[MAX][MAX];
    node *adjacents[MAX][MAX];
} Graph;

int mazeLoadFromFile(int[MAX][MAX], int *, int *, int *, int *, int *, int *);
void randomAppleSeeding(int[MAX][MAX], int, int);

node *createNode(int, int);
void addEdge(Graph *, int, int, int, int);
Graph *createGraph(int[MAX][MAX], int, int);

void render(int[MAX][MAX], Graph *, int, int, int);
int countAdjacents(node *);

int dfsPathFinder(int maze[MAX][MAX], int sizeY, int sizeX, Graph *graph, int delay, int startY, int startX, int finishY, int finishX)
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
                if (maze[adjacentY][adjacentX] == 4)
                {
                    graph->apple += 1;
                }

                maze[adjacentY][adjacentX] = 5;

                node *newItem = malloc(sizeof(node));
                newItem->y = adjacentY;
                newItem->x = adjacentX;
                stack[stackInd] = newItem;
                stackInd++;

                render(maze, graph, delay, sizeY, sizeX);
            }

            maze[adjacentY][adjacentX] = 5;
            if (graph->visit[adjacentY][adjacentX] == 0)
            {
                graph->visit[startY][startX] = 1;
                result = dfsPathFinder(maze, sizeY, sizeX, graph, delay, adjacentY, adjacentX, finishY, finishX);
                if (result == 0)
                {
                    int chk = 0;
                    int index = 0;
                    for (index = stackInd; index > 0; index--)
                    {
                        node *getNode = stack[index - 1];

                        if (chk == 0)
                        {
                            node *selAdjacent = graph->adjacents[getNode->y][getNode->x];
                            int res = countAdjacents(selAdjacent);
                            if (res == 1)
                            {
                                graph->hit++;
                            }
                            chk++;
                        }

                        if (maze[getNode->y][getNode->x] != 1)
                        {
                            maze[getNode->y][getNode->x] = 1;
                            render(maze, graph, delay, sizeY, sizeX);
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
    int delay;

    mazeLoadFromFile(maze, &mazeSizeY, &mazeSizeX, &startY, &startX, &finishY, &finishX);
    randomAppleSeeding(maze, mazeSizeY, mazeSizeX);

    Graph *graph = createGraph(maze, mazeSizeY, mazeSizeX);

    printf("Please enter a run time (ms):\n");
    scanf("%i", &delay);
    delay = delay * 1000;
    dfsPathFinder(maze, mazeSizeY, mazeSizeX, graph, delay, startY, startX, finishY, finishX);

    return 0;
}

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

void addEdge(Graph *graph, int srcY, int srcX, int destY, int destX)
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

Graph *createGraph(int maze[MAX][MAX], int y, int x)
{
    Graph *graph = malloc(sizeof(Graph));
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

void render(int maze[MAX][MAX], Graph *graph, int delay, int y, int x)
{
    system(CLEAR);

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
    printf("\n10 x apple:%i - 5 x hit:%i \n", graph->apple, graph->hit);
    printf("Score: [%i] \n", graph->apple * 10 - graph->hit * 5);
    usleep(delay);
}

int countAdjacents(node *getNode)
{
    int cnt = 0;
    node *ptr = getNode;
    while (ptr)
    {
        cnt++;
        ptr = ptr->next;
    }
    return cnt;
}