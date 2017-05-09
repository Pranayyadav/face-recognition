/**
 * @file pca.h
 *
 * Interface definitions for the PCA feature layer.
 */
#ifndef PCA_H
#define PCA_H

#include "feature.h"

class PCALayer : public FeatureLayer {
private:
	int n1;

public:
	matrix_t *W;
	matrix_t *D;

	PCALayer(int n1);
	~PCALayer();

	matrix_t * compute(matrix_t *X, const std::vector<data_entry_t>& y, int c);
	matrix_t * project(matrix_t *X);

	void save(FILE *file);
	void load(FILE *file);

	void print();
};

#endif
