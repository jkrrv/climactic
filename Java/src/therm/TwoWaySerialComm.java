package therm;

import gnu.io.*;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import java.io.FileInputStream;
import java.io.FileOutputStream;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/* this code was borrowed from the internet, then massively changed.  */

public class TwoWaySerialComm {
    public TwoWaySerialComm() {
        super();
    }
    
    void connect ( String portName ) throws Exception {
        CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
        if ( portIdentifier.isCurrentlyOwned() ) {
            // 
            System.err.println("Error: Port is currently in use");
            System.exit(0);
        } else {
            CommPort commPort = portIdentifier.open(this.getClass().getName(),2000);
            
            if ( commPort instanceof SerialPort ) {
                // figure out port stuff
                SerialPort serialPort = (SerialPort) commPort;
                serialPort.setSerialPortParams(9600,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
                
                // get streams
                InputStream in = serialPort.getInputStream();
                OutputStream out = serialPort.getOutputStream();
                
                // Start read & write threads
                (new Thread(new SerialReader(in))).start();
                (new Thread(new SerialWriter(out))).start();

            } else {
                // should be impossible... 
                System.err.println("Error: Only serial ports are handled by this example.");
            }
        }     
    }
    
    /* READ THREAD */
    public static class SerialReader implements Runnable {
        InputStream in;
        
        public SerialReader ( InputStream in ) {
            this.in = in;
            //System.out.print("read: ");
            //System.out.println(in.toString());
        }
        
        public void run () {
            byte[] buffer = new byte[1024];
            int len = -1;
            String status = "";
            try {
                //System.out.println("area3");
                
                while (( len = this.in.read(buffer)) > -1) {
                    
                    
                    try { 
                        while (( len = this.in.read(buffer)) > 0 ) {
                            //System.out.println("area7");
                            
                            while(Core.locked) {
                            }
                            
                            
                            Core.locked=true;
                            
                            //System.out.print("read: ");
                            status = status + new String(buffer,0,len);
                            //FileOutputStream fArdOut = new FileOutputStream("ArdOut.txt");
                            //fArdOut.write(buffer);
                            //fArdOut.close();
                            //System.out.println(buffer);
                            
                            Core.locked=false;
                            //System.out.println("area2");
                        }
                    } catch (Exception e){ //Catch exception if any
                        System.err.println("Error: " + e.getMessage());
                    }
                    //System.out.print(status);
                    status = status.split("\\n")[0];
                    if (status!="") {
                        Core.fromArd = status;
                        System.out.println("     Input Received.");
                        //System.out.println(Core.fromArd);
                    }
                    status = "";
                }
                
            } catch ( IOException e ) {
                e.printStackTrace();
            } 
        }
    }

    /* WRITE THREAD */
    public static class SerialWriter implements Runnable {
        OutputStream out;
        
        public SerialWriter ( OutputStream out ) {
            this.out = out;
        }
        
        public void run () {
            try {                
                int c = 0;
                while (true) { //( c = System.in.read()) > -1 ) {
                    try {
                        
                        while(Core.locked) {
                        }
                        
                        Core.locked=true;
                        
                        //FileInputStream fArdIn = new FileInputStream("ArdIn.txt");
                        this.out.write(Core.toArd);
                        System.out.println("     Output Sent");
                        //this.out.write(fArdIn.read());
                        //fArdIn.close();
                        
                        Core.locked=false;
                        
                        Thread.sleep(2000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        System.exit(0);
                    }
                }                
            } catch ( IOException e ) {
                e.printStackTrace();
            }            
        }
    }

}