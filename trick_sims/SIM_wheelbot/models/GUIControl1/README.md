# Graphics

**Contents**

* [class HomeButton](#class-HomeButton)<br>
* [class HomeDisplay](#class-HomeDisplay)<br>
* [class TrickSimMode](#class-TrickSimMode)<br>

---

<a id=class-HomeButton></a>
## class homeButton
extends [JPanel](https://docs.oracle.com/javase/7/docs/api/javax/swing/JPanel.html)
implements [ActionListener](https://docs.oracle.com/javase/10/docs/api/java/awt/event/ActionListener.html)

### Description

The HomeButton class represents a graphical button in an [HomeDisplay](#class-HomeDisplay). It utilizes ActionListener to record user input to the button.

| Access  | Member Name   | Type         | Units  | Value  |
|---------|---------------|--------------|--------|--------|
| private | homeButton1   |JButton       |   --   |        |
| private | homeCommand   |boolean       |   --   |        |
| private | label         |JLabel        |   --   |        |

### Constructor

```
public HomeButton();
```
Constructs the button with the appropriate label and adds it to the JPanel.

### Member Functions

```
public void actionPerformed(ActionEvent e);
```
Utilizes the ActionListener and record the user input in the form of a boolean variable homeCommand.

```
public void resetHomeCommand();
```
Resets the homeCommand variable to false.

```
public void getHomeCommand();
```
Returns the value of homeCommand.

---

<a id=class-TrickSimMode></a>
## class TrickSimMode

### Description

The TrickSimMode class declares variables for the state of the simulation.


<a id=class-HomeDisplay></a>
## class HomeDisplay
extends [JFrame](https://docs.oracle.com/en/java/javase/11/docs/api/java.desktop/javax/swing/JFrame.html)

### Description
This class implements a Trick variable server client application for SIM_wheelbot that displays the homing button.

#### Running the Client
```
java -jar HomeDisplay.jar <port>
```

| Access | Member Name     | Type             | Units  | Value  |
|--------|-----------------|----------------- |--------|--------|
| private | buttonPanel    | HomeButton       |   --   |        |
| private | in             | BufferedReader   |   --   |        |
| private | out            | DataOutputStream |   --   |        |

### Constructor

```
public HomeDisplay();
```
Initialize an JFrame with the HomeButton class.

### Member Functions

```
public void resetHomeCommand();
```
Extension of the resetHomeCommand function found in the HomeButton class.

```
public void getHomeCommand();
```
Extension of the getHomeCommand function found in the HomeButton class.

```
public void connectToServer(String host, int port );
```
Connect to the Trick variable server as specified by **host** and **port**.

```
private static void  printHelpText();
```

```
public static void main(String[] args);
```
* Process Args
* Validate Parameters
* Initialize HomeDisplay GUI
* Connect to the Trick variable server on the local computer ("localhost") at the port number specified by the applicaton arguments.
* Request the vehicle home variable and simulation mode.
* Enter a continuous while loop that reads, and correspondingly sets the vehicle state.

---
