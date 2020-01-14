package sample;

import javafx.application.Platform;
import javafx.scene.control.Alert;

public class Alerts {
    public static void showDisconnectAlert() {
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("ERROR!");
            alert.setHeaderText(null);
            alert.setContentText("Your opponent has disconnected!");

            alert.showAndWait();
        });
    }
}
