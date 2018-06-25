#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(void)
{
  int i, j, n = 9000, iter, me, np, local_me, local_np;
  MPI_Aint local_size;
  int disp_unit;
  double hit, trial;
  double *shared_area, **cumulative;
  double time1,time2,time3, s_read_time,e_read_time;
  double x, y, start_time, end_time,s_comm_time,e_comm_time;
  MPI_Status s;
  FILE * fp[3];
  MPI_Comm shmemcomm;
  MPI_Win win;
  MPI_Request req[n];

  MPI_Init(NULL, NULL);

  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &shmemcomm);

  MPI_Comm_rank(shmemcomm, &local_me);
  MPI_Comm_size(shmemcomm, &local_np);

  MPI_Win_allocate_shared(sizeof(double), sizeof(double), MPI_INFO_NULL, shmemcomm,  &shared_area, &win);

  j = 0; hit = 0;
  iter = 500000; trial = 0;

  if(local_me == 0)
    cumulative = (double **)malloc(sizeof(double *) * local_np);
  
  start_time = MPI_Wtime();

  if(local_me == 0)
    {
      for (j=0;j<local_np;j++)
	MPI_Win_shared_query(win, j, &local_size, &disp_unit, &cumulative[j]);
    }

  MPI_Barrier(shmemcomm);

  srand(me*time(0));

  for (j=0;j<iter;j++)
    {
      x = (double)rand() / (double)RAND_MAX;
      y = (double)rand() / (double)RAND_MAX;
      if(sqrt(x*x + y*y) < 1)
	hit++;
      trial++;
    }

  *shared_area = (double)hit/trial;

  end_time = MPI_Wtime();

  time1 = end_time - start_time;

  MPI_Barrier(shmemcomm);

  if(local_me == 0)
    {
      for(j=1;j<local_np;j++)
	*cumulative[0] += *cumulative[j];

      printf("Me: %d %lf\n",local_me,*cumulative[0]);
    }

  MPI_Finalize();

  return 0;
}
