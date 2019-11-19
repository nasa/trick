import { fade } from '@material-ui/core/styles';

const drawerWidth = 240;

export default theme => ({

  root: {
    display: 'flex',
  },

  //toolbar: {},

 // appbar: { width: `calc(100% - ${drawerWidth}px)`, marginLeft: drawerWidth },

  paper: {
    margin: '0 auto',
    marginTop: theme.spacing(20),
    marginBottom: theme.spacing(0),
    maxWidth: 800,
  },

  drawer: {
    width: drawerWidth,
    flexShrink: 0,
  },

  drawerPaper: {
    width: drawerWidth,
  },

  tv: {
    margin: '0 auto',
    marginTop: theme.spacing(5),
    marginBottom: theme.spacing(5),
    maxWidth: 800,
  },

  mockup: {
    margin: '0 auto',
    maxWidth: 800,
    padding: theme.spacing(3),
  },

  appbartitle: {
    marginLeft: theme.spacing(1),
  },

  tvtable: {
    margin: theme.spacing(3),
    paddingRight: theme.spacing(2),
    marginLeft: 0,
  },

  body: {
    marginTop: theme.spacing(20),
  },

  textfield: {
    margin: theme.spacing(1),
  },

  divider: {
    borderRight: '0.1em solid ' + theme.palette.divider,
    margin: theme.spacing(300),
  },

  button: {
    margin: theme.spacing(2),
  },

  timer: {
    margin: theme.spacing(2),
  },

  margin: {
    margin: theme.spacing(1),
  },

  dividermargin: {
    height: theme.spacing(1),
  },

  toolbar: theme.mixins.toolbar,

  appbar: {
    marginBottom: theme.spacing(20),
    zIndex: theme.zIndex.drawer + 1,
  },

  treeview: {
    margin: theme.spacing(2),
    marginTop: theme.spacing(5),
    padding: theme.spacing(1),
  },

  div: {},

  search: {
    position: 'relative',
    borderRadius: theme.shape.borderRadius,
    backgroundColor: fade(theme.palette.common.white, 0.15),
    '&:hover': {
      backgroundColor: fade(theme.palette.common.white, 0.25),
    },
    marginRight: theme.spacing(2),
    marginLeft: 0,
    width: '100%',
    [theme.breakpoints.up('sm')]: {
      marginLeft: theme.spacing(3),
      width: 'auto',
    },
  },

  searchIcon: {
    width: theme.spacing(7),
    height: '100%',
    position: 'absolute',
    pointerEvents: 'none',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
  },

  inputRoot: {
    color: 'inherit',
  },

  inputInput: {
    padding: theme.spacing(1, 1, 1, 7),
    transition: theme.transitions.create('width'),
    width: '100%',
    [theme.breakpoints.up('md')]: {
      width: 200,
    },
  },

});
