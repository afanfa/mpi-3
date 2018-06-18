#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>

int main(void)
{
  int i,me,np,n,provided,tmp,name_len;
  double *x,*y,start_time,end_time,elapsed;
  MPI_Request req[2];
  MPI_Status status;
  char processor_name[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(NULL,NULL);
  
  n = 100000;

  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&np);

  MPI_Get_processor_name(processor_name, &name_len);

  if(me == 0 || me == np-1)
    printf("Process %d has name %s\n",me,processor_name);
  
  // Send/Recv to warm-up communication
  if(me == 0)
    MPI_Send(&me,1,MPI_INT,np-1,0,MPI_COMM_WORLD);

  if(me == np-1)
    MPI_Recv(&tmp,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);

  x = (double *)malloc(n*sizeof(double));
  y = (double *)malloc(n*sizeof(double));

  if(me == 0)
    {
      for(i=0;i<n;i++)
	{
	  x[i] = (double) i;
	  y[i] = (double) i * 1/(i+1);
	}
    }

  MPI_Barrier(MPI_COMM_WORLD);

  start_time = MPI_Wtime();

  MPI_Ibcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req[0]);
  //MPI_Ibcast(y, n, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req[1]);
  /*  if(me /= 0)
    {
      for(i=0;n;i++)
	y[i] = (double) sqrt((i+1) * 0.2 + 1/log(i+1));
	}*/
  //MPI_Waitall(2,req,MPI_STATUS_IGNORE);
  MPI_Wait(&req[0],MPI_STATUS_IGNORE);
  end_time = MPI_Wtime();
  
  MPI_Barrier(MPI_COMM_WORLD);

  printf("Elapsed time process %d: %lf\n",me,end_time-start_time);

  MPI_Finalize();

  return 0;
}
