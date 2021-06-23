public class Cards {

    private String[] cardsArray = new String[52];
    Cards(){
        cardsArray = new String[]{"2 kier", "2 karo", "2 trefl", "2 pik",
                                "3 kier", "3 karo", "3 trefl", "3 pik",
                                "4 kier", "4 karo", "4 trefl", "4 pik",
                                "5 kier", "5 karo", "5 trefl", "5 pik",
                                "6 kier", "6 karo", "6 trefl", "6 pik",
                                "7 kier", "7 karo", "7 trefl", "7 pik",
                                "8 kier", "8 karo", "8 trefl", "8 pik",
                                "9 kier", "9 karo", "9 trefl", "9 pik",
                                "10 kier", "10 karo", "10 trefl", "10 pik",
                                "Walet kier", "Walet karo", "Walet trefl", "Walet pik",
                                "Dama kier", "Dama karo", "Dama trefl", "Dama pik",
                                "Krol kier", "Krol karo", "Krol trefl", "Krol pik",
                                "As kier", "As karo", "As trefl", "As pik"};

    }
    public String getCard(short a){
        return this.cardsArray[a];
    }
}
