# Internet of Things in Mining Industry

Internet of Things represents a general concept for the ability of network devices to sense and collect data from the world around us, and then share that data across the Internet where it can be processed and utilized for various interesting purposes.

## Benefits of IoT in Mining Industry 

 **IoT improving safety:**  *IoT can help prevent the collapse of unstable shafts for example, because the sensors will pick up real-time data, and predict the faulty equipment/where issues may occur, which allows for adjustments to be made before anything goes wrong.*

 **Energy and cost benefits:** *IoT reduces energy expenditure and maintenance cost for mining companies. By having a transparent system, where all parts are monitored. The cost spent on workers will reduce, allowing the industry to become more profitable.* 

 **Automation advances:** *By having standalone products, i.e. vehicles and equipment work together, more data is collected, which can improve accuracy rates.* 

 **Predictive Maintenance:** *Having a fully integrated network, monitoring every aspect of an operation becomes far easier, and only leads to greater productivity and safety. This includes detecting wear and tear on vital pieces of equipment, as well as projecting when repairs or maintenance is required.*

## Objective

The objective of the project was to design, implement and build a working model of wireless realtime data transmission from multiple sensors in a remote location to a specified web server host and monitor the sensor’s reading on an android smartphone and also store the data simultaneously.

### Design and Implementation 

The model consists of different Environmental Sensors. Initially, the Arduino MEGA is connected to aforementioned sensors to read the required data.  

The data from the sensors are read at a particular interval and send to serial, from there the data is read by XBee module and further transmitted to the receiving XBee pair. This XBee starts reading the parameters of sensors like p1, p2, p3 etc. The Arduino MEGA (on the receiving end) is connected to the internet through WiFi Shield. When the connection is established the sensor data are sent to the web server and simultaneously stored in SD Card attached to WiFi Shield.  

In the proposed model the Environmental Sensors is monitored. The data can be analyzed anywhere anytime.  

Preferably the user can monitor the data through dedicated android app. 

## Conclusion

The monitoring system using Internet of Things has been experimentally proven to work satisfactorily by connecting simple sensors to it and the readings were successfully monitored remotely through intranet. The designed system not only monitors the data from various Environmental Sensors but also stores the data regularly according to the requirement and format. 
