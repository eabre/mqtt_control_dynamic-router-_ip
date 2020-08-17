#include <stdio.h>
#include <mosquitto.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>

#define TRUE 1
#define FALSE 0
#define SECOND 1
#define MINUTE SECOND*60
#define HOUR MINUTE*60


FILE *fp;
FILE *tp;
FILE *fp_2;

int main(){

    int rc;
    char texter_to_publish[1024]={0};
    char texter[2048]={0};
    char texter_temp[2048]={0};
    char texter_third[2048]={0};
    char same_time[256]={0};
    char time_cmp[128] ="Thu Jan  1 02:00:00 1970\n";
    int count =0;
    char command[1024]="curl -4 https://icanhazip.com/ > first.txt";
    char command_second[1024]="curl -4 https://icanhazip.com/ > second.txt";
    char command_if_config[1024]= "ifconfig > third.txt";
    struct mosquitto * mosq;
    printf("burada 0\n");
    time_t t;   // not a primitive datatype
    while(1)
    {
        // first ip info
        system(command);
        fp= fopen("first.txt","r");
        if(fp == NULL)
        {
            printf("\nCould not find first.txt\n");
            return -1;
        }
        while (1)
        {
            texter[count]= fgetc(fp);
            count++;
            if(feof(fp))
            {
                break;
            }
        }
        count= 0;
    printf("\n texter %s\n",texter);
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
        sleep(5*SECOND);
        //second ip info
        system(command_second);
        fp_2 = fopen("second.txt","r");
        if(fp_2 == NULL)
        {
            printf("\nCould not find second.txt\n");
            return -1;
        }
        while (1)
        {

            texter_temp[count]= fgetc(fp_2);
            count++;
            if(feof(fp_2))
            {
                break;
            }
        }
        count= 0;
        fseek(fp_2, 0, SEEK_SET);
        fclose(fp_2);
        printf("\nFIRST:%s\n\n\nSECOND:%s\n\n",texter, texter_temp);

        //ifconfig info
        system(command_if_config);
        tp= fopen("third.txt","r");
        if(tp == NULL)
        {
            printf("Could not find third.txt\n");
            return -1;
        }
        while (1)
        {
            texter_third[count]= fgetc(tp);
            count++;
            if(feof(tp))
            {
                printf("burada 4\n");
                break;
            }
        }
        printf("\n Texter_third %s\n",texter_third);
            count= 0;
            fseek(tp, 0, SEEK_SET);
            fclose(tp);

        //comparing ips
        if(strcmp(texter, texter_temp) == 0)
        {
            mosquitto_lib_init();
            mosq = mosquitto_new("publisher-test", TRUE, NULL);
            printf("burada 5\n");

            rc = mosquitto_connect(mosq, "mqtt.eclipse.org", 1883, 10);
            if(rc != 0)
            {
                printf("Client could not connect to broker! Error Code: %d\n", rc);
                mosquitto_destroy(mosq);
                return -1;
            }
            printf("We are now connected to the broker!\n");
            strcat(same_time,"\n\nSame ip is :");
            strcat(same_time,texter);
            strcat(same_time, "\n\nIfconfig infos\n\n");
            strcat(same_time, texter_third);
            if(strcmp(ctime(&t),time_cmp) == 0)
            {
                strcat(same_time,"\n\nCurrent time is ");
                time(&t);
                strcat(same_time,ctime(&t));
            }
            else
            {
            strcat(same_time,"\n\nTime has been changed at:");
            strcat(same_time,ctime(&t));
            }
            mosquitto_publish(mosq, NULL, "Router informations" , strlen(same_time), same_time, 0, FALSE);
            memset(same_time, 0, sizeof(same_time));
            mosquitto_disconnect(mosq);
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();
        }
        else
        {
            mosquitto_lib_init();
            time(&t);
            mosq = mosquitto_new("\nPublisher-test\n", TRUE, NULL);
            printf("burada 6\n");
            rc = mosquitto_connect(mosq, "mqtt.eclipse.org", 1883, 10);
            if(rc != 0){
                printf("Client could not connect to broker! Error Code: %d\n", rc);
                mosquitto_destroy(mosq);
                return -1;
            }
            printf("We are now connected to the broker!\n");
            strcat(texter_to_publish,"\n\n\nIFCONFIG INFOS\n\n\n");
            strcat(texter_third,"\n\n\nDYNAMIC IP\n\n\n");
            strcat(texter_third,texter_temp);
            strcat(texter_third,"\n\n\ncCURRENT TIME IS:");
            strcat(texter_third,ctime(&t));
            strcat(texter_third,"\n\n\nOLD DYNAMIC IP\n\n\n");
            strcat(texter_third,texter);
            strcat(texter_to_publish,texter_third);
            mosquitto_publish(mosq, NULL, "Router informations", strlen(texter_to_publish), texter_to_publish, 0, FALSE);
            mosquitto_disconnect(mosq);
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();

            //when new ip found, system sends new ip to the server and subscribers can receives the dynamic ips'
        }
        memset(texter, 0, sizeof(texter));
        memset(texter_temp, 0, sizeof(texter_temp));
        memset(texter_third, 0, sizeof(texter_third));
        memset(texter_to_publish, 0, sizeof(texter_to_publish));
    }
    return 0;
}

