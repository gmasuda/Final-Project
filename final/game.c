#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <mosquitto.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define MAX_LEN 1024

#define MQTT_HOST "serverIP"
#define MQTT_PORT 1883

// Megan: C:/Users/megan/SchoolWork/GIT/final/World

char modifier = 'e';

int x = 0;
int y = 0;

int sockfd, connfd;

void on_connect(struct mosquitto *mosq, void *userdata, int result){
    if (result == 0){
        printf("connected to broker\n");
    } else {
        exit(EXIT_FAILURE);
    }
}

void connectSocket()
{
    struct sockaddr_in servaddr, cliaddr;
    // Creating TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Binding socket with address and port
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(sockfd, 5) != 0)
    {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client to connect...\n");
    socklen_t len = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr *) &cliaddr, &len);

    if (connfd < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");
    char message[MAX_LEN];
}

void navigate(char *currentFolder, int deltaY, int deltaX)
{
    x += deltaX;
    y += deltaY;

    char nextFolder[256];
    char parentFolder[256];

    snprintf(parentFolder, sizeof(parentFolder), "/home/garrett_masuda9/Final-Project/final/World");

    snprintf(nextFolder, sizeof(nextFolder), "%s/%c%d%d", parentFolder, modifier, x, y);

    DIR *dir = opendir(nextFolder);
    if (dir != NULL)
    {
        closedir(dir);
        strcpy(currentFolder, nextFolder);
    }
    else
    {
        x -= deltaX;
        y -= deltaY;
        printf("Invalid direction. Try again.\n");
    }
}

int readTxt(const char *filename, int directions[4])
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("fopen");
        return -1;
    }
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF)
    {
        directions[i] = c - '0';
        i++;
        if(i == 4){
            break;
        }
    }

    fclose(fp);
    return 0;
}

char * grabMessage(const char *filename, char message[16]){
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("fopen");
        //return someting;
    }
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF)
    {   
        if(i < 4){
            message[i - 4] = c - '0';
        }
        i++;
    }

    fclose(fp);
    return message;
}

int navigateWorld()
{
    char currentFolder[256];
    strcpy(currentFolder, "/home/garrett_masuda9/Final-Project/final/World/e00");

    struct mosquitto * mosq = NULL;
    const char *topic = "transferMessage";

    //Initialize the Mosquitto library
    mosquitto_lib_init();

    //Create a new Mosquitto client
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) 
    {
        printf("Error: Out of memory.\n");
    }

    //Set the callback for connection
    mosquitto_connect_callback_set(mosq, on_connect);

    //Connect to the broker
    if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS)
    {
        printf("Unable to connect to the broker.\n");
    }


    while (1)
    {
        char txtFileName[256];
        char nextTextFileName[256];
        char message[16];
        snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);

        int directions[4] = {0};
        if (readTxt(txtFileName, directions) != 0)
        {
            printf("Error reading directions from %s\n", txtFileName);
            break;
        }

        printf("Directions: Up=%d, Down=%d, Left=%d, Right=%d\n", directions[0], directions[1], directions[2], directions[3]);

        printf("Enter a direction (w = up, s = down, a = left, d = right, q = quit): ");
        char temp[1];
        //scanf(" %c", &input);
        read(connfd, temp, sizeof(temp));
        char input = temp[0];

        if (input == 'w' && (directions[0] == 1 || directions[0] == 2 || directions[0] == 3))
        {
            // Move up
            navigate(currentFolder, -1, 0);

            snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);
            char message[16];
            grabMessage(txtFileName, message);
            char temp[] = "mosquitto_pub -h -t transferMessage -m ";
            char * lcd = strcat(temp, message);
            mosquitto_publish(mosq, NULL, topic, 1, lcd, 0, false);
        }
        else if (input == 's' && (directions[1] == 1 || directions[1] == 2|| directions[1] == 3))
        {
            // Move down
            navigate(currentFolder, 1, 0);

            snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);
            char message[16];
            grabMessage(txtFileName, message);
            char temp[] = "mosquitto_pub -h -t transferMessage -m ";
            char * lcd = strcat(temp, message);
            mosquitto_publish(mosq, NULL, topic, 1, lcd, 0, false);
        }
        else if (input == 'a' && (directions[2] == 1 || directions[2] == 2|| directions[2] == 3))
        {
            // Move left
            navigate(currentFolder, 0, -1);

            snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);
            char message[16];
            grabMessage(txtFileName, message);
            char temp[] = "mosquitto_pub -h -t transferMessage -m ";
            char * lcd = strcat(temp, message);
            mosquitto_publish(mosq, NULL, topic, 1, lcd, 0, false);
        }
        else if (input == 'd' && (directions[3] == 1|| directions[3] == 2|| directions[3] == 3))
        {
            // Move right
            navigate(currentFolder, 0, 1);

            snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);
            char message[16];
            grabMessage(txtFileName, message);
            char temp[] = "mosquitto_pub -h -t transferMessage -m ";
            char * lcd = strcat(temp, message);
            mosquitto_publish(mosq, NULL, topic, 1, lcd, 0, false);
        }
        else if (input == 'q')
        {
            // Quit
            break;
        }
        else
        {
            printf("Invalid direction. Try again.\n");
        }
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

int main()
{
    connectSocket();
    navigateWorld();
    return 0;
}
