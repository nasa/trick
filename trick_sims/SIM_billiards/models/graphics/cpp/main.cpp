#include <igl/opengl/glfw/Viewer.h>
// #include <igl/readOFF.h>
#include <igl/opengl/glfw/imgui/ImGuiPlugin.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h> 
#include <string> 

#include "Socket.hh"


std::vector<Eigen::Vector3d> ball_colors = {Eigen::Vector3d(0.0,0.4,0.0), //green
                                            Eigen::Vector3d(1.0,1.0,0.0), //yellow
                                            Eigen::Vector3d(0.0,0.0,1.0), //blue
                                            Eigen::Vector3d(1.0,0.0,0.0), //red
                                            Eigen::Vector3d(0.5,0.0,0.3), //purple
                                            Eigen::Vector3d(0.7,0.5,0.0), //orange
                                            Eigen::Vector3d(0.0,0.0,0.0), //black
                                            Eigen::Vector3d(0.3,0.07,0.15), //maroon
                                            };


const double layer_BALL = 0.005;
const double layer_TABLE = 0.001;
const double layer_BUMPER = 0.002;
const double layer_RAIL = 0.000;
const double layer_CUE = 0.008;
const double layer_POCKET = 0.002;



class RenderedShape {
  public:
    RenderedShape() {}

    int baseIndex = 0;

    int getNumVertices() {
      return vertices.size();
    }

    int getNumFaces() {
      return faces.size();
    }
    
    std::vector<Eigen::Vector3i> getFacesWithBaseIndex(int newBase) {
      std::vector<Eigen::Vector3i> newFaces;
      int offset = newBase - baseIndex;
      Eigen::Vector3i baseOffset(offset, offset, offset);
      for (Eigen::Vector3i& face : faces) {
        newFaces.emplace_back(face + baseOffset);
      }
      baseIndex = newBase;

      return newFaces;
    }

    std::vector<Eigen::Vector3d> vertices;
    std::vector<Eigen::Vector3i> faces;
    std::vector<Eigen::Vector3d> colors;
};

class Point {

  public:
    Point () : point(0, 0) {}
    Point (double x, double y) : point(x, y, 0) {}
    Point (double x, double y, double layer) : point(x, y, layer) {}

    Eigen::Vector3d toVec3 () const {
      return Eigen::Vector3d(point);
    }

    double x() const {
      return point[0];
    }

    double y() const {
      return point[1];
    }

    Eigen::Vector3d point;
};

class Polygon {
  public:
    Polygon(unsigned int verts, double layer) : vertexMax(verts), layer(layer) {}

    void addPoint(double x, double y) {
      if (points.size() < vertexMax)
        points.emplace_back(x, y, layer);
    }

    void setColor(double r, double g, double b) {
      color = Eigen::Vector3d(r, g, b);
    }

    void setColor(Eigen::Vector3d& c) {
      color = Eigen::Vector3d(c);
    }

    void setLayer(double l) {
      layer = l;
    }

    Eigen::Vector3d getColor() const {
      return color;
    }

    // Works with any simple convex polygon where the points are in order
    virtual RenderedShape* render() const {
      RenderedShape * shape = new RenderedShape();
      
      if (!isValid()) {
        // Should maybe throw an error
        std::cout << "Generic has incorrect number of corners" << std::endl;
        return shape;
      }

      for (int i = 0; i < points.size(); i++) {
        shape->vertices.emplace_back(points[i].toVec3());
        shape->colors.emplace_back(color);
      }

      for (int i = 1; i < points.size()-1; i++) {
        shape->faces.emplace_back(0, i, i+1);
      }

      return shape;
    }

    virtual bool isValid() const {
      return points.size() <= vertexMax;
    }

  protected:
    std::vector<Point> points;
    Eigen::Vector3d color;
    unsigned int vertexMax;
    double layer;
    int id;
};

class Circle : public Polygon {
  public:
    Circle (double x, double y, double r, double layer) : Polygon(1, layer), radius(r) {
      addPoint(x, y);
    }

    bool isValid() const {
      return points.size() == vertexMax;
    }

    RenderedShape *render() const {
      // Circle is broken down into wedges in order to be rendered here
      RenderedShape *shape = new RenderedShape();

      if (!isValid()) {
        std::cout << "Circle has incorrect number of points" << std::endl;
        return shape;
      }

      // Add colors
      for (int i = 0; i < numWedges + 2; i++) {
        shape->colors.emplace_back(color);
      }

      // Add center
      shape->vertices.emplace_back(points[0].toVec3());

      // Add outside vertices
      for (int i = 0; i <= numWedges; i++) {
        shape->vertices.emplace_back(points[0].x() + radius * cos(2 * M_PI * i / numWedges),
                                    points[0].y() + radius * sin(2 * M_PI * i / numWedges),
                                    layer);

      }

      // Make the triangles
      for (int j = 0; j <= numWedges; j++) {
        shape->faces.emplace_back(0, j + 1, 1 + ((j + 1) % (numWedges + 1)));
      }


      return shape;
    }

  private:
    double radius;
    int numWedges = 20;
};

class Rectangle : public Polygon {
  public:

    // only need upper left and lower right corners
    Rectangle(double layer) : Polygon(2, layer) {}

    bool isValid() const {
      return points.size() == vertexMax;
    }

    void addCorner (double x, double y) {
      addPoint(x, y);
    }

    RenderedShape* render() const {
      RenderedShape *shape = new RenderedShape();

      if (!isValid()) {
        // Should maybe throw an error
        std::cerr << "Rectangle has incorrect number of corners" << std::endl;
        return shape;
      }
      
      // Add colors
      for (int i = 0; i < 6; i++) {
        shape->colors.emplace_back(color);
      }

      // Add triangles within rectangle
      shape->vertices.emplace_back(points[0].x(), points[1].y(), layer);
      shape->vertices.emplace_back(points[1].x(), points[0].y(), layer);
      shape->vertices.emplace_back(points[0].toVec3());

      shape->faces.emplace_back(0, 1, 2);

      shape->vertices.emplace_back(points[0].x(), points[1].y(), layer);
      shape->vertices.emplace_back(points[1].toVec3());
      shape->vertices.emplace_back(points[1].x(), points[0].y(), layer);

      shape->faces.emplace_back(3, 4, 5);

      return shape;
    }
};

class Triangle : public Polygon {
  public:
    Triangle (double layer) : Polygon(3, layer) {}


    void addCorner (double x, double y) {
      addPoint(x, y);
    }

    RenderedShape* render() const {
      RenderedShape *shape = new RenderedShape();

      if (!isValid()) {
        // Should maybe throw an error
        std::cerr << "Triangle has incorrect number of corners" << std::endl;
        return shape;
      }

      // Add colors and vertices at the same time
      for (int i = 0; i < 3; i++) {
        shape->colors.emplace_back(color);
        shape->vertices.emplace_back(points[i].toVec3());
      }

      // just the 1 triangle
      shape->faces.emplace_back(0, 1, 2);

      return shape;
    }
};

enum PolygonType {
  GENERIC,
  CIRCLE,
  TRIANGLE,
  RECTANGLE
};

class Table {
  public:
    Table () {}

    void clearMovingShapes() {
      for (int i = 0; i < movingShapes.size(); i++) {
        delete movingShapes[i];
        delete movingRenderedShapes[i];
      }
      movingShapes.clear();
      movingRenderedShapes.clear();

    }

    void updateMovingShape(int id) {
      // TODO
    }

    // Need to have an agreed upon way to send over variables
    int addShape(std::vector<double> shapeData, Eigen::Vector3d color, bool isStatic, PolygonType type, double layer) {
      // std::cout << "In AddShape" << std::endl;
      Polygon *shape;

      switch (type) {

        case GENERIC: {
          // Number of points is just data / 2 i guess
          // std::cout << "Creating generic polygon with " << shapeData.size()/2 << " points" << std::endl;
          Polygon *newPolygon = new Polygon(shapeData.size()/2, layer);
          for (int i = 0; i < shapeData.size(); i+=2) {
            double x = shapeData[i];
            double y = shapeData[i+1];
            newPolygon->addPoint(x,y);
          }
          shape = newPolygon;
          break;
        }
        case CIRCLE: {
          // std::cout << "Adding circle" << std::endl;
          if (shapeData.size() != 3) {
            std::cout << "Bad shapedata size for circle" << std::endl;
            return -1;
          }
          double x = shapeData[0];
          double y = shapeData[1];
          double r = shapeData[2];
          Circle *newCircle = new Circle(x, y, r, layer);

          shape = newCircle;
          break;
        }
        case TRIANGLE: {
          Triangle *newTriangle = new Triangle(layer);
          if (shapeData.size() != 6) {
            std::cout << "Bad shapedata size for triangle" << std::endl;
            return -1;
          }
          for (int i = 0; i < shapeData.size(); i+=2) {
            double x = shapeData[i];
            double y = shapeData[i+1];
            newTriangle->addCorner(x, y);
          }

          shape = newTriangle;
          break;
        }
        case RECTANGLE: {
          // std::cout << "In rectangle" << std::endl;
          Rectangle *newRectangle = new Rectangle(layer);
          if (shapeData.size() != 4) {
            std::cout << "Bad shapedata size for rectangle" << std::endl;
            return -1;
          }
          for (int i = 0; i < shapeData.size(); i+=2) {
            double x = shapeData[i];
            double y = shapeData[i+1];
            newRectangle->addCorner(x, y);
          }
          shape = newRectangle;
          break;
        }
        default: {
          break;
        }
      }

      shape->setColor(color);

      if (isStatic) {
        std::cout << "Adding to static shapes" << std::endl;
        staticShapes.emplace_back(shape);
      } else {
        // std::cout << "Adding to moving shapes" << std::endl;
        movingShapes.emplace_back(shape);
      }
      

      return 0;
    }

    // Call this once
    void renderStaticShapes() {
      staticRendered = true;
      numStaticVertices = 0;
      numStaticFaces = 0;
      int i = 0;
      staticRenderedShapes.clear();
      for (Polygon* shape : staticShapes) {
        std::cout << "Rendering shape " << i++ << std::endl;
        RenderedShape *renderedShape = shape->render();
        numStaticVertices += renderedShape->getNumVertices();
        numStaticFaces += renderedShape->getNumFaces();
        staticRenderedShapes.emplace_back(renderedShape);
      }
    }

    // Should think about how to make sure we aren't making big unnessary copies of stuff
    std::tuple<Eigen::MatrixXd, Eigen::MatrixXi, Eigen::MatrixXd> getMesh() {

      // if (!staticRendered) {
      //   renderStaticShapes();
      // }

      numStaticVertices = 0;
      numStaticFaces = 0;
      int i = 0;
      staticRenderedShapes.clear();
      
      for (Polygon* shape : staticShapes) {
        RenderedShape *renderedShape = shape->render();
        numStaticVertices += renderedShape->getNumVertices();
        numStaticFaces += renderedShape->getNumFaces();
        staticRenderedShapes.emplace_back(renderedShape);
      }

      int totalFaces = numStaticFaces;
      int totalVertices = numStaticVertices;
      for (Polygon* shape : movingShapes) {
          RenderedShape *renderedShape = shape->render();
          totalVertices += renderedShape->getNumVertices();
          totalFaces += renderedShape->getNumFaces();
          movingRenderedShapes.push_back(renderedShape);
      }

      // std::cout << "Total Vertices: " << totalVertices << std::endl;
      // std::cout << "Total Faces: " << totalFaces << std::endl;
      // std::cout << "Total Colors: " << totalVertices << std::endl;


      // Now have to put all of these into giant matrices
      Eigen::MatrixXd renderV;
      renderV.resize(totalVertices, 3);
      Eigen::MatrixXi renderF;
      renderF.resize(totalFaces, 3);
      Eigen::MatrixXd renderC;
      renderC.resize(totalVertices, 3);

      // TODO: Ideally have some matrix with preloaded static shapes
      // For now do them all here

      int vertexIndex = 0;
      int faceIndex = 0;
      for (RenderedShape* shape : staticRenderedShapes) {
        // Add vertices and colors
        for (int i = 0; i < shape->getNumVertices(); i++) {
          renderV.row(i+vertexIndex) = shape->vertices[i];
          renderC.row(i+vertexIndex) = shape->colors[i];
        }

        auto newFaces = shape->getFacesWithBaseIndex(vertexIndex);

        // Add faces - with the correct offset
        for (int i = 0; i < newFaces.size(); i++) {
          renderF.row(i+faceIndex) = newFaces[i];
        }

        vertexIndex += shape->getNumVertices();
        faceIndex += newFaces.size();
      }

      for (RenderedShape* shape : movingRenderedShapes) {
        // Add vertices and colors
        for (int i = 0; i < shape->getNumVertices(); i++) {
          renderV.row(i+vertexIndex) = shape->vertices[i];
          renderC.row(i+vertexIndex) = shape->colors[i];
        }

        auto newFaces = shape->getFacesWithBaseIndex(vertexIndex);

        // Add faces - with the correct offset
        for (int i = 0; i < newFaces.size(); i++) {
          renderF.row(i+faceIndex) = newFaces[i];
        }

        vertexIndex += shape->getNumVertices();
        faceIndex += newFaces.size();
      }

      return std::make_tuple(renderV, renderF, renderC);
    }

  private:
    std::vector<Polygon *> staticShapes;
    std::vector<RenderedShape *> staticRenderedShapes;
    std::vector<RenderedShape *> movingRenderedShapes;
    int numStaticVertices;
    int numStaticFaces;
    bool staticRendered = false;

    std::vector<Polygon *> movingShapes;

};

void printUsage() {
  std::cout << "Usage: program <portNumber>" << std::endl;
}

std::vector<std::string> split (std::string& str, const char delim) {
  std::stringstream ss(str);
  std::string s;
  std::vector<std::string> ret;
  while (std::getline(ss, s, delim)) {
    ret.push_back(s);
  }
  return ret;
}

std::vector<double> parseTrickResponse(std::vector<std::string> list) {
  std::vector<double> ret;
  for (int i = 1; i < list.size(); i++) {
    ret.push_back(stod(list[i]));
  }
  return ret;
}

std::vector<int> parseTrickResponseInt(std::vector<std::string> list) {
  std::vector<int> ret;
  for (int i = 1; i < list.size(); i++) {
    ret.push_back(stoi(list[i]));
  }
  return ret;
}

int main(int argc, char *argv[])
{
  // Parse socket number out of argv
  if (argc != 2) {
    printUsage();
    return -1;
  }

  bool socketOn = true;
  int port = 0;
  port = std::stoi(argv[1]);
  if (port == 0) {
    socketOn = false;
  }

  std::cout << "Port received: " << port << std::endl;
  Socket socket;
  socket.init("localhost", port);

  std::string reply;

  socket << "trick.var_set_client_tag(\"PoolTableDisplay\") \n";
  socket << "trick.var_add(\"dyn.table.numBalls\")\ntrick.var_send() \ntrick.var_clear() \n";
  socket >> reply;

  auto parsed = split(reply, '\t');
  int numBalls = stoi(parsed[1]);
  std::cout << "Number of balls received: " << numBalls << std::endl;

  std::string radiusRequest = "";
  char* templateString = "trick.var_add(\"dyn.table.balls[%d][0].radius\")\n";
  for (int i = 0; i < numBalls; i++) {
    char buf[64];
    sprintf(buf, templateString, i);
    radiusRequest += std::string(buf);
  }

  socket << radiusRequest;
  socket << "trick.var_send() \ntrick.var_clear() \n";
  socket >> reply;

  auto radii = parseTrickResponse(split(reply, '\t'));

  Table table;

  socket << "trick.var_add(\"dyn.table.numTablePoints\") \ntrick.var_add(\"dyn.table.tableShapeType\")\n \ntrick.var_send() \ntrick.var_clear() \n";
  socket >> reply;

  std::vector<double> tableData = parseTrickResponse(split(reply, '\t'));
  int numTablePoints = tableData[0];
  enum PolygonType tableShape = PolygonType((int)tableData[1]);

  std::string pointRequest = "";
  for (int i = 0; i < numTablePoints; i++) {
    templateString = "trick.var_add(\"dyn.table.tableShape[%d][0]._x\")\ntrick.var_add(\"dyn.table.tableShape[%d][0]._y\")\n";
    char buf[256];
    sprintf(buf, templateString, i, i);
    pointRequest += std::string(buf);
  }

  socket << pointRequest;
  socket << "trick.var_send() \ntrick.var_clear() \n";
  socket >> reply;

  std::vector<double> tablePoints = parseTrickResponse(split(reply, '\t'));
  table.addShape(tablePoints, Eigen::Vector3d(0.2, 0.6, 0.2), true, tableShape, layer_TABLE);


  // Make the rail - translate each point on the table out from center by railWidth
  std::vector<double> railData;

  if (tableShape == RECTANGLE) { 
    // If it's a rectangle then the rail is a bigger rectangle   
    double railWidth = 0.07;
    railData.push_back(tablePoints[0] - railWidth);
    railData.push_back(tablePoints[1] - railWidth);
    railData.push_back(tablePoints[2] + railWidth);
    railData.push_back(tablePoints[3] + railWidth);
  } else {
    // If it's just a shape then rail is bigger shape
    // Works with simple convex polygons
    double railWidth = 0.15;
    for (int i = 0; i < tablePoints.size(); i+=2) {
      Eigen::Vector2d point(tablePoints[i], tablePoints[i+1]);
      Eigen::Vector2d railPoint(tablePoints[i], tablePoints[i+1]);
      point *= railWidth;
      railPoint = railPoint + point;
      railData.push_back(railPoint(0));
      railData.push_back(railPoint(1));
    }
  }

  table.addShape(railData, Eigen::Vector3d(.3, .2, .15), true, tableShape, layer_RAIL);

  socket << "trick.var_add(\"dyn.table.numPockets\")\n \ntrick.var_send() \ntrick.var_clear() \n";
  socket >> reply;

  int numPockets = stoi(split(reply, '\t')[1]);
  for (int i = 0; i < numPockets; i++) {
    templateString = "trick.var_add(\"dyn.table.pockets[%d][0].pos._x\")\ntrick.var_add(\"dyn.table.pockets[%d][0].pos._y\")\n\ntrick.var_add(\"dyn.table.pockets[%d][0].radius\")\n";
    char buf[256];
    sprintf(buf, templateString, i, i, i);
    std::string pocketRequest = std::string(buf);

    socket << pocketRequest;
    socket << "trick.var_send() \ntrick.var_clear() \n";
    socket >> reply;
    std::vector<double> pocketData = parseTrickResponse(split(reply, '\t'));
    table.addShape(pocketData, Eigen::Vector3d(0.0, 0.0, 0.0), true, CIRCLE, layer_POCKET);
  }

  // Bumpers
  socket << "trick.var_add(\"dyn.table.numBumpers\")\n \ntrick.var_send() \ntrick.var_clear() \n";
  socket >> reply;

  int numBumpers = stoi(split(reply, '\t')[1]);

  std::cout << "Num bumpers: " << numBumpers << std::endl;

  for (int i = 0; i < numBumpers; i++) {
    std::string bumperRequests = "";
    
    templateString = "trick.var_add(\"dyn.table.bumpers[%d][0].numPoints\")\ntrick.var_add(\"dyn.table.bumpers[%d][0].shapeType\") \n"; 
    char buf[256];
    sprintf(buf, templateString, i, i);
    bumperRequests += std::string(buf);
    socket << bumperRequests;
    socket << "trick.var_send() \ntrick.var_clear() \n";
    socket >> reply;
    std::vector<int> bumperData = parseTrickResponseInt(split(reply, '\t'));
    int bumperPoints = bumperData[0];
    enum PolygonType bumperShape = PolygonType((int)bumperData[1]);
    
    templateString = "trick.var_add(\"dyn.table.bumpers[%d][0].renderedShape[%d][0]._x\")\ntrick.var_add(\"dyn.table.bumpers[%d][0].renderedShape[%d][0]._y\")\n";
    bumperRequests = "";
    for (int j = 0; j < bumperPoints; j++) {
      char buf[256];
      sprintf(buf, templateString, i, j, i, j);
      bumperRequests += std::string(buf);
    }
    socket << bumperRequests;
    socket << "trick.var_send() \ntrick.var_clear() \n";
    socket >> reply;

    std::vector<double> bumperBorder = parseTrickResponse(split(reply, '\t'));
    table.addShape(bumperBorder, Eigen::Vector3d(0.2,0.4,0.2), true, bumperShape, layer_BUMPER);
  }


  // socket << "trick.var_pause()\n";
  // Request all of the ball positions
  std::string positionRequest = "";
  templateString = "trick.var_add(\"dyn.table.balls[%d][0].pos._x\")\ntrick.var_add(\"dyn.table.balls[%d][0].pos._y\")\n";
  for (int i = 0; i < numBalls; i++) {
    char buf[128];
    sprintf(buf, templateString, i, i);
    positionRequest += std::string(buf);
  }

  socket << positionRequest;
  socket << "trick.var_send() \ntrick.var_clear() \n";
  socket >> reply;

  auto positions = parseTrickResponse(split(reply, '\t'));


  for (int i = 0; i < numBalls; i++) {
    std::vector<double> circleData = {positions[(i*2)], positions[(i*2 + 1)], radii[i]};
    Eigen::Vector3d circleColor = ball_colors[i % ball_colors.size()];
    table.addShape(circleData, circleColor, false, CIRCLE, layer_BALL);
  }

  bool mousePressed = false;
  double mouseX = 0;
  double mouseY = 0;

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  Eigen::MatrixXd C;

  std::tie(V, F, C) = table.getMesh();

  igl::opengl::glfw::Viewer * view = new igl::opengl::glfw::Viewer();
  igl::opengl::glfw::imgui::ImGuiPlugin *plugin = new igl::opengl::glfw::imgui::ImGuiPlugin();
  igl::opengl::glfw::imgui::ImGuiMenu *menu = new igl::opengl::glfw::imgui::ImGuiMenu();

  view->plugins.push_back(plugin);
  plugin->widgets.push_back(menu);

  view->callback_mouse_down = [&](igl::opengl::glfw::Viewer& viewer, int button, int modifier) -> bool {
    mousePressed = true;
    return false;
  };

  view->callback_mouse_up = [&] (igl::opengl::glfw::Viewer& viewer, int button, int modifier) -> bool {
    mousePressed = false;
    std::string cueRequest = "";
    std::string templateString = "dyn.table.applyCueForce(%.3f, %.3f) \n";

    char buf[128];
    sprintf(buf, templateString.c_str(), mouseX, mouseY);
    cueRequest += std::string(buf);
    socket << cueRequest;
    return false;
  };

  view->callback_mouse_move = [&] (igl::opengl::glfw::Viewer& viewer, int mouse_x, int mouse_y) {
    Eigen::Vector3f pos(mouse_x, mouse_y, 0);
    Eigen::Matrix4f model = viewer.core().view;
    // Viewer is made for 3d, so we have to do math to figure out what the 2d mouse coordinates are
    Eigen::Vector3f unproj = igl::unproject(pos, model, viewer.core().proj, viewer.core().viewport);
    mouseX = unproj[0];
    mouseY = -unproj[1];
    return true;
  };

  view->callback_pre_draw = [&](igl::opengl::glfw::Viewer& viewer) -> bool {
    // Look for new data and redraw
    socket >> reply;
    std::vector<double> replyData = parseTrickResponse(split(reply, '\t'));

    if (replyData.size() <= 1) {
      return false;
    }

    table.clearMovingShapes();

    Eigen::Vector2d cueBallPos;
    int cueBallIndex = 0;

    for (int i = 0; i < numBalls; i++) {
      double inPlay = replyData[1+(i*3 + 2)];
      if (inPlay == 0) {
        continue;
      }
      std::vector<double> circleData = {replyData[1+(i*3)], replyData[1+(i*3 + 1)], radii[i]};
      Eigen::Vector3d circleColor;
      if (i == cueBallIndex) {
        circleColor = Eigen::Vector3d(1,1,1);
        cueBallPos = Eigen::Vector2d(replyData[1+(i*2)], replyData[1+(i*2 + 1)]);
      } else {
        circleColor = ball_colors[i % ball_colors.size()];
      }
      table.addShape(circleData, circleColor, false, CIRCLE, layer_BALL);
    }

    if (mousePressed) {
      // Draw the cue
      double cue_width = 0.03;
      Eigen::Vector2d cue_end(mouseX, mouseY);
      Eigen::Vector2d vec = (cue_end - cueBallPos).normalized();
      Eigen::Vector2d off1(-vec(1), vec(0));
      Eigen::Vector2d off2(vec(1), -vec(0));
      Eigen::Vector2d point1 = cue_end + (off1 * cue_width);
      Eigen::Vector2d point2 = cue_end + (off2 * cue_width);
      std::vector<double> triangleData = {cueBallPos(0), cueBallPos(1), point1(0), point1(1), point2(0), point2(1)};
      table.addShape(triangleData, Eigen::Vector3d(0, 0, 0), false, TRIANGLE, layer_CUE);
    }

    std::tie(V, F, C) = table.getMesh();
    
    viewer.data().clear();
    viewer.core().orthographic = true;
    viewer.data().show_lines = false;
    viewer.data().set_face_based(false);
    viewer.data().double_sided = true;
    viewer.core().is_animating = true;
    viewer.core().camera_zoom = 2;

    // Set mesh and colors to new positions
    viewer.data().set_mesh(V, F);
    viewer.data().set_colors(C);

    return false;
  };

  menu->callback_draw_viewer_menu = [&] () {
    ImGui::Text("Menu");
    if (ImGui::Button("Reset Cue Ball", ImVec2(-1, 0)))
    {
      std::string message = "dyn.table.resetCueBall() \n";
      socket << message;
    }
  };


  // Initial viewer setup
  view->core().orthographic = true;
  view->core().camera_zoom = 2;
  view->data().show_lines = false;
  view->data().set_face_based(false);
  view->data().double_sided = true;
  view->core().is_animating = true;

  // Plot the mesh
  view->data().set_mesh(V, F);
  view->data().set_colors(C);

  // Viewer is blocking, have to launch it in a separate thread

  // Need to get nBalls and positions every time
  socket << "trick.var_pause() \n";
  socket << "trick.var_add(\"dyn.table.numBalls\")\n";
  positionRequest = "";
  templateString = "trick.var_add(\"dyn.table.balls[%d][0].pos._x\")\ntrick.var_add(\"dyn.table.balls[%d][0].pos._y\")\ntrick.var_add(\"dyn.table.balls[%d][0].inPlay\")\n";
  for (int i = 0; i < numBalls; i++) {
    char buf[128];
    sprintf(buf, templateString, i, i, i);
    positionRequest += std::string(buf);
  }
  socket << positionRequest;
  socket << "trick.var_ascii() \n";
  socket << "trick.var_cycle(0.010) \n";
  socket << "trick.var_unpause() \n";
    
  view->launch();
}
