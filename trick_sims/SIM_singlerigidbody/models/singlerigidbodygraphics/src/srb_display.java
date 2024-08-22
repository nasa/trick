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

    private Color waterColor;
    private double water_vrtx_world[][];

    private int water_poly_x[];
    private int water_poly_y[];

    private Color vehicleLineColor;
    private Color vehicleFillColor;

    private double[] vehiclePos;

    private double bodyToWorldRotation[][];

    private double veh_vrtx_body[][];
    private double veh_vrtx_world[][];
    private int    veh_vrtx_screen[][];

    private int veh_edges[][];
    private int veh_triangles[][];

    private double veh_unit_normals_body[][];
    private double veh_unit_normals_world[][];
    private double veh_unit_normals_vantage[][];
    
 }