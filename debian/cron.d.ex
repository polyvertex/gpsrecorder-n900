#
# Regular cron jobs for the gpsrecorder package
#
0 4	* * *	root	[ -x /usr/bin/gpsrecorder_maintenance ] && /usr/bin/gpsrecorder_maintenance
