#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int [], int[]);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;
  
     //integers value
     int forked[2];
     int buffer[10];
  
     //pipe creation
     pipe(forked);

     if (argc != 11) {
          printf("Use: %s #1 #2 ... #10 \n", argv[0]);
          exit(1);
     }

     ShmID = shmget(IPC_PRIVATE, 10*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of ten integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");

     // read arguments
     int i;
     for (i =0; i < 10; i++){
       ShmPTR[i] = atoi(argv[i+1]);
     }

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
         printf("Child: ");
          ClientProcess(ShmPTR, forked);
          exit(0);
     }

     wait(&status);
     printf("Server has detected the completion of its child...\n");
     
     //close writing
     close(forked[1]);
     
     printf("Parent: ");
     for(i = 0; i < 10; i++){
       read(forked[0], buffer, sizeof(int));
       printf("%d  ",*buffer);
     }
     printf("\n");
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}

void  ClientProcess(int  SharedMem[], int forked[])
{
     printf("   Client process started\n");
  
     int x;
     printf("Child: ");
     for (x = 0; x < 10; x++){
       printf("%d  ",SharedMem[x]);
     }
     printf("\n");
  
     //close read and write
     close(forked[0]);
  
     int y;
     for(y = 0; y < 10; y++){
       write(forked[1], &SharedMem[y], sizeof(int));  // writing in pipe
     }
     printf("Done Writing to pipe\n");
     printf("Client is about to exit\n");
     printf("***************************\n");
}
