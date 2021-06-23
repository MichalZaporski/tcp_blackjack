import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Main {
    // args[0] - host name
    // args[1] - port number


    public static void main(String args[]) throws IOException {
        if (args.length < 2) {
            System.out.println("Podaj nazwe hosta i port");
            return;
        }

        int signal = 0;
        Scanner input = new Scanner(System.in);
        File downloadFile = new File();

        System.out.println("Witamy w kasynie Mini Majkel. Zagraj w Black Jack!");
        try {
            //String a1 = "127.0.0.1";
            //String a2 = "22212";

            String a1 = args[0];
            String a2 = args[1];

            //server address
            InetAddress addr = InetAddress.getByName(a1);

            //server port
            int port = Integer.parseInt(a2);

            //new socket
            Socket socket = new Socket(addr, port);

            //input and output objects
            DataOutputStream dos = new DataOutputStream(
                    socket.getOutputStream());
            DataInputStream dis = new DataInputStream(
                    socket.getInputStream());

            //Object for sending and receiving
            SendRec communicator = new SendRec(dos, dis);

            communicator.recBytes();

            while(true) {
                System.out.println("Wybierz co chcesz zrobic:");
                System.out.println("1. Start gry (1)");
                System.out.println("2. Zasady gry (2)");
                System.out.println("3. Pobierz kod kuponu (3)");
                System.out.println("4. Wyjscie (4)");

                signal = input.nextInt();
                input.nextLine();
                communicator.sendByte(signal);
                switch (signal) {
                    case 1:
                        Game.game(dos, dis, communicator);
                        break;
                    case 2:
                        Game.rules();
                        break;
                    case 3:
                        downloadFile.runClient(socket);
                        break;
                    default:
                        dis.close();
                        dos.close();
                        socket.close();
                        System.exit(0);
                        break;
                }
            }
        }
        catch (Exception e) {
            e.printStackTrace();
            Thread.currentThread().interrupt();
        }
    }
}

