#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

// Waiting for completion
void lock(struct sembuf s, int semid)
{
    s.sem_num = 0;
    s.sem_op += -1;
    s.sem_flg = 0;
    semop(semid, &s, 1);
}

// Signaling after completion
void unlock(struct sembuf s, int semid)
{
    s.sem_num = 0;
    s.sem_op += 1;
    s.sem_flg = 0;
    semop(semid, &s, 1);
}

//Giving inputs for Banker's Algorithm
void input(int Allocation[][10],int Need[][10],int Max[10][10],int Work[1][10],int n,int m)
{
	int semid;
    semid = semget(IPC_PRIVATE, 10, IPC_CREAT|0777);
    semctl(semid, 0, SETVAL, 1);
    struct sembuf s;
	for(int i=0;i<n;i++)
	{
		if(fork() == 0)
		{
		    lock(s,semid);
			printf("\nProcess %d\n",i+1);
			for(int j=0;j<m;j++)
			{
				Allocation[i][j] = rand()%10;
				printf("Allocation %d : %d\n",j+1,Allocation[i][j]);
				//scanf("%d",&A[i][j]);
			}
			for(int j=0;j<m;j++)
			{
				printf("Maximum for resource %d : ",j+1);
				//M[i][j] = rand()%10;
				//printf("%d\n",M[i][j]);
				scanf("%d",&Max[i][j]);
				Need[i][j] = Max[i][j] - Allocation[i][j];
			}
			unlock(s,semid);
		}
		wait(NULL);
	}
	for(int i=0;i<n;i++)
    {
        wait(NULL);
    }
}

void banker(int Allocation[][10],int Need[][10],int Work[1][10],int n,int m)
{
	int i,seq[10];
	if(safety(Allocation,Need,Work,n,m,seq) != false )
	{
		printf("\n< ");
		for(int i=0;i<n;i++)
		    printf(" P%d ",seq[i]);
		printf(">\n ");
		printf("\nSafe sequence has been detected.\n");
	}
	else
	{
		printf("\nDeadlock!!! NOT SAFE");
	}
}

// Safety algorithm
int safety(int Allocation[][10],int Need[][10],int Available[1][10],int n,int m,int a[])
{
	int x=0,Work[1][10],pflag=0,flag=0;
	bool Finish[10];
	for(int i=0;i<n;i++)
		Finish[i]=0;
	for(int i=0;i<m;i++)
		Work[0][i]=Available[0][i];
	printf("\nWORK\n");
	for(int k=0;k<n;k++)
	{
		for(int i=0;i<n;i++)
		{
			if(Finish[i] == 0)
			{
				flag=0;
				for(int j=0;j<m;j++)
				{
					if(Need[i][j] > Work[0][j] || Need[i][j] < 0)
						flag=1;
				}
				if(flag == 0 && Finish[i] == 0)
				{
					printf("\n");
					for(int j=0;j<m;j++)
					{	
						Work[0][j] += Allocation[i][j];
						printf("%d\t",Work[0][j]);
					}
					Finish[i]=1;
					pflag++;
					a[x++]=i;
				}
			}
		}
		if(pflag == n)
			return 1;
	}
	return 0;
}

// Resource Request
void request(int Allocation[10][10],int Need[10][10],int Available[10][10],int pid,int m)
{
	int req[1][10];
	printf("\nEnter additional request :- \n");
	for(int i=0;i<m;i++)
	{
		printf("Request for resource %d : ",i+1);
		scanf("%d",&req[0][i]);
	}
	// Check for resource availability and if request is greater than need
	for(int i=0;i<m;i++)
		if((req[0][i] > Available[0][i])&&(req[0][i] > Need[pid][i]))
		{
			printf("\nRequest cannot be accepted.\\n");
		}
	    else
    	{
    		Available[0][i] -= req[0][i];
    		Allocation[pid][i] += req[0][i];
    		Need[pid][i] -= req[0][i];
    	}
}

// Prints Matrix
void print(int x[][10],int n,int m)
{
	for(int i=0;i<n;i++)
	{
		printf("\n");
		for(int j=0;j<m;j++)
		{
			printf("%d\t",x[i][j]);
		}
	}	
}

// Main function
int main()
{
	int Allocation[10][10],Max[10][10],Need[10][10],Available[1][10],n,m,pid,o,r;
	printf("\n__________BANKER'S ALGORITHM___________\n");
	// Giving inputs
	printf("\nEnter no. of processes : ");
	scanf("%d",&n);
	printf("\nEnter total no. of resources : ");
	scanf("%d",&m);
	printf("\nAvailable resources : \n");
    for(int i=0;i<m;i++)
	{
		printf("Resource %d : ",i+1);
		scanf("%d",&Available[0][i]);
	}
	input(Allocation,Need,Max,Available,n,m);
	printf("\nAllocation Matrix\n");
	print(Allocation,n,m);
	printf("\nMaximum Requirement Matrix\n");
	print(Max,n,m);
	printf("\nNeed Matrix\n");
	print(Need,n,m);
	banker(Allocation,Need,Available,n,m);
	while(1)
	{
		printf("\nMake an additional request ?\nYes(1)\nExit(press 'ctrl + c')\n");
		scanf("%d",&o);
		if(o == 0)
		    exit(0);
		else
		{
			printf("\nEnter process no : ");
			scanf("%d",&pid);
			request(Allocation,Need,Available,pid-1,m);
			banker(Allocation,Need,Available,n,m);
		}
	}
	return 0;
}
