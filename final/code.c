#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <direct.h>
void scanDirectory(const char *path);
int txtToMap(const char *filename);

#define SIZE_OF_MAP 10
int map[SIZE_OF_MAP][SIZE_OF_MAP];



int main()
{
    const char *folderPath = "./Map";
    scanDirectory(folderPath);

    return 0;
}

void scanDirectory(const char *path)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        char subPath[256];
        snprintf(subPath, sizeof(subPath), "%s/%s", path, entry->d_name);

        DIR *subDir = opendir(subPath);
        if (subDir != NULL)
        {
            closedir(subDir);
            scanDirectory(subPath);
        }
        else
        {
            const char *dot = strrchr(entry->d_name, '.');
            if (dot != NULL && strcmp(dot, ".txt") == 0)
            {
                printf("Found text file: %s\n", subPath);
                txtToMap(subPath);
            }
        }
    }

    closedir(dir);
}

int printMap()
{
    for (int i = 0; i < SIZE_OF_MAP; i++)
    {
        for (int j = 0; j < SIZE_OF_MAP; j++)
        {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
    return 0;
}
int *findNeighboringPoints(int x, int y)
{
    static int neighbors[4];
    //5 stands for out of bounds
    neighbors[0] = (x - 1 >= 0) ? map[x - 1][y] : 5; 
    neighbors[1] = (x + 1 < SIZE_OF_MAP) ? map[x + 1][y] : 5;
    neighbors[2] = (y - 1 >= 0) ? map[x][y - 1] : 5;
    neighbors[3] = (y + 1 < SIZE_OF_MAP) ? map[x][y + 1] : 5;

    return neighbors;
}
int createTxt(const char *folderName, int i, int j)
{
    char folderName2[256];

    snprintf(folderName2, sizeof(folderName2), "%s/neighboring_points.txt", folderName);
    FILE *fp = fopen(folderName2, "w");

    int *neighboringPoints = findNeighboringPoints(i, j);
    char content[256];
    snprintf(content, sizeof(content), "%d%d%d%d",
             neighboringPoints[0], neighboringPoints[1], neighboringPoints[2], neighboringPoints[3]);
    fputs(content, fp);

    fclose(fp);
}

int positionToFolder(const char *filename)
{
    char modifier = filename[strlen(filename) - 5];
    char folderName[256];
    char folderName2[256];
    for (int i = 0; i < SIZE_OF_MAP; i++)
    {
        for (int j = 0; j < SIZE_OF_MAP; j++)
        {

            snprintf(folderName, sizeof(folderName), "C:/Users/likapichu/Desktop/Code/2600/final/World/%c%d%d", modifier, i, j);
            if (_mkdir(folderName) != 0)
            {
                perror("mkdir");
            }
            createTxt(folderName, i, j);
        }
    }
    return 0;
}

int txtToMap(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("fopen");
        return -1;
    }

    int x = 0;
    int y = 0;

    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        for (int i = 0; i < strlen(line) && y < SIZE_OF_MAP; i++)
        {
            if (line[i] >= '0' && line[i] <= '9')
            {
                map[x][y] = line[i] - '0';
                y++;
            }
        }
        x++;
        y = 0;
    }

    fclose(fp);
    printMap();
    positionToFolder(filename);
    return 0;
}

