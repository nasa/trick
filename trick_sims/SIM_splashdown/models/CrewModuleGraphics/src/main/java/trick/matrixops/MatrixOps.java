
package trick.matrixOps;

import java.io.*;

public class MatrixOps {

    public static void printMatrix(double M[][]) {
        int M_rows = M.length;
        int M_cols = M[0].length;
        for (int i = 0; i < M_rows; i++) {
            for (int j = 0; j < M_cols; j++)
                System.out.print(" " + M[i][j]);
            System.out.println();
        }
    }

    public static void printDVector(double V[]) {
        System.out.print("(");
        for (int i = 0; i < V.length; i++) {
            System.out.print(" " + V[i]);
        }
        System.out.println(")");
    }

    public static void printIVector(int V[]) {
        System.out.print("(");
        for (int i = 0; i < V.length; i++) {
            System.out.print(" " + V[i]);
        }
        System.out.println(")");
    }

    public static void MtimesM( double R[][], double A[][], double B[][]) {
        int A_rows = A.length;
        int A_cols = A[0].length;
        int B_rows = B.length;
        int B_cols = B[0].length;
        int R_rows = R.length;
        int R_cols = R[0].length;

        if (A_cols != B_rows) {
            System.out.println( "\nNot possible to multiply matrixes,");
            System.out.println("where the first has " + A_cols + " columns,");
            System.out.println("and the second has " + B_rows + "rows.");
            return;
        }
        if ((R_rows != A_rows) || (R_cols != B_cols)) {
            System.out.println( "\n Result matrix is wrong size.");
            return;
        }

        for (int i = 0; i < A_rows; i++) {
            for (int j = 0; j < B_cols; j++) {
                R[i][j] = 0.0;
                for (int k = 0; k < B_rows; k++)
                    R[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    public static void MtimesV( double R[], double M[][], double V[]) {
        int M_rows = M.length;
        int M_cols = M[0].length;
        int V_rows = V.length;

        if (M_cols != V_rows) {
            System.out.println( "\nNot possible to multiply matrix and vector,");
            System.out.println( "where the matrix has " + M_cols + " columns,");
            System.out.println("and the vector has " + V_rows + " elements.");
            return;
        }

        if (R.length != M.length) {
          System.out.println( "\n Result vector is wrong size.");
          return;
        }

        for (int i =0; i < M_rows ; i++) {
            R[i] = 0.0;
            for (int j =0; j < M_cols ; j++) {
                R[i] += M[i][j] * V[j];
            }
        }
        return;
    }

    public static void VplusV(double R[], double A[], double B[]) {
        int A_rows = A.length;
        int B_rows = B.length;
        int R_rows = R.length;

        if ((A_rows != B_rows) || (A_rows != R_rows)) {
          System.out.println( "\n vector are wrong size.");
        }
        for (int i=0; i<A_rows ; i++) {
          R[i] = A[i] + B[i];
        }
    }

    public static double vector_magnitude (double V[]) {
        double S = 0;
        for (int i =0; i < V.length ; i++) {
            S += V[i]*V[i];
        }
        return Math.sqrt(S);
    }

}
