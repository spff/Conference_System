package main;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class Main extends Application {

	private static Main instance;

	Socket socket = null;
	PrintWriter out = null;
	BufferedReader in = null;
	InetAddress host = null;

	String usrname;
	
	public Main() {
		instance = this;
	}

	// static method to get instance of view
	public static Main getInstance() {
		return instance;
	}

	@Override
	public void start(Stage primaryStage) {
		try {

			Process p = Runtime.getRuntime().exec("whoami");
			BufferedReader bri = new BufferedReader(new InputStreamReader(
					p.getInputStream()));
			usrname = bri.readLine();
			System.out.println(usrname);
			bri.close();

			
			primaryStage.setTitle("Conference client");
			Scene scene = new Scene(FXMLLoader.load(getClass().getResource(
					"../view/mainview.fxml")));
			primaryStage.setScene(scene);
			primaryStage.show();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void main(String[] args) {
		launch(args);
	}

	public void connect(String IP, String port) {


		System.out.println("Connect");
		try {
					
			socket = new Socket(IP, Integer.parseInt(port));
			out = new PrintWriter(socket.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(
					socket.getInputStream()));
			out.println(usrname);

		} catch (UnknownHostException e) {
			System.err.println("Cannot find the host: " + host.getHostName());
			System.exit(1);
		} catch (IOException e) {
			System.err.println("Couldn't read/write from the connection: "
					+ e.getMessage());
			System.exit(1);
		}

	}

	public void disconnect() throws IOException {
		System.out.println("Disconnect");

		socket.close();
		out.close();

	}

	public void sendSignal(String signal) {
		out.println(signal);
	}

}
