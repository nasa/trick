import os, sys, unittest, vtk, inspect

thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
virgo_dir=os.path.abspath(os.path.join(thisFileDir, '../'))
sys.path.append(virgo_dir)
tests_dir=os.path.join(virgo_dir, 'tests')

class VisualizableTestCase(unittest.TestCase):
    """
    A base class for VTK unit tests that supports optional visualization of the
    unit test. It is recommended you call self.vis() from the test's tearDown()
    function and this feature can be turned on in multiple ways:
      Globally for all tests:
        VIRGO_WRITE_TEST_IMAGES=1  # Set environment variable to save images
        VIRGO_VISUALIZE_TESTS=1    # Set environment variable to visualize scene
      For individual tests, simply define these before the end of the test
        self.visualize   = True
        self.save_images = True
      The environment variable VIRGO_BATCH_TESTS_OVERRIDE=1 will override all
      windowed mechanisms, ensuring a render window never appears which is useful
      when running in a CI system.
    """
    def __init__(self, methodName='runTest', *args, **kwargs):
        super().__init__(methodName, *args, **kwargs)
        self.renderer = vtk.vtkRenderer()
        self.save_images = os.environ.get('VIRGO_WRITE_TEST_IMAGES', '0') == '1'
        self.visualize = os.environ.get('VIRGO_VISUALIZE_TESTS', '0') == '1'
        self.batch_override = os.environ.get('VIRGO_BATCH_TESTS_OVERRIDE', '0') == '1'
        self.show_grid = False
        self.show_origin = True
        self.grid_axes = self.get_grid_axes()
        self.origin_axes = self.get_origin_axes()

    def get_grid_axes(self):
        # Create a vtkCubeAxesActor for tick marks
        cube_axes = vtk.vtkCubeAxesActor()
        cube_axes.SetBounds(-5, 5, -5, 5, -5, 5)  # Set bounds for the axes (10x10x10 cube)
        cube_axes.SetXLabelFormat("%.0f")  # Integer labels
        cube_axes.SetYLabelFormat("%.0f")
        cube_axes.SetZLabelFormat("%.0f")
        cube_axes.SetXTitle("x")
        cube_axes.SetYTitle("y")
        cube_axes.SetZTitle("z")
        cube_axes.SetTickLocationToBoth()  # Show ticks on both sides
        cube_axes.SetGridLineLocation(vtk.vtkCubeAxesActor.VTK_GRID_LINES_ALL)  # Optional: Show grid lines
        cube_axes.SetFlyModeToStaticEdges()  # Render axes on the edges of the bounds
        
        # Enable minor ticks (optional)
        cube_axes.SetDrawXGridlines(True)
        cube_axes.SetDrawYGridlines(True)
        cube_axes.SetDrawZGridlines(True)
        cube_axes.SetCamera(self.renderer.GetActiveCamera())  # Required for proper rendering
        return(cube_axes)

    def get_origin_axes(self):
      origin_axes = vtk.vtkAxesActor()
      origin_axes.SetTotalLength(5, 5, 5)  # Size of axes (x, y, z lengths)
      #origin_axes.SetShaftTypeToCylinder()  # Cylindrical shafts
      origin_axes.SetAxisLabels(True)  # Show x, y, z labels
      origin_axes.SetXAxisLabelText("x")
      origin_axes.SetYAxisLabelText("y")
      origin_axes.SetZAxisLabelText("z")
      return(origin_axes)
    
    def visualize_scene(self, actors, axis_length=5):
        """
        Optionally visualize a list of actors in a render window.
        :param actors: A single vtkActor or a list of vtkActors to visualize.

        Set the environment variable VIRGO_VISUALIZE_TESTS=1 to enable
        visualization during test runs for debugging.  When visualization is
        enabled, a render window will pop up and block until closed.
        """
        if not self.visualize or self.batch_override:
            return

        print(f"Visualizing. Exit window to continue.")
        
        if not isinstance(actors, list):
            actors = [actors]

        # Create a text actor for the test name itself, lower left corner
        test_name = vtk.vtkTextActor()
        test_name.GetTextProperty().SetFontFamilyToCourier()
        test_name.GetTextProperty().SetFontSize(12)
        test_name.GetTextProperty().SetColor(1, 1, 1)
        test_name.SetDisplayPosition(5, 5)
        test_name.SetInput(f"{self.__class__.__name__} {self._testMethodName}")
        
        # Set up the scene
        for actor in actors:
            self.renderer.AddActor(actor)
        self.renderer.AddActor(test_name)
        if self.show_origin:
            self.renderer.AddActor2D(self.origin_axes)
        if self.show_grid:
            self.renderer.AddActor(self.grid_axes)
        
        # Create render window and interactor
        render_window = vtk.vtkRenderWindow()
        render_window.AddRenderer(self.renderer)
        render_window.SetSize(800, 600)  # Optional: Set window size
        
        interactor = vtk.vtkRenderWindowInteractor()
        interactor.SetInteractorStyle(vtk.vtkInteractorStyleTrackballCamera())
        interactor.SetRenderWindow(render_window)
        
        # Render and start interaction (blocks until window is closed)
        self.renderer.ResetCamera()
        render_window.Render()
        interactor.Start()

    def save_scene_to_image(self, actors, filename="scene.png"):
        """
        Save the scene with the given actors to an image file without displaying a window.
        :param actors: A single vtkActor or a list of vtkActors to render.
        :param filename: Name of the output image file (e.g., 'scene.png').
        """
        if not self.save_images:
            return
        
        if not isinstance(actors, list):
            actors = [actors]
        
        # Set up the scene
        for actor in actors:
            self.renderer.AddActor(actor)
        self.renderer.AddActor2D(self.get_origin_axes())
        
        # Create render window with off-screen rendering
        render_window = vtk.vtkRenderWindow()
        render_window.AddRenderer(self.renderer)
        render_window.SetSize(800, 600)
        render_window.SetOffScreenRendering(1)  # Enable off-screen rendering
        
        # Render the scene
        render_window.Render()
        
        # Capture the image
        window_to_image = vtk.vtkWindowToImageFilter()
        window_to_image.SetInput(render_window)
        window_to_image.SetInputBufferTypeToRGBA()  # Use RGBA for better quality
        window_to_image.ReadFrontBufferOff()  # Read from back buffer
        window_to_image.Update()
        
        # Write to PNG file
        writer = vtk.vtkPNGWriter()
        writer.SetFileName(os.path.join(tests_dir, filename))
        writer.SetInputConnection(window_to_image.GetOutputPort())
        writer.Write()

    def vis(self):
        """
        Easy-to-call function that will save scenes to image or show them in an
        interactive window depending on what the user has requested. Does nothing
        if neither of those features is enabled by the user
        """
        self.save_scene_to_image(self.instance, filename=f".{self.__class__.__name__}_{self._testMethodName}.png")
        self.visualize_scene(self.instance)