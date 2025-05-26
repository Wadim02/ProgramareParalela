#include <iostream>
#include <vector>

using Matrix = std::vector<std::vector<double>>;

// identitate n×n
Matrix identity(int n) {
    Matrix I(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++)
        I[i][i] = 1.0;
    return I;
}

// înmulțire clasică
Matrix mul(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        for (int k = 0; k < n; k++)
          C[i][j] += A[i][k] * B[k][j];
    return C;
}

// exponentiere rapidă iterativă
Matrix power_fast(const Matrix &A, int p) {
    int n = A.size();
    Matrix R = identity(n);
    Matrix X = A;
    while (p > 0) {
        if (p & 1)        // bit-ul cel mai puțin semnificativ e 1?
            R = mul(R, X);
        X = mul(X, X);
        p >>= 1;          // shift dreapta
    }
    return R;
}

int main() {
    int n, p;
    if (!(std::cin >> n >> p)) {
        std::cerr << "Format: n p\\n";
        return 1;
    }
    Matrix A(n, std::vector<double>(n));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        std::cin >> A[i][j];

    Matrix R = power_fast(A, p);

    for (auto &row : R) {
        for (double x : row) std::cout << x << ' ';
        std::cout << '\n';
    }
    return 0;
}
