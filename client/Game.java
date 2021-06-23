import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.Scanner;

public class Game {
    public static void game(DataOutputStream dos, DataInputStream dis, SendRec communicator){
        int signal, hitStand, dealerPoints, money;
        String bet;

        Scanner input = new Scanner(System.in);

        communicator.recBytes();

        //start
        money = 1;
        try{
            while(money > 0) {
                Thread.sleep(800);
                communicator.recBytes();

                //how much money player has
                Thread.sleep(800);
                communicator.recInt();

                //amount of bet
                Thread.sleep(500);
                communicator.recBytes();
                bet = input.nextLine();
                communicator.sendBytes(bet);
                communicator.recBytes();

                //bad ammount
                signal = communicator.recByte();
                if (signal == 1) {
                    continue;
                }

                //first 2 dealers cards
                Thread.sleep(1000);
                communicator.recBytes();
                Thread.sleep(1000);
                communicator.recShortCards();
                Thread.sleep(1000);
                communicator.recBytes();
                Thread.sleep(1000);
                communicator.recBytes();

                //dealer points
                Thread.sleep(1000);
                communicator.recBytes();
                Thread.sleep(1000);
                communicator.recInt();

                //client first 2 cards
                Thread.sleep(1000);
                communicator.recBytes();
                Thread.sleep(1000);
                communicator.recShortCards();

                Thread.sleep(1000);
                communicator.recBytes();
                Thread.sleep(1000);
                communicator.recShortCards();

                //client points
                Thread.sleep(1000);
                communicator.recBytes();
                Thread.sleep(1000);
                communicator.recInt();

                //after first 4 cards (black jack?)
                signal = communicator.recByte();
                if (signal == 1) {
                    communicator.recBytes();
                    continue;
                }

                //first hit or stand
                communicator.recBytes();
                hitStand = input.nextInt();
                input.nextLine();
                communicator.sendByte(hitStand);

                //hit or stand loop
                signal = 0;

                while (hitStand == 1 && signal == 0) {
                    Thread.sleep(1000);
                    communicator.recBytes();
                    Thread.sleep(1000);
                    communicator.recShortCards();
                    Thread.sleep(1000);
                    communicator.recBytes();
                    Thread.sleep(1000);
                    communicator.recInt();

                    Thread.sleep(800);
                    communicator.recBytes();
                    signal = communicator.recByte();
                    if (signal == 0) {
                        hitStand = input.nextInt();
                        input.nextLine();
                        communicator.sendByte(hitStand);
                    }
                }

                //dealer taking cards till 17
                if (signal == 0) {
                    //clients points
                    Thread.sleep(1000);
                    communicator.recBytes();
                    Thread.sleep(1000);
                    communicator.recInt();
                    //dealer second card
                    Thread.sleep(1000);
                    communicator.recBytes();
                    Thread.sleep(1000);
                    communicator.recShortCards();
                    //dealer points
                    Thread.sleep(1000);
                    communicator.recBytes();
                    Thread.sleep(1000);
                    dealerPoints = communicator.recInt();

                    while (dealerPoints < 17) {
                        Thread.sleep(1000);
                        communicator.recBytes();
                        Thread.sleep(1000);
                        communicator.recShortCards();
                        Thread.sleep(1000);
                        communicator.recBytes();
                        Thread.sleep(1000);
                        dealerPoints = dealerPoints = communicator.recInt();
                    }
                    communicator.recBytes();
                }
                //results
                communicator.recBytes();
                money = communicator.recInt();

                if (money < 1) {
                    System.out.println("Jestes banrutem!");
                } else {
                    communicator.recBytes();
                    signal = input.nextInt();
                    input.nextLine();
                    communicator.sendByte(signal);
                    if (signal == 0) {
                        break;
                    }
                }

            }

        }

        catch (Exception e){
            e.printStackTrace();
            Thread.currentThread().interrupt();
        }

        System.out.println("Zapraszamy do gry ponownie!");

    }

    public static void rules(){
        System.out.print("Zasady gry: \n" +
                "W grze gramy talia kart z czego figury sa warte 10 pkt, AS 11 pkt, numery odpowiednio wartosci liczbowe.\n" +
                "Najpierw stawiamy kwote ktora chcemy postawic.\n" +
                "Gramy przeciwko dealerowi. Na poczatku dealer dostaje 2 karty z czego jedna jest zakryta.\n" +
                "Nastepnie gracz otrzymuje 2 karty. Nastepnie gracz wybiera czy chce pobierac karty dalej.\n" +
                "Celem gry jest zblizenie sie jak najblizej do liczby 21 w sumie naszych kart.\n" +
                "Jesli gracz przekroczy 21 przegrywa. \n" +
                "Kiedy gracz zdecyduje sie przestac dobierac karty, dealer odkrywa swoja druga karte.\n" +
                "Jesli dealer po odslonieciu karty ma mniej niz 17 pkt, to dobiera z puli az do osiagniecia liczby >= 17. \n" +
                "Jezeli dealer ma wiecej niz 21 pkt przegrywa. \n" +
                "Jezeli gracz ma wiecej pkt niz dealer to otrzymuje 2x tyle pieniedzy ile postawil.\n" +
                "Jezeli gracz i dealer maja tyle samo pkt, graczowi zwracana jest stawka. \n" +
                "Uwaga! Jezeli gracz otrzyma z reki asa + figure lub 2 asy nastepuje BLACK JACK i gracz wygrywa 1,5 stawki! \n");
    }
}
