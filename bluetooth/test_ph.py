import gammu, sys, getopt

g_device = 'B4:22:7E:12:60:79'
g_message = {
	'Text': 'python-gammu testing message',
	'SMSC': {'Location': 1},
	'Number': '+71234567890',
}

connections1 = ['bluephonet', 'phonetblue', 'at', 'at19200', 'at115200', 'blueat', 'bluerfat',
	'blueobex', 'bluerfobex', 'bluefbus', 'bluerffbus', 'bluerfgnapbus', 'fbus', 'fbusirda', 'fbussdlr3', 'dku2phonet', 'dku2at',
	'fbuspl2303', 'irdaobex', 'dlr3', 'irdaat']
connections = ['bluephonet']

conn_ok = []
conn_ok_sms = []
#sm.GetAlarm             sm.GetCategory          sm.GetFilePart          sm.GetIMEI              sm.GetMemoryStatus      sm.GetNextMemory        sm.GetPPM               sm.GetSMS               sm.GetToDo
#sm.GetBatteryCharge     sm.GetCategoryStatus    sm.GetFileSystemStatus  sm.GetLocale            sm.GetModel             sm.GetNextRootFolder    sm.GetProductCode       sm.GetSMSC              sm.GetToDoStatus
#sm.GetCalendar          sm.GetConfig            sm.GetFirmware          sm.GetManufactureMonth  sm.GetNetworkInfo       sm.GetNextSMS           sm.GetSecurityStatus    sm.GetSMSFolders        
#sm.GetCalendarStatus    sm.GetDateTime          sm.GetFolderListing     sm.GetManufacturer      sm.GetNextCalendar      sm.GetNextToDo          sm.GetSignalQuality     sm.GetSMSStatus         
#sm.GetCallDivert        sm.GetDisplayStatus     sm.GetHardware          sm.GetMemory            sm.GetNextFileFolder    sm.GetOriginalIMEI      sm.GetSIMIMSI           sm.GetSpeedDial


def print_cfg(sm):
	cfg = sm.GetConfig()
	print "\'Connection\': ", "\'" + cfg['Connection'] + "\'"
	
def check_connection(sm, conn):
	print "-----------------------------------------"
	print "Set config",
	sm.SetConfig( 0, {'Device': g_device, 'Connection': conn}, )
	print "[ Ok ]"
	print_cfg(sm)
	init = 0
	try:
		print "Init..",
		sm.Init()
		print "[ OK ]"
		init = 1
		conn_ok.append(conn)
	except Exception:
		print "[ FALSE ]"
		init = 0
	if init:	
		try:
			print "Get SMSC...",
			smsc = sm.GetSMSC(1)
			print "[ OK ]"
		except Exception:
			print "[ FALSE ]"
	

		try:
			print "Sending messsage...",
			sm.SendSMS(message)
			print "[ OK ]"
			conn_ok_sms.append(conn)
		except Exception:
			print "[ FALSE ]"

	try:
		sm.Terminate()
	except Exception:
		print "Cannot terminate..."
		


def check_functions():
	print "Check functions"


def main(argv):
	global g_device
	global connections
	try:
		opts, args = getopt.getopt(argv, "hd:m:n:c:", ["device=","mtext=", "number=","conn="])
	except getopt.GetoptError:
		print 'test.py -d <device> -m <message text> -n <phone number>'
	for opt, arg in opts:
		if opt == '-h':
			print 'test.py -d <device> -m <message text>'
			sys.exit()
		elif opt in ("-d", "--device"):
			g_device = arg
		elif opt in ("-m", "--mtext"):
			g_message['Text'] = arg
		elif opt in ("-n", "--number"):
			g_message['Number'] = arg
		elif opt in ("-c", "--conn"):
			connections = [arg]
		
		
	print "Device:", g_device
	print "Connections:", connections
	print "Message:", g_message

	print "Get state machine...",
	sm = gammu.StateMachine()
	print "[ Ok ]"

	for conn in connections:
		check_connection(sm, conn)

	print "==========================="
	print "Summary:"
	print "Init:"
	for i in conn_ok:
		print i
	print "Send sms:"
	for i in conn_ok_sms:
		print i

if __name__ == "__main__":
	if len(sys.argv) > 1:
		main(sys.argv[1:])
	else:
		main([])
