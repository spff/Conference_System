package view;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.Labeled;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import main.Main;

public class Controller implements Initializable {

	@FXML
	Labeled label1, label2;
	@FXML
	Button button1;
	@FXML
	TextField IP, port;

	boolean cnt = true;

	@Override
	public void initialize(URL location, ResourceBundle resources) {

		System.out.println("View is now loaded!");

	}

	private static Controller instance;

	public Controller() {
		instance = this;
	}

	public void button1Pressod() throws IOException {

		if (cnt) {
			button1.setText("disconnect");
			Main.getInstance().connect(IP.getText(), port.getText());
			IP.setDisable(true);
			port.setDisable(true);

		} else {
			button1.setText(" connect  ");
			Main.getInstance().disconnect();
			IP.setDisable(false);
			port.setDisable(false);

		}
		cnt = !cnt;

	}

	public void mouseHandler(MouseEvent mouseEvent) {
		label1.setText(mouseEvent.getEventType() + " " + mouseEvent.getButton()
				+ " " + "(X,Y)=(" + mouseEvent.getX() + "," + mouseEvent.getY()
				+ ") ");
		if (!cnt)
			Main.getInstance().sendSignal(
					mouseEvent.getEventType() + " " + mouseEvent.getButton()
							+ " " + "(X,Y)=(" + mouseEvent.getX() + ","
							+ mouseEvent.getY() + ") ");
	}

	public void keyboardHandler(KeyEvent keyEvent) {

		label2.setText(keyEvent.getEventType() + " " + keyEvent.getText());
		if (cnt)
			Main.getInstance().sendSignal(
					keyEvent.getEventType() + " " + keyEvent.getText());
	}

}
