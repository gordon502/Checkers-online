package sample;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class Connection{

    private Socket clientSocket;
    private BufferedReader reader;
    private PrintWriter writer;

    public Connection(String address, int port) throws IOException  {
        clientSocket = new Socket(address, port);
        reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        writer = new PrintWriter(clientSocket.getOutputStream(), true);
    }

    public void closeConnection() throws IOException {
        clientSocket.close();
    }

    public String readMessage() throws IOException {
        return reader.readLine();
    }

    public void writeMessage(String message){
        writer.println(message);
    }

}
