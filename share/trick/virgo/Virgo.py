#!/usr/bin/env python3.11
"""
VIRGO: Versatile Imaging and Rendering for Galactic Operations
A Practical, Analytical, and Hardworking 3D Visualization tool
leveraging python-VTK
"""
from VirgoActor import VirgoActor
from VirgoNode import VirgoSceneNode, VirgoSceneNodeVector
from VirgoSplash import VirgoSplash

import os, sys, inspect, time
import re
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
    def __init__(self):
        super().__init__()
        self.node = None
        self.renderer = None
        self.AddObserver("CharEvent", self.onChar)
        self.AddObserver(vtk.vtkCommand.EndInteractionEvent, self.StoreRelativeCameraInfo)
        self.relative_offset = None  # Positional offset between camera and followed actor
        self.view_up = None    # Camera GetViewUp() direction when self.realtive_offset was stored

    def set_node(self, node):
        self.node = node

    def get_renderer(self):
        return(self.renderer)

    def set_renderer(self, renderer):
        self.renderer = renderer

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
        if not self.node or not self.renderer:
            return
        # Update relative offset after user interaction
        camera = self.renderer.GetActiveCamera()

        node_pos =np.array(self.node.get_world_position())

        # Store off the relative offset of camera
        self.relative_offset = np.array([camera.GetPosition()[i] - node_pos[i] for i in range(3)])
        #print(f"DEBUG: STORING camera relative offset: {self.relative_offset}")
        self.view_up = camera.GetViewUp()

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
    def __init__(self, renderer, interactor, scene, world_time=0.0):
        """
        Constructor
        """
        # Current scene world time (not necessarily aligned with sim data time)
        self.world_time = self.world_time_start = world_time 
        self.wallclock_time = time.time()  # Actual wall clock time in real life
        self.max_sim_time = 0.0            # highest sim time across all dr groups
        self.mode = 'PLAYING'              # 'PAUSED' or 'PLAYING'
        self.fs = 14      # font size
        self.renderer = renderer
        self.camera = renderer.GetActiveCamera()
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
        self.frame_rate = 50 # Frame rate for timer callback (millisec)
        # self.dt: Increment time by this amount every timer update (sec)
        self.default_dt = self.dt = self.frame_rate / 1000.0
        self.playback_speed = 1.0  # Speed of playback
        # Default playback speeds
        self.available_speeds = [1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0]
        self.huge = 1.0e30   # A huge floating point number used for finding smallest values
        self._initialized = False
        self.verbosity = 1

    def initialize(self):
        """
        Set up the scene by:
          * Adding all actors to the renderer
          * Initializing the camera
        """
        if not self.actors:
          msg = (f"ERROR: No actors found in {__class__} initialize() function. ")
          raise RuntimeError (msg)

        self._determine_max_sim_time()
        self.update_nodes()
        self.text_actors['mode'] = self.create_text_actor()
        self.text_actors['picked'] = self.create_text_actor(pos=[10,10])
        self.text_actors['time'] = self.create_text_actor()
        self.text_actors['help'] = self.create_text_actor()
        self.text_actors['camera'] = self.create_text_actor()
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

        # Add actors in scene
        for n in self.nodes:
            # Only add root nodes to the renderer, all children come along
            # automatically
            if self.nodes[n].parent == None:
                #import pdb; pdb.set_trace()
                self.renderer.AddActor(self.nodes[n].assembly)
        for a in self.trail_actors:
            self.renderer.AddActor(self.trail_actors[a])
        for t in self.text_actors:
            self.renderer.AddActor(self.text_actors[t])

        self.init_camera()
        self.init_picker()
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

        self.configure_hud()
        # This auto-adjusts clipping based on visible actors
        self.renderer.ResetCameraClippingRange()
        # The end of the main update loop, render the image
        self.renderer.GetRenderWindow().Render()

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
        window_width, window_height  = self.interactor.GetRenderWindow().GetSize()
        
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
                label="Node scene position "
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
        self.text_actors['mode'].GetBoundingBox(self.renderer, bounds)
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
        self.text_actors['time'].GetBoundingBox(self.renderer, bounds)
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
                f"\n s: Cycle playback speeds"
                f"\n t: Toggle trails"
                f"\n r: Fit camera to scene"
                f"\n <- -> : Step back/forward in time"
                f"\n  -  + : Adjust HUD text size"
                f"\n a: Toggle Axes Visibility"
                f"\n c: Toggle camera free/follow-picked"
                f"\n l: Toggle Node Label Visibility"
                f"\n h: Toggle this help message"
                f"\n BackSpace: Toggle starfield"
                f"\n j/k: Near Plane Clipping Tolerance"
                f"\n J/K: Picker Tolerance"
                f"\n Q: Quit"
                )
            self.text_actors['help'].GetBoundingBox(self.renderer, bounds)
            text_width = bounds[1] - bounds[0] + 1  # Width in pixels
            text_height = bounds[3] - bounds[2] + 1  # Height in pixels
            # Calculate position for bottom-right corner with 20-pixel padding
            x_pos = window_width - text_width - hud_padding    # Right edge minus width
            y_pos = hud_padding  # Bottom edge (20 pixels from bottom)
            self.text_actors['help'].SetPosition(x_pos, y_pos)

        else:
            self.text_actors['help'].SetInput("")

        ############################################################################
        # Camera info displayed in top center-right-ish
        ############################################################################
        camera_info = f"Camera: Follow {self.camera_follows.name}" if self.camera_follows else "Camera: Free" 
        self.text_actors['camera'].SetInput(f"{camera_info}")
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['camera'].GetBoundingBox(self.renderer, bounds)
        text_width = bounds[1] - bounds[0] + 1  # Width in pixels
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        # Calculate position for bottom-right corner with padding
        x_pos = (window_width - text_width - hud_padding)/1.3  # 2/3ish the way over on right side
        y_pos = window_height - text_height - hud_padding  # Top edge (20 pixels from bottom)
        self.text_actors['camera'].SetPosition(x_pos, y_pos)

        ############################################################################
        # Version
        ############################################################################
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['picked'].GetBoundingBox(self.renderer, bounds)
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
        self.camera.SetFocalPoint(pos[0], pos[1], pos[2])

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
        if not intstyle.get_renderer():
            intstyle.set_renderer(self.renderer)

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
            #print(f"DEBUG: INIT moving self.camera to {camera_pos}")
            #print(f"DEBUG: INIT setting self.camera focal to {pos}")
            self.camera.SetPosition(camera_pos[0], camera_pos[1], camera_pos[2])
            self.camera.SetFocalPoint(pos[0], pos[1], pos[2])
            # Store this initial offset in the VirgoInteractorStyle by calling the
            # callback which stores the relative camera offset
            self.interactor.GetInteractorStyle().StoreRelativeCameraInfo(None, None)

        pos = np.array(node.get_world_position())
        #print(f"DEBUG: self.camera_follow_offset is {self.camera_follow_offset}")
        camera_pos = pos + self.camera_follow_offset
        #print(f"DEBUG: moving self.camera to {camera_pos}")
        #print(f"DEBUG: setting self.camera focal to {pos}")
        # Set camera position
        self.camera.SetPosition(camera_pos[0], camera_pos[1], camera_pos[2])
        # Set focal point to spacecraft's center
        self.camera.SetFocalPoint(pos[0], pos[1], pos[2])
        # Ask the interactor for the stored off view_up direction from end of interaction
        view_up = self.interactor.GetInteractorStyle().GetRelativeCameraInfo('view_up')
        # If view_up isn't defined, default it to "positive Z is up"
        if not view_up:
            view_up = (0.0, 0.0, 1.0)
        self.camera.SetViewUp(view_up)

    def on_right_click(self, obj, event):
        """
        Callback for right click actions. This currently provides "picking", assigning
        self.picked_actor to the actor the mouse clicked on, and changing it's color
        to self.scene['highlight_color']
        """
        click_pos = self.interactor.GetEventPosition()
        self.picker.Pick(click_pos[0], click_pos[1], 0, self.renderer)
        self.picked_actor = self.get_actor_from_picker()

        #import pdb; pdb.set_trace()
        if self.picked_actor and self.picked_actor in self.actors.values():
            #print(f"DEBUG: self.picked_actor is {self.picked_actor.name}")
            self.configure_hud()
            # TODO this functionality is broken and I'm not sure we even want it
            # as the user experience is weird
            #self.focus_camera_on(self.picked_actor)
        if self.last_picked_actor:
            self.last_picked_actor.GetProperty().SetColor(self.original_colors[self.last_picked_actor])

        if self.picked_actor:
            # Store original color if not already stored
            if self.picked_actor not in self.original_colors:
                original_color = self.picked_actor.GetProperty().GetColor()
                self.original_colors[self.picked_actor] = original_color
            
            # Restore color of the previously picked actor
            if self.last_picked_actor and self.last_picked_actor != self.picked_actor:
                self.last_picked_actor.GetProperty().SetColor(self.original_colors[self.last_picked_actor])
            
            # Highlight the current picked actor
            self.picked_actor.GetProperty().SetColor(self.scene['highlight_color'])
            # Get the X,Y,Z coordinates of the actor's orign
            matrix = self.picked_actor.GetMatrix()
            actor_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
            
            # Update last picked actor
            self.last_picked_actor = self.picked_actor
            
        self.renderer.Render()

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

    def toggle_trails(self):
        for n in self.nodes:
            if self.nodes[n].is_trail_visible():
                self.nodes[n].hide_trail()
            else:
                self.nodes[n].show_trail()

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
        if key == 'l':
            # Turn actor axes on/off
            self.toggle_node_labels()
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
        if key == "Right" or key == 'period':
            self.mode = 'PAUSED'
            self.increment_time()
            self.update_nodes()
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
        """
        # Default camera if the scene has given us no information on it
        self.camera.SetPosition(-10.0, 0.0, 0.0)
        self.camera.SetFocalPoint(0, 0, 0)
        self.camera.SetViewUp(0.0, 0.0, 1.0)  # Z is up

        # If the camera is setup to follow a node/actor, save that node in
        # self.camera_follows and pass that node into the Interactor so
        # it can track camera information relative to an actor
        if 'camera' in self.scene:
          initial_camera_pos = None
          if 'position' in self.scene['camera']:
            initial_camera_pos = self.scene['camera']['position']
            self.camera.SetPosition(initial_camera_pos )
          if 'focal_point' in self.scene['camera']:
            self.camera.SetFocalPoint(self.scene['camera']['focal_point'])
          if 'view_up' in self.scene['camera']:
            self.camera.SetViewUp(self.scene['camera']['view_up'])
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
        self.renderer.SetNearClippingPlaneTolerance(self.near_clipping_plane_tolerance)

    def increase_near_clipping_plane_tolerance(self, multiplier=2.0):
        self.near_clipping_plane_tolerance *= multiplier
        self.renderer.SetNearClippingPlaneTolerance(self.near_clipping_plane_tolerance)

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
        print("Camera Position:", self.camera.GetPosition())
        print("  Focal Point:", self.camera.GetFocalPoint())
        print("  View Up:", self.camera.GetViewUp())
        print(f"  Clipping range: {self.camera.GetClippingRange()} ")
        print("Renderer info:")
        print(f"  NearClippingPlanTolerance: {self.renderer.GetNearClippingPlaneTolerance()} ")

    def register_callbacks(self):
        """
        Register all callbacks with the interactor
        """
        self.interactor.AddObserver("RightButtonPressEvent", self.on_right_click)
        self.interactor.AddObserver("TimerEvent", self.on_timer)
        self.interactor.AddObserver("KeyPressEvent", self.on_key_press)
        # Not operational, see comment in orient_labels_to_camera() 
        #self.interactor.AddObserver("StartEvent", self.orient_labels_to_camera)
        self.timer_id = self.interactor.CreateRepeatingTimer(self.frame_rate)

    def create_skybox(self):
        """
        Create the skybox of the starfield using images from
        https://svs.gsfc.nasa.gov/4851

        TODO: This should probably live in another class and we just
        retrieve it here
        """
        # Load cube map texture
        texture = vtk.vtkTexture()
        texture.CubeMapOn()
        texture.InterpolateOn()
        
        # File names for each direction
        faces = {
            'posx': os.path.join(thisFileDir, 'images/space_cubemap/posx.jpg'),
            'negx': os.path.join(thisFileDir, 'images/space_cubemap/negx.jpg'),
            'posy': os.path.join(thisFileDir, 'images/space_cubemap/posy.jpg'),
            'negy': os.path.join(thisFileDir, 'images/space_cubemap/negy.jpg'),
            'posz': os.path.join(thisFileDir, 'images/space_cubemap/posz.jpg'),
            'negz': os.path.join(thisFileDir, 'images/space_cubemap/negz.jpg')
        }
        
        # Map each direction to the correct image
        imageReaders = {}
        face_order = ['posx', 'negx', 'posy', 'negy', 'posz', 'negz']
        for i, face in enumerate(face_order):
            reader = vtk.vtkJPEGReader()
            reader.SetFileName(faces[face])
            reader.Update()
            texture.SetInputDataObject(i, reader.GetOutput())
        
        # Create the skybox
        skybox = vtk.vtkSkybox()
        skybox.SetTexture(texture)
        # Off by default as this doesn't yet work well in scenes with large and
        # small objects
        skybox.SetVisibility(False)

        self.renderer.AddActor(skybox)
        return skybox


class VirgoScene:
    """
    Class used as entrypoint for all VIRGO rendered scenes

    Expects to consume a dict describing the scene dict and populates
    self.nodes (the main scene graph) and other internal members
    """
    def __init__(self, scene, verbosity=1):
        self.verbosity = verbosity
        self.fs = 14      # font size
        self.max_sim_time = 0.0
        self.scene = scene # Dict of scene info from YAML file
        self.vdl = None    # VDL: Virgo Data Loader
        self._verify_scene()
        self.nodes = {}    # Scene graph dict of VirgoSceneNode instances

        self.background_color = [0.0, 0.0, 0.05]
        self.description = "Untitled VIRGO Window"
        # TODO: this checking can be removed once the dict verifier is in place
        if 'background_color' in self.scene:
            self.background_color = self.scene['background_color']
        if 'description' in self.scene:
            self.description = self.scene['description']

        self.renderer = vtk.vtkRenderer()
        self.render_window = vtk.vtkRenderWindow()
        self.interactor = vtk.vtkRenderWindowInteractor()
    
        # Set better camera interaction
        self.interactor_style = VirgoInteractorStyle()
        self.controller = VirgoControlCenter(self.renderer, self.interactor, self.scene)
        self.initialized = False

    def initialize(self):
        """
        Initialize this instance by:
        1. Initializing all actors found in the scene
        2. Configuring the controller, renderer, and interactor
        """
        self.initialize_nodes()   # Load all actors from the self.scene info

        self.renderer.SetBackground(self.background_color)
        self.render_window.AddRenderer(self.renderer)
        self.render_window.SetSize(1920, 1080)
        self.render_window.SetWindowName(self.description)
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

    def get_node(self, name):
        return self.nodes.get(name)

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
            print("Entering render window and interactor loop...")

        splash = VirgoSplash(self.render_window, self.interactor)
        splash.show_splash()

        self.render_window.Render()
        self.interactor.Start()
        self.tear_down()

        return 0

    def tear_down(self):
        self.renderer.RemoveAllObservers()
        self.interactor.RemoveAllObservers()
