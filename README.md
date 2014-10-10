Climactic
=========

Started as a project for a Java course.  Uses an Arduino, Java app, and PHP scripts to allow thermostat control via a webpage.


Basic System Design
-------------------
Bear in mind: this was to fulfill an assignment for a Java course.  If I ever have time to rewrite it, I probably won't use Java anywhere.  (RXTX has issues.) 

The Arduino provides I/O, both for data input from temp/humidity sensors and output to the relays.  When the Arduino receives a command (any command) over the serial connection, it responds with the current sensor values.  (Or something like that... it's been a while.) 

The Java application does all of the data manipulation and decision-making.  It takes the data, stores it, and makes (hopefully intelligent) decisions based on it.  Different threads handle different attributes of this process.  The Java application pushes commands over serial and parses the data that comes back.  It also responds to HTTP requests that are used by the PHP application. 
THe PHP portion provides the actual UI.  The webserver (IIS in my case) that serves the PHP files middle-mans commands regarding temperature.  

The Matlab portion does the graphs and is (currently) executed manually. 

The user accesses a webpage that is rendered by PHP.  The php server


Major Known Design Issues
------------------
- Exceptions are caught in try-catches where the catch doesn't actually do anything.  While this prevents the whole application from falling apart, some threads may die silently.  The Java application typically needs to be restarted about once a week. 
- It was rushed.  Stuff is sloppy. 
