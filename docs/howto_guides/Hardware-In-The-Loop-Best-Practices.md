# Trick Hardware in the Loop Best Practices
 
 **Contents**
 
 * [Purpose](#Purpose)<br>
 * [Prerequisite Knowledge](#prerequisite-knowledge)<br>
 * [Do's, Don'ts and Wisdom](#guidelines)<br>
 
 <a id= introduction></a>
 
 ---
 ## Purpose
 The intention of this document is to compile and share practical knowledge, based on the experience of people in the Trick simulation community regarding the development of hardware in the loop computer simulations.
 
 <a id=prerequisite-knowledge></a>
 ## Prerequisite Knowledge
 
 * (Assuming you've completed the [Trick Tutorial](https://nasa.github.io/trick/tutorial/Tutorial))
 * (Assuming you've read the [Realtime Best Practices](https://nasa.github.io/trick/docs/howto_guides/Realtime-Best-Practices.md))
 
 ---
 ## The Main Take a Way
 
 The most important thing for a develper to know is <b>When What</b> has to be <b>Where</b>.
 
 * When is at what time is the data expected.
 * What is what data needs to be moved.
 * Where is the hardware that needs the data. 
 
 ### Trick Features
 #### Trick Child Threads are your Friends
 ##### Trick depends on statement.
 #### Trick Command Line Arguments
 #### Trick Exec Terminate
 #### Trick Var Server
 #### Trick can byte swap.
 #### Trick Com is just IP Com where they check the return codes.
 #### send_hs is your friend
 ### Logging
 #### Do you log statments have enough info to be useful.
 #### How many log levels are you actually using.
 ### Devloping
 #### Virtual interfaces are very handy.
 #### You should build your code in a Dynamic libaray (lib.so) file.
 #### Always check your return codes.
 #### Mock object for API failure. 
 ### From the Development envirement into the Lab
 #### DIO/AIO Becarful.
 #### The sim exit code can tell you if you are about to have a bad day!
 #### Never Give anyone the raw command for your sim. You should always wrap them in a script you control.
 #### Always keep a copy of your std_err and std_out. 
 #### Fault Tolerance vs Ready to Launch.
 #### Some things are crazy to do when a sim is in a development env, but are very useful in a lab.
 ##### Capture the git status and all the diff files. 
