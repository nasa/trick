#!/usr/bin/env python3.11
"""
VIRGO: Versatile Imaging and Rendering for Galactic Operations
A Practical, Analytical, and Hardworking 3D Visualization tool
leveraging python-VTK
"""
from VirgoActor import VirgoDataPlaybackActor
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
        self.actor = None
        self.renderer = None
        self.AddObserver("CharEvent", self.onChar)
        self.AddObserver(vtk.vtkCommand.EndInteractionEvent, self.StoreRelativeCameraInfo)
        self.relative_offset = None  # Positional offset between camera and followed actor
        self.view_up = None    # Camera GetViewUp() direction when self.realtive_offset was stored

    def set_actor(self, actor):
        self.actor = actor
    def set_renderer(self, renderer):
        self.renderer = renderer

    # Override base class onChar to ignore the 'e' key
    def onChar(self, obj, event):
        key = self.GetInteractor().GetKeySym()
        if key != 'e':  # Ignore 'e' key to prevent exit
            super().OnChar()

    def StoreRelativeCameraInfo(self, caller, event):
        '''
        Store off the relative offset between the followed actor and the camera
        in self.relative_offset. This is called as a callback tied to
        vtk.vtkCommand.EndInteractionEvent which lets us store the camera information
        relative to self.actor when the interaction with the mouse completes

        TODO: I'm not convinced this callback needs to live in the interactor,
            it might be better placed in the bigger Controller class especially
            since we aren't really overriding any methods of
            vtkInteractorStyleTrackballCamera in this approach
        '''
        if not self.actor or not self.renderer:
            return
        # Update relative offset after user interaction
        camera = self.renderer.GetActiveCamera()
        matrix = self.actor.GetMatrix()
        actor_pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
        # Store off the relative offset of camera
        self.relative_offset = np.array([camera.GetPosition()[i] - actor_pos[i] for i in range(3)])
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
        self.picker = vtk.vtkPropPicker()
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
          * Initializing all actors
          * Adding all actors to the renderer
          * Initializing the camera
        """
        self.init_actors()
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
        for a in self.actors:
            # Add the actor
            self.renderer.AddActor(self.actors[a])
            # Add it's axes
            self.renderer.AddActor(self.actors[a].get_axes())
        for a in self.trail_actors:
            self.renderer.AddActor(self.trail_actors[a])
        for t in self.text_actors:
            self.renderer.AddActor(self.text_actors[t])

        self.init_camera()
        self._initialized = True

    def is_initialized(self):
        return self._initialized

    def get_verbosity(self):
        return self.verbosity

    def set_verbosity(self, value):
        if not isinstance(value, int):
            print("ERROR: verbosity must be an integer")
        else:
            self.verbosity = value


    def init_actors(self):
        '''
        Create the VTK actors from the scene dictionary and initialize
        their configurable parameters

        Populates self.actors dict from self.scene dict
        self.actors takes the form:
          key: Name of actor
          value: VirgoDataPlaybackActor object as loaded from self.scene data

        TODO all this checking for dict/YAML validity needs to be safer, i.e.
        checked in a YAML verifier before getting here similar to what
        TrickOps does with TrickWorkflowYamlVerifier. See VirgoDataPlayback.
        _verify_scene()

        ASSUMPTION: self.scene['actors'][a]['mesh'] is path defined
        relative to current os.getcwd()
        '''
        for a in self.scene['actors']:
            # Figure out the details of whether the actor is driven or not to prepare
            # inputs for initializing the actor
            positions = None
            rotations = None
            times = None
            driven_by= self.scene['actors'][a]['driven_by']
            if driven_by != 0:
                times = self.vdl.get_recorded_data(alias=driven_by['time'])
                # Store off the highest simulation time across all actors
                if times[-1] > self.max_sim_time:
                    self.max_sim_time = times[-1]
                if 'pos' in driven_by:
                    positions= self.vdl.get_recorded_datas(alias=driven_by['pos'])
                if 'rot' in driven_by:
                    rotations = self.vdl.get_recorded_datas(alias=driven_by['rot'])
            # Create the Actor
            if self.verbosity > 1:
                print(f"Constructing {a} ...")
            self.actors[a] = VirgoDataPlaybackActor(
                mesh=self.scene['actors'][a]['mesh'],
                offset_pos=self.scene['actors'][a]['pos'],
                offset_pyr=self.scene['actors'][a]['pyr'],
                times=times,
                positions=positions,
                rotations=rotations,
                name=a,
                fontsize=self.fs
                )
            if driven_by == 0: # If nothing drives this, set static flag to True
                self.actors[a].set_static(True)
            if 'trail' in self.scene['actors'][a] and self.scene['actors'][a]['trail'] != None:
                self.trail_actors[f"{a}-trail"] = self.actors[a].create_trail(color=self.scene['actors'][a]['trail'])
            if self.verbosity > 0:
                print(f"Initializing {a} ...")
            self.actors[a].initialize()
            self.actors[a].SetScale(self.scene['actors'][a]['scale'])
            if 'color' in self.scene['actors'][a]:
                self.actors[a].GetProperty().SetColor(self.scene['actors'][a]['color'])
            if 'pickable' in self.scene['actors'][a] and self.scene['actors'][a]['pickable'] == 0:
                self.actors[a].PickableOff()
            self.actors[a].add_axes()  # origin axes
            self.actors[a].get_axes().PickableOff()

        self.update_actors()

    def create_text_actor(self, pos=[0, 0]):
        text = vtk.vtkTextActor()
        text.GetTextProperty().SetFontFamilyToCourier()
        text.GetTextProperty().SetFontSize(self.fs)
        text.GetTextProperty().SetColor(1, 1, 1)
        text.SetDisplayPosition(pos[0], pos[1])
        return text

    def update_actors(self):
        '''
        Move actors into position based on world time compared to data
        '''
        for actor in self.actors:
            self.actors[actor].update(self.world_time)

    def reset_trails(self):
        '''
        Reset all actor trails to no points, no lines
        '''
        for actor in self.actors:
            self.actors[actor].reset_trail()

    def update_scene(self):

        if self.mode == 'PLAYING':
          if self.world_time <= self.max_sim_time:
            self.world_time += self.dt
            self.update_actors()
            if self.camera_follows:
                self.camera_follow(self.camera_follows)
          else:
             self.world_time = self.world_time_start
             self.reset_trails()

        self.configure_hud()
        # Toggle axes labels based on distance
        for actor in self.actors:
            self.actors[actor].hide_or_unhide_axes_labels(self.camera)
            self.actors[actor]._axes.Modified()
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
            matrix = actor.GetMatrix()
            # Extract translation (elements 3, 7, 11 in 4x4 matrix)
            pos =[ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)]
            name = self.get_actor_name(actor)
            self.text_actors['picked'].SetInput(
                f"{name}\n World position: {pos[0]:<10.5f}, {pos[1]:<10.5f}, {pos[2]:<10.5f} meters"
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
                f"\n  -  + : Embiggen/Ensmallen text"
                f"\n A: Toggle Axes Visibility"
                f"\n H: Toggle this help message"
                f"\n BackSpace: Toggle stars"
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

    def camera_follow(self, actor):
        """
        Move the camera to follow the given actor
        Assumes actor is positioned with matrix data, not position data
        """
        # Ask the interactor for the latest camera position
        self.camera_follow_offset = self.interactor.GetInteractorStyle().GetRelativeCameraInfo('position')
        # If it's not set, compute the initial offset from the bounding box of the actor
        if self.camera_follow_offset is None:
            # Get the bounding box of the actor
            bounds = actor.GetBounds()
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

        matrix = actor.GetMatrix()
        pos =np.array([ matrix.GetElement(0, 3),  matrix.GetElement(1, 3), matrix.GetElement(2, 3)])
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

        self.picked_actor = self.picker.GetActor()
        if self.picked_actor and self.picked_actor in self.actors.values():
            self.configure_hud()
            self.focus_camera_on(self.picked_actor)
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

    def toggle_axes(self):
        for a in self.actors:
            if self.actors[a].are_axes_visible():
                self.actors[a].hide_axes()
                if self.verbosity > 1:
                    print(f'Hiding axes for {a}')
            else:
                self.actors[a].show_axes()
                if self.verbosity > 1:
                    print(f'Showing axes for {a}')

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
            self.update_actors()
        if key == "Right" or key == 'period':
            self.mode = 'PAUSED'
            self.increment_time()
            self.update_actors()
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
        for a in self.actors:
            pt = self.actors[a].get_previous_time(self.world_time)
            if pt is None:
                continue
            if pt > largest_time:
                largest_time = pt
        # Decrease the world time
        if largest_time > - self.huge:
            #print(f"Setting (decrement) world_time to {largest_time}")
            self.world_time = largest_time
            for a in self.actors:
                self.actors[a].set_current_time(self.world_time, strategy='closest')
                
    def increment_time(self):
        """
        Increment world time by the minimum resolution across all actors logged data

        TODO: This has not been tested with multiple data record groups logging at
        specific rates.
        """
        smallest_time = self.huge # Huge
        for a in self.actors:
            nt = self.actors[a].get_next_time(self.world_time)
            if nt is None:
                continue
            if nt < smallest_time:
                smallest_time = nt
        # Decrease the world time
        if smallest_time < self.huge:
            #print(f"Setting (increment) world_time to {smallest_time}")
            self.world_time = smallest_time
            for a in self.actors:
                self.actors[a].set_current_time(self.world_time, strategy='closest')

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

        # If the camera is setup to follow an actor, save that actor in
        # self.camera_follows and pass that actor into the Interactor so
        # it can track camera information relative to an actor
        if 'camera' in self.scene and 'follow' in self.scene['camera']:
            for actor in self.actors:
                if self.actors[actor].name == self.scene['camera']['follow']:
                    self.camera_follows = self.actors[actor]
                    self.interactor.GetInteractorStyle().set_actor(self.camera_follows)
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

class VirgoDataLoader:
    """
    Class that manages loading data from the scene recorded_data: section
    Uses trickpy to load the data, then puts it into a format best suited
    for consumption in the Virgo framework
    """
    def __init__(self, run_dir, scene_recorded_data, verbosity=1):
        self.run_dir=run_dir
        self.scene_recorded_data=dict(scene_recorded_data)
        self.verbosity=verbosity
        self.drg = None

    @staticmethod
    def expand_arrays(varname):
        """
        Given a string, check the string for array syntax ([ - ]). If the given variable
        specifies an array, return the string expanded into a list containing each array
        member.

        For consistency's sake, a list is always returned, even when the given string
        is not expanded.

        For example:
        input: varname="path.to.variable[0-2]"
        output: ["path.to.variable[0]", "path.to.variable[1]", "path.to.variable[2]"]

        input: "variable"
        output: ["variable"]

        Args:
            varname (string): A string containing the trick name corresponding to the
            given alias.
        """
        # Define a regex which finds any text that uses our format for
        # multiple elements in an array: ([ - ]).
        array_regex = re.compile(r"(?<=\[)\w+(?:-\w+)+(?=\])")
        new_varname = [varname]
        found_match = True
        while found_match:
            found_match = False
            for i, s in enumerate(new_varname):
                match = array_regex.search(s)
                if match:
                    if "-" in match.group():
                        found_match = True
                        start_index = int(match.group().split("-")[0])
                        stop_index = int(match.group().split("-")[1])
                        # Replace trick-name with list of each array element:
                        new_varname[i : i + 1] = [s[: match.start()] + str(ii) + s[match.end() :] for ii in range(start_index, stop_index + 1)]
                    else:
                        print("Regex expression error.")
        return new_varname

    def load_variables(self):
        """
        Use trickpy to load the variables defined in the 'recorded_data:' section
        of the scene

        Populates: self.drg
        Returns:   self.drg
        Raises: RuntimeError if a 'var' in 'recorded_data:' is not found when loading
        the self.run data

        TODO: Need uniqueness check, no var: can be repeated twice in the recorded_data: area
        """
        import trickpy.data_record as dr
        load_these_variables = []
        # Find every var: given in the scene dict and pass them to dr.load_run
        # as the subset of variables to load.
        for v in self.scene_recorded_data:
          expanded_vars = self.expand_arrays(self.scene_recorded_data[v]['var'])
          load_these_variables += expanded_vars
        if len(load_these_variables) == 0:
          msg = (f"ERROR: There are no variables in the recorded_data: section of scene. "
                f"Nothing to load! ")
          raise RuntimeError (msg)

        if self.verbosity > 0:
            print(f"Loading data from {self.run_dir}...")
            if self.verbosity > 1:
                for v in load_these_variables:
                    print(f"  {v}")
        self.drg = dr.load_run(self.run_dir, variables=load_these_variables)
        if self.verbosity > 0:
            print(f"Done.")

        # Make sure dr.load_run found all the vars we requested (trickpy doesn't error if
        # some were not found)
        expected_vars = list(load_these_variables)
        for group in self.drg:
          for var in self.drg[group]:
            if var in load_these_variables:
              expected_vars.remove(var)
              continue
        if len(expected_vars) > 0:
          msg = (f"ERROR: The following variables from recorded_data: section of scene "
                f"were not found in data loading of {self.run_dir}\n{expected_vars}")
          raise RuntimeError (msg)

        return (self.drg)

    def get_recorded_data(self, alias):
        """
        Get the recorded data associated with a single variable alias defined in
        recorded_data: dict. For example, for this YAML equivalent of a dict:

        recorded_data:
          time:                          # Variable Alias
            group: Satellite             # DR Group  
            var: sys.exec.out.time       # DR variable name
          sat_pos:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.pos[0-2]  # DR variable name
          sat_vel:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.vel[0-2]  # DR variable name

        For variables with index ranges ([0-2] or [0-2][0-2] for example) the
        indices must be specified such that a single variable is requested.

        For example get_recorded_data(alias='sat_pos[0]') will return a list of 
        dyn.satellite.pos[0] values associated with group Satellite.

        Returns: A list of data values associated with alias

        Raises: RuntimeError if unable to find data associated with alias or there's
          an issue with the indices specified.
        """
        import re
        if not self.drg:
          msg = (f"ERROR: Cannot get_recorded_data as self.drg is not populated.")
          raise RuntimeError (msg)
        indices_pattern = r"\[.*\]"
        specified_indices = ''
        # Search for the [] specified_indices in the alias given, and split it into
        # alias=(alias with no specified_indices), specified_indices=(specified_indices) where specified_indices is
        # None if not given
        match = re.search(indices_pattern, alias)
        if match:
            specified_indices = match.group(0)
            alias = alias.split(specified_indices)[0]

        if alias not in self.scene_recorded_data:
          msg = (f"ERROR: {alias} not found in recorded_data section of scene. Make sure to use"
                 " the alias name, not the var: name.")
          raise RuntimeError (msg)

        variable = self.scene_recorded_data[alias]['var']
        # Search for the [] var_indices in the var associated with the alias
        # given, and split it into variable=(variable with no var_indices),
        # var_indices=(var_indices) where var_indices is None if not given
        match = re.search(indices_pattern, variable)
        var_indices = ''
        if match:
            var_indices = match.group(0)
            variable = variable.split(var_indices)[0]
        # TODO: This check may not be robust to two-dimensional arrays, if alias=rot[0] for
        # example but the underlying var is defined as rotation[0-2][0-2]
        if specified_indices == '' and var_indices != '':
          msg = (f"ERROR: {alias} is multidimensional. Specify indices to request a single " 
                 "variable instead, ex: pos[0] instead of pos, rot[1][2] instead of rot")
          raise RuntimeError (msg)
        grp = self.scene_recorded_data[alias]['group']
        return self.drg[grp][variable+specified_indices].tolist()
        
    def get_recorded_datas(self, alias):
        """
        Get recorded datas (more than one value at a time) associated with an
        alias defined in recorded_data: dict. For example, for this YAML
        equivalent of a dict:

        recorded_data:
          time:                          # Variable Alias
            group: Satellite             # DR Group  
            var: sys.exec.out.time       # DR variable name
          sat_pos:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.pos[0-2]  # DR variable name
          sat_vel:                       # Variable Alias
            group: Satellite             # DR Group
            var: dyn.satellite.vel[0-2]  # DR variable name

        This function returns entire sets of recorded data as lists of primitives
        arranged by array sizes of the data. For example:
           No array  (ex: time)      Returns a list of time values
           1-D array (ex sat_pos)    Returns a list of tuples where is tuple is of size
                                     associated with the 1-D array, e.g.
                                     [(x0, y0, z0), (x1, y1, z1) ... ]
           2-D array (ex sat_rot)    Returns a list of np.arrays of size associated with
                                     the 2-D array, e.g.
                                     [ 
                                       np.array((R0[0][0], R0[0][1], R0[0][2]),
                                                (R0[1][0], R0[1][1], R0[1][2]),
                                                (R0[2][0], R0[2][1], R0[2][2])),
                                       np.array((R1[0][0], R1[0][1], R1[0][2]),
                                                (R1[1][0], R1[1][1], R1[1][2]),
                                                (R1[2][0], R1[2][1], R1[2][2])),
                                       ...

                                       ]

        Returns: A list of data values associated with alias

        Raises: RuntimeError if unable to find data associated with alias or there's
          an issue with the indices specified.
        """
        datas = []
        # This function doesn't want indices, if we detect them, error out
        indices_pattern = r"\[.*\]"
        match = re.search(indices_pattern, alias)
        if match:
            msg = (f"ERROR: Indices found in {alias}. Do not include indices [] when calling "
                   "get_recorded_datas()")
            raise RuntimeError (msg)
        # Get the array indices defintion from the variable in the scene
        grp = self.scene_recorded_data[alias]['group']
        variable = self.scene_recorded_data[alias]['var']

        # Expand the variables to see how many array indices there are
        expanded_variables = self.expand_arrays(variable)
        # Search for the [] var_indices in the var associated with the alias
        # given, and split it into variable=(variable with no var_indices),
        # var_indices=(var_indices) where var_indices is None if not given
        match = re.search(indices_pattern, variable)
        var_indices = ''
        if match:
            var_indices = match.group(0)
            variable = variable.split(var_indices)[0]
        # We can assume the data type by looking at expansion of the variable notation
        # [0-2] means a size 3
        # [0-2][0-2] means a size 3x3 aka 9
        if len(expanded_variables) == 1:
            datas = self.drg[grp][variable].tolist()
        elif len(expanded_variables) == 3:
            datas = list(zip(
                self.drg[grp][variable+'[0]'].tolist(),
                self.drg[grp][variable+'[1]'].tolist(),
                self.drg[grp][variable+'[2]'].tolist()))
        elif len(expanded_variables) == 9:
            tidx = 0
            list_length = len(self.drg[grp][variable+'[0][0]'])
            for i in range(0, list_length):
                datas.append( np.array(
                [  [ self.drg[grp][variable+'[0][0]'][i], self.drg[grp][variable+'[0][1]'][i], self.drg[grp][variable+'[0][2]'][i] ],
                   [ self.drg[grp][variable+'[1][0]'][i], self.drg[grp][variable+'[1][1]'][i], self.drg[grp][variable+'[1][2]'][i] ],
                   [ self.drg[grp][variable+'[2][0]'][i], self.drg[grp][variable+'[2][1]'][i], self.drg[grp][variable+'[2][2]'][i] ] ] )
                ) 
            pass

        #return self.drg[grp][variable+specified_indices].tolist()
        return datas


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
        self.scene = scene # Dict of scene info from YAML file
        self._verify_scene()
        self.run_dir = run_dir
        # All logged data from run_dir in the dict TrickPy gives us
        # TODO: we can save on RAM by loading on the variables we care
        # about which we can get from self.scene, see the variables arg to dr.load_run() here:
        # https://web-fsl.jsc.nasa.gov/trickpy/latest/docs/trickpy.data_record.load_run.html#trickpy.data_record.load_run

        self.vdl = VirgoDataLoader(run_dir=self.run_dir, 
            scene_recorded_data=self.scene['recorded_data'], verbosity=self.verbosity)
        # TODO: change load_variables approach to returning objects we
        # control not the trickpy format, then link those to being used when VirgoActors
        # are created
        self.data_record_groups = self.vdl.load_variables()

    def _verify_scene(self):
        """
        TODO: Call a dict verifier here, similar to TrickWorkflowYamlVerifier
        """
        pass

    def run(self):
        """
        The entrypoint for starting up a rendered window
        """
        renderer = vtk.vtkRenderer()
        renderer.SetBackground(self.scene['background_color'])
    
        render_window = vtk.vtkRenderWindow()
        render_window.AddRenderer(renderer)
        render_window.SetSize(1920, 1080)
        render_window.SetWindowName(self.scene['description'])
    
        interactor = vtk.vtkRenderWindowInteractor()
        interactor.SetRenderWindow(render_window)
    
        # Set better camera interaction
        interactor_style = VirgoInteractorStyle()
        interactor.SetInteractorStyle(interactor_style)
    
        # Create controller
        controller = VirgoDataPlaybackControlCenter(renderer, interactor, self.scene, self.vdl)
        controller.set_verbosity(self.verbosity)
        controller.register_callbacks()
        controller.initialize()

        if not controller.is_initialized():
            print("ERROR: Scene is not properly initialized. Exiting.")
            return(1)
    
        if self.verbosity > 0:
            print("Entering render window and interactor loop...")
        render_window.Render()
        interactor.Start()
        return 0