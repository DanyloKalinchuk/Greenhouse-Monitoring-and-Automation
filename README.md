# Greenhouse-Monitoring-and-Automation

# !The project is in progress. All the elements are not fully finished or not implemented yet, and can/might be changed in future!   

# Conventions
*In-project terms, objects or characteristics* (italic)   
**Out-of-project terms, objects or characteristics** (bold)   
***Key words*** (bold italic)   

# Overall Architecture
![Project Architecture](./resources/Greenhouse_Automation_Architecture.png)   

## Module Descriptions
### Sensor
A device composed of board with **ATmega328P** microcontroller, **nRF24L01** radio module and the set of *environment monitoring sensors*. It binds to an available *Edge Device* and sends sensor data each n-ms period (it is defined in '/src/sensor/sensor.ino' as a *SENSOR_DELAY* macros).   
It is written as an **Arduino sketch**, thus it requires **Arduino IDE** to be uploaded on a chip. I use **Seeeduino Nano**, but it should work with any other board with **ATmega328P** microcontroller

### Edge Device(referred to as '*master*' in code and project's file names)   
A Linux board with GPIOs and the Internet connection.   
Runs two processes:   
-   *SCADA*.   
-   *WEB*.
   
Those are connected via **Unix Socket** for sensor data exchange and configuration of environmental control logic parameters.
   
#### SCADA
The process written in C++ that handles Master-Sensors communication and the environmental control logic.   
Main tasks:   
-   Handles *Sensor* registration/initialization and receives data from them.
-   Sends last acquired data to the *Web process* on demand.
-   Keeps the environmental parameters accordingly to the configuration, received from the *Web process*.

#### WEB
The process written in python with the usage of **Django** framework.   
Main tasks:   
-   Receives data from *SCADA process* and saves it in the *Database*.
-   Handles *Web-App* logic.
-   Set configurations of environmental control logic in the *SCADA process* accordingly to the *Web-App* user input.

### Web Application (Web-App) and Database   
*Web-App* and *Database* handling logic implemented in the *Web process* of the *Edge Device* using **Django** framework.   
*Web-App* provides a user the access to the historical data stored in the *Database* and handles its visualisation. It also makes possible for a user to configure the environmental parameters the *SCADA process* should keep.   
*Database* stores historical data with sensor IDs and date and time of record creation.   
   
## Data Flow and Communication Logic

### General Overview   
The environmental data flows accordingly to the next sequence:   
*Sensor/s* **-->** *Master(SCADA)* **-->** *Master(Web)*   
Where *Master(SCADA)* is a passive element that listens for messages from *Sensor/s* and *Master(Web)* and sends data only if such a request occured (in case of *Sensor* initialization or data request from *Web process*).   
   
### *Sensor/s* **-->** *Master(SCADA)*
*Sensor* stores its unique *sensor ID* and *master ID* in its **EEPROM**, both values are 8-bit unsigned integers. By default the *master ID* is set to *MASTER_DEFAULT* value that expands to unsigned 0.   
If the value of *master ID* is set to default, a *Sensor* starts its initialization by sending its *sensor ID* to *"init_address"* that is listened by *Master*.    
*Master* checks if it already has an *inner ID* associated with the received *sensor ID* and if none, *Master* assigns one.  After that *Master* sends its own unique *ID* to the *Sensor*, which is the address the data should be sent to.   
When receiving data, *Master* organizes it in a *SENS_FRAME* structure (defined in 'src/master/scada/radio_comm/radio_comm.hpp') with the *Sensor's* *inner ID*.   
The received data triggers environmental handling and is stored to be sent on *Web process* demand.
   
### *Master(SCADA)* **-->** *Master(Web)*
*SCADA process* opens **Unix Socket** on its start. *Web process* connects to the opened socket, after that *SCADA process* starts listening for messages from *Web process*.   
There are two types of messages *Web process* can send:
-   *Request message*  
-   *Configuration message*   
   
*SCADA process* identifies the type of a message by the first value sent, which is unsigned 16-bit integer of value 1 for *Configuration message* and 2 for *Request message*.   
   
On a ***data request*** (received *Request message*), *SCADA process* goes through the list of last received *SENS_FRAME* structure for each sensor and gather the ***active*** ones.   
*Sensor* is identified as ***active*** if the last *SENS_FRAME* was received during the period of 2.5 *Sensor cycles* (1 cycle equals to the value of *SENSOR_DELAY*).   
*SCADA process* sends the number of the active sensors and then each of theirs *SENS_FRAME*.   
   
On a ***configuration request*** (received *Configuration message*), *SCADA process* reads 2 values for each environment parameter (*x_perf* and *x_error*) and sets corresponding values inside of *Actuator* objects.   