#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

int main(void)
{
  int i, j, n = 9000, me, np, local_me, local_np;
  MPI_Aint local_size;
  int disp_unit;
  double time1,time2,time3, s_read_time,e_read_time;
  double *x, *y, *z, start_time, end_time,s_comm_time,e_comm_time;
  MPI_Status s;
  FILE * fp[3];
  MPI_Comm shmemcomm;
  MPI_Win win_x, win_y, win_z;
  MPI_Request req[n];

  MPI_Init(NULL, NULL);

  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &shmemcomm);

  MPI_Comm_rank(shmemcomm, &local_me);
  MPI_Comm_size(shmemcomm, &local_np);

  if(local_me == 0)
    local_size = n*n*sizeof(double);
  else
    local_size = 0;

  MPI_Win_allocate_shared(local_size, sizeof(double), MPI_INFO_NULL, shmemcomm,
			  &x, &win_x);
  MPI_Win_allocate_shared(local_size, sizeof(double), MPI_INFO_NULL, shmemcomm,
			  &y, &win_y);
  MPI_Win_allocate_shared(local_size, sizeof(double), MPI_INFO_NULL, shmemcomm,
			  &z, &win_z);

  //x = (double *)malloc(n*n*sizeof(double));
  //y = (double *)malloc(n*n*sizeof(double));
  //z = (double *)malloc(n*n*sizeof(double));

  if(local_me == 0)
    {
      fp[0] = fopen("./matrix_1.dat","rb");
      fp[1] = fopen("./matrix_2.dat","rb");
      fp[2] = fopen("./matrix_3.dat","rb");
    }

  j = 0;
  
  start_time = MPI_Wtime();

  if(local_me == 0)
    {
      for(i=0;i<n*n;i+=n)
	fread(&x[i],sizeof(double),n,fp[0]);
      for(i=0;i<n*n;i+=n)
	fread(&y[i],sizeof(double),n,fp[1]);
      for(i=0;i<n*n;i+=n)
	fread(&z[i],sizeof(double),n,fp[2]);
    }

  MPI_Win_shared_query(win_x, 0, &local_size, &disp_unit, &x);
  MPI_Win_shared_query(win_y, 0, &local_size, &disp_unit, &y);
  MPI_Win_shared_query(win_z, 0, &local_size, &disp_unit, &z);

  MPI_Barrier(shmemcomm);

  end_time = MPI_Wtime();

  time1 = end_time - start_time;

  if(local_me == 0)
    printf("Me: %d Total time: %lf TEST: %lf\n",me,time1,x[10]);
  else if(local_me == 1)
    printf("Me: %d TEST: %lf\n",me,x[10]);

  MPI_Finalize();

  return 0;
}
