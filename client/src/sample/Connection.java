package sample;

import java.io.*;
import java.net.Socket;

public class Connection {

    private Socket clientSocket;
    private InputStream inputStream;
    private PrintWriter writer;
    private boolean threadMessageflag;
    private String threadMessage;

    public boolean isConnected;

    public Connection(String address, int port) throws IOException  {
        clientSocket = new Socket(address, port);
        inputStream = clientSocket.getInputStream();
        writer = new PrintWriter(clientSocket.getOutputStream(), true);
        threadMessageflag = false;
        isConnected = true;
    }

    public void closeConnection() throws IOException {
        clientSocket.close();
    }

    public String readMessage() throws IOException {
        StringBuilder msg = new StringBuilder();
        int x;
        while ((x = inputStream.read()) != 0) {
            msg.append((char) x);
        }
        System.out.println(msg.toString());
        String s = msg.toString().trim();
        return s;
    }


    public void sendMessage(String message) {
        int size = message.length();
        writer.println(size); //wysylamy wiadomosc jak duza jest wiadomosc

        writer.print(message); //wysylamy poprawna wiadomosc
        writer.flush();
    }


    public Socket getSocket() {
        return clientSocket;
    }



}
