# ROS2 Features

##  What is Distributed Data System (**DDS**):

In a FreeRTOS system, if Task A wants to send sensor data to Task B, it passes a pointer to a struct through a queue via shared memory.
In a distributed system, Nodes **do not share memory**. Task A (on the Raspberry Pi) might need to talk to Task B (running on the Laptop). **DDS** is the engine that allows for this to happen.
- **Global Data Space**:
    - Unlike traditional networking where you need to know a specific IP address, DDS creates a "Global Data Space." Nodes simply join a Domain ID. If two nodes are on the same domain, they can "see" each other automatically.
- DDS uses **Dynamic Discovery**. When you start a node:
    - It broadcasts its presence to the network.
    - Other nodes hear it and exchange information about what topics they are publishing or subscribing to.
    - They establish a direct peer-to-peer connection.

- **Quality of Service (QoS)**: Alows you to tune how data is handled based on the requirements of our robot:
    - Reliability: Should the system retry sending a packet if it's lost (Reliable), or is it okay to drop it to keep things fast (Best Effort)?
    - Durability: Should a new subscriber receive the "last known good" message sent before it joined (Transient Local), or only new messages?
    - History: How many messages should be stored in the queue?

- The Wire Protocol: The actual communication happens over a protocol called RTPS (Real-Time Publish-Subscribe), built on top of UDP.
---
## ROS2 NODES:

A Node is simply a single executable program (a process) that performs one specific task.

In FreeRTOS, this would be a single void `vTask(void *pvParameters)` running in an infinite loop.

In ROS 2, a Node is usually a C++ class that inherits from the ROS 2 library (rclcpp::Node).


Nodes are essentially isolated from one another until they use one of the three main communication "styles" provided by ROS 2:

- Topics (Publish/Subscribe): Continuous data streams. A camera node publishes frames; a vision node subscribes to them.

- Services (Request/Response): A quick "ask and answer" interaction. A node might ask, "What is the battery level?" and receive a response.

- Actions (Goal-based): For long-running tasks. 

`ros2 node list`: Shows you all the nodes currently "alive" on the network.

`ros2 node info /my_node`: Tells you exactly what topics and services that specific node is using.


### Topics (Publish/Subscribe)

**Topics** are the most common way nodes communicate. They are used for continuous data streams. A node (the Publisher) sends data out on a specific topic name, and any node that cares (the Subscriber) listens to it.

- Behavior: Asynchronous. The publisher doesn't care if anyone is listening; it just keeps shouting data into the void.

- Cardinality: Many-to-Many. You can have five nodes subscribing to one camera feed simultaneously.

- Best For: Sensor data or state updates.


### Services (Request/Response)

Services are used when you need a specific task done or a piece of information right now. One node (the Client) sends a request, and another node (the Server) processes it and sends a response back.

- Behavior: Synchronous-style (though usually implemented non-blocking in code). The client waits for the server to finish the work and reply.

- Cardinality: One-to-One. A request goes to one specific server, and the response comes back to the specific client that asked.

- Best For: Quick "set and forget" tasks or status checks. Examples include toggling a light, resetting a counter, or calculating a complex kinematic transformation.

### Actions (Goal/Feedback/Result)
The "Project Manager"
They are designed for long-running tasks that might take time to complete and could potentially fail or be canceled.

The Three Parts:

- Goal: The client tells the robot "Drive to the other side of the room."

- Feedback: While moving, the server sends updates like "I am 40% there," "I am 60% there."

- Result: Once finished, the server sends a final "I arrived" or "I got stuck."

- Behavior: **Asynchronous** and **preemptible**. You can cancel an action halfway through if the robot is about to hit a cat.

- Best For: High-level behaviors like navigation, arm movements, or complex perception tasks.
---
## Parameters

Parameters are **stateful** variables (integers, floats, booleans, strings) that belong to a specific Node. You can read and write to them from the terminal or from other nodes—while the program is running.

---
## Launch Files


Coming from C and FreeRTOS, we have a single `main()` function where I initialize all my  hardware, create my queues, and spawn tasks (xTaskCreate) one by one before starting the scheduler.

In ROS 2, because every node is an independent process, you can't just call a single C++ main() to start the whole rocket. Instead, we use a **Launch File**.

A launch file is a Python script that tells the ROS 2 daemon:
    1. Which nodes to boot up.
    2. What initial Parameters to inject into them (so you don't have to use the CLI every time).
    3. How to rename (remap) topics if needed.
--- 

## TF2 (Transform Framework 2)

Background system that tracks the relationship between every single coordinate frame in your robot over time.

It organizes these frames into a strict parent-child tree structure. 
For the E-Rocket, your tree might look like this:word -> base\_link (the rocket CoM) -> imu\_link (the sensor)TF2 is divided into two main jobs.

### 1. TF Broadcasters (The Talkers)
These are nodes that continuously publish the mathematical relationship (translation + rotation) between a parent frame and a child frame.

- **Static Broadcasters**: the launch file might say, "The IMU (imu\_link) is permanently located exactly 0.1 meters above the CoM (base\_link), rotated 180 degrees." This never changes.

- **Dynamic Broadcasters**: The mocap\_interface node will continuously read the VRPN cameras and broadcast, "At timestamp 1.52s, base\_link is located at [X, Y, Z] with quaternion [X, Y, Z, W] relative to world."

### 2. TF Listeners (The Askers)
The PID controller acts as a TF Listener. 

When a sensor reading comes in, thr  C++ node simply asks the TF2 Buffer:"Hey TF2, give me the transform from world to tvc\_link exactly at timestamp 1.52s.
"TF2 will automatically traverse the tree (world -> base\_link -> tvc\_link), multiply all the matrices together, interpolate the exact timestamps, and han the final transformation vector."





