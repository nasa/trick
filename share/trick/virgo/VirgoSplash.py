#!/usr/bin/env python3.11

import os
import vtk
import time

virgo_dir = os.path.dirname(os.path.abspath(__file__))

class VirgoSplash():
    """
    Class to manage Virgo splash screen
    """

    def __init__(self, render_window, interactor):
        """
        Constructor

        Args: 

            render_window (vtk.vtkRenderWindow):        Main VTK render window 
            interactor (vtk.vtkRenderWindowInteractor): Main VTK interactor

            image_actor (vtk.vtkImageActor):     Splash screen Actor
            overlay_renderer (vtk.vtkRenderer):  Renderer layer for the splash screen
            reader (vtk.vtkPNGReader):           Logo png

            logo (str):       Path to the png
            duration (float): Duration splash is visible (s)
            interval (int):   Interval opacity is updated (ms)

            start_time (float): Fade start time
            timer_id (int):     Timer ID 
        """ 

        self.image_actor = vtk.vtkImageActor()
        self.overlay_renderer = vtk.vtkRenderer()
        self.reader = vtk.vtkPNGReader()          

        self.logo = f"{virgo_dir}/logo.png"
        self.render_window = render_window
        self.interactor = interactor

        self.duration = 2.0
        self.interval = 50

        self.timer_id = None
        self.start_time = None


    def show_splash(self):
        """
        Render splash in an overlay
        """

        # Load logo png 
        self.reader.SetFileName(self.logo)
        self.reader.Update()

        # Image actor for splash
        self.image_actor.GetMapper().SetInputConnection(self.reader.GetOutputPort())
        self.image_actor.GetProperty().SetOpacity(1.0)

        # Overlay renderer for splash
        self.overlay_renderer.SetLayer(1)
        self.overlay_renderer.AddActor(self.image_actor)

        # Disable user interaction on splash
        self.overlay_renderer.InteractiveOff()

        # Cover the entire window
        self.overlay_renderer.SetViewport(0, 0, 1, 1)
        
        # Create new layer on Main VTK window for splash 
        self.render_window.SetNumberOfLayers(2)
        self.render_window.AddRenderer(self.overlay_renderer)
        
        # Decrease opacity over 2 second duration
        self.fade_splash()


    def fade_splash(self):
        """
        Create fading effect by decreasing the opacity at the specified interval,
        beginning at the specified duration. Remove the splash overlay when timer
        is complete.
        """
    
        # Trigger TimerEvent at interval
        timer_id = self.interactor.CreateRepeatingTimer(self.interval)

        start_time = time.time()
        
        # Attach "TimerEvent" observer to the interactor
        self.interactor.AddObserver("TimerEvent", lambda obj,event: (

            self.image_actor.GetProperty().SetOpacity(
                # At end of duration, set opacity to 0.0
                0.0 if (time.time() - start_time) >= self.duration

                # Decrease opacity from 1.0 to 0.0 over interval
                else 1.0 - (time.time() - start_time)/self.duration
            ),

            # Render the window again so the opacity change is visible 
            self.render_window.Render(),

            # Stop timer and remove overlay when fade is complete
            ( self.render_window.RemoveRenderer(self.overlay_renderer),
              self.render_window.Render(),
              self.interactor.DestroyTimer(timer_id))
            if (time.time() - start_time) >= self.duration else None
        ))
