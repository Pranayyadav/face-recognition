/**
 * @file database.c
 *
 * Implementation of the database type.
 */
#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "image.h"
#include "timing.h"

/**
 * Map a collection of images to column vectors.
 *
 * The image matrix has size m x n, where m is the number of
 * pixels in each image and n is the number of images. The
 * images must all have the same size.
 *
 * @param entries     pointer to list of image entries
 * @param num_images  number of images
 * @return pointer to image matrix
 */
matrix_t * get_image_matrix(image_entry_t *entries, int num_images)
{
	// get the image size from the first image
	image_t *image = image_construct();

	image_read(image, entries[0].name);

	matrix_t *T = m_initialize(image->channels * image->height * image->width, num_images);

	// map each image to a column vector
	m_image_read(T, 0, image);

	int i;
	for ( i = 1; i < num_images; i++ ) {
		image_read(image, entries[i].name);
		m_image_read(T, i, image);
	}

	image_destruct(image);

	return T;
}

/**
 * Construct a database.
 *
 * @param pca
 * @param lda
 * @param ica
 * @return pointer to new database
 */
database_t * db_construct(int pca, int lda, int ica)
{
	database_t *db = (database_t *)calloc(1, sizeof(database_t));
	db->pca = pca;
	db->lda = lda;
	db->ica = ica;

	return db;
}

/**
 * Destruct a database.
 *
 * @param db  pointer to database
 */
void db_destruct(database_t *db)
{
	int i;
	for ( i = 0; i < db->num_images; i++ ) {
		free(db->entries[i].name);
	}
	free(db->entries);

	m_free(db->mean_face);

	if ( db->pca || db->lda || db->ica ) {
		m_free(db->W_pca_tr);
		m_free(db->P_pca);
	}

	if ( db->lda ) {
		m_free(db->W_lda_tr);
		m_free(db->P_lda);
	}

	if ( db->ica ) {
		m_free(db->W_ica_tr);
		m_free(db->P_ica);
	}

	free(db);
}

/**
 * Train a database with a set of images.
 *
 * @param db	pointer to database
 * @param path  directory of training images
 */
void db_train(database_t *db, const char *path, int n_opt1, int n_opt2)
{
	timing_push("Training");

	db->num_images = get_directory_rec(path, &db->entries, &db->num_classes);

	// compute mean-subtracted image matrix X
	matrix_t *X = get_image_matrix(db->entries, db->num_images);

	db->num_dimensions = X->rows;
	db->mean_face = m_mean_column(X);

	m_subtract_columns(X, db->mean_face);

	// compute PCA representation
	matrix_t *W_pca;
	matrix_t *D;

	if ( db->pca || db->lda || db->ica ) {
		if ( VERBOSE ) {
			printf("Computing PCA representation...\n");
		}

		W_pca = PCA(X, &D);

		db->W_pca_tr = m_transpose(W_pca);
		db->P_pca = m_product(db->W_pca_tr, X);
	}

	// compute LDA representation
	if ( db->lda ) {
		if ( VERBOSE ) {
			printf("Computing LDA representation...\n");

			printf("n_opt1 = %d\n", n_opt1);
			printf("n_opt2 = %d\n", n_opt2);
			putchar('\n');
		}

		db->W_lda_tr = LDA(W_pca, X, db->num_classes, db->entries, n_opt1, n_opt2);
		db->P_lda = m_product(db->W_lda_tr, X);
	}

	// compute ICA representation
	if ( db->ica ) {
		if ( VERBOSE ) {
			printf("Computing ICA representation...\n");
		}

		db->W_ica_tr = ICA(X); // W_pca, D
		db->P_ica = m_product(db->W_ica_tr, X);
	}

	timing_pop();

	// cleanup
	m_free(X);
	m_free(W_pca);
	m_free(D);
}

/**
 * Save a database to the file system.
 *
 * @param db		  pointer to database
 * @param path_tset   path to save image filenames
 * @param path_tdata  path to save matrix data
 */
void db_save(database_t *db, const char *path_tset, const char *path_tdata)
{
	// save the image filenames
	FILE *tset = fopen(path_tset, "w");

	int i;
	for ( i = 0; i < db->num_images; i++ ) {
		fprintf(tset, "%d %s\n", db->entries[i].ent_class, db->entries[i].name);
	}
	fclose(tset);

	// save the mean face and PCA/LDA/ICA representations
	FILE *tdata = fopen(path_tdata, "w");

	m_fwrite(tdata, db->mean_face);

	if ( db->pca || db->lda || db->ica ) {
		m_fwrite(tdata, db->W_pca_tr);
		m_fwrite(tdata, db->P_pca);
	}

	if ( db->lda ) {
		m_fwrite(tdata, db->W_lda_tr);
		m_fwrite(tdata, db->P_lda);
	}

	if ( db->ica ) {
		m_fwrite(tdata, db->W_ica_tr);
		m_fwrite(tdata, db->P_ica);
	}

	fclose(tdata);
}

/**
 * Load a database from the file system.
 *
 * @param db          pointer to database
 * @param path_tset   path to read image filenames
 * @param path_tdata  path to read matrix data
 */
void db_load(database_t *db, const char *path_tset, const char *path_tdata)
{
	// read the mean face and PCA/LDA/ICA representations
	FILE *tdata = fopen(path_tdata, "r");

	db->mean_face = m_fread(tdata);

	if ( db->pca || db->lda || db->ica ) {
		db->W_pca_tr = m_fread(tdata);
		db->P_pca = m_fread(tdata);
	}

	if ( db->lda ) {
		db->W_lda_tr = m_fread(tdata);
		db->P_lda = m_fread(tdata);
	}

	if ( db->ica ) {
		db->W_ica_tr = m_fread(tdata);
		db->P_ica = m_fread(tdata);
	}

	db->num_images = db->P_pca->cols;
	db->num_dimensions = db->mean_face->rows;

	fclose(tdata);

	// get image filenames
	FILE *tset = fopen(path_tset, "r");

	db->entries = (image_entry_t *)malloc(db->num_images * sizeof(image_entry_t));

	int i;
	for ( i = 0; i < db->num_images; i++ ) {
		db->entries[i].name = (char *)malloc(64 * sizeof(char));
		fscanf(tset, "%d %s", &db->entries[i].ent_class, db->entries[i].name);
	}

	fclose(tset);
}

/**
 * Find the column vector in a matrix P with minimum distance from
 * a test vector P_test.
 *
 * @param P          pointer to matrix
 * @param P_test     pointer to column vector
 * @param dist_func  pointer to distance function
 * @return index of matching column in P
 */
int nearest_neighbor(matrix_t *P, matrix_t *P_test, dist_func_t dist_func)
{
	int min_index = -1;
	precision_t min_dist = -1;

	int j;
	for ( j = 0; j < P->cols; j++ ) {
		// compute the distance between the two images
		precision_t dist = dist_func(P_test, 0, P, j);

		// update the running minimum
		if ( min_dist == -1 || dist < min_dist ) {
			min_index = j;
			min_dist = dist;
		}
	}

	return min_index;
}

typedef struct {
	int enabled;
	const char * name;
	matrix_t *W_tr;
	matrix_t *P;
	dist_func_t dist_func;
	int rec_index;
	int num_correct;
} rec_params_t;

/**
 * Test a set of images against a database.
 *
 * @param db    pointer to database
 * @param path  directory of test images
 */
void db_recognize(database_t *db, const char *path)
{
	timing_push("Recognition");

	// initialize parameters for each recognition algorithm
	rec_params_t algorithms[] = {
		{ db->pca, "PCA", db->W_pca_tr, db->P_pca, m_dist_L2, 0 },
		{ db->lda, "LDA", db->W_lda_tr, db->P_lda, m_dist_L2, 0 },
		{ db->ica, "ICA", db->W_ica_tr, db->P_ica, m_dist_COS, 0 }
	};
	int num_algorithms = sizeof(algorithms) / sizeof(rec_params_t);

	// get test images
	char **image_names;
	int num_test_images = get_directory(path, &image_names);

	// test each image against the database
	image_t *image = image_construct();
	matrix_t *T_i = m_initialize(db->num_dimensions, 1);

	int i;
	for ( i = 0; i < num_test_images; i++ ) {
		// read the test image T_i
		image_read(image, image_names[i]);
		m_image_read(T_i, 0, image);
		m_subtract(T_i, db->mean_face);

		// perform recognition for each algorithm
		int j;
		for ( j = 0; j < num_algorithms; j++ ) {
			rec_params_t *params = &algorithms[j];

			if ( params->enabled ) {
				matrix_t *P_test = m_product(params->W_tr, T_i);
				params->rec_index = nearest_neighbor(params->P, P_test, params->dist_func);

				m_free(P_test);
			}
		}

		// print results
		if ( VERBOSE ) {
			printf("test image: \'%s\'\n", rem_base_dir(image_names[i]));
		}

		for ( j = 0; j < num_algorithms; j++ ) {
			rec_params_t *params = &algorithms[j];

			if ( params->enabled ) {
				char *rec_name = db->entries[params->rec_index].name;

				if ( VERBOSE ) {
					printf("       %s: \'%s\'\n", params->name, rem_base_dir(rec_name));
				}

				if ( is_same_class(rec_name, image_names[i]) ) {
					params->num_correct++;
				}
			}
		}

		if ( VERBOSE ) {
			putchar('\n');
		}
	}

	// print accuracy results
	for ( i = 0; i < num_algorithms; i++ ) {
		rec_params_t *params = &algorithms[i];

		if ( params->enabled ) {
			float accuracy = 100.0f * params->num_correct / num_test_images;

			if ( VERBOSE ) {
				printf("%s: %d / %d matched, %.2f%%\n", params->name, params->num_correct, num_test_images, accuracy);
			}
			else {
				printf("%.2f\n", accuracy);
			}
		}
	}

	timing_pop();

	// cleanup
	image_destruct(image);
	m_free(T_i);

	for ( i = 0; i < num_test_images; i++ ) {
		free(image_names[i]);
	}
	free(image_names);
}
