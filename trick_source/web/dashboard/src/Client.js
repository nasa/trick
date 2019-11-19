import React, { useState } from "react";
import AppBar from "@material-ui/core/AppBar";
import MuiBox from "@material-ui/core/Box";
import Toolbar from "@material-ui/core/Toolbar";
import Table from "@material-ui/core/Table";
import TableBody from "@material-ui/core/TableBody";
import TableCell from "@material-ui/core/TableCell";
import TableHead from "@material-ui/core/TableHead";
import TableRow from "@material-ui/core/TableRow";
import Paper from "@material-ui/core/Paper";
import TextField from "@material-ui/core/TextField";
import Typography from "@material-ui/core/Typography";
import Button from "@material-ui/core/Button";
import ExpansionPanel from "@material-ui/core/ExpansionPanel";
import ExpansionPanelSummary from "@material-ui/core/ExpansionPanelSummary";
import ExpansionPanelDetails from "@material-ui/core/ExpansionPanelDetails";
import ExpandMoreIcon from "@material-ui/icons/ExpandMore";
import { Flex, Box } from "reflexbox";
import TreeView from "@material-ui/lab/TreeView";
import ChevronRightIcon from "@material-ui/icons/ChevronRight";
import TreeItem from "@material-ui/lab/TreeItem";
import SearchIcon from "@material-ui/icons/Search";
import InputBase from "@material-ui/core/InputBase";

import Drawer from "@material-ui/core/Drawer";

import List from "@material-ui/core/List";

import Divider from "@material-ui/core/Divider";
import ListItem from "@material-ui/core/ListItem";
import ListItemIcon from "@material-ui/core/ListItemIcon";
import ListItemText from "@material-ui/core/ListItemText";
import InboxIcon from "@material-ui/icons/MoveToInbox";
import MailIcon from "@material-ui/icons/Mail";

import Dialog from "@material-ui/core/Dialog";
import DialogTitle from "@material-ui/core/DialogTitle";
import Radio from "@material-ui/core/Radio";
import RadioGroup from "@material-ui/core/RadioGroup";
import FormHelperText from "@material-ui/core/FormHelperText";
import FormControlLabel from "@material-ui/core/FormControlLabel";
import FormControl from "@material-ui/core/FormControl";
import FormLabel from "@material-ui/core/FormLabel";
//import OutlinedInput from '@material-ui/core/OutlinedInput';

//import MenuItem from '@material-ui/core/MenuItem';

//import Select from '@material-ui/core/Select';

export default class Client extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      variables: [],
      data: [],
      units: [],
      time: 0.0,
      sie: { top_level_objects: [] },
      children: {},
      addVariable: "",
      dialogOpen: false,
      dialogText: "",
      dialogDims: [],
      dialogElems: [],
      dialogStart: [],
      selectedVar: "",
      data_record: false,
      data_record_text: "Enable Data Record" /* TODO get dr info from sim */,
      realtime: true,
      realtime_text: "Realtime Enabled" /* TODO get dr info from sim */,
    };
    console.log("Client created");
    this.ws = new WebSocket(
      "ws://localhost:8888/api/ws/VariableServer",
      "myProtocol",
    );
    this.ws.onopen = e => {
      this.var_cycle(100);
      this.var_unpause();
      //this.var_add("trick_sys.sched.time_tics");
      console.log("opened");
      this.request_sie();
    };
    this.ws.onmessage = e => {
      //console.log('Message received');
      let message = JSON.parse(e.data);
      if (message.msg_type === "error") {
        console.log(message.error);
      }
      if (message.msg_type === "values") {
        this.setState({ time: message.time, data: message.values });
      }
      if (message.msg_type === "sie") {
        console.log(message);
        this.setState({ sie: message.data });
      }
      if (message.msg_type === "units") {
        console.log(message);
        let i = this.state.variables.indexOf(message.var_name);
        this.setState({
          units: [
            ...this.state.units.slice(0, i),
            message.data,
            ...this.state.units.slice(i + 1),
          ],
        });
      }
    };
  }

  send_msg = msg => {
    //console.log(JSON.stringify(msg));
    this.ws.send(JSON.stringify(msg));
  };

  var_add = v => {
    let msg = {
      cmd: "var_add",
      var_name: v,
    };
    this.send_msg(msg);
  };

  var_unpause = () => {
    let msg = {
      cmd: "var_unpause",
    };
    this.send_msg(msg);
  };

  var_cycle = p => {
    let msg = {
      cmd: "var_cycle",
      period: p,
    };
    this.send_msg(msg);
  };

  var_clear = () => {
    let msg = {
      cmd: "var_clear",
    };
    this.send_msg(msg);
  };

  addVariable = (path, units = "--") => {
    console.log("in add var");
    this.setState(prev => {
      return {
        variables: [...prev.variables, path],
        data: [...prev.data, 0.0],
        units: [...prev.units, units],
      };
    });
    this.var_add(path);
  };

  clearVariables = () => {
    this.setState({ variables: [], data: [] });
    this.var_clear();
  };

  input_processor = cmd => {
    let msg = {
      cmd: "python",
      pycode: cmd,
    };
    this.send_msg(msg);
  };

  sim_start = () => {
    this.input_processor("trick.exec_run()");
  };

  sim_freeze = () => {
    this.input_processor("trick.exec_freeze()");
  };
  sim_shutdown = () => {
    this.input_processor("trick.stop()");
  };

  sim_data_record_toggle = () => {
    /* TODO get dr info from sim */
    this.setState(prev => {
      if (!prev.data_record) {
        this.input_processor("trick.dr_enable()");
        return { data_record: true, data_record_text: "Data Record On" };
      } else {
        this.input_processor("trick.dr_disable()");
        return { data_record: false, data_record_text: "Data Record Off" };
      }
    });
  };

  sim_realtime_toggle = () => {
    /* TODO get dr info from sim */
    this.setState(prev => {
      if (!prev.realtime) {
        this.input_processor("trick.real_time_enable()");
        return { realtime: true, realtime_text: "Realtime On" };
      } else {
        this.input_processor("trick.real_time_disable()");
        return { realtime: false, realtime_text: "Realtime Off" };
      }
    });
  };

  request_sie = () => {
    let msg = {
      cmd: "sie",
    };
    this.send_msg(msg);
  };

  request_units = var_name => {
    let msg = {
      cmd: "units",
      var_name: var_name,
    };
    this.send_msg(msg);
  };

  sim_step = () => {
    /* TODO handle debug stepping mode
    this.input_processor('trick.debug_pause_on()');
      this.input_processor('trick.exec_run()');
      */
  };

  BoxButton = ({ text, color = "primary", fullWidth = true, onClick }) => (
    <Box flexGrow={1} flexShrink={1}>
      <Button
        className={this.props.classes.button}
        variant="contained"
        color={color}
        size="large"
        fullWidth={fullWidth}
        onClick={onClick}
      >
        {text}
      </Button>
    </Box>
  );

  BoxRadio = ({ className, color = "primary", fullWidth = true, onClick }) => (
    <Box flexGrow={1} flexShrink={1}>
      <FormControl component="fieldset">
        <FormLabel component="legend">Gender</FormLabel>
        <RadioGroup aria-label="gender" name="gender1" value={3}>
          <FormControlLabel value="female" control={<Radio />} label="Female" />
          <FormControlLabel value="male" control={<Radio />} label="Male" />
          <FormControlLabel value="other" control={<Radio />} label="Other" />
          <FormControlLabel
            value="disabled"
            disabled
            control={<Radio />}
            label="(Disabled option)"
          />
        </RadioGroup>
      </FormControl>
    </Box>
  );
  count = str => {
    const re = /\[.*?\]/g;
    return ((str || "").match(re) || []).length;
  };

  TVTreeItem = ({ name, path, type, dims = [], units = "--" }) => {
    const [expanded, setExpanded] = useState(true);
    const members = this.getMembers(type);
    let dimensionString = "";
    for (let dim of dims) {
      dimensionString += "[" + dim + "]";
    }

    let pathdims = path + dimensionString;
    // console.log("name: " + name + " path: " + path + " type: " + type);
    return (
      <TreeItem
        nodeId={pathdims ? pathdims : ""}
        label={name + dimensionString}
        onDoubleClick={e => {
          e.stopPropagation();
          this.setState({ selectedVar: path });
          if (dims.length === 0) {
            this.addVariable(pathdims, units);
            return;
          } else {
            console.log(String(pathdims.match(/\[(.*?)\]/g)));
            console.log(
              String(pathdims.match(/\[(.*?)\]/g))
                .replace(/\[|\]/g, "")
                .replace(/,/g, " "),
            );
            console.log(String(pathdims.match(/\[(.*?)\]/g)).match(/\d+/g));
            let Dims = String(pathdims.match(/\[(.*?)\]/g)).match(/\d+/g);
            let Elems = [];
            let Start = [];
            Dims.map((elem, index) => {
              Elems[index] = elem > 0 ? elem - 1 : 0;
              Start[index] = 0;
            });
            this.setState({
              dialogText: "Select elements to add to TV",
              dialogDims: Dims, //   String(this.count(pathdims)),
              dialogElems: Elems,
              dialogStart: Start,
              dialogOpen: true,
              dialogUnits: units,
            });
          }
        }}
      >
        {members
          ? members &&
            members.map(member => {
              return (
                <this.TVTreeItem
                  name={member.name}
                  path={[path, member.name].join(".")}
                  type={member.type}
                  dims={member.dimensions}
                  units={member.units}
                  key={[path, member.name].join(".")}
                />
              );
            })
          : null}
      </TreeItem>
    );
  };

  getMembers = type => {
    let varClass = this.state.sie.classes.find(
      element => element.name === type,
    );
    if (varClass !== undefined && varClass.members !== undefined) {
      return varClass.members;
    }
    // console.log("returning null");
    return null;
  };

  recurse = (path, arr1, arr2) => {
    if (arr1.length === 0) {
      console.log("add variable " + path);
      this.addVariable(path, this.state.dialogUnits);
    } else {
      for (let i = arr1[0]; i <= arr2[0]; i++) {
        let s = path + "[" + i + "]";
        this.recurse(s, arr1.slice(1), arr2.slice(1));
      }
    }
  };

  addArrayVariables = () => {
    let path = this.state.selectedVar;
    let arr1 = this.state.dialogStart;
    let arr2 = this.state.dialogElems;
    this.recurse(path, arr1, arr2);
  };

  render() {
    const { classes } = this.props;
    //console.log(classes);
    return (
      <div display="flex">
        <AppBar position="fixed" className={classes.appbar}>
          <Toolbar className={classes.toolbar}>
            <Flex alignItems="center" flexGrow={1}>
              <Box>
                <img src="TrickLogoSmall.png" alt="trick logo" height={45} />
              </Box>
              <Box>
                <Typography variant="h5" className={classes.appbartitle}>
                  Trick Dashboard
                </Typography>
              </Box>
              <Box flexGrow={1} />
              <Box flexDirection="column">
                <Typography variant="h6">
                  {"\u00A0"}
                  {"\u00A0"}
                  {"\u00A0"}
                </Typography>
              </Box>
              <Box>
                <Typography variant="h6">localhost:8888</Typography>
              </Box>
            </Flex>
          </Toolbar>
        </AppBar>
        <Flex>
          <Box>
            <Drawer
              className={classes.drawer}
              variant="permanent"
              classes={{
                paper: classes.drawerPaper,
              }}
              anchor="left"
            >
              <div className={classes.toolbar} />
              <Divider />
              <List>
              <ListItem key ="coming soon">
              <ListItemText primary="Other apps below coming soon!"/>
              </ListItem>
                {["Home", "Wiki", "Data Recording", "Strip Chart"].map(
                  (text, index) => (
                    <ListItem button key={text} disabled={true}>
                      <ListItemIcon>
                        {index % 2 === 0 ? <InboxIcon /> : <MailIcon />}
                      </ListItemIcon>
                      <ListItemText primary={text} />
                    </ListItem>
                  ),
                )}
              </List>
              <Divider />
              <List>
                {["Sniffer", "Environment", "Settings"].map((text, index) => (
                  <ListItem button key={text} disabled={true}>
                    <ListItemIcon>
                      {index % 2 === 0 ? <InboxIcon /> : <MailIcon />}
                    </ListItemIcon>
                    <ListItemText primary={text} />
                  </ListItem>
                ))}
              </List>
            </Drawer>
          </Box>
          <Flex flexDirection="column" flexGrow={1}>
            <Box margin={2}>
              <Paper className={classes.paper}>
                <Toolbar className={classes.toolbar}>
                  <Typography variant="h6" color="textPrimary">
                    Sim Control
                  </Typography>
                </Toolbar>
                <Flex justifyContent="space-between">
                  <Flex flexGrow={1}>
                    <Flex
                      flexDirection="column"
                      marginRight={25}
                      flexShrink={1}
                    >
                      <this.BoxButton text="step" onClick={this.sim_step} />
                      <this.BoxButton text="start" onClick={this.sim_start} />
                      <this.BoxButton text="freeze" onClick={this.sim_freeze} />
                      <this.BoxButton
                        text="shutdown"
                        color="secondary"
                        onClick={this.sim_shutdown}
                      />
                    </Flex>
                    <Flex flexDirection="column">
                      <Box flexGrow={1} flexShrink={1}>
                        <FormControl
                          component="fieldset"
                          fullWidth={true}
                          className={this.props.classes.button}
                        >
                          <FormLabel component="legend">Data Record</FormLabel>
                          <RadioGroup
                            key="radio1"
                            aria-label="datarecord"
                            name="datarecord"
                            value={this.state.data_record}
                            onChange={event => {
                              let data_record = event.target.value === "true";
                              if (data_record) {
                                this.input_processor("trick.dr_enable()");
                              } else {
                                this.input_processor("trick.dr_disable()");
                              }
                              this.setState({
                                data_record: data_record,
                              });
                            }}
                          >
                            <FormControlLabel
                              value={true}
                              control={<Radio />}
                              label="Enabled"
                            />
                            <FormControlLabel
                              value={false}
                              control={<Radio />}
                              label="Disabled"
                            />
                          </RadioGroup>
                        </FormControl>
                      </Box>
                      <Box flexGrow={1} flexShrink={1}>
                        <FormControl
                          component="fieldset"
                          fullWidth={true}
                          className={this.props.classes.button}
                        >
                          <FormLabel component="legend">Realtime</FormLabel>
                          <RadioGroup
                            key="radio2"
                            aria-label="realtime"
                            name="realtime"
                            value={this.state.realtime}
                            onChange={event => {
                              let realtime = event.target.value === "true";
                              if (realtime) {
                                this.input_processor("trick.real_time_enable()");
                              } else {
                                this.input_processor("trick.real_time_disable()");
                              }
                              this.setState({
                                realtime: realtime,
                              });
                            }}
                          >
                            <FormControlLabel
                              value={true}
                              control={<Radio />}
                              label="Enabled"
                            />
                            <FormControlLabel
                              value={false}
                              control={<Radio />}
                              label="Disabled"
                            />
                          </RadioGroup>
                        </FormControl>
                      </Box>
                      {/*<this.BoxButton text="dump checkpoint" />
                      <this.BoxButton
                        text="load checkpoint"
                        onClick={this.request_sie}
                      />*/}
                    </Flex>
                  </Flex>
                  <Flex
                    flexDirection="column"
                    flexGrow={3}
                    flexShrink={0}
                    margin={25}
                  >
                    <Box>
                      <TextField
                        variant="outlined"
                        label="Sim Time (s)"
                        value={this.state.time.toFixed(2)}
                        disabled={true}
                        className={classes.timer}
                      />
                    </Box>
                    <Box>
                      <TextField
                        variant="outlined"
                        label="playback speed"
                        value="not yet available"
                        className={classes.timer}
                        disabled={true}
                      />
                    </Box>
                  </Flex>
                </Flex>
                <ExpansionPanel>
                  <ExpansionPanelSummary expandIcon={<ExpandMoreIcon />}>
                    Status Messages
                  </ExpansionPanelSummary>
                  <ExpansionPanelDetails>
                    <Typography>
                      <MuiBox bgcolor="black" padding={1}>
                        {"Coming Soon"}
                      </MuiBox>
                    </Typography>
                  </ExpansionPanelDetails>
                </ExpansionPanel>
              </Paper>
            </Box>
            <Box margin={2}>
              <Paper className={classes.tv}>
                <Toolbar className={classes.toolbar}>
                  <Typography variant="h6" color="textPrimary">
                    Trick View
                  </Typography>
                  <div className={classes.search}>
                    <div className={classes.searchIcon}>
                      <SearchIcon />
                    </div>
                    <InputBase
                      placeholder="Search… [coming soon]"
                      disabled={true}
                      classes={{
                        root: classes.inputRoot,
                        input: classes.inputInput,
                      }}
                      inputProps={{ "aria-label": "search" }}
                    />
                  </div>
                </Toolbar>
                <Flex justifyContent="space-between">
                  <Flex flexDirection="Column">
                    <TreeView
                      className={classes.treeview}
                      defaultCollapseIcon={<ExpandMoreIcon />}
                      defaultExpandIcon={<ChevronRightIcon />}
                    >
                      {this.state.sie.top_level_objects.map(member => (
                        <this.TVTreeItem
                          name={member.name}
                          path={member.name}
                          type={member.type}
                          key={member.name}
                        />
                      ))}
                    </TreeView>
                  </Flex>
                  <Flex flexDirection="column">
                    <Box className={classes.dividermargin}></Box>
                    <Box className={classes.divider} flex={1}></Box>
                    <Box className={classes.dividermargin}></Box>
                  </Flex>
                  <Flex flexDirection="column" flexGrow={1}>
                    <Box flexGrow={1} paddingRight={50}>
                      <Table className={classes.tvtable}>
                        <TableHead>
                          <TableRow hover={true}>
                            <TableCell>Variable</TableCell>
                            <TableCell align="right">Value</TableCell>
                            <TableCell align="right">Units</TableCell>
                          </TableRow>
                        </TableHead>
                        <TableBody>
                          {this.state.variables.map(v => (
                            <TableRow key={v} hover={true}>
                              <TableCell>{v}</TableCell>
                              <TableCell align="right">
                                {this.state.data[
                                  this.state.variables.indexOf(v)
                                ] &&
                                this.state.data[this.state.variables.indexOf(v)]
                                  .toFixed
                                  ? this.state.data[
                                      this.state.variables.indexOf(v)
                                    ].toFixed(3)
                                  : this.state.data[
                                      this.state.variables.indexOf(v)
                                    ]}
                              </TableCell>
                              <TableCell align="right">
                                {this.state.units[
                                  this.state.variables.indexOf(v)
                                ] &&
                                this.state.units[
                                  this.state.variables.indexOf(v)
                                ].toFixed
                                  ? this.state.units[
                                      this.state.variables.indexOf(v)
                                    ].toFixed(3)
                                  : this.state.units[
                                      this.state.variables.indexOf(v)
                                    ]}
                              </TableCell>
                            </TableRow>
                          ))}
                        </TableBody>
                      </Table>
                    </Box>
                    <Flex flexDirection="row">
                      <Box>
                        <TextField
                          className={classes.textfield}
                          variant="outlined"
                          label="Add Variable"
                          value={this.state.addVariable}
                          onChange={ev => {
                            this.setState({ addVariable: ev.target.value });
                          }}
                          onKeyPress={ev => {
                            if (ev.key === "Enter") {
                              if (ev.target.value !== "") {
                                this.addVariable(ev.target.value);
                                this.request_units(ev.target.value);
                                this.setState({ addVariable: "" });
                              }
                              ev.preventDefault();
                            }
                          }}
                        ></TextField>
                      </Box>
                      <this.BoxButton
                        text="clear"
                        onClick={this.clearVariables}
                        fullWidth={false}
                      />
                    </Flex>
                  </Flex>
                </Flex>
              </Paper>
            </Box>
            {/*<Box>
              <Paper className={classes.mockup}>
                <Typography variant="h6" color="textPrimary">
                  This web application mostly serves as a mockup for now. Most
                  of the elements are not functional. The following buttons and
                  fields are currently functional: START, FREEZE, SHUTDOWN, Add
                  Variable, and CLEAR
                </Typography>
              </Paper>
            </Box> */}
          </Flex>
        </Flex>
        <Flex margin={2} padding={2}>
        <Box margin={2} padding={2}>
        <Dialog open={this.state.dialogOpen}>
          <DialogTitle>Select Dimensions</DialogTitle>
          {this.state.dialogText}
          {this.state.dialogElems.map((elem, index) => {
            return (
              <div>
                Dim {index}
                <TextField
                  className={classes.textfield}
                  label="start"
                  variant="outlined"
                  value={this.state.dialogStart[index]}
                  onChange={e => {
                    this.setState({
                      dialogStart: [
                        ...this.state.dialogStart.slice(0, index),
                        e.target.value,
                        ...this.state.dialogStart.slice(index + 1),
                      ],
                    });
                  }}
                />
                <TextField
                  label="end"
                  variant="outlined"
                  value={elem}
                  onChange={e => {
                    this.setState({
                      dialogElems: [
                        ...this.state.dialogElems.slice(0, index),
                        e.target.value,
                        ...this.state.dialogElems.slice(index + 1),
                      ],
                    });
                  }}
                />
              </div>
            );
          })}
          <Button
            onClick={() => {
              this.addArrayVariables();
              this.setState({ dialogOpen: false });
            }}
          >
            Add Variables
          </Button>
          <Button onClick={() => this.setState({ dialogOpen: false })}>
            Cancel
          </Button>
        </Dialog>
        </Box>
        </Flex>
      </div>
    );
  }
}
