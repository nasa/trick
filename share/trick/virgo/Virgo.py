#!/usr/bin/env python3.11
"""
VIRGO: Versatile Imaging and Rendering for Galactic Operations
A Practical, Analytical, and Hardworking 3D Visualization tool
leveraging python-VTK
"""
from VirgoActor import VirgoDataPlaybackActor
from VirgoDataFileLoader import VirgoDataFileLoader
from VirgoNode import VirgoSceneNode
from VirgoDataSource import VirgoDataFileSource
import os, sys, inspect, time
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))

# For TrickPy, the python module that can load trick data record produced files
sys.path.append(os.path.abspath(os.path.join(thisFileDir, '../')))

import re
import vtk
import numpy as np
# Example of custom interactor style to override the default 'e' key behavior
class VirgoInteractorStyle(vtk.vtkInteractorStyleTrackballCamera):
    """
    Virgo Interactor Style which adds some capabilities on top of the VTK
    provided vtkInteractorStyleTrackballCamera
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
        node_pos = self.node.data_source.get_current_position()
        # Store off the relative offset of camera
        self.relative_offset = np.array([camera.GetPosition()[i] - node_pos[i] for i in range(3)])
        #print(f"DEBUG: Camera relative offset: {self.relative_offset}")
        self.view_up = camera.GetViewUp()

    def GetRelativeCameraInfo(self, info):
        '''
        Get the relative offset between the followed actor and the camera
        '''
        #print(f"DEBUG: Getting relative_offset: {self.relative_offset}")
        if   info == 'position':
            return self.relative_offset
        elif info == 'view_up':
            return self.view_up
        else:
            return None

class VirgoDataPlaybackControlCenter:
    """
    The control center class for Virgo Data playback
    """
    def __init__(self, renderer, interactor, scene, vdl, world_time=0.0):
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
        self.vdl = vdl
        self.skybox = self.create_skybox()
        self.actors = {}
        self.trail_actors = {}
        self.text_actors={}

        self.help = False  # Display help message when true
        #self.picker = vtk.vtkPropPicker()
        self.picker = vtk.vtkCellPicker()
        self.picker.SetTolerance(5e-7) # Picker tolerance
        self.interactor.SetPicker(self.picker)
        self.picked_actor      = None # Currently picked actor or None if not
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
        self.text_actors['warning'] = self.create_text_actor()

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
#        for a in self.actors:
#            # Add the actor
#            self.renderer.AddActor(self.actors[a])
#            # Add it's axes
#            self.renderer.AddActor(self.actors[a].get_axes())
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
        # Toggle axes labels based on distance
        #TODO modify this to work on nodes not actors
        for n in self.nodes:
            #self.nodes[n]
            pass
            #self.actors[actor].hide_or_unhide_axes_labels(self.camera)
            #self.actors[actor].axes.Modified()
        # I DONT KNOW WHY BUT THIS ONE LINE FIXES THE STARS IN THE BACKGROUND
        self.renderer.ResetCameraClippingRange() # This will auto-adjust clipping based on visible actors

        self.renderer.GetRenderWindow().Render()


    def on_timer(self, caller, event):
        self.wallclock_time = time.time()  # Actual wall clock time in real life
        self.update_scene()

        time_one_frame_took = time.time() - self.wallclock_time
        if time_one_frame_took > self.dt:
            print(f"WARNING: Frame took {time_one_frame_took} sec to complete which is "
                  f"larger than self.dt ({self.dt}). Playback rate may not be accurate")

    def configure_hud(self):
        '''
        Configure the heads-up-display in preparation for rendering.
        Does not render.
        '''
        hud_padding = 20 # pixels
        window_width, window_height  = self.interactor.GetRenderWindow().GetSize()
        
        ############################################################################
        # Picked Actor (or last picked if nothing picked) information in bottom left
        ############################################################################
        actor = self.picked_actor if self.picked_actor else self.last_picked_actor
        if actor:
            #matrix = actor.GetMatrix()
            ## Extract translation (elements 3, 7, 11 in 4x4 matrix)
            #pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
            label="Position"
            pos = self.nodes[actor.name].get_current_position()
            if not pos:
                label="Scene position"
                pos = self.nodes[actor.name].get_world_position()
            name = self.get_actor_name(actor)
            self.text_actors['picked'].SetInput(
                f"{name}\n {label}: {pos[0]:<10.5f}, {pos[1]:<10.5f}, {pos[2]:<10.5f} units"
                )
        else:
            self.text_actors['picked'].SetInput("")

        ############################################################################
        # Mode displayed in top right
        ############################################################################
        self.text_actors['mode'].SetInput(f"{self.mode} {self.playback_speed}X")
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
        self.text_actors['time'].SetPosition(hud_padding, y_pos)
        percent_complete = self.world_time / self.max_sim_time * 100
        self.text_actors['time'].SetInput(
            f"\nWorld time: {self.world_time:<10.5f} / {self.max_sim_time} sec [{percent_complete:<4.2f} %]"
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
                f"\n Scroll wheel: Zoom"
                f"\n L-click: Pick Actor"
                f"\n"
                f"\nKEYBOARD"
                f"\n SPACE: Pause/Play"
                f"\n S: Cycle playback speeds"
                f"\n R: Fit camera to scene"
                f"\n <- -> : Step back/forward in time"
                f"\n  -  + : Adjust text size"
                f"\n A: Toggle Axes Visibility"
                f"\n H: Toggle this help message"
                f"\n BackSpace: Toggle starfield"
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
        # Prototype Warning
        ############################################################################
        bounds = [0] * 4 # Get the text bounding box in display coordinates  [xmin, xmax, ymin, ymax]
        self.text_actors['picked'].GetBoundingBox(self.renderer, bounds)
        text_height = bounds[3] - bounds[2] + 1  # Height in pixels
        self.text_actors['warning'].GetTextProperty().SetColor(1.0, 0.0, 0.0)
        self.text_actors['warning'].SetPosition(hud_padding, text_height + hud_padding)
        self.text_actors['warning'].SetInput(
            f"YOU ARE USING A PROTOTYPE NOT"
            f"\nSUITABLE FOR ENGINEERING ANALYSIS"
            )

    def get_actor_name(self, vtk_obj):
        for a in self.actors:
            if vtk_obj is self.actors[a]:
                return a
        return "UNKNOWN ACTOR"

    def focus_camera_on(self, actor):
        """
        Point the camera at the given actor
        Assumes actor is positioned with matrix data, not position data
        """
        matrix = actor.GetMatrix()
        pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
        self.camera.SetFocalPoint(pos[0], pos[1], pos[2])

    def camera_follow(self, node):
        """
        Move the camera to follow the given node
        """
        # Ask the interactor for the latest camera position
        self.camera_follow_offset = self.interactor.GetInteractorStyle().GetRelativeCameraInfo('position')
        # If it's not set, compute the initial offset from the bounding box of the actor
        if self.camera_follow_offset is None:
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
            offset_distance = max_dimension * 20.0
            self.camera_follow_offset = np.array([offset_distance, offset_distance/2.0, 0.0])

        # TODO: what if data_source is NONE???
        pos =np.array(node.data_source.get_current_position())
        camera_pos = pos + self.camera_follow_offset
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

    def on_left_click(self, obj, event):
        """
        Callback for left click actions. This currently provides "picking", assigning
        self.picked_actor to the actor the mouse clicked on, and changing it's color
        to self.scene['highlight_color']
        """
        click_pos = self.interactor.GetEventPosition()
        self.picker.Pick(click_pos[0], click_pos[1], 0, self.renderer)
        self.picked_actor = self.get_actor_from_picker()

        #import pdb; pdb.set_trace()
        if self.picked_actor and self.picked_actor in self.actors.values():
            print(f"DEBUG: self.picked_actor is {self.picked_actor.name}")
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
        """Return the picked vtkActor if any, using vtkAssemblyPath inspection."""
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
#        # 1) Direct actor (works if actor was picked at top-level)
#        actor = self.picker.GetActor()
#        if actor:
#            print(f"DEBUG: Path 1 taken for {actor.name}")
#            return actor
#    
#        # 2) Try Prop3D / ViewProp helpers (some pickers set these)
#        prop3d = self.picker.GetProp3D()
#        if prop3d and prop3d.IsA("vtkActor"):
#            return prop3d
#        view_prop = self.picker.GetViewProp()
#        if view_prop and view_prop.IsA("vtkActor"):
#            print(f"DEBUG: Path 2 taken for {view_prop.name}")
#            return view_prop
#    
#        # 3) Fallback: examine the assembly path (this fully describes the pick)
#        path = self.picker.GetPath()
#        if path:
#            last_node = path.GetLastNode()
#            if last_node:
#                picked = last_node.GetViewProp()  # this is usually the actual actor
#                if picked and picked.IsA("vtkActor"):
#                    print(f"DEBUG: Path 3 taken for {picked.name}")
#                    return picked
#    
#        return None

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
            for actor in self.actors:
                self.actors[actor].report()
        if key == "c":
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
                self.actors[a].data_source.set_current_time(self.world_time, strategy='closest')
                
    def increment_time(self):
        """
        Increment world time by the minimum resolution across all actors logged data

        TODO: This has not been tested with multiple data record groups logging at
        specific rates.
        """
        smallest_time = self.huge # Huge
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
        self.camera.SetPosition(487.9, 52.2, -11.35)
        self.camera.SetFocalPoint(0, 0, 0)
        self.camera.SetViewUp(-0.10, -0.09, -0.98)

        # If the camera is setup to follow a node/actor, save that node in
        # self.camera_follows and pass that node into the Interactor so
        # it can track camera information relative to an actor
        if 'camera' in self.scene and 'follow' in self.scene['camera']:
            for node in self.nodes:
                if self.nodes[node].name == self.scene['camera']['follow']:
                    self.camera_follows = self.nodes[node]
                    self.interactor.GetInteractorStyle().set_node(self.camera_follows)
                    self.interactor.GetInteractorStyle().set_renderer(self.renderer)
                    self.camera_follow(self.camera_follows)

        # THIS VALUE IS CRITICAL, SMALLER NUMBERS (0.00001) MAKE THE BACKGROUND
        # SKYBOX STARS WIGGLE BUT LARGER NUMBERS (0.0001) MAKE SMALLER ACTORS NOT
        # VISIBLE DUE TO CLIPPING. TODO this probably shouldn't be set every frame 
        # TODO this needs checking in the verifier as well
        if 'camera' in self.scene and 'near_clipping_plane_tolerance' in self.scene['camera']:
            self.renderer.SetNearClippingPlaneTolerance(self.scene['camera']['near_clipping_plane_tolerance'])
        else:
            self.renderer.SetNearClippingPlaneTolerance(0.00005)

    def init_picker(self):
        """
        Set configurable parameters for the picker
        """
        # If the camera is setup to follow a node/actor, save that node in
        # self.camera_follows and pass that node into the Interactor so
        # it can track camera information relative to an actor
        if 'picker' in self.scene and 'tolerance' in self.scene['picker']:
            self.picker.SetTolerance(float(self.scene['picker']['tolerance']))


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
        self.interactor.AddObserver("LeftButtonPressEvent", self.on_left_click)
        self.interactor.AddObserver("TimerEvent", self.on_timer)
        self.interactor.AddObserver("KeyPressEvent", self.on_key_press)
        self.timer_id = self.interactor.CreateRepeatingTimer(self.frame_rate)

    def create_skybox(self):
        """
        Create the skybox of the starfield using images from
        https://svs.gsfc.nasa.gov/4851
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
        skybox.SetVisibility(True) # On by default

        self.renderer.AddActor(skybox)
        return skybox


class VirgoDataPlayback:
    """
    Class used as entrypoint for those wanting VIRGO data playback capability.

    Expects to consume a dict describing the scene and a Trick RUN directory
    containing logged data this scene will be driven by. The linkage between the
    data recording groups/variables and the actors in the scene is controlled by
    the  recorded_data: and driven_by: clauses of the scene dict
    """
    def __init__(self, run_dir, scene, verbosity=1):
        self.verbosity = verbosity
        self.fs = 14      # font size
        self.max_sim_time = 0.0
        self.scene = scene # Dict of scene info from YAML file
        self._verify_scene()
        self.run_dir = run_dir
        self.nodes = {}

        self.background_color = [0.0, 0.0, 0.05]
        self.description = "Untitled"
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
        self.vdl = VirgoDataFileLoader(run_dir=self.run_dir, 
            scene_recorded_data=self.scene['recorded_data'], verbosity=self.verbosity)
        self.controller = VirgoDataPlaybackControlCenter(self.renderer, self.interactor, self.scene, self.vdl)
        self.initialized = False

    def add_node(self, node, parent_name=None):
        # TODO: This function requires us to add nodes root to leaf. We need
        # a more robust strategy to not require ordering of the actors by
        # the user, we can probably jsut process nodes in "no parent" first order
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

    def create_actor(self, actor_name, actor_scene_dict=None):
        """
        Given a single scene's actors: sub-entry, return a
        VirgoDataPlaybackActor instance built from that information

        TODO all this checking for dict/YAML validity needs to be safer, i.e.
        checked in a YAML verifier before getting here similar to what
        TrickOps does with TrickWorkflowYamlVerifier. See VirgoDataPlayback.
        _verify_scene()

        ASSUMPTION: self.scene['actors'][a]['mesh'] is path defined
        relative to current os.getcwd()

        Returns: VirgoDataPlaybackActor 
        """
        # TODO: This if pos / if pyr / if scale logic can be removed once
        #  a dict verifier is in place
        offset_pos=[0.0, 0.0, 0.0]
        offset_pyr=[0.0, 0.0, 0.0]
        scale = 1.0
        if 'pos' in actor_scene_dict:
            offset_pos=actor_scene_dict['pos']
        if 'pyr' in actor_scene_dict:
            offset_pyr=actor_scene_dict['pyr']
        if 'scale' in actor_scene_dict:
            scale=actor_scene_dict['scale']
        # Create the Actor
        if self.verbosity > 1:
            print(f"Constructing VirgotDataPlaybackActor {actor_name} ...")
        actor = VirgoDataPlaybackActor(
            mesh=actor_scene_dict['mesh'],
            offset_pos=offset_pos,
            offset_pyr=offset_pyr,
            name=actor_name,
            fontsize=self.fs
            )
        # TODO: create SceneNode here and add actor to it!
        if self.verbosity > 0:
            print(f"Initializing {actor_name} ...")

        #import pdb; pdb.set_trace()
        actor.SetScale(scale)
        if 'color' in actor_scene_dict:
            actor.GetProperty().SetColor(actor_scene_dict['color'])
        if 'pickable' in actor_scene_dict and actor_scene_dict['pickable'] == 0:
            actor.PickableOff()

        return actor

    def create_node(self, actor, actor_scene_dict=None):
        """
        Creates a VirgoSceneNode associated with actor from the information
        in actor_scene_dict. This function also maps the data in the
        driven_by: section of the actor_scene_dict to a data_source in
        the created node. The result is a VirgoSceneNode ready to be
        included in the larger VirgoDataPlayback framework via
        self.add_node()

        Returns: Tuple of (VirgoSceneNode, parent_name [str])
        """
        node = VirgoSceneNode(name=f"{actor.name}", actor=actor)
        # I've got to do some connecting around here
        # from self.vdl function calls create a
        # vds = VirgoDataSource()
        # Then use set_data_source to pass in vds
        # to each node
        node.get_axes().PickableOff()
        parent_name=None
        if 'parent' in actor_scene_dict and actor_scene_dict['parent'] != None:
            parent_name=actor_scene_dict['parent']

        # Figure out the details of whether the actor/node is driven or not
        # and produce a VirgoDataSource with the data from the driven_by:
        # specification and assign that data source to node.data_source
        positions = None
        rotations = None
        scales = None
        times = None
        driven_by = None
        if 'driven_by' in actor_scene_dict:
            driven_by= actor_scene_dict['driven_by']
            if 'time' in driven_by:
                times = self.vdl.get_recorded_datas(alias=driven_by['time'])
            if 'pos' in driven_by:
                positions= self.vdl.get_recorded_datas(alias=driven_by['pos'])
            if 'rot' in driven_by:
                rotations = self.vdl.get_recorded_datas(alias=driven_by['rot'])
            if 'scale' in driven_by:
                scales = self.vdl.get_recorded_data(alias=driven_by['scale'])
            # Create the data source
            vds = VirgoDataFileSource(times=times, rotations=rotations,
                                      positions=positions, scales=scales)
            vds.initialize()
            node.set_data_source(vds)
        else:
            node.set_static(True)

        return node, parent_name

    def create_trail(self, node, actor_scene_dict=None):
        trail_actor = None
        if 'trail' in actor_scene_dict and actor_scene_dict['trail'] != None:
            trail_actor = node.create_trail(color=actor_scene_dict['trail'])
            trail_actor.SetPickable(False)
        return trail_actor

    def initialize(self):
        '''
        Initialize this instance by:
        1. Loading all variables found in the scene
        2. Initializing all actors found in the scene
        2. Configuring the controller, renderer, and interactor
        '''
        self.vdl.load_variables()  # Load all variables from VirgoDataFileLoader
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

    def initialize_nodes(self):
        """
        Create the VirgoDataPlaybackActors and VirgoSceneNodes associated with
        actors from the scene dictionary and initialize their configurable
        parameters.

        The nodes initialized in this function are passed into self.controller
        so that they can be used at runtime.
        """
        actors = {}
        trail_actors = {}
        for a in self.scene['actors']:
            actors[a] = self.create_actor(actor_name=a, actor_scene_dict=self.scene['actors'][a])
            actors[a].initialize()

            node, parent_name = self.create_node(actor=actors[a], actor_scene_dict=self.scene['actors'][a])
            self.add_node(node, parent_name=parent_name)
            trail_actor = self.create_trail(node, actor_scene_dict=self.scene['actors'][a])
            if trail_actor:
                trail_actors[a] = trail_actor

        # Pass the actors into the controller for runtime use
        self.controller.set_actors(actors)
        self.controller.set_nodes(self.nodes)
        self.controller.set_trail_actors(trail_actors)


    def run(self):
        """
        The entrypoint for starting up a rendered window
        """

        if not self.controller.is_initialized():
            print("ERROR: Scene is not properly initialized. Exiting.")
            return(1)
    
        if self.verbosity > 0:
            print("Entering render window and interactor loop...")
        self.render_window.Render()
        self.interactor.Start()
        self.tear_down()
        return 0

    def tear_down(self):
        self.renderer.RemoveAllObservers()
        self.interactor.RemoveAllObservers()