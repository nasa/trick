import React from 'react';
import Client from './Client';
import { withStyles } from '@material-ui/core/styles';
import withRoot from './withRoot';
import styles from './AppStyles';

/*
function App(props) {
  const { classes } = props;	
  return (<Client classes={classes}></Client>);
}
*/

class App extends React.Component {
	render() {
	  const { classes } = this.props;
	  console.log(classes);	
	  return (<Client classes={classes}></Client>);
	}
}



export default withRoot(withStyles(styles)(App));