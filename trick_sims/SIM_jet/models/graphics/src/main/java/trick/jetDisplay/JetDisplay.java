package trick;

import java.awt.Graphics2D;
import java.awt.Graphics;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.FileReader;
import java.net.Socket;
import java.util.*;

class ScenePoly {  //creates the objects for the panel ??
    public Color color; 
    public int n; 
    public double[] x;
    public double[] y;
}

class JetDisplay extends JPanel { //creates entire panel display 
    private double[] jetPos;
    private double[] jetVel;
    
    
}