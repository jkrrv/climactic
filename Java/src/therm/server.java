/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package therm;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

/**
 *
 * @author jkurtz
 */
public class server implements Runnable {
    ServerSocket server = null;
    Socket connSocket = null;
    
    
    public void run() {
        try {
            server = new ServerSocket( 3604 );
	} catch ( IOException e ) {
            System.out.println( "Could not open server socket." );
        }
        while (true) {
            try {
                try {
                    // accept the request
                    connSocket = server.accept();
                    
                    //figure out what it wants.
                    BufferedReader in = new BufferedReader(new InputStreamReader(
                    connSocket.getInputStream()));
                    
                    String str = ".";
                    String gets = "";
                    
                    while (!str.equals("")) {
                        str = in.readLine();
                        //System.out.println(str);
                        if (str.contains("GET") && str.contains("?")){
                            gets = str.split("\\?")[1];
                            break;
                        }
                    }

                    // give it.
                    PrintWriter pr = new PrintWriter( connSocket.getOutputStream() );
                    pr.print( "HTTP/1.0 200 OK\r\n" );
                    pr.print( "Content-Type: text/html\r\n" );
                    pr.print( "\r\n" );
                    System.out.println(gets);
                    if (gets.contains("log")) {
                        File sourceFile = new File ("log.csv");
                        FileReader fr = null;
                        try {
                            fr = new FileReader (sourceFile);
                            int inChar;
                            while ( (inChar = fr.read()) != -1 ) {
                                pr.printf ("%c", inChar);
                            }
                        } catch (IOException e) {
                            System.err.println ("Log reading failure");
                        } finally {
                            try {
                                if (fr != null) { 
                                    fr.close (); 
                                }
                            } catch (IOException e) {
                                // meh
                            }
                            
                        }
                    } else {
                        float[] nums = Core.AnalyThread.getNums();
                        try {
                            Core.AnalyThread.setTarget(Integer.parseInt(gets));
                        } catch (Exception e) {
                            // not a number, ignore and keep going.
                        }
                        for (int i=0; i<nums.length;i++) {
                            pr.print( nums[i] );
                            pr.print( "," );
                        }
                    }
                    pr.flush();
                } finally {
                    connSocket.close();
                }
            } catch ( IOException e ) {
                System.err.println( "Something awful happened!" );
            }
        }
    }
}
