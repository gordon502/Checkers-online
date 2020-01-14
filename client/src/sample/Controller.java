package sample;

import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.Node;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Circle;
import javafx.scene.shape.TriangleMesh;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.ArrayList;

public class Controller {

    //pomocnicze
    private boolean isConnected = false;
    private boolean isMyTurn = false;
    private Circle selectedCircle = null;
    private boolean isSelected = false;
    private String myPlayerNumber;

    //kolory figur
    private String whitePawnColor = "0xffcc00ff";
    private String whiteQueenColor = "0x755e03ff";
    private String blackPawnColor = "0x667a8dff";
    private String blackQueenColor = "0x222a32ff";


    private Connection connection;

    //elementy sceny
    public Stage window;
    public Button newGameButton;
    public Button exitButton;
    public Button abortButton;
    public GridPane gridPane;
    public Circle bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8, bp9, bp10, bp11, bp12, wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8, wp9, wp10, wp11, wp12;
    public Label serverPortLabel;
    public Label serverAddressLabel;
    public Label connectionLabel;
    public TextField serverAddressTextField;
    public TextField serverPortTextField;


    private void setUpPiecesAndVariables(String receivedMessage) {
        if(receivedMessage.startsWith("9"))
            myPlayerNumber = receivedMessage.substring(1,2);
        if (myPlayerNumber.equals("1")) {
            isMyTurn = true;
            gridPane.setDisable(!isMyTurn);
        }
        else {
            isMyTurn = false;
            gridPane.setDisable(!isMyTurn);
        }

        Circle[] circles = {bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8, bp9, bp10, bp11, bp12, wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8, wp9, wp10, wp11, wp12};
        int indexes[][] = {{1, 0}, {3, 0}, {5, 0}, {7, 0}, {0, 1}, {2, 1}, {4, 1}, {6, 1}, {1, 2}, {3, 2}, {5, 2}, {7, 2},
                {0, 5}, {2, 5}, {4, 5}, {6, 5}, {1, 6}, {3, 6}, {5, 6}, {7, 6}, {0, 7}, {2, 7}, {4, 7}, {6, 7}};
        for (int i = 0; i < circles.length; i++) {
            circles[i].setVisible(true);
            GridPane.setColumnIndex(circles[i], indexes[i][0]);
            GridPane.setRowIndex(circles[i], indexes[i][1]);
        }

        for (int i = 0; i < 12; i++) {
            circles[i].setFill(Paint.valueOf(blackPawnColor));
            circles[i+12].setFill(Paint.valueOf(whitePawnColor));
        }
        if (isSelected) {
            selectedCircle.setStroke(Color.BLACK);
            isSelected = false;
        }
    }

    public void newGame() {
        startNewGame();
    }

    //move figures to start positions and establish connection
    public void startNewGame() {
        try {
            closeLastConnection();
            isConnected = false;
        } catch (NullPointerException e) {
            System.out.println("Nie bylo polaczenia");
        }

        if (establishNewConnection()) {
            connectionLabel.setVisible(true);
            setSceneElements(true);
            Thread thread = new Thread(() -> {
                try {
                    //receive first message about which player you are
                    final String firstMessage = receiveMessage();
                    Platform.runLater(() -> {
                        connectionLabel.setText("Connected!\nYou are player\nnumber " + firstMessage.substring(1,2));
                        connectionLabel.setVisible(true);
                        setUpPiecesAndVariables(firstMessage);
                    } );

                    while (connection.isConnected) {
                        final String receivedMessage = receiveMessage();
                        System.out.println(receivedMessage);
                        if (receivedMessage.startsWith("0")){ //serwer informuje nas, ze przeciwnik sie rozlaczyl
                            Platform.runLater(() -> { closeActiveGame("Your opponent has disconnected!"); });
                            break;
                        }
                        else if (receivedMessage.substring(13,14).equals("1")) { //kiedy serwer informuje, ze gra skonczyla sie
                            String result;
                            if (receivedMessage.substring(14,15).equals("0")) { result = "Draw!"; }
                            else if (receivedMessage.substring(14,15).equals(myPlayerNumber)) { result = "You win!"; }
                            else  { result = "You lose!"; }
                            Platform.runLater(() -> { closeActiveGame("Game Ended!\n" + result); });
                            break;
                        }
                        else if (wasMoveCorrect(receivedMessage) && connection.isConnected) {
                            Platform.runLater(()-> {
                                updateBoard(receivedMessage);
                                updateGameVariables(receivedMessage);
                            });
                        }
                    }
                } catch (IOException e) {
                    Platform.runLater(() -> { closeActiveGame("There was a error in communication with server!"); });
                }


            });
            thread.start();
        }
        else {
            Alerts.showErrorAlert("Cannot connect to server!"); //kiedy serwer jest wylaczony badz nie mozemy sie z nim polaczyc
        }
    }

    public void abortGame() {
        closeLastConnection();
        setSceneElements(false);
        removeAllPieces();
        connectionLabel.setVisible(false);
    }

    private void closeActiveGame(String information) {
        connection.isConnected = false;
        closeLastConnection();
        Alerts.showErrorAlert(information);
        removeAllPieces();
        setSceneElements(false);
        connectionLabel.setVisible(false);
        }

    private void setSceneElements(boolean value) {
        newGameButton.setDisable(value);
        serverAddressTextField.setDisable(value);
        serverPortTextField.setDisable(value);
        abortButton.setVisible(value);
        connectionLabel.setText("Waiting for\nopponent!");
    }


    //podswietl figure
    public void figureOnMouseEntered(MouseEvent event){
        Circle source = (Circle) event.getSource();
        if(!(selectedCircle == source))
            source.setStroke(Color.RED);
    }

    public void figureOnMouseExited(MouseEvent event) {
        Circle source = (Circle) event.getSource();
        if(!(selectedCircle == source))
            source.setStroke(Color.BLACK);
    }

    public void selectFigure(MouseEvent event) {
        Circle pom = (Circle) event.getSource();
        if(isSelected == true) {
            if (selectedCircle == pom){
                selectedCircle.setStroke(Color.BLACK);
                isSelected = false;
            }
            else {
                selectedCircle.setStroke(Color.BLACK);
                selectedCircle = (Circle) event.getSource();
                selectedCircle.setStroke(Color.GREEN);
            }
        }
        else {
            if ((myPlayerNumber.equals("1") && isSelectedFigureWhite(pom) || (myPlayerNumber.equals("2") && isSelectedFigureBlack(pom)))) {
                selectedCircle = (Circle) event.getSource();
                isSelected = true;
                selectedCircle.setStroke(Color.GREEN);
            }
        }
    }

    private boolean isSelectedFigureWhite(Circle circle) {
        if (circle.getFill().equals(Paint.valueOf(whitePawnColor)) || circle.getFill().equals(Paint.valueOf(whiteQueenColor)))
            return true;
        return false;
    }



    private boolean isSelectedFigureBlack(Circle circle) {
        if (circle.getFill().equals(Paint.valueOf(blackPawnColor)) || circle.getFill().equals(Paint.valueOf(blackQueenColor)))
            return true;
        return false;
    }


    //przenies figure z jednego pola na drugie
    public void makeMove(MouseEvent mouseEvent){
        if (isMyTurn && isSelected)
            sendMessage(mouseEvent);
    }

    private boolean sendMessage(MouseEvent mouseEvent) {
        Integer cindex = (int) (mouseEvent.getX()/(gridPane.getWidth()/8));
        Integer rindex = (int) (mouseEvent.getY()/(gridPane.getHeight()/8));
        if (cindex != GridPane.getColumnIndex(selectedCircle) && rindex != GridPane.getRowIndex(selectedCircle)) {
            String message = GridPane.getRowIndex(selectedCircle).toString() + GridPane.getColumnIndex(selectedCircle).toString() +
                    rindex.toString() + cindex.toString();
            connection.sendMessage(message);
            return true;
        }
        return false;
    }

    private String receiveMessage() throws IOException{
        return connection.readMessage();
    }

    private boolean wasMoveCorrect(String message) {
        if (message.substring(1, 2).equals("1"))
            return true;
        else
            return false;
    }

    private void updateBoard(String message) {
        System.out.println("updateBoad: " + message);
        Integer fromrowindex = Integer.valueOf(message.substring(3, 4));
        Integer fromcolumnindex = Integer.valueOf(message.substring(4, 5));
        Integer torowindex = Integer.valueOf(message.substring(5, 6));
        Integer tocolumnindex = Integer.valueOf(message.substring(6, 7));

        Node movedCircle = getNodeByRowColumnIndex(fromrowindex, fromcolumnindex);
        GridPane.setRowIndex(movedCircle, torowindex);
        GridPane.setColumnIndex(movedCircle, tocolumnindex);

        if (newQueen(message)) {
            int rowindex = Integer.valueOf(message.substring(11, 12));
            int colindex = Integer.valueOf(message.substring(12, 13));

            Circle figure = (Circle) getNodeByRowColumnIndex(rowindex, colindex);
            if (figure.getFill().equals(Paint.valueOf(whitePawnColor)))
                figure.setFill(Paint.valueOf(whiteQueenColor));
            else
                figure.setFill(Paint.valueOf(blackQueenColor));
        }

        if (wasFigureJumped(message)) {
            int rowindex = Integer.valueOf(message.substring(8, 9));
            int columnindex = Integer.valueOf(message.substring(9, 10));
            System.out.println(rowindex + " " + columnindex);
            Circle removedFigure = (Circle) getNodeByRowColumnIndex(rowindex, columnindex);
            removedFigure.setVisible(false);
        }
    }

    private void updateGameVariables(String message) {
        if (message.substring(2, 3).equals("0") && isMyTurn) {
            isMyTurn = false;
            gridPane.setDisable(!isMyTurn);
        }
        else if (message.substring(2,3).equals("0")) {
            isMyTurn = true;
            gridPane.setDisable(!isMyTurn);
        }
        if (isSelected) {
            selectedCircle.setStroke(Color.BLACK);
            isSelected = false;
            selectedCircle = null;
        }
    }

    private boolean newQueen(String message) {
        if (message.substring(10, 11).equals("1"))
            return true;
        else
            return false;
    }

    private boolean wasFigureJumped(String message) {
        if (message.substring(7, 8).equals("1"))
            return true;
        else
            return false;
    }

    //zamknij program
    public void exit(){
        try {
            closeLastConnection();
        }
        catch (Exception e) { }
        Platform.exit();
        System.exit(0);
    }

    private void closeLastConnection(){
        try {
            connection.closeConnection();
        }
        catch (IOException e){
            System.out.println("Nie udalo zamknac sie polaczenia");
        }
    }

    private boolean establishNewConnection(){
        try{
            if (!serverPortTextField.getText().isEmpty() && !serverAddressTextField.getText().isEmpty()) {
                connection = new Connection(serverAddressTextField.getText(), Integer.valueOf(serverPortTextField.getText()));
                return true;
            }
            else {
                return false;
            }
        }
        catch (IOException e){
            return false;
        }
    }

    public Node getNodeByRowColumnIndex (final int row, final int column) {
        Node result = null;
        ObservableList<Node> childrens = gridPane.getChildren();

        for (Node node : childrens) {
            if(GridPane.getRowIndex(node) == row && GridPane.getColumnIndex(node) == column && node.isVisible()) {
                result = node;
                break;
            }
        }

        return result;
    }

    private void removeAllPieces() {
        Circle[] circles = {bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8, bp9, bp10, bp11, bp12, wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8, wp9, wp10, wp11, wp12};

        for (Circle circle : circles) {
            circle.setVisible(false);
        }
    }

    public void initialize() {
        //zmuszamy pole z portem zeby przyjmowal tylko cyfry
        serverPortTextField.textProperty().addListener(new ChangeListener<String>() {
            @Override
            public void changed(ObservableValue<? extends String> observable, String oldValue,
                                String newValue) {
                if (!newValue.matches("\\d*")) {
                    serverPortTextField.setText(newValue.replaceAll("[^\\d]", ""));
                }
            }
        });
    }
}




