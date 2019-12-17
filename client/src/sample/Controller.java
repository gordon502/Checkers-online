package sample;

import javafx.application.Platform;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;

public class Controller {

    //pomocnicze
    private Circle selectedCircle = null;
    private boolean isSelected = false;

    //elementy sceny
    public Button newGameButton;
    public Button exitButton;
    public GridPane gridPane;
    public Circle bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8, bp9, bp10, bp11, bp12, wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8, wp9, wp10, wp11, wp12;

    //przenies figury na pozycje startowe i ustaw z powrotem ich widocznosc
    public void newGame(){
        Circle[] circles = {bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8, bp9, bp10, bp11, bp12, wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8, wp9, wp10, wp11, wp12};
        int indexes[][] = { {1,0}, {3,0}, {5,0}, {7,0}, {0,1}, {2,1}, {4,1}, {6,1}, {1,2}, {3,2}, {5,2}, {7,2},
                            {0,5}, {2,5}, {4,5}, {6,5}, {1,6}, {3,6}, {5,6}, {7,6}, {0,7}, {2,7}, {4,7}, {6,7}};
        for (int i = 0; i < circles.length; i++){
            circles[i].setVisible(true);
            GridPane.setColumnIndex(circles[i], indexes[i][0]);
            GridPane.setRowIndex(circles[i], indexes[i][1]);
        }
        if(isSelected) {
            selectedCircle.setStroke(Color.BLACK);
            isSelected = false;
        }
    }

    //podswietl figure
    public void figureOnMouseEntered(MouseEvent event){
        Circle source = (Circle) event.getSource();
        if(!(selectedCircle == source))
            source.setStroke(Color.RED);
    }

    public void figureOnMouseExited(MouseEvent event){
        Circle source = (Circle) event.getSource();
        if(!(selectedCircle == source))
            source.setStroke(Color.BLACK);
    }

    public void selectFigure(MouseEvent event){
        if(isSelected == true)
            selectedCircle.setStroke(Color.BLACK);
        selectedCircle = (Circle) event.getSource();
        isSelected = true;
        selectedCircle.setStroke(Color.GREEN);
    }

    //przenies figure z jednego pola na drugie
    public void makeMove(MouseEvent mouseEvent){
        if (isSelected == false)
            System.out.println("nie wybrales figury");
        else{
            int cindex = (int) (mouseEvent.getX()/(gridPane.getWidth()/8));
            int rindex = (int) (mouseEvent.getY()/(gridPane.getHeight()/8));
            GridPane.setRowIndex(selectedCircle, rindex);
            GridPane.setColumnIndex(selectedCircle, cindex);
        }
    }

    //zamknij program
    public void exit(){
        Platform.exit();
        System.exit(0);
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




