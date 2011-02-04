/**
 * @file   real_shadowhand_node.cpp
 * @author Ugo Cupcic <ugo@ugo-kubuntu.local>
 * @date   Wed Apr  7 15:37:06 2010
 *
 * @brief Contains the main for the real Shadow Dextrous Hand. We start the publishers / subscribers in this node.
 * They all share the same RealShadowhand object, this way the subscriber can update the hand properties,
 * while the publishers publish up to date data. The diagnostics and the other publisher are started
 * in two different threads, to allow them to be published at different frequencies.
 *
 *
 */

#include <ros/ros.h>

#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>

#include "sr_hand/sr_subscriber.h"
#include "sr_hand/sr_publisher.h"
#include "sr_hand/sr_diagnosticer.h"
//#include "shadowhand/shadowhand_config_server.h"
#include "sr_hand/hand/real_shadowhand.h"

using namespace std;
using namespace ros;
using namespace shadowrobot;
//using namespace shadowhand_config_server;

  /////////////////////////////////
  //           MAIN              //
  /////////////////////////////////

void run_diagnotics(boost::shared_ptr<SRDiagnosticer> shadowhand_diag)
{
  while( ok() )
  {
    shadowhand_diag->publish();
  }
}

void run_publisher(boost::shared_ptr<SRPublisher> shadowhand_pub)
{
  while( ok() )
  {
    shadowhand_pub->publish();
  }
}

/** 
 * The main function initializes the links with the robot, initializes
 * this ROS subscriber and sets the different callbacks.
 * This ROS subscriber will listen for new commands and send them to
 * the robot.
 * 
 * @param argc 
 * @param argv 
 * 
 * @return -1 if error linking with the robot (i.e. robot code not started)
 */
int main(int argc, char** argv)
{
  ros::init(argc, argv, "shadowhand");
  NodeHandle n;

  boost::shared_ptr<RealShadowhand> real_sh(new RealShadowhand());
  boost::shared_ptr<SRSubscriber> shadowhand_subscriber(new SRSubscriber(real_sh));


  boost::shared_ptr<SRPublisher> shadowhand_pub( new SRPublisher(real_sh));
  boost::shared_ptr<SRDiagnosticer> shadowhand_diag( new SRDiagnosticer(real_sh, sr_hand_hardware));

  boost::thread thrd1( boost::bind( &run_diagnotics, shadowhand_diag ));
  boost::thread thrd2( boost::bind( &run_publisher, shadowhand_pub ));
  thrd1.join();
  thrd2.join();
  
  return 0;
}
