const char *gap(float jarak) {
    if (jarak <= 3.5)
    {
        return "Gogogo";
    }
    else if (jarak <= 10){
        return "Push";
    }
    else{
        return "Stay out of Trouble";
    }
}

const char *fuel(int bensin) {
    if (bensin < 50)
    {
        return "Conserve fuel";
    }
    else if (bensin <= 80){
        return "You can go";
    }
    else{
        return "Push Push Push";
    }
}

const char *ban(int sisa) {
    if (sisa < 30)
    {
        return "Box Box Box";
    }
    else if (sisa <= 50){
        return "Conserve Your Tire";
    }
    else if (sisa <= 80){
        return "Good Tire Wear";
    }
    else{
        return "Push Go Push";
    }
}

const char *tire(char *tipe) {
    if (strcmp("Soft", tipe) == 0)
    {
        return "Mediums Ready";
    }
    else if(strcmp("Medium", tipe) == 0){
        return "Box for Softs";
    }
    else {
        return "Invalid tire type";
    }
}
