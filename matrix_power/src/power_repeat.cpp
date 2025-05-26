#include <iostream>
#include <vector>

using Matrix = std::vector<std::vector<double>>;

// Creează matrice identitate n×n
Matrix identity(int n) {
    Matrix I(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++)
        I[i][i] = 1.0;
    return I;
}

// Înmulțire clasică n×n
Matrix mul(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        for (int k = 0; k < n; k++)
          C[i][j] += A[i][k] * B[k][j];
    return C;
}

// Ridicare la putere repetitivă
Matrix power_repeat(const Matrix &A, int p) {
    Matrix R = identity(A.size());
    for (int k = 0; k < p; k++) {
        R = mul(R, A);
    }
    return R;
}

int main() {
    int n, p;
    // 1) citesc dimensiunea și exponentul
    if (!(std::cin >> n >> p)) {
        std::cerr << "Format: n p  (dimensiune și exponent)\n";
        return 1;
    }

    // 2) citesc matricea A
    Matrix A(n, std::vector<double>(n));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        std::cin >> A[i][j];

    // 3) calculez A^p
    Matrix R = power_repeat(A, p);

    // 4) afișez rezultatul
    for (auto &row : R) {
        for (double x : row) std::cout << x << ' ';
        std::cout << '\n';
    }
    return 0;
}
