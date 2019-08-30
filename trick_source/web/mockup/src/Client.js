import React from 'react';
import AppBar from '@material-ui/core/AppBar';
import MuiBox from '@material-ui/core/Box';
import Toolbar from '@material-ui/core/Toolbar';
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableHead from '@material-ui/core/TableHead';
import TableRow from '@material-ui/core/TableRow';
import Paper from '@material-ui/core/Paper';
import TextField from '@material-ui/core/TextField';
import Typography from '@material-ui/core/Typography';
import Button from '@material-ui/core/Button';
import ExpansionPanel from '@material-ui/core/ExpansionPanel';
import ExpansionPanelSummary from '@material-ui/core/ExpansionPanelSummary';
import ExpansionPanelDetails from '@material-ui/core/ExpansionPanelDetails';
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';
import { Flex, Box } from 'reflexbox';
import TreeView from '@material-ui/lab/TreeView';
import ChevronRightIcon from '@material-ui/icons/ChevronRight';
import TreeItem from '@material-ui/lab/TreeItem';
import SearchIcon from '@material-ui/icons/Search';
import InputBase from '@material-ui/core/InputBase';

import Drawer from '@material-ui/core/Drawer';

import List from '@material-ui/core/List';

import Divider from '@material-ui/core/Divider';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import InboxIcon from '@material-ui/icons/MoveToInbox';
import MailIcon from '@material-ui/icons/Mail';

import OutlinedInput from '@material-ui/core/OutlinedInput';

import MenuItem from '@material-ui/core/MenuItem';

import Select from '@material-ui/core/Select';

export default class Client extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      variables: [],
      data: [],
      unit: [],
      time: 0.0,
    };
    console.log('Client created');
    this.ws = new WebSocket(
      'ws://localhost:8888/api/ws/VariableServer',
      'myProtocol',
    );
    this.ws.onopen = e => {
      this.var_cycle(100);
      this.var_unpause();
      //this.var_add("trick_sys.sched.time_tics");
      console.log('opened');
    };
    this.ws.onmessage = e => {
      console.log('Message received');
      let message = JSON.parse(e.data);
      console.log(message);
      this.setState({ time: message.time, data: message.values });
    };
  }

  send_msg = msg => {
    //console.log(JSON.stringify(msg));
    this.ws.send(JSON.stringify(msg));
  };

  var_add = v => {
    let msg = {
      cmd: 'var_add',
      var_name: v,
    };
    this.send_msg(msg);
  };

  var_unpause = () => {
    let msg = {
      cmd: 'var_unpause',
    };
    this.send_msg(msg);
  };

  var_cycle = p => {
    let msg = {
      cmd: 'var_cycle',
      period: p,
    };
    this.send_msg(msg);
  };

  var_clear = () => {
    let msg = {
      cmd: 'var_clear',
    };
    this.send_msg(msg);
  };

  addVariable = v => {
    this.setState({
      variables: [...this.state.variables, v],
      data: [...this.state.data, 0.0],
    });
    this.var_add(v);
  };

  clearVariables = () => {
    this.setState({ variables: [], data: [] });
    this.var_clear();
  };

  input_processor = cmd => {
    let msg = {
      cmd: 'python',
      pycode: cmd,
    };
    this.send_msg(msg);
  };

  sim_start = () => {
    this.input_processor('trick.exec_run()');
  };

  sim_freeze = () => {
    this.input_processor('trick.exec_freeze()');
  };
  sim_shutdown = () => {
    this.input_processor('trick.stop()');
  };
  sim_step = () => {
    /* TODO handle debug stepping mode
    this.input_processor('trick.debug_pause_on()');
      this.input_processor('trick.exec_run()');
      */
  };

  BoxButton = ({ text, color = 'primary', fullWidth = true, onClick }) => (
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

  render() {
    const { classes } = this.props;
    //console.log(classes);
    return (
      <div display="flex">
        <AppBar position="fixed" className={classes.appbar}>
          <Toolbar className={classes.toolbar}>
            <Flex alignItems="center" flexGrow={1}>
              <Box>
                <img src="TrickLogoSmall.png" height={45} />
              </Box>
              <Box>
                <Typography variant="h5" className={classes.appbartitle}>
                  Trick Dashboard
                </Typography>
              </Box>
              <Box flexGrow={1} />
              <Box flexDirection="column">
                <Typography variant="h6">
                  SIM_cannon_numeric{'\u00A0'}
                  {'\u00A0'}
                  {'\u00A0'}
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
                {['Home', 'Wiki', 'Data Recording', 'Strip Chart'].map(
                  (text, index) => (
                    <ListItem button key={text}>
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
                {['Sniffer', 'Environment', 'Settings'].map((text, index) => (
                  <ListItem button key={text}>
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
                      <this.BoxButton text="data record on" />
                      <this.BoxButton text="realtime on" />
                      <this.BoxButton text="dump checkpoint" />
                      <this.BoxButton text="load checkpoint" />
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
                        value="1.0"
                        className={classes.timer}
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
                        {
                          '|L   1|2019/08/06,09:29:13|thanos| |T 0|1.500000| Freeze OFF.'
                        }
                        <br />
                        {
                          '|L  10|2019/08/06,09:29:14|thanos| |T 0|3.200000| 0x7f6950000fd0 tag=<SimControl> var_server received: trick.exec_freeze()'
                        }
                        <br />
                        <br />
                        {
                          '|L   1|2019/08/06,09:29:14|thanos| |T 0|3.300000| Freeze ON. Simulation time holding at 3.300000 seconds.'
                        }
                        <br />
                        {
                          '|L  10|2019/08/06,09:29:21|thanos| |T 0|3.300000| 0x7f6950000fd0 tag=<SimControl> var_server received: trick.exec_run()'
                        }
                        <br />
                        <br />
                        {
                          '|L   1|2019/08/06,09:29:21|thanos| |T 0|3.300000| Freeze OFF.'
                        }
                        <br />
                        {
                          '|L  10|2019/08/06,09:29:23|thanos| |T 0|4.600000| 0x7f6950000fd0 tag=<SimControl> var_server received: trick.exec_freeze()'
                        }
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
                      placeholder="Search…"
                      classes={{
                        root: classes.inputRoot,
                        input: classes.inputInput,
                      }}
                      inputProps={{ 'aria-label': 'search' }}
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
                      <TreeItem nodeId="1" label="Trick Variables">
                        <TreeItem nodeId="2" label="trick_cmd_args">
                          <TreeItem nodeId="3" label="dummy data" />
                        </TreeItem>
                        <TreeItem nodeId="4" label="trick_sys">
                          <TreeItem nodeId="5" label="dummy data" />
                        </TreeItem>
                        <TreeItem nodeId="6" label="trick_ip">
                          <TreeItem nodeId="7" label="dummy data" />
                        </TreeItem>
                        <TreeItem nodeId="8" label="trick_vs">
                          <TreeItem nodeId="9" label="dummy data" />
                        </TreeItem>
                      </TreeItem>
                      <TreeItem nodeId="10" label="Dynamic Allocations">
                        <TreeItem nodeId="11" label="dyn_integloop">
                          <TreeItem nodeId="12" label="name" />
                          <TreeItem nodeId="13" label="id" />
                          <TreeItem nodeId="14" label="integ_sched">
                            <TreeItem nodeId="15" label="dummy data" />
                          </TreeItem>
                        </TreeItem>
                      </TreeItem>
                      <TreeItem nodeId="16" label="dyn">
                        <TreeItem nodeId="17" label="name" />
                        <TreeItem nodeId="18" label="id" />
                        <TreeItem nodeId="19" label="cannon">
                          <TreeItem nodeId="20" label="vel0[2]" />
                          <TreeItem nodeId="21" label="pos0[2]" />
                          <TreeItem nodeId="22" label="init_speed" />
                          <TreeItem nodeId="23" label="init_angle" />
                          <TreeItem nodeId="24" label="g" />
                          <TreeItem nodeId="25" label="acc[2]" />
                          <TreeItem nodeId="26" label="vel[2]" />
                          <TreeItem nodeId="27" label="pos[2]" />
                          <TreeItem nodeId="28" label="time" />
                          <TreeItem nodeId="29" label="timeRate" />
                          <TreeItem nodeId="30" label="impact" />
                          <TreeItem nodeId="31" label="impactTime" />
                          <TreeItem nodeId="32" label="rf">
                            <TreeItem nodeId="33" label="dummy data" />
                          </TreeItem>
                          <TreeItem nodeId="34" label="connection">
                            <TreeItem nodeId="36" label="connection">
                              <TreeItem nodeId="37" label="connection">
                                <TreeItem nodeId="38" label="connection">
                                  <TreeItem nodeId="39" label="connection">
                                    <TreeItem nodeId="40" label="connection">
                                      <TreeItem nodeId="41" label="connection">
                                        <TreeItem
                                          nodeId="35"
                                          label="dummy data"
                                        />
                                      </TreeItem>
                                    </TreeItem>
                                  </TreeItem>
                                </TreeItem>
                              </TreeItem>
                            </TreeItem>
                          </TreeItem>
                        </TreeItem>
                        <TreeItem nodeId="36" label="http">
                          <TreeItem nodeId="37" label="name" />
                          <TreeItem nodeId="38" label="id" />
                          <TreeItem nodeId="39" label="http_server">
                            <TreeItem nodeId="40" label="dummy data" />
                          </TreeItem>
                        </TreeItem>
                      </TreeItem>
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
                            <TableCell align="right">Unit</TableCell>
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
                                {this.state.unit[
                                  this.state.variables.indexOf(v)
                                ] &&
                                this.state.unit[this.state.variables.indexOf(v)]
                                  .toFixed
                                  ? this.state.unit[
                                      this.state.variables.indexOf(v)
                                    ].toFixed(3)
                                  : this.state.unit[
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
                          onKeyPress={ev => {
                            if (ev.key === 'Enter') {
                              if (ev.target.value !== '') {
                                this.addVariable(ev.target.value);
                                ev.target.value = '';
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
            <Box>
              <Paper className={classes.mockup}>
                <Typography variant="h6" color="textPrimary">
                  This web application mostly serves as a mockup for now. Most of the elements are not
                  functional. The following buttons and fields are currently functional: START, FREEZE, SHUTDOWN, Add
                  Variable, and CLEAR
                </Typography>
              </Paper>
            </Box>
          </Flex>
        </Flex>
      </div>
    );
  }
}
