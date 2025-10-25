#!/usr/bin/env python3.11
"""
VIRGO: Versatile Imaging and Rendering for Galactic Operations
A Practical, Analytical, and Hardworking 3D Visualization tool
leveraging python-VTK
"""
from VirgoActor import VirgoActor
from VirgoNode import VirgoSceneNode, VirgoSceneNodeVector
from VirgoSplash import VirgoSplash
from VirgoUtils import cprint

import os, sys, inspect, time, tempfile
import math, textwrap
import vtk
import numpy as np

thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))

# For TrickPy, the python module that can load trick data record produced files
sys.path.append(os.path.abspath(os.path.join(thisFileDir, '../')))

# Example of custom interactor style to override the default 'e' key behavior
class VirgoInteractorStyle(vtk.vtkInteractorStyleTrackballCamera):
    """
    Virgo Interactor Style which adds some capabilities on top of the VTK
    provided vtkInteractorStyleTrackballCamera, mostly to support keeping the
    camera at a relative position to a followed node as that node moves
    """
    def __init__(self, renderers=None):
        super().__init__()
        self.node = None  # Node this camera follows (if defined)
        self.sun = None   # Sun sphere actor to update
        self.renderers = renderers
        self.AddObserver("CharEvent", self.onChar)
        self.AddObserver(vtk.vtkCommand.EndInteractionEvent, self.StoreRelativeCameraInfo, 0.0)
        self.relative_offset = None  # Positional offset between camera and followed actor
        self.view_up = None    # Camera GetViewUp() direction when self.realtive_offset was stored

    def set_node(self, node):
        self.node = node


    def get_renderers(self):
        return(self.renderers)

    def set_renderers(self, renderers):
        self.renderers = renderers

    # Override base class onChar to ignore the 'e' key
    def onChar(self, obj, event):
        key = self.GetInteractor().GetKeySym()
        if key != 'e':  # Ignore 'e' key to prevent exit
            super().OnChar()

    def StoreRelativeCameraInfo(self, caller, event):
        '''
        Store off the relative offset between the followed node and the camera
        in self.relative_offset. This is called as a callback tied to
        vtk.vtkCommand.EndInteractionEvent which lets us store the camera information
        relative to self.node when the interaction with the mouse completes

        TODO: I'm not convinced this callback needs to live in the interactor,
            it might be better placed in the bigger Controller class especially
            since we aren't really overriding any methods of
            vtkInteractorStyleTrackballCamera in this approach
        '''
        #print(f"DEBUG: Entered StoreRelativeCameraInfo with event {event}")
        if not self.node or not self.renderers:
            return
        # Update relative offset after user interaction
        camera = self.renderers['foreground'].GetActiveCamera()

        node_pos =np.array(self.node.get_world_position())

        # Store off the relative offset of camera
        self.relative_offset = np.array([camera.GetPosition()[i] - node_pos[i] for i in range(3)])
        #print(f"DEBUG: STORING camera relative offset: {self.relative_offset}")
        self.view_up = camera.GetViewUp()
        #print(f"DEBUG: Finished StoreRelativeCameraInfo with event {event}")

    def GetRelativeCameraInfo(self, info):
        """
        Get the relative offset between the followed actor and the camera
        """
        #print(f"DEBUG: Getting relative_offset: {self.relative_offset}")
        if   info == 'position':
            return self.relative_offset
        elif info == 'view_up':
            return self.view_up
        else:
            return None

    def ClearRelativeCameraInfo(self):
        """
        Clear relative camera info
        """
        self.relative_offset = None
        self.view_up = None

class VirgoControlCenter:
    """
    The main control center class for Virgo Data playback which provides:
      World time: Authoritative scene time
      Navigation: Play/Pause and stepping forward/backward through time and
        playback speed
      Camera: Configuration and node-following capability
      HUD: Heads-up display text bordering the scene
      Picking: The ability to click on objects in the scene to get more info
        about them
    """
    def __init__(self, renderers, render_window, interactor, scene,
                 world_time=0.0, images_dir=None):
        """
        Constructor
        """
        # Current scene world time (not necessarily aligned with sim data time)
        self.world_time = self.world_time_start = world_time 
        self.wallclock_time = time.time()  # Actual wall clock time in real life
        self.max_sim_time = 0.0            # highest sim time across all dr groups
        self.images_dir = images_dir
        if images_dir == None:
            self.images_dir = os.path.expanduser("~/Desktop")
        self.mode = 'PLAYING'              # 'PAUSED' or 'PLAYING'
        self.fs = 14      # font size
        self.renderers = renderers
        self.render_window = render_window
        self.cameras = {}
        # One camera per renderer
        for r in self.renderers:
            self.cameras[r] = renderers[r].GetActiveCamera()
        self.camera_follows = None  # if the camera should follow an actor, this is the one
        # TODO this hardcoded value assummes the actor is HUGE
        self.camera_follow_offset = None
        self.interactor = interactor
        self.scene = scene
        self.skybox = self.create_skybox()
        self.actors = {}
        self.vectors={}
        self.frames={}
        self.trail_actors = {}
        self.text_actors={}

        self.help = False  # Display help message when true
        self.picker = vtk.vtkCellPicker()
        self.interactor.SetPicker(self.picker)
        self.picked_actor      = None # Currently picked actor or None if not
        self.near_clipping_plane_tolerance = 0.00005
        self.picker_tolerance = 5e-7
        self.last_picked_actor = None
        self.original_colors = {}  # Store original colors for actors

        # TODO: do we need to check this is smaller than minimum delta t in logged data?
        self.callback_rate = 50 # Frame rate for timer callback (millisec)
        # self.dt: Increment time by this amount every timer update (sec)
        self.default_dt = self.dt = self.callback_rate / 1000.0
        self.frame_rate = 1/self.dt
        self.playback_speed = 1.0  # Speed of playback
        # Default playback speeds
        self.available_speeds = [1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0]
        self.huge = 1.0e30   # A huge floating point number used for finding smallest values
        self._initialized = False
        self.sun_light = None   # vtkLight source of the sun
        self.sun_actor = None   # Sphere used to represent sun in the distance
        self.sun_direction = [1.0, 0.0, 0.0]  # Default world vector pointing towards sun
        self.sun_distance = None   # Actual distance to place sun away from camera
        self.sun_driven_by = None  # Node sun vector is driven by
        self.lighting_mode = 'headlight'  # Default lighting mode of the scene
        self.bright_ambient = 0.7  # Default ambient light on all nodes in 'ultrabright'
        self.dark_ambient = 0.1    # Default ambient light on all nodes in 'realistic'
        self.camera_pass = None    # Used for rendering in 'realistic' lighting mode
        self.verbosity = 1

    def initialize(self):
        """
        Set up the scene by:
          * Adding all actors to the renderer
          * Initializing the camera
          * Setting up lights
        """
        if not os.path.exists(self.images_dir):
          try:
            os.makedirs(self.images_dir, exist_ok=True)
          except Exception as e:
            print("ERROR: Unable to create {self.images_dir}:\n{e}\ntaking "
                  "pictures will not work.")

        if not self.actors:
          msg = (f"ERROR: No actors found in {__class__} initialize() function. ")
          raise RuntimeError (msg)

        for r in self.renderers:
          self.render_window.AddRenderer(self.renderers[r])

        self._determine_max_sim_time()
        self.update_nodes()
        self.text_actors['mode'] = self.create_text_actor()
        self.text_actors['picked'] = self.create_text_actor(pos=[10,10])
        self.text_actors['time'] = self.create_text_actor()
        self.text_actors['help'] = self.create_text_actor()
        self.text_actors['camera'] = self.create_text_actor()
        self.text_actors['lighting'] = self.create_text_actor()
        self.text_actors['version'] = self.create_text_actor()

        # TODO these options also need to go in the verifier
        if 'start_mode' in self.scene:
            self.mode = self.scene['start_mode'] # Set start mode
        if 'starfield' in self.scene:
            self.skybox.SetVisibility(bool(self.scene['starfield']))
        if 'playback_speeds' in self.scene:
            self.available_speeds = self.scene['playback_speeds']
        if 'playback_speed' in self.scene:
            # TODO verify self.playback_speed is in self.available_speeds
            self.playback_speed = self.scene['playback_speed']
            self.dt = self.dt * self.playback_speed
        if 'pick' in self.scene and self.scene['pick'] in self.actors:
            self.picked_actor = self.actors[self.scene['pick']]
            self.nodes[self.picked_actor.name].highlight_on()
            self.last_picked_actor = self.picked_actor

        # Assign the silhoutte_polydata to a camera
        # for all actors with highlightable sillhoutte
        for n in self.nodes:
            sp = self.nodes[n].silhouette_polydata 
            if sp != None:
                sp.SetCamera(self.cameras['foreground'])
        # Add actors in scene
        for n in self.nodes:
            # Only add root nodes to the renderer, all children come along
            # automatically
            if self.nodes[n].parent == None:
                #import pdb; pdb.set_trace()
                self.renderers['foreground'].AddActor(self.nodes[n].assembly)
        for a in self.trail_actors:
            self.renderers['foreground'].AddActor(self.trail_actors[a])
        for t in self.text_actors:
            self.renderers['foreground'].AddActor(self.text_actors[t])

        self.init_camera()
        self.init_picker()
        self.init_lighting()
        if self.sun_actor:
            self.renderers['background'].AddActor(self.sun_actor)
        self._initialized = True

    def _determine_max_sim_time(self):
        """
        Store off the highest simulation time known across all non-static
        nodes/actors.
        """
        for n in self.nodes:
            # Skip any static actors as they dont have self._times
            if self.nodes[n].is_static() or self.nodes[n].data_source == None:
                continue
            last_time_for_actor = self.nodes[n].data_source.get_last_time()
            if last_time_for_actor > self.max_sim_time:
                self.max_sim_time = last_time_for_actor

    def is_initialized(self):
        return self._initialized

    def get_verbosity(self):
        return self.verbosity

    def set_verbosity(self, value):
        if not isinstance(value, int):
            print("ERROR: verbosity must be an integer")
        else:
            self.verbosity = value

    def set_actors(self, actors_dict):
        self.actors = actors_dict

    def set_nodes(self, nodes_dict):
        self.nodes = nodes_dict

    def set_vectors(self, vectors_dict):
        self.vectors = vectors_dict

    def set_frames(self, frames_dict):
        self.frames = frames_dict

    def set_trail_actors(self, trail_actors_dict):
        self.trail_actors = trail_actors_dict

    def create_text_actor(self, pos=[0, 0]):
        text = vtk.vtkTextActor()
        text.GetTextProperty().SetFontFamilyToCourier()
        text.GetTextProperty().SetFontSize(self.fs)
        text.GetTextProperty().SetColor(1, 1, 1)
        text.SetDisplayPosition(pos[0], pos[1])
        return text

    def update_nodes(self):
        '''
        Move actors into position based on world time compared to data
        '''
        for n in self.nodes:
            self.nodes[n].update(self.world_time)

    def reset_trails(self):
        '''
        Reset all actor trails to no points, no lines
        '''
        for n in self.nodes:
            self.nodes[n].reset_trail()

    def update_scene(self):

        if self.mode == 'PLAYING':
          if self.world_time <= self.max_sim_time:
            self.world_time += self.dt
            self.update_nodes()
            if self.camera_follows:
                self.camera_follow(self.camera_follows)
          else:
             self.world_time = self.world_time_start
             self.reset_trails()

        self.position_sun_light()
        self.position_sun_actor(None, None)
        self.configure_hud()
        # This auto-adjusts clipping based on visible actors
        self.renderers['foreground'].ResetCameraClippingRange()
        # The end of the main update loop, render the image
        self.render_window.Render()

    def on_timer(self, caller, event):
        """
        Main callback for executing the scene update loop
        """
        self.wallclock_time = time.time()  # Actual wall clock time in real life
        self.update_scene()

        time_one_frame_took = time.time() - self.wallclock_time
        if time_one_frame_took > self.dt:
            print(f"WARNING: Frame took {time_one_frame_took} sec to complete which is "
                  f"larger than self.dt ({self.dt}). Playback rate may not be accurate")

    def configure_hud(self):
        """
        Configure the heads-up-display in preparation for rendering.
        Does not render.
        """
        hud_padding = 20 # pixels
        window_width, window_height  = self.render_window.GetSize()
        ############################################################################
        # Picked Actor (or last picked if nothing picked) information in bottom left
        ############################################################################
        actor = self.picked_actor
        if actor:
            text=""
            parent=""
            node_curr_time=""
            label="Node driven position"
            # TODO: not sure if the distinction between current_position and
            # world_position is clear enough here.
            node = self.nodes[actor.name]
            if not node.is_static():
                node_curr_time=f" @ t={node.data_source.get_current_time()}"
            if node.parent:
                parent = f" (parent: {node.parent.name})"
            pos = node.get_current_position()
            if not pos:
                label="Node world position "
                pos = self.nodes[actor.name].get_world_position()
            if isinstance(node, VirgoSceneNodeVector):
                label="vector tip position "
            name = actor.name
            text+=f"{name}{parent}\n {label}: {pos[0]:<10.5f}, {pos[1]:<10.5f}, {pos[2]:<10.5f} units {node_curr_time}"
            if node.parent:
                label = "Node  local position"
                pos = self.nodes[actor.name].get_local_position()
                text+=f"\n {label}: {pos[0]:<10.5f}, {pos[1]:<10.5f}, {pos[2]:<10.5f} units {node_curr_time}"
                label = "Actor local position"
                pos = actor.GetPosition()
                text+=f"\n {label}: {pos[0]:<10.5f}, {pos[1]:<10.5f}, {pos[2]:<10.5f} units {node_curr_time}"
            self.text_actors['picked'].SetInput(text)
        else:
            self.text_actors['picked'].SetInput("")

        ############################################################################
        # Mode displayed in top right
        ############################################################################
        top_right=f"{self.mode} {self.playback_speed}X"
        top_right+=f"\nNCPT:{self.near_clipping_plane_tolerance:.2e}"
        top_right+=f"\nPT:  {self.picker_tolerance:.2e}"
        self.text_actors['mode'].SetInput(top_right)
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['mode'].GetBoundingBox(self.renderers['foreground'], bounds)
        text_width = bounds[1] - bounds[0] + 1  # Width in pixels
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        # Calculate position for bottom-right corner with padding
        x_pos = window_width - text_width - hud_padding  # Right edge minus width
        y_pos = window_height - text_height - hud_padding  # Bottom edge (20 pixels from bottom)
        self.text_actors['mode'].SetPosition(x_pos, y_pos)
        
        ############################################################################
        # Time in top left
        ############################################################################
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['time'].GetBoundingBox(self.renderers['foreground'], bounds)
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        y_pos = window_height - text_height - hud_padding
        self.text_actors['time'].SetPosition(hud_padding, y_pos)
        percent_complete = self.world_time / self.max_sim_time * 100
        self.text_actors['time'].SetInput(
            f"World time: {self.world_time:<10.5f} / {self.max_sim_time} sec [{percent_complete:<4.2f} %]"
            )
        ############################################################################
        # Help message in bottom right (if active)
        ############################################################################
        if self.help == True:
            #self.text_actors['help'].SetDisplayPosition(window_width - 600, 40)
            self.text_actors['help'].SetInput(
                f"\nMOUSE"
                f"\n L-click drag: Rotate"
                f"\n Shift+L-click: Pan"
                f"\n Ctrl+L-click: Roll"
                f"\n Scroll wheel: Dolly In/Out"
                f"\n R-click: Pick Actor"
                f"\n"
                f"\nKEYBOARD"
                f"\n SPACE: Pause/Play"
                f"\n p: Take Picture"
                f"\n s: Cycle playback speeds"
                f"\n t: Toggle trails"
                f"\n <- -> : Step back/forward in time"
                f"\n  -  + : Adjust HUD text size"
                f"\n a: Toggle Axes Visibility"
                f"\n c: Toggle camera free/follow-picked"
                f"\n l: Toggle Lighting Mode"
                f"\n L: Toggle Node Label Visibility"
                f"\n v: Print picked node info in terminal"
                f"\n BackSpace: Toggle starfield (experimental)"
                f"\n h: Toggle this help message"
                f"\n j/k: Near Plane Clipping Tolerance"
                f"\n J/K: Picker Tolerance"
                f"\n Q: Quit"
                )
            self.text_actors['help'].GetBoundingBox(self.renderers['foreground'], bounds)
            text_width = bounds[1] - bounds[0] + 1  # Width in pixels
            text_height = bounds[3] - bounds[2] + 1  # Height in pixels
            # Calculate position for bottom-right corner with 20-pixel padding
            x_pos = window_width - text_width - hud_padding    # Right edge minus width
            y_pos = hud_padding  # Bottom edge (20 pixels from bottom)
            self.text_actors['help'].SetPosition(x_pos, y_pos)

        else:
            self.text_actors['help'].SetInput("")

        ############################################################################
        # Camera and lighting info displayed in top center-right-ish
        ############################################################################
        camera_info = f"Camera: Follow {self.camera_follows.name}" if self.camera_follows else "Camera: Free" 
        self.text_actors['camera'].SetInput(f"{camera_info}")
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['camera'].GetBoundingBox(self.renderers['foreground'], bounds)
        text_width = bounds[1] - bounds[0] + 1  # Width in pixels
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        # Calculate position for bottom-right corner with padding
        x_pos = (window_width - text_width - hud_padding)/1.3  # 2/3ish the way over on right side
        y_pos = window_height - text_height - hud_padding  # Top edge (20 pixels from bottom)
        self.text_actors['camera'].SetPosition(x_pos, y_pos)

        lighting_info = f"Lighting: {self.lighting_mode}" 
        self.text_actors['lighting'].SetInput(f"{lighting_info}")
        self.text_actors['lighting'].SetPosition(x_pos, y_pos-text_height)

        ############################################################################
        # Version
        ############################################################################
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['picked'].GetBoundingBox(self.renderers['foreground'], bounds)
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        self.text_actors['version'].GetTextProperty().SetColor(0.7, 0.7, 0.7)
        self.text_actors['version'].SetPosition(hud_padding, text_height + hud_padding)
        self.text_actors['version'].SetInput(f"VIRGO alpha version")

    def focus_camera_on(self, actor):
        """
        Point the camera at the given actor
        Assumes actor is positioned with matrix data, not position data
        """
        matrix = actor.GetMatrix()
        pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
        self.cameras['foreground'].SetFocalPoint(pos[0], pos[1], pos[2])

    def automatic_camera_offset(self, node, distance_factor=20.0):
        """
        If no initial offset given, compute it from the bounding box of the actor
        """
        # Get the bounding box of the actor
        bounds = node.actor.GetBounds()
        # Bounds are returned as (xmin, xmax, ymin, ymax, zmin, zmax)
        center = [(bounds[0] + bounds[1]) / 2.0,  # x-center
                  (bounds[2] + bounds[3]) / 2.0,  # y-center
                  (bounds[4] + bounds[5]) / 2.0]  # z-center
        # Calculate the size of the bounding box
        size = [bounds[1] - bounds[0],  # x-size
                bounds[3] - bounds[2],  # y-size
                bounds[5] - bounds[4]]  # z-size
        # Determine an offset distance based on the bounding box size
        # For example, place the camera at a distance of 5 times the maximum dimension
        max_dimension = max(size)
        offset_distance = max_dimension * distance_factor
        self.camera_follow_offset = np.array([offset_distance, offset_distance/2.0, 0.0])
        #print(f"DEBUG: AUTOMATIC self.camera_follow_offset is {self.camera_follow_offset}")

    def camera_follow(self, node, initial_offset=None):
        """
        Move the camera to follow the given node

        Params:
          initial_offset (list or None): Initial Position vector offset for
            camera following node. If not specified, one is calculated
            automatically
        """
        #import pdb; pdb.set_trace()
        # Store off the node the camera follows
        self.camera_follows=node
        # Tell the interactor style which node to follow
        intstyle = self.interactor.GetInteractorStyle()
        intstyle.set_node(self.camera_follows)
        # If the camera has never followed before, set the renderer as well
        if not intstyle.get_renderers():
            intstyle.set_renderers(self.renderers['foreground'])

        # Ask the interactor for the latest camera position
        self.camera_follow_offset = self.interactor.GetInteractorStyle().GetRelativeCameraInfo('position')

        # If this is None, the interactor hasn't run yet so we need to define
        # the camera offset for the followed node and move the camera to it
        # TODO: this is really a "first camera follow init" thing, maybe it
        # belongs in it's own function
        if self.camera_follow_offset is None:
            if initial_offset:
                self.camera_follow_offset = np.array(initial_offset)
            else:
                self.automatic_camera_offset(node)
            pos =np.array(node.get_world_position())
            camera_pos = pos + self.camera_follow_offset
            #print(f"DEBUG: INIT moving self.cameras['foreground'] to {camera_pos}")
            #print(f"DEBUG: INIT setting self.cameras['foreground'] focal to {pos}")
            self.cameras['foreground'].SetPosition(camera_pos[0], camera_pos[1], camera_pos[2])
            self.cameras['foreground'].SetFocalPoint(pos[0], pos[1], pos[2])
            # Store this initial offset in the VirgoInteractorStyle by calling the
            # callback which stores the relative camera offset
            self.interactor.GetInteractorStyle().StoreRelativeCameraInfo(None, None)

        pos = np.array(node.get_world_position())
        #print(f"DEBUG: self.camera_follow_offset is {self.camera_follow_offset}")
        camera_pos = pos + self.camera_follow_offset
        #print(f"DEBUG: moving self.cameras['foreground'] to {camera_pos}")
        #print(f"DEBUG: setting self.cameras['foreground'] focal to {pos}")
        # Set camera position
        self.cameras['foreground'].SetPosition(camera_pos[0], camera_pos[1], camera_pos[2])
        # Set focal point to spacecraft's center
        self.cameras['foreground'].SetFocalPoint(pos[0], pos[1], pos[2])
        # Ask the interactor for the stored off view_up direction from end of interaction
        view_up = self.interactor.GetInteractorStyle().GetRelativeCameraInfo('view_up')
        # If view_up isn't defined, default it to "positive Z is up"
        if not view_up:
            view_up = (0.0, 0.0, 1.0)
        self.cameras['foreground'].SetViewUp(view_up)

    def on_right_click(self, obj, event):
        """
        Callback for right click actions. This currently provides "picking",
        assigning self.picked_actor to the actor the mouse clicked on,
        highlighting it with a silhoutte outline in the scene, and displaying
        information about it in the HUD
        """
        click_pos = self.interactor.GetEventPosition()
        self.picker.Pick(click_pos[0], click_pos[1], 0, self.renderers['foreground'])
        self.picked_actor = self.get_actor_from_picker()

        if self.picked_actor and self.picked_actor in self.actors.values():
            self.configure_hud()
            # TODO this functionality is broken and I'm not sure we even want it
            # as the user experience is weird
            #self.focus_camera_on(self.picked_actor)
        if self.last_picked_actor:
            self.nodes[self.last_picked_actor.name].highlight_off()

        if self.picked_actor:
            self.nodes[self.picked_actor.name].highlight_on()
            # Un-highlight color of the previously picked actor
            if self.last_picked_actor and self.last_picked_actor != self.picked_actor:
                self.nodes[self.last_picked_actor.name].highlight_off()
            
            # Update last picked actor
            self.last_picked_actor = self.picked_actor
            
        self.render_window.Render()

    def get_actor_from_picker(self):
        """
        Return the picked vtkActor if any, using vtkAssemblyPath inspection.
        Note this currently uses the more computationally intensive
          self.picker = vtk.vtkCellPicker()
        But this was the only picker approach I could get to work with the
        vtkAssembly configuration we are using.
        """
        actor = self.picker.GetActor()
        if actor:
            return actor
    
        path = self.picker.GetPath()
        if path:
            last = path.GetLastNode()
            if last:
                vp = last.GetViewProp()
                if vp and vp.IsA("vtkActor"):
                    return vp
        return None

    def toggle_node_labels(self):
        for n in self.nodes:
            if self.nodes[n].are_labels_visible():
                self.nodes[n].hide_labels()
            else:
                self.nodes[n].show_labels()

    def toggle_lighting_modes(self):
        # TODO: this should iterate over a dict of self.lighting_modes
        # then check for that mode being in self.scene in init_lighting
        if self.lighting_mode == 'realistic':
            self.lighting_mode = 'headlight'
        elif self.lighting_mode == 'headlight':
            self.lighting_mode = 'ultrabright'
        else:
            self.lighting_mode = 'realistic'
        self.set_lighting_mode(self.lighting_mode)
        #print(f"DEBUG: self.renderers['foreground'].GetPass() is {self.renderers['foreground'].GetPass()}")

    def set_lighting_mode(self, mode):
        all_lights = self.renderers['foreground'].GetLights()
        num_lights = all_lights.GetNumberOfItems()
        if mode == 'realistic':
            print("(Somewhat) realistic lighting on")
            self.renderers['foreground'].RemoveAllLights()
            if self.sun_light and self.sun_light not in all_lights:
                self.renderers['foreground'].AddLight(self.sun_light)
            self.renderers['foreground'].SetPass(self.camera_pass)
            #self.renderer.ReleaseGraphicsResources(self.render_window)
            self.set_ambient_all_nodes(self.dark_ambient)
        elif mode == 'headlight':
            print("Headlight lighting on")
            #self.renderer.RemoveAllLights()
            if self.sun_light and self.sun_light in all_lights:
                self.renderers['foreground'].RemoveLight(self.sun_light)
            headlight = vtk.vtkLight()
            headlight.SetLightTypeToHeadlight()
            headlight.SetIntensity(1.0)
            self.renderers['foreground'].AddLight(headlight)
            self.renderers['foreground'].SetPass(None)
        elif mode == 'ultrabright':
            print("Ultrabright lighting on")
            #self.renderers['foreground'].RemoveAllLights()
            if self.sun_light in all_lights:
                self.renderers['foreground'].RemoveLight(self.sun_light)
            self.renderers['foreground'].SetPass(None)
            #self.renderers['foreground'].ReleaseGraphicsResources(self.render_window)
            self.set_ambient_all_nodes(self.bright_ambient)
        #print(f"DEBUG: Number of lights in scene: {num_lights}")

    def set_ambient_all_nodes(self, value):
        for n in self.nodes:
            if self.nodes[n].actor:
                self.nodes[n].actor.GetProperty().SetAmbient(value)

    def toggle_trails(self):
        for n in self.nodes:
            if self.nodes[n].is_trail_visible():
                self.nodes[n].hide_trail()
            else:
                self.nodes[n].show_trail()

    def take_picture(self, filename=None):
        if filename == None:
            filename = os.path.join(self.images_dir,
              (f"virgo_pic_{time.strftime('%Y%m%d_%H%M%S')}.png"))
        self.save_frame(filename=filename)
        print(f"Picture taken: {filename}")

    def toggle_axes(self):
        for n in self.nodes:
            if self.nodes[n].are_axes_visible():
                self.nodes[n].hide_axes()
                if self.verbosity > 1:
                    print(f'Hiding axes for {n}')
            else:
                self.nodes[n].show_axes()
                if self.verbosity > 1:
                    print(f'Showing axes for {n}')

    def toggle_camera_modes(self):
        # If we're already following a node, free the camera
        if self.camera_follows:
            if self.picked_actor:
                self.camera_follow( self.nodes[self.picked_actor.name] )
            else:
                self.camera_follows = None
        # Otherwise change the node we follow to the picked node
        elif self.last_picked_actor:
            self.camera_follow( self.nodes[self.last_picked_actor.name] )

    def fontsize(self, direction='up'):
        """
        Increment/decrement fontsize of all text actors given the direction
        """
        increment = 2 if direction == 'up' else -2
        for t in self.text_actors:
            self.fs = self.text_actors[t].GetTextProperty().GetFontSize() + increment
            self.text_actors[t].GetTextProperty().SetFontSize(self.fs)
        # Adjust the fontsize inside the actor instances
        # TODO this should probably be done with a setter rather than straight assignment
        # Also this was intended for axes label size but I never quite got that to work
        for a in self.actors:
            self.actors[a].fs = self.fs

    def save_frame(self, filename):
        """
        Save the state of self.render_window to a png file named filename
        """
        if not self.render_window:
            msg = (f"ERROR: Cannot save frame to {filename},"
                   " self.render_window is None!")
            raise RuntimeError (msg)
        w2i = vtk.vtkWindowToImageFilter()
        w2i.SetInput(self.render_window)
        w2i.Update()
        writer = vtk.vtkPNGWriter()
        writer.SetFileName(filename)
        writer.SetInputConnection(w2i.GetOutputPort())
        writer.Write()
        if self.verbosity > 3:
            print(f"Saved image: {filename}")

    def orient_labels_to_camera(self, caller, event):
        # This was an attempt to get labels to face the camera but I never got
        # it working right. We should have all the information we need to orient
        # each label in the direction of the camera. We have the world position
        # and orientation of the camera and the node the label lives inside keeps
        # that same position/orientation information inside self.local_transform
        # I think we need this function to take the world orientation of the label's
        # normal and point it in the direction of the camera relative to the label

        #node = self.nodes['satellite']
        #label = node.get_label('name')
        #mat = vtk.vtkMatrix4x4()
        #node.assembly.GetMatrix(mat)  # fill mat with the assembly's world matrix
        #world = [0.0, 0.0, 0.0, 0.0]
        #local_offset = [node.get_local_position()[0], node.get_local_position()[1],node.get_local_position()[2], 1.0]
        #mat.MultiplyPoint(local_offset, world)
        ## world is homogeneous; set follower to world xyz
        #label.get_follower().SetPosition(world[0], world[1], world[2])
        pass

    def on_key_press(self, caller, event):
        key = self.interactor.GetKeySym()
        #print(f'DEBUG: Key pressed: {key}')
        if key == 'equal':
            self.fontsize('up')
        if key == 'minus':
            pass
            self.fontsize('down')
        if key == 'a':
            # Turn actor axes on/off
            self.toggle_axes()
        if key == 'L':
            # Turn actor axes on/off
            self.toggle_node_labels()
        if key == 'l':
            # Turn actor axes on/off
            self.toggle_lighting_modes()
        if key == "p":
            self.take_picture()
        if key == "t":
            self.toggle_trails()
        if key == "s":
            # Cycle through the available playback speeds
            self.cycle_playback_speed()
        if key == "h":
            self.help = True if self.help == False else False
        if key == "BackSpace":
            # Toggle skybox visibility
            self.skybox.SetVisibility(False) if self.skybox.GetVisibility() else self.skybox.SetVisibility(True)
        if key == "v":
            # Dump the info about the picked actor to the terminal
            if self.picked_actor:
                print(self.text_actors['picked'].GetInput())
        if key == "V":
            # Verbosely print state of all non-text actors
            for n in self.nodes:
                # Call report() on root nodes only since they recurse
                if self.nodes[n].parent == None:
                    self.nodes[n].report()
        if key == "j":
            self.decrease_near_clipping_plane_tolerance()
        if key == "k":
            self.increase_near_clipping_plane_tolerance()
        if key == "J":
            self.decrease_picker_tolerance()
        if key == "K":
            self.increase_picker_tolerance()
        if key == "c":
            self.toggle_camera_modes()
            if self.verbosity > 1:
                self.camera_report()
        if key == "space":
            self.mode = 'PLAYING' if self.mode == 'PAUSED' else 'PAUSED'
            self.text_actors['mode'].SetInput(self.mode)
        if key == "Left" or key =='comma':
            self.mode = 'PAUSED'
            self.decrement_time()
            self.update_nodes()
            if self.camera_follows:
                self.camera_follow(self.camera_follows)
        if key == "Right" or key == 'period':
            self.mode = 'PAUSED'
            self.increment_time()
            self.update_nodes()
            if self.camera_follows:
                self.camera_follow(self.camera_follows)
        self.update_scene()

    def cycle_playback_speed(self):
        """
        Cycle the playback speed by iterating forward in the list of 
        self.available_speeds
        """
        # Find our current speed index in the list of available speeds
        idx = self.available_speeds.index(self.playback_speed)
        if idx < len(self.available_speeds) - 1:
            self.playback_speed = self.available_speeds[idx+1]
        else:
            self.playback_speed = self.available_speeds[0]
        # Adjust self.dt so the scene runs based on new speed
        self.dt = self.default_dt * self.playback_speed

    def decrement_time(self):
        """
        Decrement world time by the minimum resolution across all actors logged data

        Across all nodes with a data source, find the time before (to the left of)
        self.world_time that is closest to self.world_time and set self.world_time
        to that value

        TODO: This has not been tested with multiple data record groups logging at
        specific rates.
        """
        largest_time = - self.huge # Negative Huge
        for n in self.nodes:
            if self.nodes[n].data_source == None:
                continue
            pt = self.nodes[n].data_source.get_previous_time(self.world_time)
            if pt is None:
                continue
            if pt > largest_time:
                largest_time = pt
        # Decrease the world time
        if largest_time > - self.huge:
            #print(f"Setting (decrement) world_time to {largest_time}")
            self.world_time = largest_time
            for n in self.nodes:
                if self.nodes[n].data_source == None:
                    return
                self.actors[n].data_source.set_current_time(self.world_time, strategy='closest')
                
    def increment_time(self):
        """
        Increment world time by the minimum resolution across all actors logged data

        Across all nodes with a data source, find the time after (to the right of)
        self.world_time that is closest to self.world_time and set self.world_time
        to that value

        TODO: This has not been tested with multiple data record groups logging at
        specific rates.
        """
        smallest_time = self.huge # Huge
        #import pdb; pdb.set_trace()
        for n in self.nodes:
            if self.nodes[n].data_source == None:
                continue
            nt = self.nodes[n].data_source.get_next_time(self.world_time)
            if nt is None:
                continue
            if nt < smallest_time:
                smallest_time = nt
        # Decrease the world time
        if smallest_time < self.huge:
            #print(f"Setting (increment) world_time to {smallest_time}")
            self.world_time = smallest_time
            for n in self.nodes:
                if self.nodes[n].data_source == None:
                    continue
                self.nodes[n].data_source.set_current_time(self.world_time, strategy='closest')

    # Put the camera in a nice position to see docking
    def init_camera(self):
        """
        Set the initial camera position

        TODO: The hardcoded values should probably be either removed entirely
        and replaced with whatever 'r' keypress is doing via the default
        behavior of vtk.vtkInteractorStyleTrackballCamera, or a better approach
        which places the camera automatically based on what's in the scene, or
        based on camera init information read from the YAML file

        TODO: The camera follow capability is locked to a single actor here, we
        should consider the ability to change which actor is followed during
        runtime. We could consider using self.picked_actor to drive the camera
        follow mechanism.

        TODO: shouldn't we sync_cameras() here?
        """
        # Default camera if the scene has given us no information on it
        self.cameras['foreground'].SetPosition(-10.0, 0.0, 0.0)
        self.cameras['foreground'].SetFocalPoint(0, 0, 0)
        self.cameras['foreground'].SetViewUp(0.0, 0.0, 1.0)  # Z is up
        # TODO not sure these go best here, also do we want a user to be able
        # to change this in the yaml file?
        # NOTE this skybox clipping range appears optimal and I'm not sure
        # why, other ranges result in the skybox becoming erratic in scenes
        # with inertial sized bodies and distances
        self.cameras['skybox'].SetClippingRange(1e6, 1e10)
        self.cameras['background'].SetClippingRange(1e10, 1e17)

        # If the camera is setup to follow a node/actor, save that node in
        # self.camera_follows and pass that node into the Interactor so
        # it can track camera information relative to an actor
        if 'camera' in self.scene:
          initial_camera_pos = None
          if 'position' in self.scene['camera']:
            initial_camera_pos = self.scene['camera']['position']
            self.cameras['foreground'].SetPosition(initial_camera_pos )
          if 'focal_point' in self.scene['camera']:
            self.cameras['foreground'].SetFocalPoint(self.scene['camera']['focal_point'])
          if 'view_up' in self.scene['camera']:
            self.cameras['foreground'].SetViewUp(self.scene['camera']['view_up'])
          if 'follow' in self.scene['camera']:
            for node in self.nodes:
              if self.nodes[node].name == self.scene['camera']['follow']:
                self.camera_follows = self.nodes[node]
                self.camera_follow(self.camera_follows, initial_offset=initial_camera_pos)
                # Follow node found, stop searching for it
                break
            if self.camera_follows == None:
              msg = (f"ERROR: Camera cannot follow {self.scene['camera']['follow']}, actor/node not found!")
              raise RuntimeError (msg)

        # NearClippingPlaneTolerance IS CRITICAL, SMALLER NUMBERS (0.00001) MAKE
        # THE BACKGROUND SKYBOX STARS WIGGLE BUT LARGER NUMBERS (0.0001) MAKE
        # SMALLER ACTORS NOT VISIBLE DUE TO CLIPPING.
        if 'camera' in self.scene and 'near_clipping_plane_tolerance' in self.scene['camera']:
            self.near_clipping_plane_tolerance = float(self.scene['camera']['near_clipping_plane_tolerance'])
        self.renderers['foreground'].SetNearClippingPlaneTolerance(self.near_clipping_plane_tolerance)

    def increase_near_clipping_plane_tolerance(self, multiplier=2.0):
        self.near_clipping_plane_tolerance *= multiplier
        self.renderers['foreground'].SetNearClippingPlaneTolerance(self.near_clipping_plane_tolerance)

    def decrease_near_clipping_plane_tolerance(self, multiplier=2.0):
        self.near_clipping_plane_tolerance /= multiplier
        self.picker.SetTolerance(self.picker_tolerance)

    def increase_picker_tolerance(self, multiplier=2.0):
        self.picker_tolerance *= multiplier
        self.picker.SetTolerance(self.picker_tolerance)

    def decrease_picker_tolerance(self, multiplier=2.0):
        self.picker_tolerance /= multiplier
        self.picker.SetTolerance(self.picker_tolerance)

    def init_picker(self):
        """
        Set configurable parameters for the picker
        """
        if 'picker' in self.scene and 'tolerance' in self.scene['picker']:
            self.picker_tolerance = float(self.scene['picker']['tolerance'])
        self.picker.SetTolerance(self.picker_tolerance)

    def camera_report(self):
        """
        Debug function for seeing where the camera is
        """
        for c in self.cameras:
          print(f"Camera: {c}")
          print("  Position:", self.cameras[c].GetPosition())
          print("  Focal Point:", self.cameras[c].GetFocalPoint())
          print("  View Up:", self.cameras[c].GetViewUp())
          print(f"  Clipping range: {self.cameras[c].GetClippingRange()}")
          print("Renderer info:")
        for r in self.renderers:
          print(f"  {r} NearClippingPlanTolerance: {self.renderers[r].GetNearClippingPlaneTolerance()} ")

    def register_callbacks(self):
        """
        Register all callbacks with the interactor
        """
        self.cameras['foreground'].AddObserver("ModifiedEvent", self.sync_cameras)
        self.interactor.AddObserver("RightButtonPressEvent", self.on_right_click)
        self.interactor.AddObserver("TimerEvent", self.on_timer)
        self.interactor.AddObserver("KeyPressEvent", self.on_key_press)

        # Not operational, see comment in orient_labels_to_camera() 
        #self.interactor.AddObserver("StartEvent", self.orient_labels_to_camera)
        self.timer_id = self.interactor.CreateRepeatingTimer(self.callback_rate)

    def sync_cameras(self, obj, event):
        """
        Sync the position of all cameras to that of the foreground camera
        This is required so that we can use multiple renderers for different
        sized actors in the scene. The cameras have to be in the same place
        for each renderer layer in order to for it to look correct as the
        scenes are composited together.
        """
        #print(f"DEBUG: syncing cameras")
        main_camera = self.cameras['foreground'] # Sync all to this one
        for cam in self.cameras:
            if cam == 'foreground':
                continue
            self.cameras[cam].SetViewUp(main_camera.GetViewUp())
            self.cameras[cam].SetFocalPoint(main_camera.GetFocalPoint())
            self.cameras[cam].SetPosition(main_camera.GetPosition())
            self.cameras[cam].SetViewAngle(main_camera.GetViewAngle())

    def position_sun_actor(self, obj, event):
        """
        Position the sun relative to camera each frame.
        This fakes out the sun being very far away when it's really
        just positioned relative to the camera to make it look that way

        TODO: This entire function isn't used right now as there are many issues
        with this approach of putting a sun sphere near the camera each render
        event frame. The biggest problem is that having a sphere near the camera
        means when we zoom out the single precision depth buffer problems are
        more consistently seen. This could maybe be overcome by removing
        self.sun_actor when zoomed way out, but then we can't see where the sun
        is. Another issue is that dollying out with scroll wheel has the sun
        flicker in its update, breaking realism. Furthermore the farther out
        you go the more erratic the movement of the sun is. On top of all of that
        we still render the sun when it should be occulded by large objects like
        the earth because it's actually close to the camera. There's a standalone
        example in my pyvista testing repoa (sun_billboard2.py) that provides an
        example on how to turn the sun visibility off if's occluded, which could
        help with this particular problem. In order to fulfill a sun in the scene
        correctly I think we need to dive deep into renderer layers and shaders
        which is too much to tackle right now for this one feature - Jordan 10/2025
        """

        if not self.sun_actor:
            return

        cam = self.renderers['foreground'].GetActiveCamera()
        cam_pos = cam.GetPosition()
    
        # Position the billboard along the sun direction, at fixed distance
        sun_pos = [
            cam_pos[0] + self.sun_direction[0] * self.sun_distance,
            cam_pos[1] + self.sun_direction[1] * self.sun_distance,
            cam_pos[2] + self.sun_direction[2] * self.sun_distance
        ]
        #import pdb; pdb.set_trace()
        #print(f"DEBUG: setting sun actor position to {sun_pos}")
        self.sun_actor.SetPosition(sun_pos)
        self.sun_actor.SetOrientation(cam.GetOrientation())

    def position_sun_light(self):
        if self.sun_driven_by:
            # In the case of vector sub-class, assign the sun direction to
            # the vector in world coordinates
            if isinstance(self.sun_driven_by, VirgoSceneNodeVector):
                self.sun_direction = self.sun_driven_by.get_world_vector()
            # In the case of a regular SceneNode, assign the sun direction to
            # the position of that node in world coordinates
            else:
                self.sun_direction = self.sun_driven_by.get_world_position()
        #print(f"DEBUG: self.sun_direction: {self.sun_direction}")
        # Normalize the sun direction
        sun_magnitude = math.sqrt(sum(c**2 for c in self.sun_direction))
        sun_dir_norm = [c / sun_magnitude for c in self.sun_direction]
        # Place the light in the direction of the vector 
        # Note that for this directional (SetPositional(False)) light it's
        # distance away from objects doesn't matter
        self.sun_light.SetPosition(sun_dir_norm)
        # Set direction of light to origin
        self.sun_light.SetFocalPoint(0, 0, 0)

    def create_skybox(self):
        """
        Create the skybox of the starfield using images from
        https://svs.gsfc.nasa.gov/4851

        TODO: This should probably live in another class and we just
        retrieve it here
        """
        texture_reader = vtk.vtkJPEGReader()
        texture_reader.SetFileName(
            os.path.join(thisFileDir,"images/space/starmap_2020_8k.jpg"))
        texture_reader.Update()
        
        texture = vtk.vtkTexture()
        texture.SetInputConnection(texture_reader.GetOutputPort())
        texture.MipmapOn()
        texture.InterpolateOn()
        texture.RepeatOn()
        
        # Create the skybox
        skybox = vtk.vtkSkybox()
        skybox.SetTexture(texture)
        skybox.SetProjectionToSphere()  # For equirectangular maps
        skybox.SetVisibility(False)     # Off by default

        self.renderers['skybox'].AddActor(skybox)
        return skybox

    def init_lighting(self):
        # TODO more verifier needed here
        if 'lighting' in self.scene and self.scene['lighting']:
            if 'start_mode' in self.scene['lighting']:
                self.lighting_mode = self.scene['lighting']['start_mode']
            if 'dark_ambient' in self.scene['lighting']:
                self.dark_ambient = self.scene['lighting']['dark_ambient']
            if 'bright_ambient' in self.scene['lighting']:
                self.bright_ambient = self.scene['lighting']['bright_ambient']
        # Initialize the sun light source
        self.init_sun()

        # Configure the 'realisitc' lighting mode with shadow pass
        # by building up self.camera_pass which the renderer will use
        shadows = vtk.vtkShadowMapPass()
        seq = vtk.vtkSequencePass()
        passes = vtk.vtkRenderPassCollection()
        # Baker pass provides inter-actor shadows but there's a major
        # bug that prevents us from using it. See details here:
        #bp = shadows.GetShadowMapBakerPass()
        #bp.SetResolution(4096)
        # https://discourse.vtk.org/t/invalid-shadow-shape-affected-by-the-aspect-ratio-of-the-window/12000/2
        #passes.AddItem(bp)
        passes.AddItem(shadows)
        # Add overlay and translucent passes, this is needed for text_actors
        # and actors with less than 1.0 opacity to render when more realisitic
        # lighting is on
        passes.AddItem(vtk.vtkTranslucentPass())
        passes.AddItem(vtk.vtkOverlayPass())
        seq.SetPasses(passes)
        self.camera_pass = vtk.vtkCameraPass()
        self.camera_pass.SetDelegatePass(seq)

        self.set_lighting_mode(self.lighting_mode)

    def get_node(self, name):
        if name not in self.nodes:
            return None
        return self.nodes.get(name)

    def init_sun(self):
        sun_scale=1.0
        if 'sun' in self.scene and self.scene['sun'] != None:
          if 'direction' in self.scene['sun']:
            self.sun_direction = self.scene['sun']['direction']
          if 'scale' in self.scene['sun']:
            sun_scale = self.scene['sun']['scale']
          if 'driven_by' in self.scene['sun']:
            n = self.scene['sun']['driven_by']
            self.sun_driven_by = self.get_node(n)
            if not self.sun_driven_by:
                msg = (f"ERROR: sun: driven_by: {n} is not a "
                       "valid node in the scene")
                raise RuntimeError (msg)

        # Create the light coming from the sun 
        self.sun_light = vtk.vtkLight()
        self.sun_light.SetLightTypeToSceneLight()
        self.sun_light.SetPositional(False)
        self.sun_light.SetDirectionAngle(0, 0)  # placeholder, we’ll set manually below
        self.sun_light.SetColor(1.0, 0.95, 0.8)
        self.sun_light.SetIntensity(0.8)
        
        self.position_sun_light()

        # Create the sun actor - this sphere is not a normal actor
        # but will be placed at a fixed distance relative to the camera
        # to make it appear to be very far away
        sun_sphere = vtk.vtkSphereSource()
        sun_size = 6.957e8  # Radius in meters
        sun_sphere.SetRadius(sun_size)
        sun_sphere.SetThetaResolution(32)
        sun_sphere.SetPhiResolution(32)
    
        sun_mapper = vtk.vtkPolyDataMapper()
        sun_mapper.SetInputConnection(sun_sphere.GetOutputPort())
    
        self.sun_actor = vtk.vtkActor()
        self.sun_actor.SetMapper(sun_mapper)
        self.sun_actor.GetProperty().SetColor(1.0, 1.0, 0.0) # Yellow
        self.sun_actor.GetProperty().SetLighting(False)  # always bright (emissive)
        self.sun_actor.SetPickable(False)
        self.sun_actor.SetScale(sun_scale, sun_scale, sun_scale)
        self.sun_distance = 1.4959e11

        self.position_sun_actor(None, None)

class VirgoScene:
    """
    Class used as entrypoint for all VIRGO rendered scenes

    Expects to consume a dict describing the scene dict and populates
    self.nodes (the main scene graph) and other internal members
    """
    def __init__(self, scene, verbosity=1, headless=False, stop_time=None,
                 images_dir="/tmp/", video_filename="/tmp/virgo.mp4", splash=True):
        self.scene = scene # Dict of scene info from YAML file
        self.verbosity = verbosity
        self.splash = splash
        self.headless = headless
        # TODO: consider using stop_time in interactive mode as well
        self.stop_time = stop_time  # For headless mode, stop at this time
        self.images_dir = images_dir
        self.video_filename = video_filename
        self.fs = 14      # font size
        self.max_sim_time = 0.0
        self.vdl = None    # VDL: Virgo Data Loader
        self._verify_scene()
        self.nodes = {}    # Scene graph dict of VirgoSceneNode instances

        self.background_color = [0.0, 0.0, 0.05]
        self.highlight_color = [1.0, 1.0, 0.0]
        self.description = "Untitled VIRGO Window"
        self.name = "Untitled_VIRGO_scene"
        self.window_width = 800
        self.window_height = 600
        # TODO: this checking can be removed once the dict verifier is in place
        if 'background_color' in self.scene:
            self.background_color = self.scene['background_color']
        if 'highlight_color' in self.scene:
            self.highlight_color = self.scene['highlight_color']
        if 'description' in self.scene:
            self.description = self.scene['description']
        if 'name' in self.scene:
            self.name = self.scene['name']
        if 'resolution' in self.scene:
            self.window_width, self.window_height = map(int, self.scene['resolution'].split('x'))
        if 'splash' in self.scene:
            self.splash = self.scene['splash']

        self.render_window = vtk.vtkRenderWindow()
        self.renderers = {}
        # We have multiple renderers to help overcome single precision depth buffer issues.
        # Each renderer operates in it's own layer composited on top of the last,
        # background first ending with foreground
        self.renderers['skybox'] = vtk.vtkRenderer()  # For actors 1e-10 -> 1e17
        self.renderers['skybox'].SetLayer(0)
        self.renderers['skybox'].InteractiveOff()
        self.renderers['skybox'].SetBackground(self.background_color)
        self.renderers['background'] = vtk.vtkRenderer()  # For actors 1e-10 -> 1e17
        self.renderers['background'].SetLayer(1)
        self.renderers['background'].InteractiveOff()
        self.renderers['background'].SetBackground(self.background_color)
        # TODO: probably want a 'midground' renderer      # For actors 1e-4 -> 1e11
        self.renderers['foreground'] = vtk.vtkRenderer()  # For actors 1e-2 -> 1e5
        self.renderers['foreground'].SetLayer(2)
        self.renderers['foreground'].SetBackground(self.background_color)
        self.render_window.SetNumberOfLayers(len(self.renderers.keys()))
        self.interactor = vtk.vtkRenderWindowInteractor()
    
        # Set better camera interaction
        self.interactor_style = VirgoInteractorStyle(renderers=self.renderers)
        self.controller = VirgoControlCenter(self.renderers, self.render_window,
                                             self.interactor, self.scene)
        self.initialized = False

    def initialize(self):
        """
        Initialize this instance by:
        1. Initializing all actors found in the scene
        2. Configuring the controller, renderer, and interactor
        """
        self.initialize_nodes()   # Load all actors from the self.scene info

        self.render_window.SetSize(self.window_width, self.window_height)
        self.render_window.SetWindowName(self.description)

        if self.headless:
            print("Running in headless (non-interactive) mode.")
            self.render_window.OffScreenRenderingOn()
        else:
            print("Running in interactive mode.")
            self.interactor.SetRenderWindow(self.render_window)
    
        # Set custom interactor style
        self.interactor.SetInteractorStyle(self.interactor_style)
    
        # Create controller
        self.controller.set_verbosity(self.verbosity)

        self.controller.register_callbacks()
        self.controller.initialize()
        self.initialized = True

    def add_node(self, node, parent_name=None):
        """
        Add a node to the scene graph represented by self.nodes dict
        """
        self.nodes[node.name] = node
        if parent_name is not None:
            parent = self.nodes[parent_name]
            parent.add_child(node)


    def _verify_scene(self):
        """
        TODO: Call a dict verifier here, similar to TrickWorkflowYamlVerifier
        """
        pass

    def create_actor(self, actor_name, actor_scene_dict=None,):
        """
        Given a single scene's actors: sub-entry, return a
        VirgoActor instance built from that information

        TODO all this checking for dict/YAML validity needs to be safer, i.e.
        checked in a YAML verifier before getting here similar to what
        TrickOps does with TrickWorkflowYamlVerifier. See VirgoDataPlayback.
        _verify_scene()

        Returns: VirgoActor 
        """
        offset_pos=[0.0, 0.0, 0.0]
        offset_ypr=[0.0, 0.0, 0.0]
        scale = 1.0
        opacity = 1.0
        if 'pos' in actor_scene_dict:
            offset_pos=actor_scene_dict['pos']
        if 'ypr' in actor_scene_dict:
            offset_ypr=actor_scene_dict['ypr']
        if 'scale' in actor_scene_dict:
            scale=actor_scene_dict['scale']
        if 'opacity' in actor_scene_dict:
            opacity=actor_scene_dict['opacity']
        # Create the Actor
        if self.verbosity > 1:
            print(f"Constructing VirgotDataPlaybackActor {actor_name} ...")
        actor = VirgoActor(
            mesh=actor_scene_dict['mesh'],
            offset_pos=offset_pos,
            offset_ypr=offset_ypr,
            name=actor_name,
            fontsize=self.fs
            )
        # TODO: create SceneNode here and add actor to it!
        if self.verbosity > 0:
            print(f"Initializing {actor_name} ...")

        actor.SetScale(scale)
        actor.GetProperty().SetOpacity(opacity)
        if 'color' in actor_scene_dict:
            actor.GetProperty().SetColor(actor_scene_dict['color'])
        if 'pickable' in actor_scene_dict and actor_scene_dict['pickable'] == 0:
            actor.PickableOff()

        return actor


    def create_vector(self, vector_name, vector_scene_dict=None):
        """
        Creates a VirgoActor with a VIRGO_PREFAB:arrow mesh
        from the information in the vector_scene_dict
        TODO: need checking for YAML field correctness!
        """
        # The vector fields are similar to actor, but mesh isn't repected
        # Force the arrow mesh then create the vector actor
        vector_scene_dict['mesh'] = 'VIRGO_PREFAB:arrow'
        vector = self.create_actor(actor_name=vector_name, actor_scene_dict=vector_scene_dict)
        if 'tip_length' in vector_scene_dict:
            vector.source.SetTipLength(vector_scene_dict['tip_length'])
        if 'tip_radius' in vector_scene_dict:
            vector.source.SetTipRadius(vector_scene_dict['tip_radius'])
        if 'tip_resolution' in vector_scene_dict:
            vector.source.SetTipResolution(vector_scene_dict['tip_resolution'])
        if 'shaft_radius' in vector_scene_dict:
            vector.source.SetShaftRadius(vector_scene_dict['shaft_radius'])
        if 'shaft_resolution' in vector_scene_dict:
            vector.source.SetShaftResolution(vector_scene_dict['shaft_resolution'])

        return vector

    def create_frame(self, frame_name, frame_scene_dict=None):
        """
        Creates a VirgoActor with no mesh from the information in the
        frame_scene_dict

        TODO: need checking for YAML field correctness! For example frames only
        respect a subset of actor parameters: parent, pos, and ypr. Things like
        color, and scale are meaningless as there's no mesh for a frame
        """
        # Frames cannot have a mesh, so set it to None before we create the actor
        frame_scene_dict['mesh'] = None
        frame = self.create_actor(actor_name=frame_name, actor_scene_dict=frame_scene_dict)
        return frame

    def create_node(self, actor, actor_scene_dict=None, _class=VirgoSceneNode):
        """
        Creates a VirgoSceneNode associated with actor from the information
        in actor_scene_dict. 

        Args:
          _class (cls): Class to instantiate, must be or derive from VirgoSceneNode

        Returns: Tuple of (VirgoSceneNode, parent_name [str])
        """
        name = None
        if actor:
            name = actor.name
        node = _class(name=name, actor=actor)
        node.set_highlight_color(self.highlight_color)
        parent_name=None
        if 'parent' in actor_scene_dict and actor_scene_dict['parent'] != None:
            parent_name=actor_scene_dict['parent']

        # If labels: are provided for the node/actor, 
        if 'labels' in actor_scene_dict:
            labels = actor_scene_dict['labels']
            for label in labels:
                position = [0.0, 0.0, 0.0]
                ypr = [0.0, 0.0, 0.0]
                scale = 0.3
                color = [1.0, 1.0, 1.0]
                if 'text' in labels[label]:
                  text = labels[label]['text']
                if 'pos' in labels[label]:
                  position = labels[label]['pos']
                if 'scale' in labels[label]:
                  scale = labels[label]['scale']
                if 'ypr' in labels[label]:
                  ypr = labels[label]['ypr']
                if 'color' in labels[label]:
                  color = labels[label]['color']
                # Add the label to the node
                node.add_label(name=label, text=text, position=position, ypr=ypr, scale=scale, color=color)
                # Tell the label to follow the camera so it always faces it,
                # THIS ISNT WORKING RIGHT NOW I THINK BECAUSE OF THE ASSEMBLY SYSTEM
                #node.get_label(label).get_follower().SetCamera(self.renderer.GetActiveCamera())

        return node, parent_name

    def create_trail(self, node, actor_scene_dict=None):
        """Create a trail for node and assign it

        Creates a trail_actor (which also stores it inside node)
        """
        trail_actor = None
        if 'trail' in actor_scene_dict and actor_scene_dict['trail'] != None:
          if 'enabled' in actor_scene_dict['trail'] and actor_scene_dict['trail']['enabled']:
            color = [1.0, 1.0, 1.0]
            thickness = 2
            opacity = 1.0
            if 'color' in actor_scene_dict['trail']:
              color=actor_scene_dict['trail']['color']
            if 'thickness' in actor_scene_dict['trail']:
              thickness=actor_scene_dict['trail']['thickness']
            if 'opacity' in actor_scene_dict['trail']:
              opacity=actor_scene_dict['trail']['opacity']
            trail_actor = node.create_trail(color=color, thickness=thickness, opacity=opacity)
        return trail_actor

    def initialize_nodes(self):
        """
        Create the actors and VirgoSceneNodes associated with entries in
        the scene dictionary and initialize their configurable parameters. Then
        add them all to self.nodes and pass them into self.controller so that
        they can be accessed at runtime.
        """
        nodes_to_add = []
        frames = {}
        if 'frames' in self.scene:
            for f in self.scene['frames']:
                node, parent_name = self.create_node(actor=None, actor_scene_dict=self.scene['frames'][f])
                node.set_name(f)
                node.set_static=True
                # TODO I'm not convinced using 'scale' to represent axes size is
                # the best idea as it may cause confusion to the user - after
                # all frames dont have meshes...
                axes_scale = 1.0
                if 'scale' in self.scene['frames'][f]:
                    axes_scale = self.scene['frames'][f]['scale']
                node.set_axes_length(axes_scale,axes_scale,axes_scale)
                node.set_axes_pickable_on()
                node.set_pose(pos=self.scene['frames'][f]['pos'], ypr=self.scene['frames'][f]['ypr'])
                nodes_to_add.append( (node, parent_name) )

        actors = {}
        trail_actors = {}
        if 'actors' in self.scene:
            for a in self.scene['actors']:
                actors[a] = self.create_actor(actor_name=a, actor_scene_dict=self.scene['actors'][a])
                actors[a].initialize()
    
                node, parent_name = self.create_node(actor=actors[a], actor_scene_dict=self.scene['actors'][a])
                nodes_to_add.append( (node, parent_name) )
                trail_actor = self.create_trail(node, actor_scene_dict=self.scene['actors'][a])
                if trail_actor:
                    trail_actors[a] = trail_actor

        vectors = {}
        if 'vectors' in self.scene:
            for v in self.scene['vectors']:
                vectors[v] = self.create_vector(vector_name=v, vector_scene_dict=self.scene['vectors'][v],)
                vectors[v].initialize()
                node, parent_name = self.create_node(actor=vectors[v], actor_scene_dict=self.scene['vectors'][v],
                                                     _class=VirgoSceneNodeVector)
                # A vector is a special VIRGO_PREFAB:arrow actor that cannot specify rotations
                if node.data_source._rotations is not None:
                    msg = (f"ERROR: vector {v} should not specify rotations as they are computed automatically")
                    raise RuntimeError (msg)
                nodes_to_add.append( (node, parent_name) )

        self.populate_nodes(nodes_to_add)

        # Pass the nodes/actors into the controller for runtime use
        self.controller.set_actors(actors)
        self.controller.set_vectors(vectors)
        self.controller.set_frames(frames)
        self.controller.set_nodes(self.nodes)
        self.controller.set_trail_actors(trail_actors)

    def populate_nodes(self, nodes_to_add):
        """
        Add nodes to self.nodes in root to leaf order

        nodes_to_add is a list of  [ (node, parent_name of that node), ... ]

        Nodes must be added to self.nodes in root-to-leaf order, otherwise
        parent nodes might not exist when the internal add_child() call runs
        """
        pending = list(nodes_to_add)
        added = set()

        while pending:
            progress = False
            for (node, parent_name) in pending[:]:
                if parent_name is None or parent_name in self.nodes:
                    self.add_node(node, parent_name)
                    added.add(node.name)
                    pending.remove((node, parent_name))
                    progress = True
            if not progress:
                raise RuntimeError(
                    f"Could not resolve parents for nodes: "
                    f"{[n.name for n, _ in pending]}"
                )

    def run(self):
        """
        The entrypoint for starting up a rendered window
        """

        if not self.controller.is_initialized():
            print("ERROR: Scene is not properly initialized. Exiting.")
            return(1)
        if self.verbosity > 0:
            self.report()
    
        if self.headless:
            self.run_headless(stop_time=self.stop_time)
        else:
            if self.verbosity > 0:
                print("Entering render window and interactor loop...")

            if self.splash == True:
                splash = VirgoSplash(self.render_window, self.interactor)
                splash.show_splash()

            self._run_interactive()

        self.tear_down()
        return 0

    def report(self):
        # Get some diagnostics about how much RAM we are using
        cprint(f"Scene: {self.name}", bold=True)
        cprint(f"Description:", bold=True)
        cprint(textwrap.indent(f"{self.description}", "  "), bold=True)
        try:
            import psutil
            process = psutil.Process(os.getpid())
            memory_mb = process.memory_info().rss / 1024 ** 2  # RSS = Resident Set Size
            cprint(f"Current RAM usage: {memory_mb:.2f} MB")
        except Exception as e:
            pass

    def _run_interactive(self):
        self.render_window.Render()
        self.interactor.Start()

    def run_headless(self, stop_time=None):
        if not stop_time:
            stop_time = self.controller.max_sim_time 
        if self.controller.world_time >= stop_time:
            print(f"Nothing to render in headless mode. Starting world time "
                  f"{self.controller.world_time} >= stop time {stop_time}. "
                  f"This is often caused by all actors/nodes being static "
                  f"(no driven_by: section) in the scene dict.")
            return
        self.controller.mode = 'PLAYING' # Force playing
        finished = False
        frame_num = 0
        percent_complete = 0.0
        # Dump frames for the scene
        sys.stdout.write(f'Generating frames:')
        tmp_dir = tempfile.mkdtemp()
        while not finished:
          percent_complete = self.controller.world_time / stop_time * 100.0
          #import pdb; pdb.set_trace()
          sys.stdout.write(f'\rGenerating frames: {percent_complete:8.2f}%')
          sys.stdout.flush()  # Ensure it updates immediately
          self.controller.update_scene()
          self.render_window.Render()
          filename = os.path.join(tmp_dir,
                                f"frame_{frame_num:07d}.png")
          self.controller.save_frame(filename=filename)
          frame_num += 1
          if math.isclose(self.controller.world_time, stop_time):
          #if math.isclose(self.controller.world_time, 3.0):
            finished = True
        percent_complete=100.0
        sys.stdout.write(f'\rGenerating frames: {percent_complete:8.2f}%\n')

        # Render an mp4 video file
        try:
          import imageio
        except Exception as e:
          msg = (f"ERROR: imageio not found in virtual environment, cannot render"
                 f" images in {tmp_dir} to video file {self.video_filename}.")
          print(msg)
          self.tear_down()
          raise(e)
        print(f"Rendering {self.video_filename} ...")
        frames = [imageio.imread(f"{tmp_dir}/frame_{i:07d}.png")
                   for i in range(frame_num)]
        imageio.mimsave(self.video_filename, frames, fps=self.controller.frame_rate)
        print(f"Done.")


    def tear_down(self):
        for renderer in self.renderers:
            self.renderers[renderer].RemoveAllObservers()
        self.interactor.RemoveAllObservers()
