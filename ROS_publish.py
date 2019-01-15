#!/usr/bin/env python
import rospy
from std_msgs.msg import Int16

class control(object):
	def __init__(self):
		self._pub_control = rospy.Publisher('/control', Int16, queue_size=1)		

		while(1):
			self._ren()

	def _ren(self):
		print "command"		
		s = input()

		control = Int16()
		control.data = s
		self._pub_control.publish(control)
		print "\n"

if __name__ == '__main__':
	rospy.init_node('control')
	control = control()
	try:
		rospy.spin()
	except KeyboardInterrupt:
		pass
