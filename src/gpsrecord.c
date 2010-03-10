//***************************************************************************
//
// Author     : Jean-Charles Lefebvre
// Created On : 2010-03-09 18:29:46
//
// $Id$
//
//***************************************************************************

// standard headers
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

// maemo - glib
//
// get glib include paths :
// * mad pkg-config --cflags glib-2.0
// * mad pkg-config --libs glib-2.0
//
// INCLUDEPATH += /usr/include/glib-2.0 /usr/lib/glib-2.0/include
// LIBS        += -lglib-2.0 -llocation
#include <glib.h>

// maemo - location
#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#include <location/location-distance-utils.h>
#include <location/location-misc.h>


//---------------------------------------------------------------------------
// Base Types
//---------------------------------------------------------------------------
#ifndef NULL
#define NULL  0
#endif

#if !defined(_STDINT_H) && !defined(__BIT_TYPES_DEFINED__)
  typedef unsigned char      uint8_t;   // must always be 8bits wide
  typedef unsigned short     uint16_t;  // must always be 16bits wide
  typedef unsigned int       uint32_t;  // must always be 32bits wide
  typedef unsigned long long uint64_t;  // must always be 64bits wide
#endif

typedef unsigned int  uint_t;
typedef unsigned char uchar_t;
typedef unsigned char bool_t;  // *** NEVER USE THIS TYPE IN STORED STRUCTURES ***

#ifndef false
#define false  0
#define true   1
#endif


//---------------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------------
// program parameters
static struct option OPTIONS[] =
{
  { "help",   required_argument, NULL, 'h' },
  { "output", required_argument, NULL, 'o' },
  { "yes",          no_argument, NULL, 'y' },
  { 0, 0, 0, 0 },
};
static const int OPTIONS_COUNT = sizeof(OPTIONS) / sizeof(OPTIONS[0]);

// gps fixes desired frequency
// http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDControl.html#LocationGPSDControlInterval
static const LocationGPSDControlInterval GPSFIXES_INTERVAL = LOCATION_INTERVAL_5S;

// number of gps fixes to get before actually writing them to the output file
#define GPSFIXES_BUFCOUNT  5


//---------------------------------------------------------------------------
// Local Variables
//---------------------------------------------------------------------------
// parameters
static bool_t g_b_alwaysyes = false;
static char*  g_psz_outfile = NULL;

// maemo - location lib
LocationGPSDevice*   g_p_gps_device = NULL;
LocationGPSDControl* g_p_gpsd_control = NULL;

// runtime
int        g_i_fdout = -1;
char       g_sz_line[1024];
bool_t     g_b_hasfix = false;
GMainLoop* g_p_gmainloop = NULL;

// gps fixes buffer
static struct GpsFixLine_
{
  LocationGPSDeviceStatus status;
  int                     satellites_in_view;
  int                     satellites_in_use;
  LocationGPSDeviceFix    fix;
}
g_as_gpsfixes[GPSFIXES_BUFCOUNT];
int g_i_gpsfixes = 0;



//---------------------------------------------------------------------------
// _usage
//---------------------------------------------------------------------------
static void _usage(const char* psz_argv0)
{
  printf(
    "\n"
    "gpsrecord\n"
    "  Compiled on " __DATE__ " at " __TIME__ "\n"
    "\n"
    "Usage :\n"
    "  %s [options]\n"
    "\n"
    "Parameters :\n"
    "  -h,--help\n"
    "    Prints this message.\n"
    "  -o,--output {file_path}\n"
    "    This is the output file path.\n"
    "    Output data will be written into it.\n"
    "\n"
    "Miscellaneous parameters :\n"
    "  -y,--yes\n"
    "    Force to overwrite existing file(s).\n"
    "    Otherwise, this program will exit instead of overwrite a file.\n"
    "\n"
    , psz_argv0
  );
}

//---------------------------------------------------------------------------
// _param_parse
//---------------------------------------------------------------------------
static void _param_parse(int i_argc, const char** ppsz_argv)
{
  int    i_optidx;
  char   sz_shortopts[64] = "";
  uint_t ui_len;
  char   c;

  // print help when called without parameters
  if (i_argc < 2)
  {
    _usage((i_argc >= 1) ? ppsz_argv[0] : "?");
    exit(1);
  }

  // prepare the shortopts list to call getopt()
  for (i_optidx=0, ui_len=0 ; i_optidx<OPTIONS_COUNT ; ++i_optidx)
  {
    if (OPTIONS[i_optidx].flag != NULL)
      continue;

    sz_shortopts[ui_len++] = (char)OPTIONS[i_optidx].val;
    if (OPTIONS[i_optidx].has_arg == required_argument)
      sz_shortopts[ui_len++] = ':';
  }
  sz_shortopts[ui_len] = '\0';

  // getopt loop
  while (1)
  {
    i_optidx = 0;
    c = getopt_long(i_argc, (char**)ppsz_argv, (char*)&sz_shortopts, (struct option*)&OPTIONS, &i_optidx);
    if (c == (char)-1)
      break;

    switch (c)
    {
      case 0 : // returned if (option.flag != NULL)
        err(1, "getopt_long() returned 0 !");
        break;

      case '?' :
      case ':' :
        // getopt_long already printed an error message
        _usage(ppsz_argv[0]);
        err(1, "Incorrect parameter !");
        break;

      // help
      case 'h' :
        _usage(ppsz_argv[0]);
        exit(0);
        break;

      // output
      case 'o' :
        g_psz_outfile = optarg;
        break;

      // yes
      case 'y' :
        g_b_alwaysyes = true;
        break;

      default :
        err(1, "Unknown option '%c' !", c);
        break;
    }
  }
}

//---------------------------------------------------------------------------
// _param_validate
//---------------------------------------------------------------------------
static void _param_validate(void)
{
  struct stat s_stats;

  // output file
  if (!g_psz_outfile)
    err(1, "Missing output file !");
  if (stat(g_psz_outfile, &s_stats) == 0)
  {
    if (!g_b_alwaysyes)
      err(1, "Output file \"%s\" already exists !", g_psz_outfile);
  }
}




//---------------------------------------------------------------------------
// _gpsfix_flushall
//---------------------------------------------------------------------------
static void _gpsfix_flushall(void)
{
  char  sz_buffer[256 * GPSFIXES_BUFCOUNT]; // 256 bytes per line should be enough
  int   i_bufpos;
  char* psz_buffer;
  char  sz_status[8];
  char  sz_mode[8];
  int   i;

  if (g_i_gpsfixes <= 0)
    return;
  printf("Flushing %d GPS fixes.\n", g_i_gpsfixes);

  // prepare output buffer
  sz_buffer[0] = '\0';
  psz_buffer   = (char*)&sz_buffer;
  for (i = 0; i < g_i_gpsfixes; ++i)
  {
    switch (g_as_gpsfixes[i].status)
    {
      case LOCATION_GPS_DEVICE_STATUS_NO_FIX :
        strcpy((char*)&sz_status, "nx");
        break;
      case LOCATION_GPS_DEVICE_STATUS_FIX :
        strcpy((char*)&sz_status, "fx");
        break;
      case LOCATION_GPS_DEVICE_STATUS_DGPS_FIX :
        strcpy((char*)&sz_status, "dx");
        break;
      default :
        strcpy((char*)&sz_status, "??");
        break;
    }

    switch (g_as_gpsfixes[i].fix.mode)
    {
      case LOCATION_GPS_DEVICE_MODE_NOT_SEEN :
        strcpy((char*)&sz_mode, "ns");
        break;
      case LOCATION_GPS_DEVICE_MODE_NO_FIX :
        strcpy((char*)&sz_mode, "nx");
        break;
      case LOCATION_GPS_DEVICE_MODE_2D :
        strcpy((char*)&sz_mode, "2d");
        break;
      case LOCATION_GPS_DEVICE_MODE_3D :
        strcpy((char*)&sz_mode, "3d");
        break;
      default :
        strcpy((char*)&sz_mode, "??");
        break;
    }

    i_bufpos = sprintf(psz_buffer,
      "fix;%s;%d;%d;%s;0x%02X;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n"
      , (char*)&sz_status
      , g_as_gpsfixes[i].satellites_in_view
      , g_as_gpsfixes[i].satellites_in_use
      , (char*)&sz_mode
      , g_as_gpsfixes[i].fix.fields    // uint32
      , g_as_gpsfixes[i].fix.time      // double
      , g_as_gpsfixes[i].fix.ept       // double
      , g_as_gpsfixes[i].fix.latitude  // double
      , g_as_gpsfixes[i].fix.longitude // double
      , g_as_gpsfixes[i].fix.eph       // double
      , g_as_gpsfixes[i].fix.altitude  // double
      , g_as_gpsfixes[i].fix.epv       // double
      , g_as_gpsfixes[i].fix.track     // double
      , g_as_gpsfixes[i].fix.epd       // double
      , g_as_gpsfixes[i].fix.speed     // double
      , g_as_gpsfixes[i].fix.eps       // double
      , g_as_gpsfixes[i].fix.climb     // double
      , g_as_gpsfixes[i].fix.epc       // double
    );
    if (i_bufpos > 0)
      psz_buffer += i_bufpos;
  }

  // reset counter
  g_i_gpsfixes = 0;

  // write buffer to output file
  psz_buffer = (char*)&sz_buffer;
  i_bufpos   = strlen((char*)&sz_buffer); // bytes to write
  while (i_bufpos > 0)
  {
    i = write(g_i_fdout, psz_buffer, i_bufpos);
    if (i < 0)
    {
      perror("Error while writing to output file !");
    }
    else if (i == 0)
    {
      usleep(10 * 1000);
    }
    else if (i > 0)
    {
      psz_buffer += i;
      i_bufpos   -= i;
    }
  }
}

//---------------------------------------------------------------------------
// _gpsfix_push
//---------------------------------------------------------------------------
static void _gpsfix_push(LocationGPSDeviceStatus e_status, LocationGPSDevice* ps_gps_device)
{
  // http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html

  // flush now if needed
  if (g_i_gpsfixes >= GPSFIXES_BUFCOUNT) // this should never happen here...
    _gpsfix_flushall();

  // push gps fix data into our buffer
  {
    memset(&g_as_gpsfixes[g_i_gpsfixes], 0, sizeof(g_as_gpsfixes[0]));

    g_as_gpsfixes[g_i_gpsfixes].status             = e_status;
    g_as_gpsfixes[g_i_gpsfixes].satellites_in_view = ps_gps_device->satellites_in_view;
    g_as_gpsfixes[g_i_gpsfixes].satellites_in_use  = ps_gps_device->satellites_in_use;

    memcpy(&g_as_gpsfixes[g_i_gpsfixes].fix, ps_gps_device->fix, sizeof(g_as_gpsfixes[g_i_gpsfixes].fix));
  }

  // next
  ++g_i_gpsfixes;

  // flush now if needed
  if (g_i_gpsfixes >= GPSFIXES_BUFCOUNT)
    _gpsfix_flushall();
}

//---------------------------------------------------------------------------
// _gpsfix_push_log
//---------------------------------------------------------------------------
static void _gpsfix_push_log(const char* psz_line)
{
  printf(psz_line);
  _gpsfix_flushall();
}



//---------------------------------------------------------------------------
// _locationcb_gpsd_running
//---------------------------------------------------------------------------
static void _locationcb_gpsd_running(LocationGPSDControl* p_gpsd_control, gpointer p_userdata)
{
  p_gpsd_control = p_gpsd_control;
  p_userdata = p_userdata;

  _gpsfix_push_log("# GPSD started.\n");
}

//---------------------------------------------------------------------------
// _locationcb_gpsd_stopped
//---------------------------------------------------------------------------
static void _locationcb_gpsd_stopped(LocationGPSDControl* p_gpsd_control, gpointer p_userdata)
{
  p_gpsd_control = p_gpsd_control;
  p_userdata = p_userdata;

  _gpsfix_push_log("# GPSD STOPPED !\n");
}

//---------------------------------------------------------------------------
// _locationcb_gpsd_error
//---------------------------------------------------------------------------
static void _locationcb_gpsd_error(LocationGPSDControl* p_gpsd_control, gpointer p_userdata)
{
  p_gpsd_control = p_gpsd_control;
  p_userdata = p_userdata;

  _gpsfix_push_log("# GPSD ERROR !\n");
}

//---------------------------------------------------------------------------
// _locationcb_connected
//---------------------------------------------------------------------------
static void _locationcb_connected(LocationGPSDevice* p_gps_device, gpointer p_userdata)
{
  p_gps_device = p_gps_device;
  p_userdata = p_userdata;

  _gpsfix_push_log("# GPS device connected.\n");
}

//---------------------------------------------------------------------------
// _locationcb_disconnected
//---------------------------------------------------------------------------
static void _locationcb_disconnected(LocationGPSDevice* p_gps_device, gpointer p_userdata)
{
  p_gps_device = p_gps_device;
  p_userdata = p_userdata;

  _gpsfix_push_log("# GPS device DISCONNECTED !\n");
}

//---------------------------------------------------------------------------
// _locationcb_changed
//---------------------------------------------------------------------------
static void _locationcb_changed(LocationGPSDevice* p_gps_device, gpointer p_userdata)
{
  p_userdata = p_userdata;

  if (p_gps_device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET)
  {
    bool_t b_flushall = false;

    if (!g_b_hasfix && (p_gps_device->fix->eph < 9000))
    {
      _gpsfix_push_log("# Got GPS Fix.\n");
      g_b_hasfix = true;
      b_flushall = true;
    }

    _gpsfix_push(p_gps_device->status, p_gps_device);
    if (b_flushall)
      _gpsfix_flushall();
  }
  else
  {
    if (g_b_hasfix)
    {
      _gpsfix_push_log("# LOST GPS Fix !\n");
      g_b_hasfix = false;
    }
  }
}




//---------------------------------------------------------------------------
// _sighandler_quit
//---------------------------------------------------------------------------
static void _sighandler_quit(int i_signal)
{
  sprintf((char*)&g_sz_line, "# SIGNAL %d CAUGHT !\n", i_signal);
  _gpsfix_push_log((char*)&g_sz_line);

  g_main_loop_quit(g_p_gmainloop);
}




//---------------------------------------------------------------------------
// m a i n
//---------------------------------------------------------------------------
int main(int i_argc, const char** ppsz_argv)
{
  guint aui_sighl_gpsdevice[3];
  guint aui_sighl_gpsdcontrol[3];

  // init glib
  g_type_init();
  g_thread_init(NULL);

  // parse parameters
  _param_parse(i_argc, ppsz_argv);
  _param_validate();

  // create output file
  {
    printf("Creating output file.\n");

    g_i_fdout = open(g_psz_outfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if (g_i_fdout < 0)
      perror("Failed to create output file !");

    strcpy((char*)&g_sz_line,
      "# http://maemo.org/api_refs/5.0/5.0-final/liblocation/LocationGPSDevice.html\n"
      "# fields :\n"
      "#   0x01 altitude set\n"
      "#   0x02 speed set\n"
      "#   0x04 track set\n"
      "#   0x08 climb set\n"
      "#   0x10 latlong set\n"
      "#   0x20 time set\n"
      "# fix;devstatus;satview;satuse;mode;fields;time;ept;latitude;longitude;eph;altitude;epv;track;epd;speed;eps;climb;epc\n");
    write(g_i_fdout, &g_sz_line, strlen((char*)&g_sz_line));
    printf((char*)&g_sz_line);
  }

  // init location lib and connect gps
  {
    printf("Init location lib.\n");

    g_p_gps_device   = (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);
    g_p_gpsd_control = location_gpsd_control_get_default();

    g_object_set(G_OBJECT(g_p_gpsd_control), "preferred-interval", GPSFIXES_INTERVAL, NULL);

	  aui_sighl_gpsdevice[0] = g_signal_connect(G_OBJECT(g_p_gps_device), "changed",      G_CALLBACK(_locationcb_changed), NULL);
	  aui_sighl_gpsdevice[1] = g_signal_connect(G_OBJECT(g_p_gps_device), "connected",    G_CALLBACK(_locationcb_connected), NULL);
	  aui_sighl_gpsdevice[2] = g_signal_connect(G_OBJECT(g_p_gps_device), "disconnected", G_CALLBACK(_locationcb_disconnected), NULL);

	  aui_sighl_gpsdcontrol[0] = g_signal_connect(G_OBJECT(g_p_gpsd_control), "gpsd_running", G_CALLBACK(_locationcb_gpsd_running), NULL);
	  aui_sighl_gpsdcontrol[1] = g_signal_connect(G_OBJECT(g_p_gpsd_control), "gpsd_stopped", G_CALLBACK(_locationcb_gpsd_stopped), NULL);
	  aui_sighl_gpsdcontrol[2] = g_signal_connect(G_OBJECT(g_p_gpsd_control), "error",        G_CALLBACK(_locationcb_gpsd_error), NULL);

    printf("Connect GPS.\n");
    location_gps_device_reset_last_known(g_p_gps_device);
    location_gpsd_control_start(g_p_gpsd_control); // if (g_p_gpsd_control->can_control)
  }

  // register signal handlers
  signal(SIGINT, _sighandler_quit);

  // main loop
  printf("Entering main loop.\n");
  g_p_gmainloop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(g_p_gmainloop);

  // exiting
  {
    g_main_loop_unref(g_p_gmainloop);
    g_p_gmainloop = NULL;

    location_gpsd_control_stop(g_p_gpsd_control); // if (g_p_gpsd_control->can_control)

    g_signal_handler_disconnect(g_p_gps_device, aui_sighl_gpsdevice[0]);
    g_signal_handler_disconnect(g_p_gps_device, aui_sighl_gpsdevice[1]);
    g_signal_handler_disconnect(g_p_gps_device, aui_sighl_gpsdevice[2]);

    g_signal_handler_disconnect(g_p_gpsd_control, aui_sighl_gpsdcontrol[0]);
    g_signal_handler_disconnect(g_p_gpsd_control, aui_sighl_gpsdcontrol[1]);
    g_signal_handler_disconnect(g_p_gpsd_control, aui_sighl_gpsdcontrol[2]);

    g_object_unref(g_p_gpsd_control);
    g_object_unref(g_p_gps_device);
  }

  // flush remaining gps fixes to output file
  if (g_i_gpsfixes > 0)
  {
    printf("Flushing remaining lines.\n");
    _gpsfix_flushall();
  }

  // close output file
  close(g_i_fdout);

	return 0;
}
