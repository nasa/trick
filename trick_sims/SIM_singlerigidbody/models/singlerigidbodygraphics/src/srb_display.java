// This file contains the architecture for the single rigid body simulation's 3D graphics client.

package trick;

// AWT is the abstract window toolkit.
import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.Color;
import java.awt.event.MouseEvent;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileReader;

import java.net.Socket;

import java.util.*;

// Swing is a GUI widget toolkit.
import javax.swing.JFrame; // Allows you to manage a top-level window.
import javax.swing.JPanel; // The stuff within the window (controls, buttons, etc.).
import javax.swing.event.MouseInputAdapter;

import trick.matrixOps.MatrixOps;

/**
 * @author Aditya Girish
 */

 class SRBView extends JPanel { // Main body class of how we view the SRB.
    
    private int screen_half_width;
    private int screen_half_height;

    private double illumination_vector[]; // World coordinates

    private double vantageAzimuth; // Secifies the direction of the celestial light source.
    private double vantageElevation;

    private double vantageDistance;
    private double beta;

    private double worldToVantageRotation[][];

    private Color bodyLineColor;
    private Color bodyFillColor;

    private double[] bodyPos;

    private double bodyToWorldRotation[][];

    private double veh_vrtx_body[][];
    private double veh_vrtx_world[][];
    private int    veh_vrtx_screen[][];

    private int veh_edges[][];
    private int veh_triangles[][];

    private double veh_unit_normals_body[][];
    private double veh_unit_normals_world[][];
    private double veh_unit_normals_vantage[][];

    public SRBView() {

        ViewListener viewListener = new ViewListener();
        addMouseListener(viewListener);
        addMouseMotionListener(viewListener);

        // Direction of light.
        illumination_vector = new double[] {-0.707, 0.0, -0.707};

        // Location of the observer, looking toward the world's origin.
        vantageAzimuth   = Math.toRadians(45.0);
        vantageElevation = Math.toRadians(20.0);
        vantageDistance = 12.0;

        // Half field of view angle
        beta = Math.toRadians(40.0);

        worldToVantageRotation = new double[3][3];
        setAzElRotation(worldToVantageRotation, vantageAzimuth, vantageElevation);

        bodyLineColor =  Color.GRAY;
        bodyFillColor = new Color(255, 255, 250);

        bodyPos = new double[] {0.0, 0.0, 0.0};

        bodyToWorldRotation = new double[][] {{1.0, 0.0, 0.0},
                                              {0.0, 1.0, 0.0},
                                              {0.0, 0.0, 1.0}};


    }

    public void setAzElRotation(double RotationMatrix[][], double azimuth, double elevation) { // Sets the azimuth and elevation rotation.
        double Rotation_about_Y[][] = {
          { Math.cos(elevation), 0.0, Math.sin(elevation)},
          {                 0.0, 1.0,                 0.0},
          {-Math.sin(elevation), 0.0, Math.cos(elevation)}
        };
  
        double Rotation_about_Z[][] = {
          {Math.cos(azimuth), -Math.sin(azimuth), 0.0},
          {Math.sin(azimuth),  Math.cos(azimuth), 0.0},
          {              0.0,                0.0, 1.0}
        };
        MatrixOps.MtimesM( RotationMatrix, Rotation_about_Y, Rotation_about_Z);
      }

 }