package sample;

import javafx.application.Platform;
import javafx.scene.control.Alert;

public class Alerts {
    public static void showErrorAlert(String errorMessage) {
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("ERROR!");
            alert.setHeaderText(null);
            alert.setContentText(errorMessage);

            alert.showAndWait();
        });
    }
}
