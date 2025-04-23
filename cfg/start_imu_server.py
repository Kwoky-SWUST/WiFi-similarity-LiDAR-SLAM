#update for Smart City code // running since 5th May
# last update on 26th May
# Major update on Nov 10 - used in Smart city Gateway instead Async method
# **********LATEST SMART CITY  code *************
#install sudo apt-get install python-pip python-dev build-essential
#install xbee sudo pip install xbee
#the run sudo python start_imu_server.py
#! /usr/bin/python
from xbee import ZigBee


import pprint
import serial
import time
import urllib
import urllib2
import socket,time
import httplib2
import datetime
from urllib2 import Request,urlopen,URLError
import subprocess
import binascii
import sys
try:
   timeout = 10 # changed from 10 to 5 and 5 to 3 and 8.Now 10
   socket.setdefaulttimeout(timeout)
   s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
   s.connect(('8.8.8.8',0))
   ip = s.getsockname()[0]
   print('My Ip is '+ip)
except:
   pass
datform=0;
def receiveVal():
   # Open serial port
   try:
      ser = serial.Serial('/dev/ttyUSB0',115200)
   except:
      print 'unable to open serial port try running in sudo mode!'
      sys.exit(0)
   
   while True:
       try:
          x=ser.readline()
          data= x.split('\r\n')[0]
          val={'data': data}
          url =  'http://127.0.0.1/server_testing/insertData.php?data='+x # To test HDB from Jurong East site
       except:
          print 'No data arrived within timeout region'
          break
       if(len(data.split(','))==5):
	     try:
		t=time.time()
             	urllib2.urlopen(url)
		print time.time()-t
		pass
             except:
		print "error openning url   "
             vals=data.split(',');
#             print url
#	     print x 
	     if (vals[0]=="042"):

	             print vals[0],'-',vals[1],'-',vals[2],'-',vals[3],'-',vals[4]
       else:
          print 'invalid data format',data
    



receiveVal()




