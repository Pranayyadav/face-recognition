/**
 * @file test_matrix.c
 *
 * Test suite for the matrix library.
 *
 * Tests are based on examples in the MATLAB documentation
 * where appropriate.
 *
 * NOTE: This source file is not C++ compliant because the
 * function m_initialize_data depends on a C99 language
 * feature (2D array parameter). As a result, this file
 * will not compile with g++ or icpc.
 */
#include <math.h>
#include <stdio.h>
#include "matrix.h"

typedef void (*test_func_t)(void);

/**
 * Construct a matrix with arbitrary data.
 *
 * @param rows
 * @param cols
 * @param data
 * @return pointer to matrix
 */
matrix_t * m_initialize_data (int rows, int cols, precision_t data[][cols])
{
	matrix_t *M = m_initialize(rows, cols);

	int i, j;
	for ( i = 0; i < M->rows; i++ ) {
		for ( j = 0; j < M->cols; j++ ) {
			elem(M, i, j) = data[i][j];
		}
	}

	cublas_set_matrix(M);

	return M;
}

/**
 * Test identity matrix.
 */
void test_m_identity()
{
	matrix_t *I = m_identity(4);

	printf("I = eye(%d) = \n", I->rows);
	m_fprint(stdout, I);

	m_free(I);
}

/**
 * Test ones matrix.
 */
void test_m_ones()
{
	matrix_t *X = m_ones(4, 4);

	printf("X = ones(%d, %d) = \n", X->rows, X->cols);
	m_fprint(stdout, X);

	m_free(X);
}

/**
 * Test random matrix.
 */
void test_m_random()
{
	matrix_t *X = m_random(5, 5);

	printf("X = randn(%d, %d) = \n", X->rows, X->cols);
	m_fprint(stdout, X);

	m_free(X);
}

/**
 * Test zero matrix.
 */
void test_m_zeros()
{
	matrix_t *X = m_zeros(4, 4);

	printf("X = zeros(%d, %d) = \n", X->rows, X->cols);
	m_fprint(stdout, X);

	m_free(X);
}

/**
 * Test matrix copying.
 */
void test_m_copy()
{
	precision_t data[][4] = {
		{ 16,  2,  3, 13 },
		{  5, 11, 10,  8 },
		{  9,  7,  6, 12 },
		{  4, 14, 15,  1 }
	};

	matrix_t *A = m_initialize_data(4, 4, data);

	printf("A = \n");
	m_fprint(stdout, A);

	matrix_t *C1 = m_copy(A);

	int begin = 1;
	int end = 3;
	matrix_t *C2 = m_copy_columns(A, begin, end);

	printf("C1 = A = \n");
	m_fprint(stdout, C1);
	printf("C2 = A(:, %d:%d) = \n", begin + 1, end);
	m_fprint(stdout, C2);

	m_free(A);
	m_free(C1);
	m_free(C2);
}

#ifdef UNDEFINED

/**
 * Test the matrix convariance.
 */
void test_m_covariance()
{
	precision_t data[][4] = {
		{ 5, 0, 3, 7 },
		{ 1, -5, 7, 3 },
		{ 4, 9, 8, 10 }
	};

	matrix_t *A = m_initialize_data(3, 4, data);
	matrix_t *C = m_covariance(A);

	printf("A = \n");
	m_fprint(stdout, A);
	printf("cov(A) = \n");
	m_fprint(stdout, C);

	m_free(A);
	m_free(C);
}

/**
 * Test the diagonal matrix.
 */
void test_m_diagonalize()
{
	precision_t data[][5] = {
		{ 2, 1, -1, -2, -5 }
	};

	matrix_t *v = m_initialize_data(1, 5, data);
	matrix_t *D = m_diagonalize(v);

	printf("v = \n");
	m_fprint(stdout, v);
	printf("diag(v) = \n");
	m_fprint(stdout, D);

	m_free(v);
	m_free(D);
}

/**
 * Test the vector distance functions.
 */
void test_m_distance()
{
	precision_t data[][2] = {
		{ 1, 0 },
		{ 0, 1 },
		{ 0, 0 }
	};

	matrix_t *M = m_initialize_data(3, 2, data);

	printf("M = \n");
	m_fprint(stdout, M);

	printf("d_COS(M(:, 0), M(:, 1)) = % 8.4lf\n", m_dist_COS(M, 0, M, 1));
	printf("d_L1(M(:, 0), M(:, 1))  = % 8.4lf\n", m_dist_L1(M, 0, M, 1));
	printf("d_L2(M(:, 0), M(:, 1))  = % 8.4lf\n", m_dist_L2(M, 0, M, 1));

	m_free(M);
}

/**
 * Test eigenvalues, eigenvectors.
 */
void test_m_eigen()
{
	precision_t data[][4] = {
		{ 1.0000, 0.5000, 0.3333, 0.2500 },
		{ 0.5000, 1.0000, 0.6667, 0.5000 },
		{ 0.3333, 0.6667, 1.0000, 0.7500 },
		{ 0.2500, 0.5000, 0.7500, 1.0000 }
	};

	matrix_t *M = m_initialize_data(4, 4, data);
	matrix_t *M_eval;
	matrix_t *M_evec;

	m_eigen(M, &M_eval, &M_evec);

	printf("M = \n");
	m_fprint(stdout, M);

	printf("eigenvalues of M = \n");
	m_fprint(stdout, M_eval);

	printf("eigenvectors of M = \n");
	m_fprint(stdout, M_evec);

	m_free(M);
	m_free(M_eval);
	m_free(M_evec);
}

// TODO: find more examples, check this example against MATLAB
/**
 * Test generalized eigenvalues, eigenvectors for two matrices.
 */
void test_m_eigen2()
{
	precision_t data_A[][3] = {
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 }
	};

	precision_t data_B[][3] = {
		{ 1.0, 0.1, 0.1 },
		{ 0.1, 2.0, 0.1 },
		{ 1.0, 0.1, 3.0 }
	};

	matrix_t *A = m_initialize_data(3, 3, data_A);
	matrix_t *B = m_initialize_data(3, 3, data_B);
	matrix_t *J_eval;
	matrix_t *J_evec;

	m_eigen2(A, B, &J_eval, &J_evec);

	printf("A = \n");
	m_fprint(stdout, A);

	printf("B = \n");
	m_fprint(stdout, B);

	printf("eigenvalues of J = \n");
	m_fprint(stdout, J_eval);

	printf("eigenvectors of J = \n");
	m_fprint(stdout, J_evec);

	m_free(A);
	m_free(B);
	m_free(J_eval);
	m_free(J_evec);
}

/**
 * Test matrix inverse.
 */
void test_m_inverse()
{
	precision_t data[][3] = {
		{  1,  0,  2 },
		{ -1,  5,  0 },
		{  0,  3, -9 }
	};

	matrix_t *X = m_initialize_data(3, 3, data);
	matrix_t *Y = m_inverse(X);
	matrix_t *Z = m_product(Y, X);

	printf("X = \n");
	m_fprint(stdout, X);
	printf("Y = inv(X) = \n");
	m_fprint(stdout, Y);
	printf("Y * X = \n");
	m_fprint(stdout, Z);

	m_free(X);
	m_free(Y);
	m_free(Z);
}

/**
 * Test matrix mean column.
 */
void test_m_mean_column()
{
	precision_t data[][3] = {
		{ 0, 1, 1 },
		{ 2, 3, 2 }
	};

	matrix_t *A = m_initialize_data(2, 3, data);
	matrix_t *m = m_mean_column(A);

	printf("A = \n");
	m_fprint(stdout, A);

	printf("mean(A, 2) = \n");
	m_fprint(stdout, m);

	m_free(A);
	m_free(m);
}

/**
 * Test matrix mean row.
 */
void test_m_mean_row()
{
	precision_t data[][3] = {
		{ 0, 1, 1 },
		{ 2, 3, 2 },
		{ 1, 3, 2 },
		{ 4, 2, 2 }
	};

	matrix_t *A = m_initialize_data(4, 3, data);
	matrix_t *m = m_mean_row(A);

	printf("A = \n");
	m_fprint(stdout, A);

	printf("mean(A, 1) = \n");
	m_fprint(stdout, m);

	m_free(A);
	m_free(m);
}

/**
 * Test vector norm.
 */
void test_m_norm()
{
	precision_t data[][3] = {
		{ -2, 3, 1 }
	};

	matrix_t *v = m_initialize_data(1, 3, data);
	precision_t n = m_norm(v);

	printf("v = \n");
	m_fprint(stdout, v);

	printf("norm(v) = % 8.4lf\n", n);

	m_free(v);
}

#endif

/**
 * Test matrix product.
 */
void test_m_product()
{
	// multiply two vectors, A * B
	precision_t data_A1[][4] = {
		{ 1, 1, 0, 0 }
	};
	precision_t data_B1[][1] = {
		{ 1 },
		{ 2 },
		{ 3 },
		{ 4 }
	};

	matrix_t *A = m_initialize_data(1, 4, data_A1);
	matrix_t *B = m_initialize_data(4, 1, data_B1);
	matrix_t *C = m_product(A, B);

	cublas_get_matrix(C);

	printf("A = \n");
	m_fprint(stdout, A);
	printf("B = \n");
	m_fprint(stdout, B);

	printf("A * B = \n");
	m_fprint(stdout, C);
	m_free(C);

	// multiply two vectors, B * A
	C = m_product(B, A);

	cublas_get_matrix(C);

	printf("B * A = \n");
	m_fprint(stdout, C);
	m_free(C);

	m_free(A);
	m_free(B);

	// multiply two arrays
	precision_t data_A2[][3] = {
		{ 1, 3, 5 },
		{ 2, 4, 7 }
	};
	precision_t data_B2[][3] = {
		{ -5, 8, 11 },
		{  3, 9, 21 },
		{  4, 0,  8 }
	};

	A = m_initialize_data(2, 3, data_A2);
	B = m_initialize_data(3, 3, data_B2);
	C = m_product(A, B);

	cublas_get_matrix(C);

	printf("A = \n");
	m_fprint(stdout, A);
	printf("B = \n");
	m_fprint(stdout, B);

	printf("A * B = \n");
	m_fprint(stdout, C);

	m_free(A);
	m_free(B);
	m_free(C);
}

#ifdef UNDEFINED

/**
 * Test matrix square root.
 */
void test_m_sqrtm()
{
	precision_t data[][5] = {
		{  5, -4,  1,  0,  0 },
		{ -4,  6, -4,  1,  0 },
		{  1, -4,  6, -4,  1 },
		{  0,  1, -4,  6, -4 },
		{  0,  0,  1, -4,  6 }
	};

	matrix_t *A = m_initialize_data(5, 5, data);
	matrix_t *X = m_sqrtm(A);
	matrix_t *X_sq = m_product(X, X);

	printf("A = \n");
	m_fprint(stdout, A);
	printf("X = sqrtm(A) = \n");
	m_fprint(stdout, X);
	printf("X * X = \n");
	m_fprint(stdout, X_sq);

	m_free(A);
	m_free(X);
	m_free(X_sq);
}

/**
 * Test matrix transpose.
 */
void test_m_transpose()
{
	precision_t data[][4] = {
		{ 16,  2,  3, 13 },
		{  5, 11, 10,  8 },
		{  9,  7,  6, 12 },
		{  4, 14, 15,  1 }
	};

	matrix_t *A = m_initialize_data(4, 4, data);
	matrix_t *B = m_transpose(A);

	printf("A = \n");
	m_fprint(stdout, A);

	printf("B = A' = \n");
	m_fprint(stdout, B);

	m_free(A);
	m_free(B);
}

/**
 * Test matrix addition.
 */
void test_m_add()
{
	precision_t data_A[][2] = {
		{ 1, 0 },
		{ 2, 4 }
	};
	precision_t data_B[][2] = {
		{ 5, 9 },
		{ 2, 1 }
	};

	matrix_t *A = m_initialize_data(2, 2, data_A);
	matrix_t *B = m_initialize_data(2, 2, data_B);

	printf("A = \n");
	m_fprint(stdout, A);

	printf("B = \n");
	m_fprint(stdout, B);

	m_add(A, B);

	printf("A + B = \n");
	m_fprint(stdout, A);

	m_free(A);
	m_free(B);
}

/**
 * Test matrix column assingment.
 */
void test_m_assign_column()
{
	precision_t data_A[][4] = {
		{ 16,  2,  3, 13 },
		{  5, 11, 10,  8 },
		{  9,  7,  6, 12 },
		{  4, 14, 15,  1 }
	};
	precision_t data_B[][1] = {
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	};

	matrix_t *A = m_initialize_data(4, 4, data_A);
	matrix_t *B = m_initialize_data(4, 1, data_B);
	int i = 2;
	int j = 0;

	printf("A = \n");
	m_fprint(stdout, A);

	printf("B = \n");
	m_fprint(stdout, B);

	printf("A(:, %d) = B(:, %d)\n", i, j);

	m_assign_column(A, i, B, j);

	printf("A = \n");
	m_fprint(stdout, A);

	m_free(A);
	m_free(B);
}

/**
 * Test matrix subtraction.
 */
void test_m_subtract()
{
	precision_t data_A[][2] = {
		{ 1, 0 },
		{ 2, 4 }
	};
	precision_t data_B[][2] = {
		{ 5, 9 },
		{ 2, 1 }
	};

	matrix_t *A = m_initialize_data(2, 2, data_A);
	matrix_t *B = m_initialize_data(2, 2, data_B);

	printf("A = \n");
	m_fprint(stdout, A);

	printf("B = \n");
	m_fprint(stdout, B);

	m_subtract(A, B);

	printf("A - B = \n");
	m_fprint(stdout, A);

	m_free(A);
	m_free(B);
}

/**
 * Test matrix element-wise function application.
 */
void test_m_elem_apply()
{
	precision_t data[][3] = {
		{ 1, 0, 2 },
		{ 3, 1, 4 }
	};

	matrix_t *A = m_initialize_data(2, 3, data);

	printf("A = \n");
	m_fprint(stdout, A);

	m_elem_apply(A, sqrt);

	printf("sqrt(A) = \n");
	m_fprint(stdout, A);

	m_free(A);
}

/**
 * Test matrix multiplication by scalar.
 */
void test_m_elem_mult()
{
	precision_t data[][3] = {
		{ 1, 0, 2 },
		{ 3, 1, 4 }
	};

	matrix_t *A = m_initialize_data(2, 3, data);
	precision_t c = 3;

	printf("A = \n");
	m_fprint(stdout, A);

	m_elem_mult(A, c);

	printf("%lg * A = \n", c);
	m_fprint(stdout, A);

	m_free(A);
}

/**
 * Test matrix column shuffling.
 */
void test_m_shuffle_columns()
{
	precision_t data[][4] = {
		{ 1, 2, 3, 4 },
		{ 5, 6, 7, 8 }
	};

	matrix_t *A = m_initialize_data(2, 4, data);

	printf("A = \n");
	m_fprint(stdout, A);

	m_shuffle_columns(A);

	printf("m_shuffle_columns (A) = \n");
	m_fprint(stdout, A);

	m_free(A);
}

/**
 * Test matrix column subtraction.
 */
void test_m_subtract_columns()
{
	precision_t data_M[][4] = {
		{ 0, 2, 1, 4 },
		{ 1, 3, 3, 2 },
		{ 1, 2, 2, 2 }
	};
	precision_t data_a[][1] = {
		{ 0 },
		{ 1 },
		{ 1 }
	};

	matrix_t *M = m_initialize_data(3, 4, data_M);
	matrix_t *a = m_initialize_data(3, 1, data_a);;

	printf("M = \n");
	m_fprint(stdout, M);

	printf("a = \n");
	m_fprint(stdout, a);

	m_subtract_columns(M, a);

	printf("M - a * 1_N' = \n");
	m_fprint(stdout, M);

	m_free(M);
	m_free(a);
}

/**
 * Test matrix row subtraction.
 */
void test_m_subtract_rows()
{
	precision_t data_M[][4] = {
		{ 0, 2, 1, 4 },
		{ 1, 3, 3, 2 },
		{ 1, 2, 2, 2 }
	};
	precision_t data_a[][4] = {
		{ 0, 2, 1, 4 }
	};

	matrix_t *M = m_initialize_data(3, 4, data_M);
	matrix_t *a = m_initialize_data(1, 4, data_a);

	printf("M = \n");
	m_fprint(stdout, M);

	printf("a = \n");
	m_fprint(stdout, a);

	m_subtract_rows(M, a);

	printf("M - 1_N * a = \n");
	m_fprint(stdout, M);

	m_free(M);
	m_free(a);
}

#endif

int main (int argc, char **argv)
{
	test_func_t tests[] = {
		test_m_identity,
		test_m_ones,
		test_m_random,
		test_m_zeros,
		test_m_copy,
//		test_m_covariance,
//		test_m_diagonalize,
//		test_m_distance,
//		test_m_eigen,
//		test_m_eigen2,
//		test_m_inverse,
//		test_m_mean_column,
//		test_m_mean_row,
//		test_m_norm,
		test_m_product,
//		test_m_sqrtm,
//		test_m_transpose,
//		test_m_add,
//		test_m_subtract,
//		test_m_assign_column,
//		test_m_elem_apply,
//		test_m_elem_mult,
//		test_m_shuffle_columns,
//		test_m_subtract_columns,
//		test_m_subtract_rows
	};
	int num_tests = sizeof(tests) / sizeof(test_func_t);

	int i;
	for ( i = 0; i < num_tests; i++ ) {
		test_func_t test = tests[i];

		printf("TEST %d\n\n", i + 1);

		test();
		putchar('\n');
	}

	return 0;
}
