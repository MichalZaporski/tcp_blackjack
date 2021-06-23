import java.net.*;
import java.io.*;
import java.util.concurrent.TimeUnit;

public class File {

    public void runClient(Socket socket) throws IOException {
        String outputFile = "money.png";
        InputStream in = null;
        OutputStream out = null;
        try {
            in = socket.getInputStream();
            out = new FileOutputStream(outputFile);

            DataInputStream din = new DataInputStream(in);
            int file_size = din.readInt();
            System.out.println("File size: " + file_size);
            byte[] byteArray = new byte[file_size];
            int allBytesRead = 0;
            int tempBytesRead;

            do {
                tempBytesRead = in.read(byteArray, allBytesRead, (byteArray.length-allBytesRead));
                System.out.println("crb:"+tempBytesRead+" trb:"+allBytesRead);
                if(tempBytesRead > 0) allBytesRead += tempBytesRead;
            } while(tempBytesRead > 0);


            out.write(byteArray);
            out.flush();
            System.out.println("Read " + file_size + " bytes of message from server.");

        } catch (Exception e) {
            e.printStackTrace();
        }

    }
}
