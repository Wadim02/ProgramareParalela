#include <iostream>
#include <vector>
#include <algorithm>

using Matrix = std::vector<std::vector<double>>;
const int THRESHOLD = 64;  // pragul de trecere la înmulțirea clasică

// 1) utilitare: creare identitate
Matrix identity(int n) {
    Matrix I(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) I[i][i] = 1.0;
    return I;
}

// 2) adunare și scădere de matrici
Matrix add(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, std::vector<double>(n));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        C[i][j] = A[i][j] + B[i][j];
    return C;
}
Matrix sub(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, std::vector<double>(n));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        C[i][j] = A[i][j] - B[i][j];
    return C;
}

// 3) înmulțire clasică (caz de bază)
Matrix classicalMul(const Matrix &A, const Matrix &B) {
    int n = A.size();
    Matrix C(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        for (int k = 0; k < n; k++)
          C[i][j] += A[i][k] * B[k][j];
    return C;
}

// 4) Strassen recursiv
Matrix strassenMul(const Matrix &A, const Matrix &B, int threshold) {
    int n = A.size();
    if (n <= threshold) {
        return classicalMul(A, B);
    }
    int m = n/2;
    // inițializare submatrici
    Matrix A11(m, std::vector<double>(m)), A12(m, std::vector<double>(m)),
           A21(m, std::vector<double>(m)), A22(m, std::vector<double>(m)),
           B11 = A11, B12 = A12, B21 = A21, B22 = A22;
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < m; j++) {
        A11[i][j] = A[i][j];
        A12[i][j] = A[i][j + m];
        A21[i][j] = A[i + m][j];
        A22[i][j] = A[i + m][j + m];
        B11[i][j] = B[i][j];
        B12[i][j] = B[i][j + m];
        B21[i][j] = B[i + m][j];
        B22[i][j] = B[i + m][j + m];
      }
    }
    // cele 7 produse Strassen
    Matrix M1 = strassenMul(add(A11, A22), add(B11, B22), threshold);
    Matrix M2 = strassenMul(add(A21, A22), B11, threshold);
    Matrix M3 = strassenMul(A11, sub(B12, B22), threshold);
    Matrix M4 = strassenMul(A22, sub(B21, B11), threshold);
    Matrix M5 = strassenMul(add(A11, A12), B22, threshold);
    Matrix M6 = strassenMul(sub(A21, A11), add(B11, B12), threshold);
    Matrix M7 = strassenMul(sub(A12, A22), add(B21, B22), threshold);

    // reconstruiește C din M1..M7
    Matrix C(n, std::vector<double>(n));
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < m; j++) {
        C[i][j]       = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j];
        C[i][j + m]   = M3[i][j] + M5[i][j];
        C[i + m][j]   = M2[i][j] + M4[i][j];
        C[i + m][j + m] = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j];
      }
    }
    return C;
}

// 5) exponentiere rapidă folosind Strassen la înmulțiri
Matrix power_strassen(const Matrix &A, int p) {
    int n = A.size();
    Matrix R = identity(n), X = A;
    while (p > 0) {
        if (p & 1) R = strassenMul(R, X, THRESHOLD);
        X = strassenMul(X, X, THRESHOLD);
        p >>= 1;
    }
    return R;
}

int main() {
    int n, p;
    if (!(std::cin >> n >> p)) { std::cerr << "Format: n p\n"; return 1; }
    Matrix A(n, std::vector<double>(n));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        std::cin >> A[i][j];

    Matrix R = power_strassen(A, p);
    for (auto &row : R) {
        for (double x : row) std::cout << x << ' ';
        std::cout << '\n';
    }
    return 0;
}
