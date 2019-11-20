Trick provides the following graphical user interfaces:

### Simulation Control Panel

Send control commands to and view the status of a simulation.

### [Trick View](TrickView)

Browse and modify a simulation's variables while it's running. Launch integrated strip charts.

### [Events/Malfunctions Trick View](MalfunctionsTrickView)

Manage the events and malfunctions of a simulation.

### [Monte Monitor](MonteMonitor)

Monitor the status of a Monte Carlo simulation; create, start, and stop slaves.

### Managing External Applications

External applications are managed by instantiating and invoking calls on instances of launcher classes, which provide interfaces for setting up external applications from the input file or user model code. For instance, to manipulate Trick View from the input file:
```
trickView = trick.TrickView()
```

You now have an instance of the Trick View launcher class on which you can invoke calls to modify the actual Trick View application's behavior at launch. For instance, you could set the host and port:
```
trickView.set_host("localhost")
trickView.set_port(7000)
```

And then add it for automatic launching:
```
trick.add_external_application(trickView)
```

Provided launcher classes are derived from and listed in Trick::ExternalApplication. Some functionality is shared among launcher classes, and each class provides its own specific additional options. Trick allows any number of instances of any subclass of ExternalApplication. This means you could automatically launch two different Trick Views with completely separate settings (if you find that sort of thing useful).

### Automatically Launching Applications

Applications can be set to automatically launch during the initialization job phase by adding them to the queue of external applications managed by Trick. To do this, instantiate an instance of the appropriate launcher class (see above) and call Trick::ExternalApplication::add_external_application.

### Manually Launching Applications

Trick automatically launches all applications that have been added to its queue during simulation initialization. However, applications may also be manually launched via Trick::ExternalApplication::launch.

### Launch Command

Default commands suitable to launch each application are provided by their individual constructors. However, they may be changed, if desired, via Trick::ExternalApplication::set_startup_command.

### Launching Custom Applications or Commands

Having Trick automatically launch a custom application, or execute any command at all really, is a simple matter of instantiating an instance of the base class ExternalApplication and then setting the desired command as described above.
```
customApplication = trick.ExternalApplication()
customApplication.set_startup_command("echo Hello World!")
customApplication.set_arguments("")
trick.add_external_application(customApplication)
```

Note that ExternalApplication automatically appends the host and port argument unless you set the arguments by calling Trick::ExternalApplication::set_arguments.

Why would anyone want to have Trick automatically run additional commands? Well, for one thing, it saves you from manually running them each time you run a sim or writing a script to do it. But more importantly, applications managed by Trick are included in binary checkpoints, which means they can be saved and restored with no work on your part!

### Shared Options

As each Trick GUI was written by a different developer and few standards were in place, most options vary according to each GUI. The following apply to at least Trick View and Monte Monitor. Application-specific options can be passed from the input file or user model code via Trick::ExternalApplication::set_arguments or Trick::ExternalApplication::add_arguments.

#### Host

The host of the Variable Server to which the application will connect at launch can be specified via one of:

- From the command line, use the --host option.
- From the input file or user model code, use Trick::ExternalApplication::set_host

If no host is specified, it will automatically be set to that of the simulation which is launching this application.

#### Port

The port of the Variable Server to which the application will connect at launch can be specified via one of:

- From the command line, use the --port option.
- From the input file or user model code, use Trick::ExternalApplication::set_port

If no port is specified, it will automatically be set to that of the simulation which is launching this application.

#### Automatically Reconnect

The application can be configured to automatically reestablish lost connections via one of:

- From the command line, use the --autoReconnect option.
- From the input file or user model code, use Trick::ExternalApplication::set_auto_reconnect
- From the application, use the File->Settings menu.

The default value is false.

#### Window Size and Placement

The window's coordinates and dimensions at launch can be set via one of:
- From the command line, use the --x, --y, --width, or --height options.
- From the input file or user model code, use Trick::ExternalApplication::set_x, Trick::ExternalApplication::set_y, Trick::ExternalApplication::set_width, or Trick::ExternalApplication::set_height.

#### Cycle Period

The period (in seconds) at which the Variable Server sends information to the application can be specified via one of:

- From the command line, use the --cycle option.
- From the input file or user model code, use Trick::ExternalApplication::set_cycle_period.
- From the application, use the File->Settings menu.

The cycle period must be a non-negative number. Specify 0 to recieve updates at the maximum rate. Values below the minimum cycle period will be set to the minimum cycle period.

#### Minimum Cycle Period

The minimum period (in seconds) at which the Variable Server can be requested to send information to the application can be specified via one of:

- From the command line, use the --minCycle option.
- From the input file or user model code, use Trick::ExternalApplication::set_minimum_cycle_period.

The minimum cycle period must be a non-negative number. The recommended and default value is 0.1. Values below this may cause instability in Trick GUIs.

#### Application Behavior

The application can take one of several actions when it loses the connection with the simulation:

- Close
    Terminate and close.

- Notify
    Present a notification dialog describing the failure.

- Nothing
    Do nothing.

This behavior can be specified via one of:

- From the command line, use the --disconnectBehavior option.
- From the input file or user model code, use Trick::ExternalApplication::set_disconnect_behavior.
- From the application, use the File->Settings menu.

[Continue to Runtime Output](../Runtime-Output)
