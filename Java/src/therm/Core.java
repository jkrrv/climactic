/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package therm;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.json.*;

import java.io.*;
import java.net.URL;
import java.net.URLConnection;

/**
 *
 * @author jkurtz
 */
public class Core {
    public static String fromArd="";
    public static int toArd=0;
    
    public Core() {
        toArd=0;
        fromArd="";
    }
    
    public static Heat HeatThread = null;
    public static TreeMaster TreeMasterThread = null;
    
    public static void main ( String[] args ) {
        HeatThread = new Heat();
        (new Thread(HeatThread)).start();
        TreeMasterThread = new TreeMaster();
        (new Thread(TreeMasterThread)).start();

        
        //(new Thread(new server())).start();
        
    }


    public static class TreeMaster implements Runnable {
        // DECLARE ADMINISTRATIVE THINGS
        long tl = 0; // timestamp of last state change
        long t = 0; // timestamp of "now"
        int state = 0;
        int delay = 700;
        boolean changed = false;

        // DECLARE INPUTS

        // DECLARE VARS

        // INIT INPUT CONNECTIONS


        // RUN
        public void run() {
            final String url = "http://192.168.1.107:8080/lights";

            while (true) {
                // GET INPUTS
                try {

                } catch (Exception e) {
                    //TODO: error handling
                    System.out.println(e.getMessage());
                }

                // UPDATE TIMESTAMP && PREPARE OUTPUTS
                t = System.currentTimeMillis() * 1000;

                JSONObject out = null;
                out = new JSONObject();
                JSONObject outU1 = null;
                outU1 = new JSONObject();

                out.put("1", outU1);

                // STATE MACHINE
                switch (state) {
                    case 0:
                        if (true) {
                            state = 1;
                            changed = true;

                            outU1.put("2", 255);
                            outU1.put("3", 0);
                            outU1.put("4", 0);

                            break;
                        }
                        break;
                    case 1:
                        if (true) {
                            state = 2;
                            changed = true;

                            outU1.put("2", 0);
                            outU1.put("3", 255);
                            outU1.put("4", 0);

                            break;
                        }
                        break;
                    case 2:
                        if (true) {
                            state = 3;
                            changed = true;

                            outU1.put("2",0);
                            outU1.put("3",0);
                            outU1.put("4",255);

                            break;
                        }
                        break;
                    case 3:
                        if (true) {
                            state = 4;
                            changed = true;

                            outU1.put("2",180);
                            outU1.put("3",180);
                            outU1.put("4",0);

                            break;
                        }
                        break;
                    case 4:
                        if (true) {
                            state = 5;
                            changed = true;

                            outU1.put("2",0);
                            outU1.put("3",180);
                            outU1.put("4",180);

                            break;
                        }
                        break;
                    case 5:
                        if (true) {
                            state = 0;
                            changed = true;

                            outU1.put("2",180);
                            outU1.put("3",0);
                            outU1.put("4",180);

                            break;
                        }
                        break;
                }

                // DEAL WITH CHANGES
                if (changed) {
                    tl = t;

                    System.out.print("TreeMaster Machine: Changing to state: ");
                    System.out.println(state);

                    HttpClient httpClient = new DefaultHttpClient();

                    try {
                        HttpPost request = new HttpPost(url);
                        StringEntity body = new StringEntity(out.toString());
                        request.setEntity(body);

                        httpClient.execute(request);

                    }catch (Exception e) {
                        // TODO: error handling..
                        System.out.println(e.getMessage());
                    } finally {
                        httpClient.getConnectionManager().shutdown();
                    }

                }

                // MANUAL DEBUG CODE


                // CLEANUP
                changed = false;

                try {
                    Thread.sleep(delay);
                } catch (InterruptedException e) {
                    //TODO: error handling
                }
            }
        }
    }


    public static class Heat implements Runnable {
        // DECLARE ADMINISTRATIVE THINGS
        long tl = 0; // timestamp of last state change
        long t = 0; // timestamp of "now"
        int state = 0;
        int delay = 10*1000;
        boolean changed = false;

        // DECLARE INPUTS
        boolean heatStatus = intToBool(1);
        boolean fanStatus = intToBool(0);
        float currHum1 = -1;
        float currTemp1 = -1;

        // DECLARE VARS
        int minCycleTime = 60;
        int minCycleTemp = 2;
        int setTemp = 27;

        // INIT INPUT CONNECTIONS
        URLConnection ardConn;

        // RUN
        public void run() {
            final String url = "http://192.168.1.104:8090/therm";

            while (true) {
                // GET INPUTS
                try {
                    ardConn = new URL(url).openConnection();
                    //InputStream ardInStream = ardConn.getInputStream();

                    JSONTokener tokener = new JSONTokener(ardConn.getInputStream());
                    JSONObject root = new JSONObject(tokener);

                    JSONObject jsDHT = root.getJSONObject("DHT");
                    JSONObject jsFurn = root.getJSONObject("Furn");

                    currHum1 = jsDHT.getLong("H");
                    currTemp1 = jsDHT.getLong("T");

                    heatStatus = intToBool(jsFurn.getInt("heat"));
                    fanStatus = intToBool(jsFurn.getInt("fan"));

                } catch (IOException e) {
                    //TODO: error handling
                    System.out.println(e.getMessage());
                }

                // UPDATE TIMESTAMP && PREPARE OUTPUTS
                t = System.currentTimeMillis() * 1000;

                JSONObject out = new JSONObject();
                JSONObject outFurn = new JSONObject();

                out.append("Furn",outFurn);

                // STATE MACHINE
                switch (state) {
                    case 0:
                        if (currTemp1 < setTemp - minCycleTemp &&
                                t - tl > minCycleTime) {
                            state = 1;
                            changed = true;

                            outFurn.append("heat",1);
                            break;
                        }
                        if (heatStatus = false) {
                            state = 0;
                            changed = true;

                            outFurn.append("heat",0);
                            break;
                        }
                        break;
                    case 1:
                        if (currTemp1 > setTemp &&
                                t - tl > minCycleTime) {
                            state = 0;
                            changed = true;

                            outFurn.append("heat",0);
                            break;
                        }
                        if (heatStatus = false) {
                            state = 1;
                            changed = true;

                            outFurn.append("heat",1);
                            break;
                        }
                        break;
                }

                // DEAL WITH CHANGES
                if (changed) {
                    tl = t;

                    System.out.print("Furn Machine: Changing to state: ");
                    System.out.println(state);

                    HttpClient httpClient = new DefaultHttpClient();

                    try {
                        HttpPost request = new HttpPost(url);
                        StringEntity body = new StringEntity(out.toString());
//                        request.addHeader("content-type", "application/x-www-form-urlencoded");
                        request.setEntity(body);
                        HttpResponse response = httpClient.execute(request);

                        System.out.println("POST RESPONSE:");
                        System.out.println(response);

                    }catch (Exception e) {
                        // TODO: error handling..
                        System.out.println(e.getMessage());
                    } finally {
                        httpClient.getConnectionManager().shutdown();
                    }

                }

                // MANUAL DEBUG CODE
                System.out.print("H: ");
                System.out.print(currHum1);
                System.out.println();

                System.out.print("T: ");
                System.out.print(currTemp1);
                System.out.println();

                System.out.print("ht: ");
                System.out.print(heatStatus);
                System.out.println();

                System.out.print("fn: ");
                System.out.print(fanStatus);
                System.out.println();


                // CLEANUP
                changed = false;

                try {
                    Thread.sleep(delay);
                } catch (InterruptedException e) {
                    //TODO: error handling
                }
            }
        }
    }


    // Utility function for type conversions. Assumes domain of "0" and "!0"
    public static boolean intToBool(int i) {
        return (i != 0);
    }

}
