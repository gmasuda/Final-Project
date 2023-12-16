#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <mosquitto.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/stat.h>

// Sockets
#define PORT 8888
#define MAX_LEN 1024

// For MQTT
#define MQTT_HOST "34.136.238.197"
#define MQTT_PORT 1883

char modifier[2] = {'e', 'o'};
int world = 0;

int x = 0;
int y = 0;

int sockfd, connfd;
char sendstr[10];

int main(int argc, char* argv[])
{
FILE *fp= NULL;
pid_t process_id = 0;
pid_t sid = 0;
// Create child process
process_id = fork();
// Indication of fork() failure
if (process_id < 0)
{
printf("fork failed!\n");
// Return failure in exit status
exit(1);
}
// PARENT PROCESS. Need to kill it.
if (process_id > 0)
{
printf("process_id of child process %d \n", process_id);
    exit(0);
}
//unmask the file mode
umask(0);
//set new session
sid = setsid();
if(sid < 0)
{
// Return failure
exit(1);
}
// Change the current working directory to root.
chdir("/");
// Close stdin. stdout and stderr
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
// Open a log file in write mode.
fp = fopen ("Log.txt", "w+");
while (1)
{
    connectSocket();
    navigateWorld();
}
fclose(fp);
return (0);
}

void on_connect(struct mosquitto *mosq, void *userdata, int result) {
    if (result == 0) {
        printf("Connected to broker\n");
    } else {
        exit(EXIT_FAILURE);
    }
}
void changeWorld(char *currentFolder){
        printf("changed Worlds");
        x = 0;
        y = 0;
        world++;
char nextFolder[256];
    char parentFolder[256];

    snprintf(parentFolder, sizeof(parentFolder), "/home/garrett_masuda9/Final-Project/final/World");

    snprintf(nextFolder, sizeof(nextFolder), "%s/%c%d%d", parentFolder, modifier[world], y, x);
    nextFolder[sizeof(nextFolder) - 1] = '\0';  // Ensure null-termination

strcpy(currentFolder, nextFolder);
}
void connectSocket() {
    struct sockaddr_in servaddr, cliaddr;
    // Creating TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Binding socket with address and port
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(sockfd, 5) != 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client to connect...\n");
    socklen_t len = sizeof(cliaddr);
    connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);

    if (connfd < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");
    char message[MAX_LEN];
}

void navigate(char *currentFolder, int deltaY, int deltaX) {
    x += deltaX;
    y += deltaY;

    char nextFolder[256];
    char parentFolder[256];

    snprintf(parentFolder, sizeof(parentFolder), "/home/garrett_masuda9/Final-Project/final/World");

    snprintf(nextFolder, sizeof(nextFolder), "%s/%c%d%d", parentFolder, modifier[world], y, x);
    nextFolder[sizeof(nextFolder) - 1] = '\0';  // Ensure null-termination

    DIR *dir = opendir(nextFolder);
printf("x:%d", x);
        printf("y:%d", y);   
 if (dir != NULL) {
        closedir(dir);
        
strcpy(currentFolder, nextFolder);

    } else {
        x -= deltaX;
        y -= deltaY;
        printf("Invalid direction. Try again.\n");
    }
}


int readTxt(const char *filename, int directions[4]) {
printf("%s", filename);   
 FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Readtxt: %s", filename);
        perror("fopen");
        return -1;
    }
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        directions[i] = c - '0';
        i++;
        if (i == 4) {
            break;
        }
    }
int a  = 5;
char str;
memset(sendstr, 0, sizeof(sendstr));
printf("Message");
while ((str = fgetc(fp)) != EOF){
        printf("%c", str);
sendstr[a-5] = str;
a++;
}


    fclose(fp);
    return 0;
}

void grabMessage(const char *filename, char message[16]) {
static char wordd[10 + 1];

        
}

int navigateWorld() {
    char currentFolder[256];
    strcpy(currentFolder, "/home/garrett_masuda9/Final-Project/final/World/e00");

    struct mosquitto *mosq = NULL;
    const char *topic = "transferMessage";
    //const char *topic2 = "receiver";
    // Initialize the Mosquitto library
    mosquitto_lib_init();

    // Create a new Mosquitto client
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        printf("Error: Out of memory.\n");
        return EXIT_FAILURE;
    }

    // Set the callback for connection
    mosquitto_connect_callback_set(mosq, on_connect);

    // Connect to the broker
    if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS) {
        printf("Unable to connect to the broker.\n");
        return EXIT_FAILURE;
    }

    while (1) {
        char txtFileName[256];

        snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);
        txtFileName[sizeof(txtFileName) - 1] = '\0';  // Ensure null-termination

        char message[16];  // Declare message here

        int directions[4];
        if (readTxt(txtFileName, directions) != 0) {
            printf("Error reading directions from %s\n", txtFileName);
            break;
        }

        printf("Directions: Up=%d, Down=%d, Left=%d, Right=%d\n", directions[0], directions[1], directions[2], directions[3]);

        printf("Enter a direction (w = up, s = down, a = left, d = right, q = quit): ");
        char temp[1];
        read(connfd, temp, sizeof(temp));
        char input = tolower(temp[0]);
        printf("\nchar grabbed: %c\n", input); 

        if (input == 'w' && (directions[0] == 1 || directions[0] == 2 || directions[0] == 3)) {
            // Move up
            if (directions[0] == 3){changeWorld(currentFolder);}
            else{
                navigate(currentFolder, -1, 0);
                grabMessage(currentFolder, message);
                mosquitto_publish(mosq, NULL, topic, strlen(sendstr), sendstr, 0, false);}
        } else if (input == 's' && (directions[1] == 1 || directions[1] == 2 || directions[1] == 3)) {    
            // Move down
            if (directions[1] == 3){changeWorld(currentFolder);}
            else{

                navigate(currentFolder, 1, 0);
                printf("s is valid");
            
                grabMessage(currentFolder, message);
                            
                mosquitto_publish(mosq, NULL, topic, strlen(sendstr), sendstr, 0, false);}
        } else if (input == 'a' && (directions[2] == 1 || directions[2] == 2 || directions[2] == 3)) {
            // Move left
        if (directions[2] == 3){changeWorld(currentFolder);}
        else{
            navigate(currentFolder, 0, -1);

            snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);
            grabMessage(txtFileName, message);
           
            mosquitto_publish(mosq, NULL, topic, strlen(sendstr), sendstr, 0, false);}
        } else if (input == 'd' && (directions[3] == 1 || directions[3] == 2 || directions[3] == 3)) {
            // Move right
        if (directions[3] == 3){changeWorld(currentFolder);}
        else{
            navigate(currentFolder, 0, 1);

            snprintf(txtFileName, sizeof(txtFileName), "%s/neighboring_points.txt", currentFolder);
            grabMessage(txtFileName, message);
            
            mosquitto_publish(mosq, NULL, topic, strlen(sendstr), sendstr, 0, false);}
        } 
        else if (input == 'q') {
            // Quit
            break;
        }
        else{
            printf("\ndirections: %d\n", directions[3]);
            char temp2[] = "Cannot move";
            mosquitto_publish(mosq, NULL, topic, strlen(temp2), temp2, 0, false);
            printf("Invalid direction. Try again.\n");
        }
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return EXIT_SUCCESS;
}