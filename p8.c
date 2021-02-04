#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

double f(double x) {
  double pi, value;
  value = 50.0 / (M_PI * (2500.0 * x * x + 1.0));
  return value;
}

int main (int argc, char *argv[]) {
  double a, b, error, exact;
  int i, master = 0;
  int my_id, my_n, n, p, p_num, source, tag, target;
  double my_a, my_b, my_total;
  double total, wtime, x;
  MPI_Status status;

  a =  0.0;
  b = 10.0;
  n = 10000000;
  exact = 0.49936338107645674464;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  MPI_Comm_size(MPI_COMM_WORLD, &p_num);

  if (my_id == master) {
    //We want N to be the total number of evaluations.
    //If necessary, we adjust N to be divisible by the number of processes.
    my_n = n / (p_num - 1);
    n = (p_num - 1) * my_n;

    wtime = MPI_Wtime ();
  }
  source = master;
  MPI_Bcast(&my_n, 1, MPI_INT, source, MPI_COMM_WORLD);
  /* Process 0 assigns each process a subinterval of [A,B].*/
  if (my_id == master) {
    for (p = 1; p <= p_num - 1; p++) {
      my_a = ((double) (p_num - p) * a
          + (double) (p - 1) * b)
        / (double) (p_num - 1);

      target = p; tag = 1;
      MPI_Send(&my_a, 1, MPI_DOUBLE, target, tag, MPI_COMM_WORLD);

      my_b = ((double) (p_num - p - 1) * a
          + (double) (p) * b)
        / (double) (p_num - 1);

      target = p; tag = 2;
      MPI_Send(&my_b, 1, MPI_DOUBLE, target, tag, MPI_COMM_WORLD);
    }
    total = 0.0;
    my_total = 0.0;
  }
  /*Processes receive MY_A, MY_B, and compute their part of the integral.*/
  else {
    source = master; tag = 1;
    MPI_Recv(&my_a, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);

    source = master; tag = 2;
    MPI_Recv(&my_b, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);

    my_total = 0.0;
    for (i = 1; i <= my_n; i++) {
      x = ((double) (my_n - i) * my_a
          + (double) (i - 1) * my_b)
        / (double) (my_n - 1);
      my_total = my_total + f(x);
    }

    my_total = (my_b - my_a) * my_total / (double) (my_n);
    printf ("Process %d contributed MY_TOTAL = %f\n", my_id, my_total);
  }
  /*Each process sends its value to the master process.*/
  MPI_Reduce(&my_total, &total, 1, MPI_DOUBLE, MPI_SUM, master, MPI_COMM_WORLD);
  /* Compute the weighted estimate.*/
  if (my_id == master) {
    error = fabs(total - exact);
    wtime = MPI_Wtime() - wtime;

    printf ("Estimate = %.16f\n", total);
    printf ("Error = %e\n\n", error);
    printf ("Time = %f\n\n", wtime);
  }
  MPI_Finalize ();

  return 0;
}
