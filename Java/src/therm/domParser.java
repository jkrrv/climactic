/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package therm;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

/**
 *
 * @author jkurtz
 */

public class domParser {
    
    //this is only designed to work for a very specific format (more like JSON than XML, but not really either).
    
    private Map<String,String> vals = new HashMap<String,String>(); 
    
    public domParser(String fileName) {
        try {
            InputStream inStream = new FileInputStream(fileName);
            Scanner in = new Scanner( inStream );
            String ln = "";
            String name = "";
            String value = "";
            while (in.hasNextLine()) {
                ln = in.nextLine();
                if (ln.contains(":")) {
                    name = ln.split(":")[0];
                    value = ln.split(":")[1];
                    
                    name = name.replaceAll("\\s+","");
                    //System.out.print(name);
                    //System.out.print('.');
                    value = value.replaceAll("\\s+","");
                    //System.out.println(value);
                    
                    vals.put(name, value);
                }
            }
            
            
        } catch (FileNotFoundException e) {
            System.out.println("Error.  Could not find xml.");
            System.out.println(e);
        } 
    }
    
    public String getDomValue(String key) {
        //System.out.print(key);
        //System.out.println(vals.get(key));
        return vals.get(key);
    }
    
}
    
