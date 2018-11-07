#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <libhomescreen.hpp>

#define DBUS_DEST				"org.AGL.afm.user"
#define DBUS_PATH				"/org/AGL/afm/user"
#define DBUS_IFACE				"org.AGL.afm.user"
#define DBUS_METHOD_RUNNABLES	"runnables"

DBusConnection *dbus_conn = NULL;
static int running = 1;

void connect_dbus(){
	DBusError err;

	dbus_error_init(&err);
	dbus_conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(dbus_error_is_set(&err)){
		fprintf(stderr,"ConnectionErr : %s\n", err.message);
		dbus_error_free(&err);
	}
	if(dbus_conn == NULL) {
		return NULL;
	}
}

void send_a_method_call(DBusConnection * connection,const char* method, const char * params)
{
	DBusError err;
	DBusMessage * msg;
	DBusMessageIter    arg;
	DBusPendingCall * pending;
	int arg_type;
	const char * str_arg = NULL;
	char reply_msg_content[5000] = {0};

	dbus_error_init(&err);
	//Constructs a new message to invoke a method on a remote object.
	msg =dbus_message_new_method_call(DBUS_DEST, DBUS_PATH, DBUS_IFACE, method);
	if(msg == NULL){
		fprintf(stderr,"MessageNULL");
		return;
	}

	//Append arguments
	dbus_message_iter_init_append(msg, &arg);
	if(!dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &params)){
		fprintf(stderr,"Out of Memory!");
		exit(1);
	}

	//Queues a message to send
	if(!dbus_connection_send_with_reply(connection, msg, &pending, -1)){
		fprintf(stderr,"Out of Memory!");
		exit(1);
	}

	if(pending == NULL){
		fprintf(stderr,"Pending Call NULL: connection is disconnected ");
		dbus_message_unref(msg);
		return;
	}

	dbus_connection_flush(connection);
	dbus_message_unref(msg);

	dbus_pending_call_block (pending);
	//get the reply message，Gets the reply, or returns NULL if none has been received yet.
	msg =dbus_pending_call_steal_reply(pending);
	if (msg == NULL) {
		fprintf(stderr, "Reply Null\n");
		exit(1);
	}
	//free the pending message handle
	dbus_pending_call_unref(pending);
	//read the parameters
	if(dbus_message_iter_init(msg, &arg)) {
		do {
			memset(reply_msg_content, 0, sizeof(reply_msg_content));
			arg_type = dbus_message_iter_get_arg_type(&arg);
			if (arg_type == DBUS_TYPE_STRING) {
				dbus_message_iter_get_basic(&arg, &str_arg);
				sprintf(reply_msg_content, "%s\n", str_arg);
				break;
			}
		} while(dbus_message_iter_next(&arg));
	}

	dbus_message_unref(msg);
}

static void
signal_int(int signum)
{
	running = 0;
}

using namespace std;

int
main(int argc, char **argv)
{
	struct sigaction sigint;
	int port = 1050;
    string command;
	string token = string("HELLO");
	string app_name = string("simple-hs");

	if(argc > 2)
	{
		port = strtol(argv[1], NULL, 10);
		token = argv[2];
	}

	fprintf(stderr, "app_name: %s, port: %d, token: %s. ", app_name.c_str(), port, token.c_str());

	LibHomeScreen* hs = new LibHomeScreen();

	if(hs->init(port, token)!=0)
	{
		fprintf(stderr, "homescreen init failed. ");
		return -1;
	}

	//Ctrl+C
	sigint.sa_handler = signal_int;
	sigemptyset(&sigint.sa_mask);
	sigint.sa_flags = SA_RESETHAND;
	sigaction(SIGINT, &sigint, NULL);

	if (!dbus_conn) {
		connect_dbus();
	}

	while (running) {
        cout << "Please input your command: ";
        getline (cin, command);
		fprintf(stderr, "command is %s. \n", command.c_str());
        if(command == "start settings")
        {
			if (dbus_conn) {
				//start with id@version: "settings@0.1"
				send_a_method_call(dbus_conn, "start", "settings@0.1");
			}
			//tapshortcut with name: "Settings"
            hs->tapShortcut("Settings");
        }else if(command == "start dashboard"){
			if (dbus_conn) {
				send_a_method_call(dbus_conn, "start", "dashboard@0.1");
			}
            hs->tapShortcut("Dashboard");
        }else if(command == "start hvac"){
			if (dbus_conn) {
				send_a_method_call(dbus_conn, "start", "hvac@0.1");
			}
            hs->tapShortcut("HVAC");
        }else if(command == "quit"){
			running = 0;
			break;
        }
  	}

	fprintf(stderr, "simple-hs exiting! ");

	return 0;
}
