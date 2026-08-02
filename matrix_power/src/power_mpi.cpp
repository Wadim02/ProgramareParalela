#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> 

using Real   = double;
using Matrix = std::vector<Real>;  // store the n×n matrix in a flat vector

// --- MPI utilities for row distribution ---
void parallelMatMul(const Matrix &A, const Matrix &B, Matrix &C,
                    int n, int rank, int size) {
    int base = n / size;
    int rem  = n % size;
    int local_rows = base + (rank < rem ? 1 : 0);
    int offset     = rank * base + std::min(rank, rem);

    std::vector<Real> Cloc(local_rows * n, 0.0);

    for (int i = 0; i < local_rows; ++i) {
        int gi = offset + i;
        for (int j = 0; j < n; ++j) {
            Real sum = 0;
            for (int k = 0; k < n; ++k) {
                sum += A[gi*n + k] * B[k*n + j];
            }
            Cloc[i*n + j] = sum;
        }
    }

    std::vector<int> recvcounts(size), displs(size);
    for (int r = 0; r < size; ++r) {
        int rows = base + (r < rem ? 1 : 0);
        recvcounts[r] = rows * n;
        displs[r]     = (r*base + std::min(r, rem)) * n;
    }

    C.resize(n*n);
    MPI_Allgatherv(
      Cloc.data(),
      local_rows*n,
      MPI_DOUBLE,
      C.data(),
      recvcounts.data(),
      displs.data(),
      MPI_DOUBLE,
      MPI_COMM_WORLD
    );
}

Matrix makeIdentity(int n) {
    Matrix I(n*n, 0.0);
    for (int i = 0; i < n; ++i) I[i*n + i] = 1.0;
    return I;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0)
            std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    const char* filename = argv[1];

    int n, p;
    Matrix A;   // read only by rank 0

    if (rank == 0) {
        std::ifstream fin(filename);
        if (!fin) {
            std::cerr << "Cannot open file: " << filename << "\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fin >> n >> p;
        A.resize(n*n);
        for (int i = 0; i < n*n; ++i)
            fin >> A[i];
    }

    // broadcast n and p to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // prepare buffer for A and broadcast it
    if (rank != 0) A.resize(n*n);
    MPI_Bcast(A.data(), n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    Matrix X = A;
    Matrix R = makeIdentity(n);

    // fast exponentiation with MPI multiplications
    while (p > 0) {
        if (p & 1) parallelMatMul(R, X, R, n, rank, size);
        parallelMatMul(X, X, X, n, rank, size);
        p >>= 1;
    }

    if (rank == 0) {
        // print result
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cout << R[i*n + j] << ' ';
            }
            std::cout << '\n';
        }
    }

    MPI_Finalize();
    return 0;
}
