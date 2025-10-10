from Virgo import VirgoScene
from VirgoTrickpyFileLoader import VirgoTrickpyFileLoader
from VirgoDataSource import VirgoDataFileSource
from VirgoNode import VirgoSceneNode
from VirgoLabel import VirgoLabel
class VirgoDataPlayback(VirgoScene):
    """
    Class used as entrypoint for those wanting VIRGO data playback capability.

    Expects to consume a dict describing the scene and a Trick RUN directory
    containing logged data this scene will be driven by. The linkage between the
    data recording groups/variables and the actors in the scene is controlled by
    the  trickpy: and driven_by: clauses of the scene dict
    """
    def __init__(self, run_dir, scene, verbosity=1, headless=False,
                 images_dir="/tmp/", video_filename="/tmp/virgo.mp4"):
        super().__init__(scene=scene, verbosity=verbosity, headless=headless,
                         images_dir=images_dir, video_filename=video_filename)
        self.run_dir = run_dir
        # TODO: this check on 'data_source':'trickpy' existence might be
        # better done elsewhere but leaving it here for now
        #import pdb; pdb.set_trace()
        if 'data_source' in self.scene and 'trickpy' in self.scene['data_source']:
            self.vdl = VirgoTrickpyFileLoader(run_dir=self.run_dir, 
                trickpy_dict=self.scene['data_source']['trickpy'], verbosity=self.verbosity)

    def initialize(self):
        """
        Initialize this instance by:
        1. Loading all variables found in the scene
        2. Calling base class initialize()
        """
        if self.vdl:
          self.vdl.load_variables()  # Load all variables from VirgoTrickpyFileLoader
        super().initialize()

    def create_node(self, actor, actor_scene_dict=None, _class=VirgoSceneNode):
        """
        Creates a VirgoSceneNode associated with actor from the information
        in actor_scene_dict. This function also maps the data in the
        driven_by: section of the actor_scene_dict to a data_source in
        the created node. The result is a VirgoSceneNode ready to be
        included in the larger VirgoDataPlayback framework via
        self.add_node()

        Args:
          _class (cls): Class to instantiate, must be or derive from VirgoActor

        Returns: Tuple of (VirgoSceneNode, parent_name [str])
        """
        node, parent_name = super().create_node(actor=actor, actor_scene_dict=actor_scene_dict, _class=_class)

        # Figure out the details of how the actor/node is driven and produce a
        # VirgoDataSource with the data from the driven_by: specification and
        # assign that data source to node vis node.set_data_source.
        positions = None
        rotations = None
        scales = None
        opacities = None
        times = None
        driven_by = None
        if 'driven_by' in actor_scene_dict:
            driven_by= actor_scene_dict['driven_by']
            if 'time' in driven_by:
                times = self.vdl.get_alias_datas(alias=driven_by['time'])
            if 'pos' in driven_by:
                positions= self.vdl.get_alias_datas(alias=driven_by['pos'])
            if 'rot' in driven_by:
                rotations = self.vdl.get_alias_datas(alias=driven_by['rot'])
            if 'scale' in driven_by:
                scales = self.vdl.get_alias_data(alias=driven_by['scale'])
            if 'opacity' in driven_by:
                opacities = self.vdl.get_alias_data(alias=driven_by['opacity'])
            # Create the data source
            vds = VirgoDataFileSource(times=times, rotations=rotations,
                                      positions=positions, scales=scales,
                                      opacities=opacities )
            vds.initialize()
            node.set_data_source(vds)
        else:
            node.set_static(True)
        # If labels: are provided for the node/actor, 
        if 'labels' in actor_scene_dict:
            labels = actor_scene_dict['labels']
            for label in labels:
                # TODO: can we just node.get_label(label).get_text() here instead?
                if 'text' in labels[label]:
                  text = labels[label]['text']
                vds_for_labels = self.get_data_source_from_label_text(label_text=text)
                node.get_label(label).set_data_source(data_source=vds_for_labels)

        return node, parent_name

    def get_data_source_from_label_text(self, label_text):
        """
        Given a single text: field, parse it for variables in self.vdl using the
        special notation {<alias>}. For example:
          labels:
            velocity:
              text: "velocity: {sat_vel}"
              pos: [5.0, 5.0, 5.0]
        In this example text: contains a regular string and a section to be filled in from
        the sat_vel alias defined in the trickpy: section of the scene dict 

        returns a VirgoDataSource containing the variables/aliases found in label_text
        None is returned if no variables/aliases are found
        """
        vds_for_variables = None
        additional_datas = {}
        split_fstrings_list = VirgoLabel.find_clauses(label_text)
        for cs in split_fstrings_list:
            #import pdb; pdb.set_trace()
            if not self.vdl.does_alias_exist(alias=cs.var):
                msg = (f"ERROR: alias {cs.var} cannot be found in self.vdl!")
                raise RuntimeError (msg)
            # TODO: this is a loose check that could break, consider adding brace
            # dimension to ClauseSplit which could be queried here robustly
            if '[' in cs.var:
                additional_datas[cs.var] = self.vdl.get_alias_data(alias=cs.var)
            else:
                additional_datas[cs.var] = self.vdl.get_alias_datas(alias=cs.var)

            times = self.vdl.get_alias_datas(alias='time')
            vds_for_variables = VirgoDataFileSource(times=times, **additional_datas)
        return  vds_for_variables
