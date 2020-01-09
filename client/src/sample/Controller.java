package sample;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.Node;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
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

    private Connection connection;



    //elementy sceny
    public Stage window;
    public Button newGameButton;
    public Button exitButton;
    public GridPane gridPane;
    public Circle bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8, bp9, bp10, bp11, bp12, wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8, wp9, wp10, wp11, wp12;


    private void setUpPiecesAndVariables(String receivedMessage) {
        if(receivedMessage.startsWith("9"))
            myPlayerNumber = receivedMessage.substring(1,2);
        if (myPlayerNumber.equals("1")) {
            isMyTurn = true;
        }
        else {
            isMyTurn = false;
            System.out.println("wylaczono Grid");
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
            Thread thread = new Thread(() -> {
                try {
                    final String firstMessage = receiveMessage();
                    Platform.runLater(() -> { setUpPiecesAndVariables(firstMessage); } );

                    while (connection.isConnected) {
                        final String receivedMessage = receiveMessage();
                        if (receivedMessage.startsWith("0")){
                            connection.isConnected = false;
                            System.out.println("Utracono polaczenie");
                            closeLastConnection();
                            break;
                        }
                        if (wasMoveCorrect(receivedMessage)) {
                            Platform.runLater(()-> { updateBoard(receivedMessage); });
                            Platform.runLater(() -> { updateGameVariables(receivedMessage); });
                        }
                    }
                } catch (IOException e) {
                    connection.isConnected = false;
                }


            });
            thread.start();
        }
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
            if ((myPlayerNumber.equals("1") && pom.getFill().equals(wp1.getFill())) || (myPlayerNumber.equals("2") && pom.getFill().equals(bp1.getFill()))) {
                System.out.println("zaznaczylem figure");
                selectedCircle = (Circle) event.getSource();
                isSelected = true;
                selectedCircle.setStroke(Color.GREEN);
            }
        }
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

            System.out.println(message);
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
        System.out.println(message.substring(3,4));
        Integer fromrowindex = Integer.valueOf(message.substring(3, 4));
        Integer fromcolumnindex = Integer.valueOf(message.substring(4, 5));
        Integer torowindex = Integer.valueOf(message.substring(5, 6));
        Integer tocolumnindex = Integer.valueOf(message.substring(6, 7));

        Circle movedCircle = (Circle) getNodeFromGridPane(fromcolumnindex, fromrowindex);
        GridPane.setRowIndex(movedCircle, torowindex);
        GridPane.setColumnIndex(movedCircle, tocolumnindex);

        if (newQueen(message)) {

        }

        if (wasFigureJumped(message)) {
            int rowindex = Integer.valueOf(message.substring(8, 9));
            int columnindex = Integer.valueOf(message.substring(9, 10));
            System.out.println(rowindex + " " + columnindex);
            Circle removedFigure = (Circle) getNodeFromGridPane(columnindex, rowindex);
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
        closeLastConnection();
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
            connection = new Connection("localhost", 2223);
            return true;
        }
        catch (IOException e){
            System.out.println("Blad w tworzeniu polaczenia");
            return false;
        }
    }

    private Node getNodeFromGridPane(int col, int row) {
        for (Node node : gridPane.getChildren()) {
            if (GridPane.getColumnIndex(node) == col && GridPane.getRowIndex(node) == row) {
                return node;
            }
        }
        return null;
    }
}




