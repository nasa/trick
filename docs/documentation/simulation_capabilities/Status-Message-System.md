
The Message Publisher publishes executive and/or model messages. A Message Subscriber gets the messages published by the Publisher.

### Message Publisher 

Trick creates one instance of the Message Publisher (in the `S_define` file). It is responsible for keeping track of all Message Subscribers,
and sending any message that is published to all of the Message Subscribers. A subscriber is made known to the Message Publisher by calling its
`::message_subscribe` routine (and conversely can be removed from the publisher by calling `::message_unsubscribe`).
Publishing a message that you want to be output by all subscribers is done by calling `::message_publish`.
If there are no subscribers, then publishing a message has no effect.

### Message Subscriber

There can be any number of Message Subscribers, whose job is to receive (and usually output) published messages. Trick automatically creates three Message Subscribers:
- `Trick::MessageCout` - outputs messages to the standard output stream
- `Trick::MessageFile` - outputs messages to a file named `send_hs` in the RUN directory
- `Trick::MessageTCDevice` - outputs messages to a socket at port 7200, used by the Simulation Control Panel for its Status Messages display

When you publish a message, it will be output by the three subscribers above.
A subscriber can be enabled / disabled at any time during simulation execution to output / ignore messages as desired.
The user may also add their own subscriber by creating a derived class from `Trick::MessageSubscriber`.

- `Trick::MessageThreadedCout` - outputs messages to the standard output stream asynchronously.

The `MessageThreadedCout` class is included with the simulation but not activated by default.  When activated messages will be written to the standard output stream like `MessageCout`, but internally we use a separate thread to do the writing.  This helps real-time performance.

To activate the `MessageThreadedCout` class, add these 2 lines to the input file.

```python
trick_message.mtcout.init()
trick.message_subscribe(trick_message.mtcout)
```

### User accessible routines

To publish a message:

```cpp
int ::message_publish(int level, const char * format_msg, ...) ;
int ::send_hs(FILE * fp, const char * format_msg, ...) ;
```

The level number can be any integer from 0 to 99. Trick has a few predefined levels (`Trick::MessagePublisher::MESSAGE_TYPE`) that it uses for publishing messages.
If the message subscriber's color is enabled (see below), then a particular colored message will be displayed for each of these levels:
- 0 - normal message, default color
- 1 - informational message, green
- 2 - warning message, yellow
- 3 - error message, red
- 10 - debug message, cyan

To subscribe / unsubscribe Trick's default subscribers (these call `::message_subscribe` / `::message_unsubscribe` mentioned above):

By default these are all subscribed. You can use subscribe/unsubscribe throughout a simulation to turn on/off messages at will.

```python
trick.message_unsubscribe(trick_message.mcout)
trick.message_unsubscribe(trick_message.mdevice)
trick.message_unsubscribe(trick_message.mfile)
```

To enable / disable Trick's default subscribers:

By default these are all enabled. If you disable `file` at startup, the `send_hs` file will not be created/opened. If you disable `device` at startup, its connection will not be established.
That means you cannot then later try to enable and subscribe to file/device.

```cpp
int message_cout_set_enabled(int yes_no) ;
int message_file_set_enabled(int yes_no) ;
int message_device_set_enabled(int yes_no) ;
```

To enable / disable showing colored messages in Trick's default subscribers:

By default color is enabled for `cout` and `device`. Color is achieved by adding ANSI escape codes around the message.

```cpp
int message_cout_set_color(int yes_no) ;
int message_file_set_color(int yes_no) ;
int message_device_set_color(int yes_no) ;
```

[Continue to Command Line Arguments](Command-Line-Arguments)

