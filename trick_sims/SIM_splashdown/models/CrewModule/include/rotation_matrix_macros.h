#ifndef ROTATION_MATRIX_MACROS_H
#define ROTATION_MATRIX_MACROS_H

#define SET_X_ROTATION_MATRIX( matrix, angle ) { \
matrix[0][0] =  1.0; \
matrix[0][1] =  0.0; \
matrix[0][2] =  0.0; \
matrix[1][0] =  0.0; \
matrix[1][1] =  cos((angle)); \
matrix[1][2] = -sin((angle)); \
matrix[2][0] =  0.0; \
matrix[2][1] =  sin((angle)); \
matrix[2][2] =  cos((angle)); \
}

#define SET_Y_ROTATION_MATRIX( matrix, angle ) { \
matrix[0][0] =  cos((angle)); \
matrix[0][1] =  0.0; \
matrix[0][2] =  sin((angle)); \
matrix[1][0] =  0.0; \
matrix[1][1] =  1.0; \
matrix[1][2] =  0.0; \
matrix[2][0] =  -sin((angle)); \
matrix[2][1] =  0.0; \
matrix[2][2] =  cos((angle)); \
}

#define SET_Z_ROTATION_MATRIX( matrix, angle ) { \
matrix[0][0] =  cos((angle)); \
matrix[0][1] =  -sin((angle)); \
matrix[0][2] =  0.0; \
matrix[1][0] =  sin((angle)); \
matrix[1][1] =  cos((angle)); \
matrix[1][2] =  0.0; \
matrix[2][0] =  0.0; \
matrix[2][1] =  0.0; \
matrix[2][2] =  1.0; \
}

#endif
