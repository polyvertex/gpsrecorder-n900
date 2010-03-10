#
# Regular cron jobs for the gpsrecord package
#
0 4	* * *	root	[ -x /usr/bin/gpsrecord_maintenance ] && /usr/bin/gpsrecord_maintenance
