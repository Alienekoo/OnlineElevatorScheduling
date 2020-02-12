/*
Submitted by Alekhya
Online part of the Elevator Scheduling 

PLEASE READ:
>please use:"gcc project.c -lm -o " instead of just gcc
        >gcc project.c -lm -o main
        >./main 0 4 passengers.txt s_e.txt s_p.txt
?I assumed argv[1] as 0 for online and did not use it in the code as it is just one part now.
in the passengers.txt I made an END line to end the passngers. It should not be altered for this code. If the number of passengers increase then pls edit the END lines time to be something very higher than the highest arrival time. And also change END_TIME in the code to it.
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define END_TIME  5000 //edit it to something very higher than the final arrival time
#define MAX_FLOOR  12
#define MAX_WAITLIST_SIZE  50
#define MAX_INPUTS 1000
struct Passenger
{
    char *Pid;
    int ta;
    int fa;
    int fd;
    int dir;
    int getElevator;
    int intime;
    int offtime;
};
//typedef
struct elevator
{
    char *Eid;
    int getdir;
    int switch_on;
    int pickordropmode;
    int getPassengerCount;
    int getPassengers[10]; //passengers inside the elevator
    int getWaitlist[MAX_WAITLIST_SIZE];   //passengers assgined to the elevator but waiting
    int getLastModifiedTime;
    int getPurpose; // 0-stop, 1-goes up, 2-goes down, 3-picks up 4-drops off
    int getFloor;
    int getTotalFloors;
    int getTotalPassengers;
};

//typedef

char **str_split(char *a_str, const char a_delim)
{
    char **result = 0;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char *) * count);

    if (result)
    {
        char delim[2] = {a_delim, '\0'};
        size_t idx = 0;
        char *token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

//defs AddToList to enter Passenger into lists and sorting as per the fa or fd
// see getdir of elevator then add at the front or back
int AddToList(struct Passenger *input, int b, struct elevator *lift, int i, int f)
{

    int j = 0, maxposition = 0, n = 1, c = 0;
    //(input+b)->fa/fd into (lift+i)->getWaitlist if f=0 or (lift+i)->getPassengers if f=1
    //add to waitlist
    if (f == 0)
    {
        
        int d[MAX_WAITLIST_SIZE] = {0};
        //d = (lift+i)->getWaitlist;
        for (c = 0; (lift + i)->getWaitlist[c] != '\0'; ++c)
        {
            d[c] = (lift + i)->getWaitlist[c];
            n++;
        }
        //if the elevator going up ascending till highest floor and descending
        //if passenger has to go in the same direction as the elevator start from the beginning

        if ((lift + i)->getdir == 0)
        {
            if ((lift + i)->getFloor <= (input + b)->fa)
            {
                //locate the position j where to fit the passenger as per their fa
                while ((d[j]) != '\0')
                {
                    if ((input + d[j])->fa <= (input + b)->fa)
                    {
                        maxposition = j + 1;
                    }
                    else
                        break;
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }
            // if the passenger has to go in the opposite direction add in the back
            else if ((lift + i)->getFloor > (input + b)->fa)
            {
                while ((d[j]) != '\0')
                {
                    if ((input + d[j])->fa > (input + b)->fa)
                    {
                        maxposition = j + 2;
                        if ((input + d[j + 1])->fa == '\0') maxposition = j+1;
                    }
                    else if ((input + d[j])->fa > (input + d[j + 1])->fa)
                    {
                        if ((input + d[j])->fa >= (input + b)->fa)
                        {
                            maxposition = j + 1;
                        }
                    }
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }
        }
        else if ((lift + i)->getdir == 1)
        {
            if ((lift + i)->getFloor >= (input + b)->fa)
            {
                //locate the position j where to fit the passenger as per their fa
                while ((d[j]) != '\0')
                {
                    if ((input + d[j])->fa >= (input + b)->fa)
                    {
                        maxposition = j + 1;
                    }
                    else
                        break;
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }

            else if ((lift + i)->getFloor < (input + b)->fa)
            {
                while ((d[j]) != '\0')
                {
                    if ((input + d[j])->fa >= (input + d[j + 1])->fa)
                    {
                        maxposition = j + 2;
                        if ((input + d[j + 1])->fa == '\0') maxposition = j+1;
                    }
                    else if ((input + d[j])->fa < (input + d[j + 1])->fa)
                    {
                        if ((input + d[j])->fa <= (input + b)->fa)
                        {
                            maxposition = j + 1;
                        }
                    }
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }
        }
        //add b into d[] at maxposition

        if (n != 0)
        {
            for (c = n - 1; c >= maxposition; c--)
                d[c + 1] = d[c];
        }

        d[maxposition] = b;
        //(lift+i)->getWaitlist = d;

        for (c = 0; d[c] != '\0'; ++c)
        {
            (lift + i)->getWaitlist[c] = d[c];
        }
    }
    if (f == 1)
    {
        int d[10] = {0};
        //d = (lift+i)->getWaitlist;
        for (c = 0; (lift + i)->getPassengers[c] != '\0'; ++c)
        {
            d[c] = (lift + i)->getPassengers[c];
            n++;
        }
        //if the elevator going up ascending till highest floor and descending
        //if passenger has to go in the same direction as the elevator start from the beginning

        if ((lift + i)->getdir == 0)
        {
            if ((lift + i)->getFloor <= (input + b)->fd)
            {
                //locate the position j where to fit the passenger as per their fa
                while ((d[j]) != '\0')
                {
                    if ((input + d[j])->fd <= (input + b)->fd)
                    {
                        maxposition = j + 1;
                    }
                    else
                        break;
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }
            // if the passenger has to go in the opposite direction add in the back
            else if ((lift + i)->getFloor > (input + b)->fd)
            {
                while ((d[j]) != '\0')
                {
                    if ((input + d[j])->fd <= (input + d[j + 1])->fd)
                    {
                        maxposition = j + 2;
                    }
                    else if ((input + d[j])->fd > (input + d[j + 1])->fd)
                    {
                        if ((input + d[j])->fd >= (input + b)->fd)
                        {
                            maxposition = j + 1;
                        }
                    }
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }
        }
        if ((lift + i)->getdir == 1)
        {
            if ((lift + i)->getFloor >= (input + b)->fd)
            {
                //locate the position j where to fit the passenger as per their fa
                while ((d[j]) != '\0')
                {
                    
                    if ((input + d[j])->fd >= (input + b)->fd)
                    {
                        maxposition = j + 1;
                    }
                    else
                        break;
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }
            else if ((lift + i)->getFloor < (input + b)->fd)
            {
                while ((d[j]) != '\0')
                {
                    
                    if ((input + d[j])->fd >= (input + d[j + 1])->fd)
                    {
                        maxposition = j + 2;
                        if ((input + d[j + 1])->fd == '\0')
                            maxposition = j + 1;
                    }
                    else if ((input + d[j])->fd < (input + d[j + 1])->fd)
                    {
                        if ((input + d[j])->fd <= (input + b)->fd)
                        {
                            maxposition = j + 1;
                        }
                    }
                    j++;
                } //maxposition is the position where the passenger goes
                // add b into the waitlist at position j (waitlist cant exceed max size but that's gotta be looked bfor adding)
            }

        }
        //add b into d[] at maxposition

        if (n != 0)
        {
            for (c = n - 1; c >= maxposition; c--)
                d[c + 1] = d[c];
        }

        d[maxposition] = b;
        //(lift+i)->getWaitlist = d;

        for (c = 0; d[c] != '\0'; ++c)
        {
            (lift + i)->getPassengers[c] = d[c];
        }
    }

    return 0;
}
//def Remove to remove 0 th element
int Remove(struct Passenger *input, struct elevator *lift, int i, int f)
{
    //remove 0 th element or first element
    int j = 0, position = 0, n = 0, c;
    if (f == 0)
    {
        int d[MAX_WAITLIST_SIZE];
        //d = (lift+i)->getWaitlist;
        for (c = 0; (lift + i)->getWaitlist[c] != '\0'; ++c)
        {
            d[c] = (lift + i)->getWaitlist[c];
            n++;
        }
        for (c = position; c < n - 1; c++)
            d[c] = d[c + 1];

        d[n - 1] = '\0';

        for (int c = 0; d[c] != '\0'; ++c)
        {
            (lift + i)->getWaitlist[c] = d[c];
        }
        (lift + i)->getWaitlist[n - 1] = '\0';
        if (n == 1)
            (lift + i)->getWaitlist[0] = '\0';
    }

    if (f == 1)
    {
        int d[10];
        //d = (lift+i)->getPassengers;
        for (c = 0; (lift + i)->getPassengers[c] != '\0'; ++c)
        {
            d[c] = (lift + i)->getPassengers[c];
            n++;
        }
        for (c = position; c < n - 1; c++)
            d[c] = d[c + 1];

        d[n - 1] = '\0';

        for (int c = 0; d[c] != '\0'; ++c)
        {
            (lift + i)->getPassengers[c] = d[c];
        }
        (lift + i)->getPassengers[n - 1] = '\0';
        if (n == 1)
            (lift + i)->getPassengers[0] = '\0';
    }
    return 0;
}
int main(int argc, char *argv[])
{
    FILE *fp, *fptr, *se, *sp;
    char line[30],linez[30], *line1,*line2;
    size_t len = 0;
    ssize_t read;
    int i, j, l, a, b, c, z = 1, k = 0, q, t, t1, nextstop, diff = 0, fc, n = 0, m = 0, maxfa, maxfd, minfa, minfd, mincombo_elevator, maxwait, maxdest, mincombo;
    float avgwait, avgdest;
    char str[3];
    char *arr[4];
    char *ptrarg;
    int arr1[3];
    int PassengerCountLimit = MAX_INPUTS;
    const int ElevatorCount = strtol(argv[2], &ptrarg, 10);
    struct Passenger *input = malloc(sizeof(struct Passenger) * PassengerCountLimit);
    struct elevator *lift = malloc(sizeof(struct elevator) * ElevatorCount);

    if (!input || !lift)
    {
        perror("Error allocating memory");
        abort();
    }

    for (i = 0; i < ElevatorCount; i++)
    {
        (lift + i)->switch_on = 0;
        (lift + i)->pickordropmode = 0;
        (lift + i)->getdir = 0;
        (lift + i)->getTotalPassengers = 0;
        (lift + i)->getTotalFloors = 0;
        (lift + i)->getPurpose = 0;
        (lift + i)->getFloor = 1;
        (lift + i)->getLastModifiedTime = 0;
    }
    fp = fopen(argv[3], "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    fptr = fopen(argv[4], "w+");
    
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        
        char **tokens;
        line1 = line;
        
        tokens = str_split(line1, ' ');
        if (tokens)
        {
            for (i = 0; *(tokens + i); i++)
            {
                arr[i] = *(tokens + i);
            }
        }
        for (i = 0; i < 3; i++)
        {
            
            //for arr1[i]
            arr1[i] = 0;
            j = 0;
            q = 0;
            while (*(arr[i + 1] + j) != 0 && *(arr[i + 1] + j) != 13)
            {
                j++;
            }
            q = j - 1;
            for (k = 0; k < j; k++)
            {
                arr1[i] += pow(10, q) * (*(arr[i + 1] + k) - 48);
                q--;
            }
        }
        
        (input + z)->Pid = arr[0];
        (input + z)->ta = arr1[0];
        (input + z)->fa = arr1[1];
        (input + z)->fd = arr1[2];
        if (((arr1[2]) - (arr1[1])) > 0)
            (input + z)->dir = 0; //up
        if (((arr1[2]) - (arr1[1])) < 0)
            (input + z)->dir = 1; //down
        else
            (input + z)->dir = 0;

        //===============================================================================================================================================
        //elevator status update

        for (i = 0; i < ElevatorCount; i++)
        {
            t1 = 0;
            a = ((input + z)->ta) - ((lift + i)->getLastModifiedTime);
            if ((lift + i)->getPurpose == 3 || (lift + i)->getPurpose == 4)
            {
                //if up
                if ((lift + i)->getdir == 0)
                {

                    (lift + i)->getPurpose = 1;
                }
                //if down
                else if ((lift + i)->getdir == 1)
                {

                    (lift + i)->getPurpose == 2;
                }
            }
            //when it's in stop position
            if ((lift + i)->getPurpose == 0)
            {
                //elevator is at stop of last dropped floor. Now the new arrival will decide the direction
                if ((lift + i)->getWaitlist[0] != '\0')
                {
                    b = (lift + i)->getWaitlist[0];
                    if ((input + b)->fa < (lift + i)->getFloor)
                        (lift + i)->getdir = 1;
                    else
                        (lift + i)->getdir = 0;
                    //if up
                    if ((lift + i)->getdir == 0)
                    {
                        (lift + i)->getLastModifiedTime = (input + b)->ta;
                        (lift + i)->getPurpose = 1;
                    }
                    //if down
                    else if ((lift + i)->getdir == 1)
                    {
                        (lift + i)->getLastModifiedTime = (input + b)->ta;
                        (lift + i)->getPurpose = 2;
                    }
                }
                else
                    continue;
            }

            // add more conditions when just picking or just dropping off, what if the elevator has to change the directions
            do
            {
                // while going up
                if ((lift + i)->getPurpose == 1)
                {
                    b = (lift + i)->getWaitlist[0];
                    if ((input + b)->fa < (lift + i)->getFloor && (input + b)->fa != 0)
                    {
                        (lift + i)->getdir = 1;
                        (lift + i)->getPurpose = 2;
                        break;
                    }
                    if ((lift + i)->getPassengers[0] == '\0' && (lift + i)->getWaitlist[0] == '\0')
                    {
                        (lift + i)->getPurpose = 0;
                        break;
                    }
                    //when Passenger list is empty, pick up passengers
                    else if ((lift + i)->getPassengers[0] == '\0' && (lift + i)->getWaitlist[0] != '\0')
                    {

                        b = (lift + i)->getWaitlist[0];
                        c = (lift + i)->getPassengers[0];
                        t = (((input + b)->fa - (lift + i)->getFloor) * 2) + 1;

                        if (t == 1)
                            t = 0;
                        if (a >= t + t1)
                        {
                            if ((lift + i)->getPassengers[9] == '\0')
                            {
                                AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1);
                                Remove(input, lift, i, 0);
                                //write into the output form that the Passenger is picked up at time ((lift+i)->getLastModifiedTime + t
                                (lift + i)->getPurpose = 3;
                                (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                (lift + i)->getTotalFloors += ((input + b)->fa - (lift + i)->getFloor);
                                (lift + i)->getFloor = (input + b)->fa;
                                t1 = t1 + t;
                                (input + b)->getElevator = i;
                                if ((lift + i)->pickordropmode == 0)
                                {
                                    (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                    fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                }
                                else
                                {
                                    (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                    fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                }
                            }
                        }
                        else
                        {
                            //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                            break;
                        }
                    }
                    else if ((lift + i)->getPassengers[0] != '\0' && (lift + i)->getWaitlist[0] != '\0')
                    {

                        b = (lift + i)->getWaitlist[0];
                        c = (lift + i)->getPassengers[0];
                        //if both arrival and destinations on the way up
                        if ((input + b)->fa >= (lift + i)->getFloor && (input + c)->fd >= (lift + i)->getFloor)
                        {
                            // when dropping off passengers
                            if ((input + b)->fa > (input + c)->fd)
                            {
                                //time to reach fd
                                t = (((input + c)->fd - (lift + i)->getFloor) * 2) + 1;
                                if (t == 1)
                                    t = 0;
                                if (a >= t + t1)
                                {
                                    Remove(input, lift, i, 1);
                                    //write into the output form that the Passenger is dropped at time ((lift+i)->getLastModifiedTime + t
                                    (lift + i)->getPurpose = 4;
                                    (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                    (lift + i)->getTotalPassengers++;
                                    (lift + i)->getTotalFloors += ((input + c)->fd - (lift + i)->getFloor);
                                    (lift + i)->getFloor = (input + c)->fd;
                                    t1 = t1 + t;
                                    if ((lift + i)->pickordropmode == 0)
                                    {
                                        (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                        fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                                    }
                                    else
                                    {
                                        (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                        fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                                    }
                                }
                                else
                                {
                                    //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                    break;
                                }
                            }
                            //pick up passengers
                            else if ((input + b)->fa < (input + c)->fd)
                            {
                                //pick up the passenger of waitlist[0]
                                //time it takes to reach the arrival getFloor
                                t = (((input + b)->fa - (lift + i)->getFloor) * 2) + 1;
                                if (t == 1)
                                    t = 0;
                                // if t = 0;
                                if (a >= t + t1)
                                {
                                    if ((lift + i)->getPassengers[9] == '\0')
                                    {
                                        AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1);
                                        Remove(input, lift, i, 0);
                                        //write into the output form that the Passenger is picked up at time ((lift+i)->getLastModifiedTime + t
                                        (lift + i)->getPurpose = 3;
                                        (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                        (lift + i)->getTotalFloors += ((input + b)->fa - (lift + i)->getFloor);
                                        (lift + i)->getFloor = (input + b)->fa;
                                        t1 = t1 + t;
                                        (input + b)->getElevator = i;
                                        if ((lift + i)->pickordropmode == 0)
                                        {
                                            (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                            fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                        }
                                        else
                                        {
                                            (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                            fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                        }
                                    }
                                }
                                else
                                {
                                    //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                    break;
                                }
                            }
                            //both drop off and pick up on same floor
                            else
                            {
                                t = (((input + b)->fa - (lift + i)->getFloor) * 2) + 1;
                                if (t == 1)
                                    t = 0;
                                if (a >= t + t1)
                                {
                                    Remove(input, lift, i, 1);
                                    if ((lift + i)->getPassengers[9] == '\0')
                                    {
                                        AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1);
                                        Remove(input, lift, i, 0);
                                        //write down to the output file
                                        (lift + i)->getPurpose = 3;
                                        (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                        (lift + i)->getTotalFloors += ((input + b)->fa - (lift + i)->getFloor);
                                        (lift + i)->getFloor = (input + b)->fa;
                                    }
                                    else
                                    {
                                        //write down to the output files
                                        (lift + i)->getPurpose = 4;
                                        (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                        (lift + i)->getTotalPassengers++;
                                        (lift + i)->getTotalFloors += ((input + c)->fd - (lift + i)->getFloor);
                                        (lift + i)->getFloor = (input + c)->fd;
                                    }
                                    t1 = t1 + t;
                                    (input + b)->getElevator = i;
                                    if ((lift + i)->pickordropmode == 0)
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                    else
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                    if ((lift + i)->getPassengers[9] == '\0')
                                        if ((lift + i)->pickordropmode == 0)
                                        {
                                            (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                            fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                                        }
                                        else
                                        {
                                            (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                            fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                                        }
                                }
                                else
                                {
                                    //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                    break;
                                }
                            }
                        }
                        //if arrival is ahead but for next drop off the elevator has to change the direction
                        else if ((input + b)->fa >= (lift + i)->getFloor && (input + c)->fd < (lift + i)->getFloor)
                        {
                            //pick up arrivals in waitlist as long as the highest floor is reached
                            //pick up the passenger of waitlist[0]
                            //time it takes to reach the arrival getFloor
                            t = (((input + b)->fa - (lift + i)->getFloor) * 2) + 1;
                            if (t == 1)
                                t = 0;
                            if (a >= t + t1)
                            {
                                if ((lift + i)->getPassengers[9] == '\0')
                                {
                                    AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1);
                                    Remove(input, lift, i, 0);
                                    //write into the output form that the Passenger is picked up at time ((lift+i)->getLastModifiedTime + t
                                    (lift + i)->getPurpose = 3;
                                    (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                    (lift + i)->getTotalFloors += ((input + b)->fa - (lift + i)->getFloor);
                                    (lift + i)->getFloor = (input + b)->fa;
                                    t1 = t1 + t;
                                    (input + b)->getElevator = i;
                                    if ((lift + i)->pickordropmode == 0)
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                    else
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                }
                            }
                            else
                            {
                                //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                break;
                            }
                        }
                        //if drop off is ahead but the next arrival is on the lower floor. drop off and change direction
                        else if ((input + b)->fa < (lift + i)->getFloor && (input + c)->fd >= (lift + i)->getFloor)
                        {
                            t = (((input + c)->fd - (lift + i)->getFloor) * 2) + 1;
                            if (t == 1)
                                t = 0;
                            //printf("while21 a, t and t1 are %d %d %d \n", a, t, t1);
                            if (a >= t + t1)
                            {
                                Remove(input, lift, i, 1);
                                //write into the output form that the Passenger is dropped at time ((lift+i)->getLastModifiedTime + t
                                (lift + i)->getPurpose = 4;
                                (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                (lift + i)->getTotalPassengers++;
                                (lift + i)->getTotalFloors += ((input + c)->fd - (lift + i)->getFloor);
                                (lift + i)->getFloor = (input + c)->fd;
                                t1 = t1 + t;
                                if ((lift + i)->pickordropmode == 0)
                                {
                                    (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                    fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                                }
                                else
                                {
                                    (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                    fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                                }
                            }
                            else
                            {
                                //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                break;
                            }
                        }
                        //if both arrival and destination are on opposite side/lower floors
                        else
                        {
                            if ((lift + i)->getPurpose == 3 || (lift + i)->getPurpose == 4)
                            {
                                //add 2 seconds to lastmodified time
                                (lift + i)->getLastModifiedTime += 2;
                            }
                            (lift + i)->getPurpose = 2;
                            (lift + i)->getdir = 1;
                            //print onto the output form that the elevaor goes down at time ((lift+i)->getLastModifiedTime + 2 (2 seconds to pick up or drop off)
                        }

                    } //Passengers and waitlist not empty
                    //when waitlist is empty, just drop off the passengers
                    else if ((lift + i)->getPassengers[0] != '\0' && (lift + i)->getWaitlist[0] == '\0')
                    {
                        b = (lift + i)->getWaitlist[0];
                        c = (lift + i)->getPassengers[0];
                        t = (((input + c)->fd - (lift + i)->getFloor) * 2) + 1;
                        if (t == 1)
                            t = 0;
                        if (a >= t + t1)
                        {
                            Remove(input, lift, i, 1);
                            //write into the output form that the Passenger is dropped at time ((lift+i)->getLastModifiedTime + t
                            (lift + i)->getPurpose = 4;
                            (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                            (lift + i)->getTotalPassengers++;
                            (lift + i)->getTotalFloors += ((input + c)->fd - (lift + i)->getFloor);
                            (lift + i)->getFloor = (input + c)->fd;
                            t1 = t1 + t;
                            if ((lift + i)->pickordropmode == 0)
                            {
                                (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                            }
                            else
                            {
                                (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                            }
                        }
                        else
                        {
                            //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                            break;
                        }
                    }
                }

                //while going down
                else if ((lift + i)->getPurpose == 2)
                {
                    b = (lift + i)->getWaitlist[0];
                    c = (lift + i)->getPassengers[0];
                    if ((input + b)->fa > (lift + i)->getFloor)
                    {
                        (lift + i)->getdir = 0;
                        (lift + i)->getPurpose = 1;
                        break;
                    }
                    if ((lift + i)->getPassengers[0] == '\0' && (lift + i)->getWaitlist[0] == '\0')
                    {
                        (lift + i)->getPurpose = 0;
                    }
                    //when Passenger list is empty, pick up passengers
                    else if ((lift + i)->getPassengers[0] == '\0' && (lift + i)->getWaitlist[0] != '\0')
                    {
                        b = (lift + i)->getWaitlist[0];
                        c = (lift + i)->getPassengers[0];
                        t = ((lift + i)->getFloor - (input + b)->fa) + 2;
                        if (t == 2)
                            t = 0;
                        if (t == 3)
                            t = 2;
                        if (a >= t + t1)
                        {
                            if ((lift + i)->getPassengers[9] == '\0')
                            {
                                AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1);
                                Remove(input, lift, i, 0);
                                //write into the output form that the Passenger is picked up at time ((lift+i)->getLastModifiedTime + t
                                (lift + i)->getPurpose = 3;
                                (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + b)->fa);
                                (lift + i)->getFloor = (input + b)->fa;
                                t1 = t1 + t;
                                (input + b)->getElevator = i;
                                if ((lift + i)->pickordropmode == 0)
                                {
                                    (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                    fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                }
                                else
                                {
                                    (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                    fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                }
                            }
                        }
                        else
                        {
                            //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                            break;
                        }
                    }
                    else if ((lift + i)->getPassengers[0] != '\0' && (lift + i)->getWaitlist[0] != '\0')
                    {
                        b = (lift + i)->getWaitlist[0];
                        c = (lift + i)->getPassengers[0];
                        //if both arrival and destinations on the way down
                        if ((input + b)->fa <= (lift + i)->getFloor && (input + c)->fd <= (lift + i)->getFloor)
                        {
                            // when dropping off passengers
                            if ((input + b)->fa < (input + c)->fd)
                            {

                                //time to reach fd
                                t = ((lift + i)->getFloor - (input + c)->fd) + 2;
                                if (t == 2)
                                    t = 0;
                                if (t == 3)
                                    t = 2;
                                if (a >= t + t1)
                                {
                                    Remove(input, lift, i, 1);
                                    //write into the output form that the Passenger is dropped at time ((lift+i)->getLastModifiedTime + t
                                    (lift + i)->getPurpose = 4;
                                    (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                    (lift + i)->getTotalPassengers++;
                                    (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + c)->fd);
                                    (lift + i)->getFloor = (input + c)->fd;
                                    t1 = t1 + t;
                                    if ((lift + i)->pickordropmode == 0)
                                    {
                                        (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                        fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                                    }
                                    else
                                    {
                                        (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                        fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                                    }
                                }
                                else
                                {
                                    //print onto the output form that the elevator goes down as purpose at time ((lift+i)->getLastModifiedTime + t
                                    break;
                                }
                            }
                            //pick up passengers
                            else if ((input + b)->fa > (input + c)->fd)
                            {
                                //pick up the passenger of waitlist[0]
                                //time it takes to reach the arrival getFloor
                                t = ((lift + i)->getFloor - (input + b)->fa) + 2;
                                if (t == 2)
                                    t = 0;
                                if (t == 3)
                                    t = 2;
                                if (a >= t + t1)
                                {
                                    if ((lift + i)->getPassengers[9] == '\0')
                                    {
                                        AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1);
                                        Remove(input, lift, i, 0);
                                        //write into the output form that the Passenger is picked up at time ((lift+i)->getLastModifiedTime + t
                                        (lift + i)->getPurpose = 3;
                                        (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                        (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + b)->fa);
                                        (lift + i)->getFloor = (input + b)->fa;
                                        t1 = t1 + t;
                                        (input + b)->getElevator = i;
                                        if ((lift + i)->pickordropmode == 0)
                                        {
                                            (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                            fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                        }
                                        else
                                        {
                                            (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                            fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                        }
                                    }
                                }
                                else
                                {
                                    //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                    break;
                                }
                            }
                            //both drop off and pick up on same floor
                            else
                            {
                                t = ((lift + i)->getFloor - (input + b)->fa) + 2;
                                if (t == 2)
                                    t = 0;
                                if (t == 3)
                                    t = 2;
                                if (a >= t + t1)
                                {
                                    Remove(input, lift, i, 1);
                                    if ((lift + i)->getPassengers[9] == '\0')
                                    {
                                        AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1);
                                        Remove(input, lift, i, 0);
                                        //write down to the output file
                                        (lift + i)->getPurpose = 3;
                                        (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                        (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + b)->fa);
                                        (lift + i)->getFloor = (input + b)->fa;
                                    }
                                    else
                                    {
                                        //write down to the output files
                                        (lift + i)->getPurpose = 4;
                                        (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                        (lift + i)->getTotalPassengers++;
                                        (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + c)->fd);
                                        (lift + i)->getFloor = (input + c)->fd;
                                    }
                                    t1 = t1 + t;
                                    (input + b)->getElevator = i;
                                    if ((lift + i)->pickordropmode == 0)
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                    else
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                    if ((lift + i)->getPassengers[9] == '\0')
                                        if ((lift + i)->pickordropmode == 0)
                                        {
                                            (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                            fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                                        }
                                        else
                                        {
                                            (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                            fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                                        }
                                }
                                else
                                {
                                    //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                    break;
                                }
                            }
                        }
                        //if arrival is ahead but for next drop off the elevator has to change the direction
                        else if ((input + b)->fa <= (lift + i)->getFloor && (input + c)->fd > (lift + i)->getFloor)
                        {
                            //pick up arrivals in waitlist as long as the highest floor is reached
                            //pick up the passenger of waitlist[0]
                            //time it takes to reach the arrival getFloor
                            t = ((lift + i)->getFloor - (input + b)->fa) + 2;
                            if (t == 2)
                                t = 0;
                            if (t == 3)
                                t = 2;
                            if (a >= t + t1)
                            {
                                if ((lift + i)->getPassengers[9] == '\0')
                                {
                                    AddToList(input, (lift + i)->getWaitlist[0], lift, i, 1); // sorted as per fd and ta and getdir
                                    Remove(input, lift, i, 0);
                                    //write into the output form that the Passenger is picked up at time ((lift+i)->getLastModifiedTime + t
                                    (lift + i)->getPurpose = 3;
                                    (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                    (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + b)->fa);
                                    (lift + i)->getFloor = (input + b)->fa;
                                    t1 = t1 + t;
                                    (input + b)->getElevator = i;
                                    if ((lift + i)->pickordropmode == 0)
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                    else
                                    {
                                        (input + b)->intime = (lift + i)->getLastModifiedTime + t1 + 1;
                                        fprintf(fptr, "%d E%d %d picksup:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + b)->Pid);
                                    }
                                }
                            }
                            else
                            {
                                //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                break;
                            }
                        }
                        //if drop off is ahead but the next arrival is on the upper floor. drop off and change direction
                        else if ((input + b)->fa > (lift + i)->getFloor && (input + c)->fd <= (lift + i)->getFloor)
                        {
                            t = ((lift + i)->getFloor - (input + c)->fd) + 2;
                            if (t == 2)
                                t = 0;
                            if (t == 3)
                                t = 2;
                            if (a >= t + t1)
                            {
                                Remove(input, lift, i, 1);
                                //write into the output form that the Passenger is dropped at time ((lift+i)->getLastModifiedTime + t
                                (lift + i)->getPurpose = 4;
                                (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                                (lift + i)->getTotalPassengers++;
                                (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + c)->fd);
                                (lift + i)->getFloor = (input + c)->fd;
                                t1 = t1 + t;
                                if ((lift + i)->pickordropmode == 0)
                                {
                                    (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                    fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                                }
                                else
                                {
                                    (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                    fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                                }
                            }
                            else
                            {
                                //print onto the output form that the elevator goes up as purpose at time ((lift+i)->getLastModifiedTime + t
                                break;
                            }
                        }
                        //if both arrival and destination are on opposite side/upper floors
                        else
                        {
                            if ((lift + i)->getPurpose == 3 || (lift + i)->getPurpose == 4)
                            {
                                //add 2 seconds to lastmodified time
                                (lift + i)->getLastModifiedTime += 2;
                            }
                            (lift + i)->getPurpose = 1;
                            (lift + i)->getdir = 0;
                            //print onto the output form that the elevaor goes up at time ((lift+i)->getLastModifiedTime + 2 (2 seconds to pick up or drop off)
                        }

                    } //Passengers and waitlist not empty

                    //when waitlist is empty, just drop off the passengers
                    else if ((lift + i)->getPassengers[0] != '\0' && (lift + i)->getWaitlist[0] == '\0')
                    {
                        b = (lift + i)->getWaitlist[0];
                        c = (lift + i)->getPassengers[0];
                        t = ((lift + i)->getFloor - (input + c)->fd) + 2;
                        if (t == 2)
                            t = 0;
                        if (t == 3)
                            t = 2;
                        if (a >= t + t1)
                        {
                            Remove(input, lift, i, 1);
                            //write into the output form that the Passenger is dropped at time ((lift+i)->getLastModifiedTime + t
                            (lift + i)->getPurpose = 4;
                            (lift + i)->getLastModifiedTime = ((lift + i)->getLastModifiedTime + t);
                            (lift + i)->getTotalPassengers++;
                            (lift + i)->getTotalFloors += ((lift + i)->getFloor - (input + c)->fd);
                            (lift + i)->getFloor = (input + c)->fd;
                            t1 = t1 + t;
                            if ((lift + i)->pickordropmode == 0)
                            {
                                (input + c)->offtime = (lift + i)->getLastModifiedTime + t1;
                                fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor, (input + c)->Pid);
                            }
                            else
                            {
                                (input + c)->offtime = (lift + i)->getLastModifiedTime + t1 + 1;
                                fprintf(fptr, "%d E%d %d drops:%s \n", (lift + i)->getLastModifiedTime + t1 + 1, i, (lift + i)->getFloor, (input + c)->Pid);
                            }
                        }
                        else
                            break;
                    }
                }
                if ((lift + i)->getPurpose == 3 || (lift + i)->getPurpose == 4)
                {
                    b = (lift + i)->getWaitlist[0];
                    c = (lift + i)->getPassengers[0];
                    if ((lift + i)->getPassengers[0] == '\0' && (lift + i)->getWaitlist[0] == '\0')
                    {
                        (lift + i)->getPurpose = 0;
                        //(lift + i)->getLastModifiedTime += 2;
                        fprintf(fptr, "%d E%d %d stops \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor);
                        break;
                    }
                    if ((lift + i)->getPassengers[0] == '\0' && (lift + i)->getWaitlist[0] != '\0')
                    {
                        nextstop = (input + b)->fa;
                        if ((lift + i)->getdir == 0)
                        {
                            if ((input + b)->fa >= (lift + i)->getFloor)
                                (lift + i)->getdir = 0;
                            else
                                (lift + i)->getdir = 1;
                        }
                        //if down
                        else if ((lift + i)->getdir == 1)
                        {
                            if ((input + b)->fa > (lift + i)->getFloor)
                                (lift + i)->getdir = 0;
                            else
                                (lift + i)->getdir = 1;
                        }
                    }

                    else if ((lift + i)->getWaitlist[0] == '\0' && (lift + i)->getPassengers[0] != '\0')
                    {
                        nextstop = (input + c)->fd;

                        if ((lift + i)->getdir == 0)
                        {
                            if ((input + c)->fd >= (lift + i)->getFloor)
                                (lift + i)->getdir = 0;
                            else
                                (lift + i)->getdir = 1;
                        }
                        //if down
                        else if ((lift + i)->getdir == 1)
                        {
                            if ((input + c)->fd > (lift + i)->getFloor)
                                (lift + i)->getdir = 0;
                            else
                                (lift + i)->getdir = 1;
                        }
                    }

                    else
                    {
                        if ((lift + i)->getdir == 0)
                        {
                            if ((input + b)->fa > (input + c)->fd)
                            {
                                nextstop = (input + c)->fd;
                                if ((input + c)->fd >= (lift + i)->getFloor)
                                    (lift + i)->getdir = 0;
                                else
                                    (lift + i)->getdir = 1;
                            }
                            else
                            {
                                nextstop = (input + b)->fa;
                                if ((input + b)->fa >= (lift + i)->getFloor)
                                    (lift + i)->getdir = 0;
                                else
                                    (lift + i)->getdir = 1;
                            }
                        }
                        else if ((lift + i)->getdir == 1)
                        {
                            if ((input + b)->fa > (input + c)->fd)
                            {
                                nextstop = (input + b)->fa;
                                if ((input + b)->fa > (lift + i)->getFloor)
                                    (lift + i)->getdir = 0;
                                else
                                    (lift + i)->getdir = 1;
                            }
                            else
                            {
                                nextstop = (input + c)->fd;
                                if ((input + c)->fd > (lift + i)->getFloor)
                                    (lift + i)->getdir = 0;
                                else
                                    (lift + i)->getdir = 1;
                            }
                        }
                    }
                    if ((input + z)->fa == (lift + i)->getFloor)
                    {
                        if ((input + z)->ta == (lift + i)->getLastModifiedTime || (input + z)->ta == (lift + i)->getLastModifiedTime + 1)
                        {
                            break;
                        }
                    }
                    //if up and purpose is picking up
                    if ((lift + i)->getdir == 0)
                    {
                        (lift + i)->getPurpose = 1;

                        if (nextstop != (lift + i)->getFloor)
                        {
                            (lift + i)->getLastModifiedTime += 2;
                            (lift + i)->getFloor += 1;
                            (lift + i)->pickordropmode = 0;
                            fprintf(fptr, "%d E%d %d goesup \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor);
                        }
                        else
                            (lift + i)->pickordropmode = 1;
                    }
                    //if down
                    else if ((lift + i)->getdir == 1)
                    {
                        (lift + i)->getPurpose = 2;
                        if (nextstop != (lift + i)->getFloor)
                        {
                            (lift + i)->getLastModifiedTime += 2;
                            (lift + i)->getFloor -= 1;
                            (lift + i)->pickordropmode = 0;
                            fprintf(fptr, "%d E%d %d goesdown \n", (lift + i)->getLastModifiedTime + t1, i, (lift + i)->getFloor);
                        }
                        else
                            (lift + i)->pickordropmode = 1;
                    }
                }
            } while (1);
        }

        //================================================================================================================================================
        //decide which elevator the current passenger takes and add into that elevaor's waitlist.

        //lastmodified time is less than current time or equal to the current time
        // if equal to the current time ta, then the elevator's current status can be used to compare with the passenger details to decide where he goes
        // if less then current time ta, then figure out which floor the elevator is currently at. There can not be any stops in between so just use normal time that takes to travel b/w floors

        //get the elevator with min (floors it takes to reach fa of the current passenger + number of passengers already in waitlist + number of passengers inside the elevator)
        mincombo = 100000;
        for (i = 0; i < ElevatorCount; i++)
        {
            diff = (input + z)->ta - (lift + i)->getLastModifiedTime;

            /*while ((lift + i)->getWaitlist[m] != '\0')
                m++; 
            while ((lift + i)->getPassengers[n] != '\0')
                n++; */

            maxfa = 1;
            maxfd = 1;
            minfa = MAX_FLOOR;
            minfd = MAX_FLOOR;
            m = 0;
            n = 0;
            //number of passengers in waitlist
            for (c = 0; (lift + i)->getWaitlist[c] != 0; c++)
            {
                b = (lift + i)->getWaitlist[c];
                if ((input + b)->fa > maxfa)
                    maxfa = (input + b)->fa;
                if ((input + b)->fa < minfa)
                    minfa = (input + b)->fa;
                m++;
            }
            //number of passengers inside elevator
            for (c = 0; (lift + i)->getPassengers[c] != 0; c++)
            {
                b = (lift + i)->getPassengers[c];
                if ((input + b)->fd > maxfd)
                    maxfd = (input + b)->fd;
                if ((input + b)->fd < minfd)
                    minfd = (input + b)->fd;
                n++;
            }
            if ((lift + i)->getLastModifiedTime < (input + z)->ta && (lift + i)->getPurpose != 0 && (lift + i)->getPurpose != 3 && (lift + i)->getPurpose != 4)
            {
                if ((lift + i)->getdir == 0)
                {
                    if (diff == 1)
                        fc = (lift + i)->getFloor;
                    else
                        fc = floor(0.5 * (diff - 1) + (lift + i)->getFloor) + 1;
                }
                else if ((lift + i)->getdir == 1)
                {
                    if (diff == 1)
                        fc = (lift + i)->getFloor;

                    else
                    {
                        fc = (lift + i)->getFloor - (diff - 2) - 1;
                    }
                }
            }
            else
            {
                fc = (lift + i)->getFloor;
            }

            diff = 0;
            // gotta add more conditions below.....................
            if ((lift + i)->getdir == 0)
            {
                if ((input + z)->fa >= fc)
                    diff = ((input + z)->fa - fc);
                else
                {
                    if (maxfa <= maxfd)
                        diff = (maxfd - fc) + (maxfd - (input + z)->fa);
                    else
                        diff = (maxfa - fc) + (maxfa - (input + z)->fa);
                }
            }
            else if ((lift + i)->getdir == 1)
            {
                if ((input + z)->fa <= fc)
                    diff = (fc - (input + z)->fa);
                else
                {
                    if (minfa >= minfd)
                        diff = (fc - minfd) + ((input + z)->fa - minfd);
                    else
                        diff = (fc - minfa) + ((input + z)->fa - minfa);
                }
            }
            // gotta add more conditions above.....................
            if ((m + n + diff) < mincombo)
            {

                mincombo_elevator = i;
                mincombo = (m + n + diff);
            }
        }
        if ((input + z)->ta == END_TIME)
            break;

        if ((input + z)->fa != (input + z)->fd)
        {

            //adding the passenger to the waitlist if the selected elevator
            if ((lift + mincombo_elevator)->switch_on == 0)
            {
                (lift + mincombo_elevator)->getLastModifiedTime = (input + z)->ta;
                (lift + mincombo_elevator)->switch_on = 1;
            }
            AddToList(input, z, lift, mincombo_elevator, 0);
        }

        z++;
    }

    //========================================================================================================================================================
    //make extra files
    l = z;
    fclose(fp);
    fclose(fptr);
    se = fopen(argv[5], "w+");
    sp = fopen(argv[6], "w+");

    for (c = 0; c < ElevatorCount; c++)
    {
        fptr = fopen(argv[4], "r");
        fprintf(se, "=====================================Elevator E%d =========================================\n", c);
        while (fgets(linez, sizeof(linez), fptr) != NULL)
        {
            strncpy(linez, linez, strlen(linez) - 1);
            i = 0;
            line2 = strtok(linez, " ");

            while (line2 != NULL)
            {
                
                arr[i] = line2;
                line2 = strtok (NULL, " ");
                i++;
            }
            *arr[4] = '\0';
            snprintf(str, 3, "E%d", c);
            if (arr[1] = str)
            {
                if (i != 1)
                    fprintf(se, "%s %s %s \n", arr[0], arr[2], arr[3]);
            }
        }
        fprintf(se, "Total number of floors travelled %d \n", (lift + c)->getTotalFloors);
        fprintf(se, "Total number of passengers picked up %d \n", (lift + c)->getTotalPassengers);
        fclose(fptr);
    }

    avgwait = 0;
    avgdest = 0;
    maxwait = 0;
    maxdest = 0;
    b = 0;
    a = 0;
    for (z = 1; z <= l; z++)
    {
        if ((input + z)->fa == (input + z)->fd)
            continue;
        fprintf(sp, "%s E%d %d %d %d \n", (input + z)->Pid, (input + z)->getElevator, (input + z)->ta, (input + z)->intime, (input + z)->offtime);
        a = a + (input + z)->intime - (input + z)->ta;
        if (((input + z)->intime - (input + z)->ta) > maxwait)
            maxwait = (input + z)->intime - (input + z)->ta;
        b = b + (input + z)->offtime - (input + z)->intime;
        if (((input + z)->offtime - (input + z)->intime) > maxdest)
            maxdest = (input + z)->offtime - (input + z)->intime;
    }
    avgwait = (float)a / z;
    avgdest = (float)b / z;
    fprintf(sp, "Average waiting time %f \n", avgwait);
    fprintf(sp, "Maxiaml waiting time %d \n", maxwait);
    fprintf(sp, "Average time to arrive destination %f \n", avgdest);
    fprintf(sp, "Maximal time to arrive destination %d \n", maxdest);


    fclose(se);
    fclose(sp);
    exit(EXIT_SUCCESS);

    return (0);
}
