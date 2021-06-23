import java.io.DataInputStream;
import java.io.DataOutputStream;

public class SendRec{
    private DataOutputStream dos;
    private DataInputStream dis;
    private Cards cards;

    SendRec(DataOutputStream dos, DataInputStream dis){
        this.dis = dis;
        this.dos = dos;
        this.cards = new Cards();
    }

    public void recBytes(){
        int readByte = 0;
        try {
            while (readByte != 10) {
                readByte = dis.read();
                System.out.print((char) readByte);
            }
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    public int recByte(){
        int readByte = 0;
        try {
            readByte = dis.read();
        }
        catch (Exception e){
            e.printStackTrace();
        }
        return readByte;
    }

    public int recInt(){
        int a = 0;
        try {
            a = dis.readInt();
            System.out.println(a);
        }
        catch (Exception e){
            e.printStackTrace();
        }
        return a;
    }

    public void recShortCards(){
        short a;
        try {
            a = dis.readShort();
            System.out.println(cards.getCard(a));
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }


    public void sendBytes(String a){
        try {
            dos.writeByte(a.length() + 48);
            dos.writeBytes(a);
            dos.flush();
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
    public void sendByte(int a){
        try {
            dos.writeByte(a);
            dos.flush();
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
}