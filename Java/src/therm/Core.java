/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package therm;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.net.URLConnection;
import java.util.Date;

/**
 *
 * @author jkurtz
 */
public class Core {
    
    public static boolean locked;
    public static String fromArd="";
    public static int toArd=0;
    
    public Core() {
        locked=false;
        toArd=0;
        fromArd="";
    }
    
    public static RecurringAnalysis AnalyThread = null;
    
    public static void main ( String[] args ) {
        try {
            TwoWaySerialComm c = new TwoWaySerialComm();
            c.connect("COM3");
        } catch ( Exception e ) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        
        AnalyThread = new RecurringAnalysis();
        
        (new Thread(AnalyThread)).start();
        
        (new Thread(new server())).start();
        
    }
    
    public static class RecurringAnalysis implements Runnable {
        int furnStatus = 0; // heat on or off
        long timeFurnLastChanged = 0;
        long timeLastText = 0;
        int minCycleTime = 60;
        int minCycleTemp;
        char scale;
        int defaultTemp;
        int defaultTempTime;
        
        float currHum = 0;
        float currTemp;
        boolean furnStat = true;  // furn switch on or off
        
        boolean logForce = false;
        final int logTime = 60*1000;
        long logLast = 0;
        
        int targetTemp;
        
        public RecurringAnalysis() {
            domParser config = new domParser("config.cfg");
            minCycleTime = Integer.parseInt(config.getDomValue("minCycleTime")); 
            minCycleTemp = Integer.parseInt(config.getDomValue("minCycleTemp")); 
            scale = config.getDomValue("scale").toLowerCase().charAt(0); 
            defaultTemp = Integer.parseInt(config.getDomValue("defaultTemp")); 
            defaultTempTime = Integer.parseInt(config.getDomValue("defaultTempTime")); 
            
            //pre-load a couple values before they get populated by sensors
            currTemp = defaultTemp; 
            targetTemp = defaultTemp;
        }
        
        public void run () {
            while (true) {
                
                // Load Sensor Data
                String[] arduino = {""};
                //System.out.println("area5");
                while(locked) {
                    
                }
                //System.out.println("area6");
                locked=true;
                //try {
                    //InputStream inStream = new FileInputStream("ArdOut.txt");
                    //Scanner in = new Scanner( inStream );
                    //if (in.hasNextLine()) {
                    //    arduino = in.nextLine().split("\\t");
                    //}
                    //in.close();
                    arduino = fromArd.split("\\t");
                //} catch (FileNotFoundException e) {
                //    System.err.print(e);
                //} 
                locked=false;
                //System.out.println("area1");
                
                // Parse Sensor Data
                try {
                    //System.out.println(arduino.length);
                    System.out.println("     Parsing input");
                    if (arduino.length==10) { 
                        currHum = Float.parseFloat(arduino[6]);
                        //System.out.println(arduino[6]);
                        currTemp = c2f(Float.parseFloat(arduino[7]));
                        //System.out.println(arduino[7]);
                        furnStat = (Integer.parseInt(arduino[8])!=0);
                        //System.out.println(arduino[8]);
                    }
                } catch (ArrayIndexOutOfBoundsException e) {
                    continue; //restart loop.
                } catch (NumberFormatException e) {
                    continue; //restart loop
                }
                
                // Now
                Date now = new Date();
                
                //Resetting to default
                if (now.getTime() % (defaultTempTime*1000) < 20000) {  //resets to default temp every x seconds, on the xth second
                    targetTemp = defaultTemp;
                }
                
                // adjust furnace as needed.  
                if ((now.getTime() >= (timeFurnLastChanged + minCycleTime*1000)) && true) {  //it is permissible time-wise to change the furnace status.  
                    if (((float)currTemp <= (float)targetTemp - (float)minCycleTemp/2) && furnStatus==0) { // turn furnace on.
                        //try {
                            furnStatus = 1;
                            while(Core.locked) {
                            }

                            Core.locked=true;
                            //FileOutputStream fArdIn = new FileOutputStream("ArdIn.txt");
                            //fArdIn.write(furnStatus);
                            //fArdIn.close();
                            timeFurnLastChanged = now.getTime();
                            Core.locked=false;
                        //} catch (FileNotFoundException e) {
                        //    furnStatus = 0;
                        //} catch (IOException e) {
                        //    furnStatus = 0;
                        //}
                            logForce = true;
                    }
                    if (((float)currTemp >= (float)targetTemp + (float)minCycleTemp/2) && furnStatus==1) { // turn furnace off.
                        //try {
                            furnStatus = 0;
                            while(Core.locked) {
                            }

                            Core.locked=true;
                            //FileOutputStream fArdIn = new FileOutputStream("ArdIn.txt");
                            //fArdIn.write(furnStatus);
                            //fArdIn.close();
                            timeFurnLastChanged = now.getTime();
                            Core.locked=false;
                        //} catch (FileNotFoundException e) {
                        //    furnStatus = 0;
                        //} catch (IOException e) {
                        //    furnStatus = 0;
                        //}
                            logForce = true;
                    }
                    toArd=furnStatus;
                    //System.out.println(furnStatus);
                }
                
                // Log stuff
                if (currHum!=0 && (logForce || logLast+logTime<=now.getTime())) { //prevents initial settings from beign logged; determines when logging is beneficial.
                    System.out.println("     Writing to Log...");
                    String log = "";
                    log = log+now.getTime()+",";
                    log = log+currTemp+",";
                    log = log+targetTemp+",";
                    log = log+currHum+",";
                    log = log+furnStat+",";
                    log = log+furnStatus+",";
                    try {
                        PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter("log.csv", true)));
                        out.println(log);
                        out.close();
                        logLast = now.getTime();
                        logForce = false;
                    } catch (IOException e) {
                        System.err.println("! could not write to log");
                    }
                }
                
                // SEND NOTIFICATIONS AS NEEDED
                //System.out.println(furnStat);
                if (!furnStat && (now.getTime() >= (timeLastText + minCycleTime*1000))) {
                    try {
                        URL oracle = new URL("http://housing.drexelforchrist.org/climactic/pester.php");
                        URLConnection yc = oracle.openConnection();
                        BufferedReader in = new BufferedReader(new InputStreamReader(yc.getInputStream()));
                        String inputLine;
                        while ((inputLine = in.readLine()) != null) {
                            //System.out.println(inputLine);
                        }
                        in.close();
                        timeLastText=now.getTime();
                    } catch (IOException e) {
                        // meh. 
                    }
                }
                
                System.out.print(currTemp);
                System.out.print("/");
                System.out.print(targetTemp);
                System.out.print("    Furn Power: ");
                if (furnStat) {
                    System.out.print("Ready.");
                } else {
                    System.out.print("Not Ready.");
                }
                System.out.print("   Heat: ");
                if (furnStatus==1) {
                    System.out.print("Running.");
                } else {
                    System.out.print("Not Running.");
                }
                System.out.println();
                
                try {
                    Thread.sleep(3200); // wait for a lil while.
                } catch (InterruptedException e) {
                    System.err.print(e); 
                    System.exit(0); 
                }
            }
        }
        
        private float c2f(float c) {
            return c * 9/5 + 32;
        }
        
        public void setTarget(int targetTemp) {
            this.targetTemp = targetTemp;
        }
        
        public float[] getNums() {
            float k[] = {0,0,0,0,0};
            k[0] = this.targetTemp;
            k[1] = this.currTemp;
            k[2] = this.furnStatus;
            k[3] = this.currHum;
            if (this.furnStat)
                    k[4] = 1;
            return k;
        }
    }
}
