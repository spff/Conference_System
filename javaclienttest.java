package javaclienttest;

import java.io.*;
import java.net.*;


public class javaclienttest {
    public static void main(String[] args) throws IOException {



        String hostName = "127.0.0.1";
        int portNumber = Integer.parseInt("3412");
        Socket firstSocket = new Socket(hostName, portNumber);
        PrintWriter out = new PrintWriter(firstSocket.getOutputStream(), true);
        BufferedReader in = new BufferedReader(new InputStreamReader(firstSocket.getInputStream()));
        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
        String userInput;
        
        out.println("spff");
        System.out.println("received: " + in.readLine());

        while ((userInput = stdIn.readLine()) != null) 
        {
            out.println(userInput);
        }
        in.close();
        stdIn.close();
        firstSocket.close();

    }
}