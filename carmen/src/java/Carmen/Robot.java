package Carmen;

import java.util.*;
import IPC.*;
/** Carmen class for  a Robot */
public class Robot {
  private static final String CARMEN_ROBOT_VELOCITY_NAME = "carmen_robot_vel";
  private static final String CARMEN_ROBOT_VELOCITY_FMT = 
    "{double,double,double,string}";

  private static final String CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME =
    "carmen_robot_follow_trajectory";
  private static final String CARMEN_ROBOT_FOLLOW_TRAJECTORY_FMT = 
    "{int,<{double,double,double,double,double}:1>,{double,double,double,double,double},double,string}";

  private static final String CARMEN_ROBOT_VECTOR_MOVE_NAME = 
    "carmen_robot_vector_move";
  private static final String CARMEN_ROBOT_VECTOR_MOVE_FMT = 
    "{double,double,double,string}";

  private static final String CARMEN_ROBOT_VECTOR_STATUS_NAME =
    "carmen_robot_vector_status";
  private static final String CARMEN_ROBOT_VECTOR_STATUS_FMT =
    "{double,double,double,string}";
  
  private static final String CARMEN_BASE_RESET_COMMAND_NAME = "carmen_base_reset_command";
  private static final String CARMEN_BASE_RESET_COMMAND_FMT = 
  	"{double,string}";

  /** Event Handler receives this and can set its fields */
  public static class CarmenBaseResetMessage extends Message {
  }


/** Event Handler receives this and can set its fields */
  public static class RobotVelocityMessage extends Message {
    public double tv, rv;

  /** Event Handler receives this and can set its fields */
    RobotVelocityMessage(double tv, double rv) {
      this.tv = tv;
      this.rv = rv;
    }
  }


  /** Event Handler receives this and can set its fields */
  public static class RobotVectorMoveMessage extends Message {
    public double distance, theta;

    RobotVectorMoveMessage(double distance, double theta) {
      this.distance = distance;
      this.theta = theta;
    }
  }

  /** Event Handler receives this and can set its fields */
  public static class RobotFollowTrajectoryMessage extends Message {
    public int trajectory_length;
    public TrajPoint trajectory [];
    public TrajPoint robotPosition;
    
    RobotFollowTrajectoryMessage(TrajPoint trajectory [], TrajPoint
				 robotPosition) {
      this.trajectory_length = trajectory.length;
      this.trajectory = trajectory;
      this.robotPosition = robotPosition;
    }
  }

  private static class RobotPrivateVectorStatusHandler implements IPC.HANDLER_TYPE {
    private static VectorStatusHandler userHandler = null;
    RobotPrivateVectorStatusHandler(VectorStatusHandler userHandler) {
      this.userHandler = userHandler;
    }
    public void handle (IPC.MSG_INSTANCE msgInstance, Object callData) {
      VectorStatusMessage message = (VectorStatusMessage)callData;
      userHandler.handleVectorStatus(message);
    }
  }
  
  /** Application module calls this to subscribe to VectorStatusMessage.
   *  Application module must extend VectorStatusHandler
   */
  public static void subscribeVectorStatus(VectorStatusHandler handler) {
    IPC.defineMsg(CARMEN_ROBOT_VECTOR_STATUS_NAME, 
		  CARMEN_ROBOT_VECTOR_STATUS_FMT);
    IPC.subscribeData(CARMEN_ROBOT_VECTOR_STATUS_NAME, 
		      new RobotPrivateVectorStatusHandler(handler),
		      VectorStatusMessage.class);
    IPC.setMsgQueueLength(CARMEN_ROBOT_VECTOR_STATUS_NAME, 1);
  }

  /** Application module calls this class method to direct the robot
   * at stated translational velocity (m/s) and rotational velocity (r/s).
   * This results in a message via IPC to the Orc_daemon then out to the 
   * robot base. To view how the message is processed in more detail, see
   * the Carmen c-code base in subdirectory orc_daemon file orclib.c and
   * procedure: carmen_base_direct_set_velocity 
   */
  public static void setVelocity(double tv, double rv) {
    RobotVelocityMessage velocityMessage = new RobotVelocityMessage(tv, rv);
    IPC.defineMsg(CARMEN_ROBOT_VELOCITY_NAME,
		  CARMEN_ROBOT_VELOCITY_FMT);
    IPC.publishData(CARMEN_ROBOT_VELOCITY_NAME, velocityMessage);
  }

  /** Application module calls this class method to direct the robot
   * to proceed in a vector stated by distance and orientation.
   */
   public static void moveAlongVector(double distance, double theta) {
    RobotVectorMoveMessage vectorMessage = 
      new RobotVectorMoveMessage(distance, theta);
    IPC.defineMsg(CARMEN_ROBOT_VECTOR_MOVE_NAME,
		  CARMEN_ROBOT_VECTOR_MOVE_FMT);
    IPC.publishData(CARMEN_ROBOT_VECTOR_MOVE_NAME, vectorMessage);
  }

/** Application module calls this class method to reset the 
   * robot base to the initialized state.
   */
   public static void resetRobotBase() {
    CarmenBaseResetMessage  resetMessage = 
      new CarmenBaseResetMessage();
    IPC.defineMsg(CARMEN_BASE_RESET_COMMAND_NAME,
		  CARMEN_BASE_RESET_COMMAND_FMT);
    IPC.publishData(CARMEN_BASE_RESET_COMMAND_NAME, resetMessage);
  }


   /** Application module calls this class method to direct the robot
   * to follow a trajectory given by a set of poses to a stated position.
   */
  public static void followTrajectory(TrajPoint trajectory[], 
				      TrajPoint robotPosition) {
    RobotFollowTrajectoryMessage trajectoryMessage = 
      new RobotFollowTrajectoryMessage(trajectory, robotPosition);
    IPC.defineMsg(CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME,
		  CARMEN_ROBOT_FOLLOW_TRAJECTORY_FMT);
    IPC.publishData(CARMEN_ROBOT_FOLLOW_TRAJECTORY_NAME, trajectoryMessage);
  }

  public static void initialize(String moduleName, String SBCName)
  {
    IPC.connectModule(moduleName, SBCName);
  }

  public static void dispatch()
  {
    IPC.dispatch();
  }

  // Dispatch but only for as long as timeoutMSecs 

  public static int listen (long timeoutMSecs) {
    return IPC.listen(timeoutMSecs);
  }

}
