import vtk
import re

class VirgoLabel():
    """
    A VirgoLabel is a string rendered in 3D space inside a Virgo Scene

    TODO: The self.label_follower is intended to always face the camera but
    we haven't hooked it up by running:
      self.label_follower.SetCamera(renderer.GetActiveCamera())
    but in my independent testing this feature doesn't work as intended when
    the vtkFollower() is inside an assembly. For now the label text is positioned
    and rendered with respect to the parent origin but we could explore fleshing
    out this true "always point at the camera" capability. I also tried an approach
    using vtkBillboardTextActor3D but that didn't work out well. See also
    VirgoDataPlaybackController.orient_labels_to_camera for a 3rd failed attempt
    """
    def __init__(self, name, text, scale=0.3):
        self.original_text = str(text) # Copy of original text field upon label creation
        self.name = name    # Name of this label
        self.scale = scale  # Size in units of the text in the scene
        self.label_text = vtk.vtkVectorText()
        label_mapper = vtk.vtkPolyDataMapper()
        label_mapper.SetInputConnection(self.label_text.GetOutputPort())
        self.label_follower = vtk.vtkFollower()
        self.label_follower.SetMapper(label_mapper)
        self.label_follower.SetScale(self.scale, self.scale, self.scale)
        self.label_follower.PickableOff()        # Labels can't be picked
        self.data_source = None # Is a VirgoDataSource if this label is driven by sim data
        self.variable = None    # variable name in self.data_source to use
        self.set_text(self.original_text)
        self.enabled = True
        self.disable()  # Turn it off by default

    def set_scale(self, scale):
        self.scale = scale

    def is_enabled(self):
        return self.enabled

    def disable(self):
        """Disable this label"""
        self.enabled = False
        self.label_follower.SetVisibility(False)

    def enable(self):
        """Enable this label"""
        self.enabled = True
        self.label_follower.SetVisibility(True)

    def get_follower(self):
        """Get the self.label_follower member"""
        return(self.label_follower)

    def set_text(self, text):
        """Set the text the label displays"""
        self.label_text.SetText(f"{text}")

    def set_position(self, position):
        """
        Set the position of the label (relative to it's parent)
        """
        self.label_follower.SetPosition(position)

    def set_yaw_pitch_roll(self, yaw_pitch_roll):
        """
        Set the yaw-pitch-roll rotation (Z-Y-X order)
        of the label (relative to it's parent)
        """
        rz, ry, rx = yaw_pitch_roll
        self.label_follower.RotateZ(rz)
        self.label_follower.RotateY(ry)
        self.label_follower.RotateX(rx)

    def set_color(self, color):
        """
        Set the color of the label text
        """
        self.label_follower.GetProperty().SetColor(color)

    def set_data_source(self, data_source):
        """Set the data source of the label

        A data_source of None is supported and means "no data source"

        Args:
            data_source (VirgoDataSource): Set self.data_source to this
        """
        self.data_source = data_source
        if self.data_source and not self.data_source.is_initialized():
            self.data_source.initialize()

    def update(self, world_time):
        """Update this label with variable information from the data source

        This is the function which updates text strings relying on a data
        source with actual values from that data source. Strings containing
        {<variable>:<format>} notation are parsed and <variable> is replaced
        with the value of <variable> at world_time per the VirgoDataSource
        internal mechanisms. Python formatting is then applied based on the
        <format> string. The result is appied to the text part of the label
        using set_text()
    
        Args:
            world_time (double): World time of the scene
        """
        if not self.enabled:
            return
        #import pdb; pdb.set_trace()
        if self.data_source == None:
            self.set_text(f"{self.original_text}")
            return
        self.data_source.set_current_time(world_time)
        if not self.data_source.is_there_additional_data():
            msg = (f"ERROR: self.data_source.additional_data is unexpectedly" 
                    " empty for label {self.name}!")
            raise RuntimeError (msg)

        text = str(self.original_text)
        found_clauses = self.find_clauses(text)
        for cs in found_clauses:
            value = self.data_source.get_additional_data(cs.var)
            raw_string = "{myval:" + cs.fmt + "}"  # 2 decimal places
            result = raw_string.format(myval=value)
            text = text.replace('{'+cs.clause+'}', result)

        self.set_text(text)

    @staticmethod
    def find_clauses(text):
        """Split a string in {<variable>:<format>} notation into clauses

        Given a string text, find all python format strings enclosed
        in curly braces and return a list of ClauseSplit instances
        representing the clause, variable, and format content
        separately.
         Example 1:
          text = "hello {myvar[0]}"
          split_format_strings(text) = [ cs0 ]
            where cs0 is a ClauseSplit with members:
              clause = myvar[0]
              var    = myvar[0]
              fmt    = ''
         Example 2:
          text = "hello {myvar[0]:.2}"
          split_format_strings(text) = [ cs0 ]
            where cs0 is a ClauseSplit with members:
              clause = myvar[0]:.2
              var    = myvar[0]
              fmt    = '.2'
         Example 3:
          text = "hello {myvar[0]:.2} {myvar[1]:.1f}"
          split_format_strings(text) = [ cs0, cs1 ]
            where cs0 is a ClauseSplit with members:
              clause = myvar[0]:.2
              var    = myvar[0]
              fmt    = '.2'
            and cs1 is a ClauseSplit with members:
              clause = myvar[1]:.1f
              var    = myvar[1]
              fmt    = '.1f

        A ClauseSplit class is local to the scope of this function and
        contains the original clause found, the variable part, and the
        format part. See class definition below.

        Args:
            text (str): String to search for clauses

        Returns:
            list: List of ClauseSplit class instances resulting from the
                  split of one or more clauses

        Raises:
            RuntimeError: If parsing of text fails
        """
        class ClauseSplit:
            """Container class for clause split"""
            def __init__(self, clause):
                self.clause = clause
                self.var = ''
                self.fmt = ''

        clauses = []
        found_fstrings = re.findall(r'\{(.*?)\}', text)
        if found_fstrings:
            # replace all {} fields with values from the data source
            for fstring in found_fstrings:
                cs = ClauseSplit(fstring)
                # Grab the value of the variable from the data source
                cs.fmt = ''
                sp = fstring.split(':')
                if len(sp) == 1:
                    cs.var = sp[0]
                elif len(sp) == 2:
                    var, fmt = sp
                    cs.var = var
                    cs.fmt = fmt
                else:
                    msg = (f"ERROR: Bad formatting in text: {text}. Simple "
                           "variable aliases and python format strings are "
                           "supported but mathematical operations are not")
                    raise RuntimeError(msg)
                clauses.append(cs)
        return(clauses)