// This file contains helper functions for the matrix operations necessary for the graphics to be generated.

package trick.matrixOps;



public class MatrixOps { // Defines the matrix operation class.
    
    public static void printMatrix(double M[][]) { // Prints out the matrix.
        int M_rows = M.length;
        int M_cols = M[0].length;
        for (int i = 0; i < M_rows; i++) {
            for (int j = 0; j < M_cols; j++)
                System.out.print(" " + M[i][j]);
            System.out.println();
        }
    }
    
    public static void printDVector(double V[]) { // Prints out a vector passed as a double.
        System.out.print("(");
        for (int i = 0; i < V.length; i++) {
            System.out.print(" " + V[i]);
        }
        System.out.println(")");
    }
    
    public static void printIVector(int V[]) { // Prints out a vector passed as an integer.
        System.out.print("(");
        for (int i = 0; i < V.length; i++) {
            System.out.print(" " + V[i]);
        }
        System.out.println(")");
    }
    
    public static void MtimesM( double R[][], double A[][], double B[][]) { // Multiplies two matrices together.
        int A_rows = A.length;
        int A_cols = A[0].length;
        int B_rows = B.length;
        int B_cols = B[0].length;
        int R_rows = R.length;
        int R_cols = R[0].length;

        if (A_cols != B_rows) { // Checks if the matrices can be multiplied.
            System.out.println( "\nNot possible to multiply matrixes,");
            System.out.println("where the first has " + A_cols + " columns,");
            System.out.println("and the second has " + B_rows + "rows.");
            return;
        }
        if ((R_rows != A_rows) || (R_cols != B_cols)) { // Checks if the defined result matrix is the wrong size.
            System.out.println( "\n Result matrix is wrong size.");
            return;
        }

        for (int i = 0; i < A_rows; i++) { // Multiplies the two matrices together.
            for (int j = 0; j < B_cols; j++) {
                R[i][j] = 0.0;
                for (int k = 0; k < B_rows; k++)
                    R[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    public static void MtimesV( double R[], double M[][], double V[]) { // Multiplies a matrix with a vector. 
        int M_rows = M.length;
        int M_cols = M[0].length;
        int V_rows = V.length;

        if (M_cols != V_rows) { // Checks if the matrix and the vector can be multiplied together.
            System.out.println( "\nNot possible to multiply matrix and vector,");
            System.out.println( "where the matrix has " + M_cols + " columns,");
            System.out.println("and the vector has " + V_rows + " elements.");
            return;
        }

        if (R.length != M.length) { // Checks if the defined result vector is the wrong size.
          System.out.println( "\n Result vector is wrong size.");
          return;
        }

        for (int i =0; i < M_rows ; i++) { // Multiplies the vector with the matrix.
            R[i] = 0.0;
            for (int j =0; j < M_cols ; j++) {
                R[i] += M[i][j] * V[j];
            }
        }
        return;
    }
    
    public static void VplusV(double R[], double A[], double B[]) { // Adds two matrices together.
        if ((A.length != B.length) || (A.length != R.length)) {
          System.out.println( "\n MatrixOps::VplusV : Vectors are not the same size.");
        }
        for (int i=0; i<A.length ; i++) {
          R[i] = A[i] + B[i];
        }
    }
    
    public static void VminusV(double R[], double A[], double B[]) { // Subtracts two matrices together.
        if ((A.length != B.length) || (A.length != R.length)) {
          System.out.println( "\n MatrixOps::VminusV : Vectors are not the same size.");
          return;
        }
        for (int i=0; i<A.length ; i++) {
          R[i] = A[i] - B[i];
        }
    }
    
    public static void VcrossV(double R[], double A[], double B[]) { // Finds the cross product of two matrices.
        if ((R.length != 3) || (A.length != 3) || (B.length != 3)) {
          System.out.println( "\n MatrixOps::VcrossV : All vector args must be length 3.");
          return;
        }
        R[0] = A[1] * B[2] - A[2] * B[1];
        R[1] = A[2] * B[0] - A[0] * B[2];
        R[2] = A[0] * B[1] - A[1] * B[0];
    }
    
    public static double VdotV(double A[], double B[]) { // Finds the dot product of two matrices.
        if (A.length != B.length) {
          System.out.println( "\n MatrixOps::VdotV : Vectors are not the same size.");
          return 0.0;
        }
        double R = 0.0;
        for (int i=0; i<A.length ; i++) {
          R += A[i] * B[i];
        }
        return R;
    }
    
    public static void Vscale(double R[], double A[], double S) { // Scales a vector (Multiplies by a given number).
        if (A.length != R.length) {
          System.out.println( "\n MatrixOps::Vscale : Input and output vectors are not the same size.");
          return;
        }
        for (int i=0; i<A.length ; i++) {
          R[i] = A[i] * S;
        }
    }
    
    public static double Vmagnitude (double V[]) { // Returns the magnitude of a vector (Length of a vector).
        double S = 0;
        for (int i =0; i < V.length ; i++) {
            S += V[i]*V[i];
        }
        return Math.sqrt(S);
    }

}
    
    
       

