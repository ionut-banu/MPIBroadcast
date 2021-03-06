#include "stdafx.h"
#include "mpi.h"
#include "math.h"
#include "vector"
#define M_LOG2E 1.44269504088896340736 // log2(e)

using namespace std;

int main(int argc, char* argv[])
{
	int rank, size; //I am process RANK and we are a total of SIZE
	MPI_Status status;
	//MPI_Request request;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	//Generate the RANK in the WORLD
	MPI_Comm_size(MPI_COMM_WORLD, &size);	//Generate the SIZE of the WORLD

	MPI_Comm cube;
	int nDim = log2(size);					//number of dimensions of cartesian grid
	int *processPerDim;						//integer array of size ndims specifying the number of processes in each dimension
	int *period;							//logical array of size ndims specifying whether the grid is periodic (1) or not (0) in each dimension 
	processPerDim = new int[nDim];
	period = new int[nDim];
	for (int i = 0; i < nDim; i++) {
		processPerDim[i] = 2;
		period[i] = 1;
	}
	MPI_Cart_create(MPI_COMM_WORLD, nDim, processPerDim, period, true, &cube);	//create hypercube

	int source, *neighbors, rec;
	neighbors = new int(nDim);
	//result = new int(size);
	//rec = new int(size);
	// get the addresses of neighbor processors
	int rhs = rank != 0 ? log2(rank & -rank) : 0;
	printf("RANK: %d.--Rightmost bit: %d.--Neighbors: ", rank, rhs);
	for (int i = 0; i < nDim; i++) {
		MPI_Cart_shift(cube, i, 1, &source, &neighbors[i]);
		printf("%d ", neighbors[i]);
	}
	if (rank == 0) {
		rec = 123;
	}
	else {
		rec = -1;
	}

	for (int i = nDim; i >= 0; i--) {
		if (rhs > i || rank == 0) {
			for (int j = 0; j < nDim; j++) {
				if (log2(neighbors[j] & -neighbors[j]) == i) {
					printf("Step %d Sending to %d--", i, neighbors[j]);
					MPI_Send(&rec, 1, MPI_INT, neighbors[j], 0, MPI_COMM_WORLD);
					break;
				}
			}
		}
		else if (rhs == i) {
			for (int j = 0; j < nDim; j++) {
				if (log2(neighbors[j] & -neighbors[j]) > i || neighbors[j] == 0) {
					printf("Receiving from %d--", neighbors[j]);
					MPI_Recv(&rec, 1, MPI_INT, neighbors[j], 0, MPI_COMM_WORLD, &status);
					break;
				}
			}
		}
	}
	printf("Received message: %d", rec);

	MPI_Finalize();
}


const int log2(const int x) {
	return log(x) * M_LOG2E;
}

