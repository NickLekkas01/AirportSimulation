#include "header.h"
using namespace std;
ifstream fileFlights;
ifstream Appfile;

class Flight;

class Application
{
  private:
    string id;
    string name;
    string surname;
    string Destination;
    time_t Airport_Time;
    time_t Destination_Time;
    char EconomyClass; //0=economy ... 1=luxury
  public:
    Application() { /*cout << "I just created an application" << endl;*/ }

    ~Application() { /*cout << "I just destroyed an application" << endl;*/ }

    void setdata(string ID,string nam,string surn,string Destin,time_t Air_time,time_t Dest_time,char Econ_class)
    {
        id=ID;
        name=nam;
        surname=surn;
        Destination=Destin;
        Airport_Time=Air_time;
        Destination_Time=Dest_time;
        EconomyClass=Econ_class;
    }

    string get_id() const { return id; }

    string get_name() const
    {
        string WholeName;
        WholeName = name + " " + surname;
        return WholeName;
    }

    string get_appDestination() const { return Destination; }

    time_t get_AirportTime() const { return Airport_Time; }

    time_t arrived_by() const { return Destination_Time; }

    char is_luxury_class() const{ return EconomyClass; }

    int matches(Flight &);   
};

class Flight
{
  private:
    list<class Application> ApplicationsList;
    string Destination;
    time_t Departure_Time;
    time_t Travel_Duration;
    int max_numberA; //Economy
    int max_numberB; //Luxury
    int *ReservationA;
    string *IDReservationA;
    int *ReservationB;
    string *IDReservationB;
    int counterA; //Economy
    int counterB; //Luxury
    int terminal_number;

  public:
    const time_t get_TravelDuration() const { return Travel_Duration; }

    void set_terminal_number(int term) { terminal_number=term; }

    const int get_terminal_number() const{ return terminal_number; }

    void add_passenger(class Application &App)
    {
        if ((App).is_luxury_class() == 'A')
        {
            if (counterA < max_numberA)
            {
                ApplicationsList.push_back(App);        //Add passenger to application List
                counterA++;                             //add the counterA by one if he was from economy class
            }
        }
        else
        {
            if (counterB < max_numberB)
            {
                ApplicationsList.push_back(App);        //Add passenger to application List
                counterB++;                             //add the counterB by one if he was from luxury class
            }
        }
    }

    list<class Application> edit_AppList() const { return ApplicationsList; }      //take ApplicationList to edit it

    void cancel_reservations(string ID)
    {
        list<class Application>::iterator it;
        for (it = ApplicationsList.begin() ; it != ApplicationsList.end() ; )
        {
            if ((*it).get_id().compare(ID) == 0)            //Find the Application with the specific ID
            {
                if ((*it).is_luxury_class() == 'A')
                    counterA--;                             //If he was from Economy class subtract counter by 1
                else
                    counterB--;                             //If he was from Luxury class subtract counter by 1
                it=ApplicationsList.erase(it);              //Erase application with specific ID
            }
            else
                it++;
        }
    }

    const time_t departs_at() const{ return Departure_Time; }

    const string get_destination()const { return Destination; }

    const int get_available() const{return ((max_numberA + max_numberB) - (counterA + counterB)); }
    
    const list<class Application> get_bookings()const { return ApplicationsList; }

    const int get_counter(char ch) const
    {
        if (ch == 'A')
            return counterA;
        else
            return counterB;
    }   

    const int get_maxnumber(char ch) const
    {
        if (ch == 'A')
            return max_numberA;
        else
            return max_numberB;
    }

    void setdata(string Dest,time_t Dep_time,time_t Tr_Dur,int max_a,int max_b)                                                          //insert data
    {
        Destination=Dest;
        Departure_Time=Dep_time;
        Travel_Duration=Tr_Dur;
        max_numberA=max_a;
        max_numberB=max_b;
    }

    Flight() : counterA(0), counterB(0)
    {
        //cout << "I just created a Flight" << endl;
    }

    ~Flight()
    {
        list<Application>::iterator it = ApplicationsList.begin();
        while (it != ApplicationsList.end())
        {
            it=ApplicationsList.erase(it);                                  //Delete the ApplicationList
        }
        //cout << "I just deleted a Flight" << endl;
    }
};

class Airport
{
  private:
    time_t Present_time;
    int terminals;
    int *terminal_availability;
    list<class Application> WaitingList;
    list<class Flight> FlightsList;
    int counterFlights;
    int counter_cancelled;

  public:
    void distribute_terminal()
    {
        list<class Flight>::iterator it;
        it = FlightsList.begin();
        int i=0;
        time_t previous_time=(*it).departs_at();                //insert the first flight's destination time
        int counter=1;                                          //we have one flight with the same value
        (*it).set_terminal_number(i%get_terminals());           //set the terminal of the flights
        i++;
        it++;
        while(it!=FlightsList.end())
        {
            if((*it).departs_at()==previous_time)               //If we find a flight with the same destination time as the previous one
                counter++;                                      //we have counter flights with the same value
            else
                counter=1;                                      //else we have one(the one we checked now)
            previous_time=(*it).departs_at();                   //make the current value as the previous
            if(counter>get_terminals())                         //If we have more flights than terminals with the same value then erase the extra
                it=FlightsList.erase(it);
            else
            {
                (*it).set_terminal_number(i%get_terminals());
                i++;
                it++;
            }
        }
    }

    void sortFlights() { FlightsList.sort(compareFlights); }    //sort the FlightsList

    const int get_terminals() const { return terminals; }

    time_t get_time() 
    {
        time(&Present_time);
        return Present_time;
    }

    void add_application(string ID,string nam,string surn,string Destin,time_t Air_time,time_t Dest_time,char Econ_class)
    {
        class Application *App;
        App = new Application();                            //Create an application
        App->setdata(ID,nam,surn,Destin,Air_time,Dest_time,Econ_class);                                     //Insert Data from file
        int flag = 0;
        list<class Flight>::iterator it;
        for (it = FlightsList.begin(); it != FlightsList.end(); it++)
        {
            if (App->matches(*it) == 0)                     //If the application matches the proper conditins      
            {
                (*it).add_passenger(*App);                  //Add the passenger to the ApplicationList
                flag = 1;
                break;
            }
        }
        if (flag == 0)
            WaitingList.push_back(*App);                    //Else add the passenger to the waiting list
    }

    void cancel_application(string ID)
    {
        list<class Application>::iterator it;
        for (it = WaitingList.begin(); it != WaitingList.end(); )
        {
            if ((*it).get_id().compare(ID) == 0)            //Find the application with the specific ID
                it=WaitingList.erase(it);                   //Erase application from WaitingList
            else
                it++;
        }
        list<class Flight>::iterator it2;
        for (it2 = FlightsList.begin(); it2 != FlightsList.end(); it2++)
        {
            list<class Application>::iterator it3,it4;
            list<class Application> applist = (*it2).edit_AppList();
            it3=applist.begin();
            it4=applist.end();
            for( ; it3 != it4; )
            {
                if ((*it3).get_id().compare(ID) == 0)
                    it3=applist.erase(it3);
                else
                    it3++;
            }
        }
    }

    void add_flight(string Dest,time_t Dep_time,time_t Tr_Dur,int max_a,int max_b)
    {
        Flight *Fl;
        Fl = new Flight();
        Fl->setdata(Dest,Dep_time,Tr_Dur,max_a,max_b);
        counterFlights++;
        FlightsList.push_back(*Fl);
    }

    void cancel_flight(int terminal)
    {
        list<class Flight>::iterator it;
        for (it = FlightsList.begin(); it != FlightsList.end(); )
        {
            if ((*it).get_terminal_number() == terminal)                //Find the flight with specific terminal
            {
                list<class Application>::iterator it2, it3;
                list<class Application> applist = (*it).edit_AppList();
                it2=applist.begin();
                it3=applist.end();
                for (; it2 != it3; it2++)
                {
                    WaitingList.push_back(*it2);                        //Add the passengers of the flight to WaitingList
                    counter_cancelled++;                                //Counter tha cancelled applications 
                }
                it=FlightsList.erase(it);                               //Erase Flight
            }
            else
                it++;
        }
    }

    const int failed_applications() const{ return counter_cancelled; }

    static bool compareFlights(class Flight &a,class Flight &b)
    {
        return a.departs_at() < b.departs_at();                                                                     //Compare function for sort
    }

    void show_timetable()
    {
        list<class Flight>::iterator it;
        cout<<endl<<"Flights "<<endl;
        for (it = FlightsList.begin(); it != FlightsList.end(); it++)
        {
            cout << (*it).get_destination() << endl;                                                                //Print FlightsList
        }
        cout<<endl<<"Available Flights "<<endl<<endl;
        for (it = FlightsList.begin(); it != FlightsList.end(); it++)
        {
            if((*it).get_available()>0)
                cout << (*it).get_destination() << endl;                                                            //Print Available FlightsList
        }
        cout << endl;
    }

    void show_people_waiting()
    {
        list<class Application>::iterator it;
        for (it = WaitingList.begin(); it != WaitingList.end(); it++)
        {
            cout << "Waiting List:" << (*it).get_name() << endl;                                                    //Print WaitingList
        }
    }

    void flow_time(time_t time)
    {
        list<class Flight>::iterator it;
        list<class Application>::iterator it4;
        it = FlightsList.begin();
        while(it!=FlightsList.end())
        {
            if ((*it).departs_at() <= time)                                                                         //Flight before "time"
            {
                cout<<"Plane entered terminal "<<(*it).get_terminal_number()<<" at "<<(*it).departs_at()<<endl;     //An airplane entered the terminal at that time
                terminal_availability[(*it).get_terminal_number()]=1;                                               //Fix terminal availability to 1
                cout<<"Plane left terminal "<<(*it).get_terminal_number()<<" at "<<(*it).departs_at()<<endl;        //An airplane left the terminal at that time
                terminal_availability[(*it).get_terminal_number()]=0;                                               //Fix terminal availability to 0
                cout << "Destination " << (*it).get_destination() << endl;                                          //Print Flight's destination
                list<class Application>::iterator it2, it3;
                list<class Application> applist = (*it).edit_AppList();
                it2 = applist.begin(); 
                it3 = applist.end();
                for (; it2 != it3; )
                {
                    cout << "Name " << (*it2).get_name() << endl;                                                   //Print the names of the people in the flight
                    it2=applist.erase(it2);
                }
                it=FlightsList.erase(it);                                                                           //Erase the Flight which deployed
                continue;
            }
            it++;
        }
        for(it4=WaitingList.begin(); it4!=WaitingList.end() ; )
        {
            if(time>(*it4).arrived_by())
                it4=WaitingList.erase(it4);                                   //If the time he want to arrive is passed then erase the passenger from waiting list
            else
                it4++;
        }
    }

    /*
    void flow_time(time_t time)
    {
        list<class Flight>::iterator it;
        list<class Application>::iterator it4;
        it = FlightsList.begin();
        for(time_t i=0 ; i<time ; i++)                                                                                  //Simulation of running time
        {
            while(it!=FlightsList.end())
            {
                if ((*it).departs_at() == i)                                                                            //Flight before "time"
                {
                    cout<<"Plane entered terminal "<<(*it).get_terminal_number()<<" at "<<(*it).departs_at()<<endl;     //An airplane entered the terminal at that time
                    terminal_availability[(*it).get_terminal_number()]=1;                                               //Fix terminal availability to 1
                    cout<<"Plane left terminal "<<(*it).get_terminal_number()<<" at "<<(*it).departs_at()<<endl;        //An airplane left the terminal at that time
                    terminal_availability[(*it).get_terminal_number()]=0;                                               //Fix terminal availability to 0
                    cout << "Destination " << (*it).get_destination() << endl;                                          //Print Flight's destination
                    list<class Application>::iterator it2, it3;
                    list<class Application> applist = (*it).edit_AppList();
                    it2 = applist.begin(); 
                    it3 = applist.end();
                    for (; it2 != it3; it2++)
                    {
                        cout << "Name " << (*it2).get_name() << endl;   //Print the names of the people in the flight
                    }
                    it=FlightsList.erase(it);                           //Erase the Flight which deployed
                    continue;
                }
                it++;
            }
            for(it4=WaitingList.begin(); it4!=WaitingList.end() ; )
            {
                if(i==(*it4).arrived_by())
                    it4=WaitingList.erase(it4);                         //If the time he want to arrive is passed then erase the passenger from waiting list
                else
                    it4++;
            }
        }
    }
    */

    Airport() : terminals(20), counterFlights(0), counter_cancelled(0)  //I have 20 terminals but the size can change
    {
        cout << "I just constructed an Airport" << endl;
        terminal_availability = new int[terminals];         //Availability of the terminals (0 or 1)
        for (int i = 0; i < terminals; i++)
            terminal_availability[i] = 0;                   //0 for empty
    }

    ~Airport()
    {
        list<Application>::iterator it = WaitingList.begin();
        while (it != WaitingList.end())
        {
            it = WaitingList.erase(it);                     //erase the waitinglist
        }
        list<Flight>::iterator it1 = FlightsList.begin();
        while (it1 != FlightsList.end())
        {
            it1 = FlightsList.erase(it1);                   //erase the flightslist
        }
        cout << "I just destroyed an airport" << endl;
    }
};

int Application::matches(Flight &Fl)
{       //If the time to arrive at the airport is before the departure time && if the arrival time is before the time the passenger wants to arrive
        if (Airport_Time <= Fl.departs_at() && Destination_Time>Fl.departs_at()+Fl.get_TravelDuration())
        {
            if (Fl.get_available() > 0)                     //If there are empty seats in the flight
            {
                if (Fl.get_destination() == Destination)    //If the Destination he wants is the same destination of the flight
                    return 0;                               //Then we have a match
                else
                    return 1;                               //Not a match
            }
            else
            {
                return 1;                                   //Not a match
            }
        }
        else
            return 1;                                       //Not a match
}

int main(int argc, char *argv[])
{
    time_t time = (time_t)atoi(argv[1]);    //take time from command line
    Airport Venizelos;
    string s1;
    fileFlights.open("airportData.txt");
    Appfile.open("applicationData.txt");
    string line;
    getline(fileFlights, line);             //read the first line which has the types
    string Dest;
    time_t Dep_time,Tr_Dur;
    int max_a,max_b;

    while (true)
    {
        //cout<<"Give me Destination"<<endl;
        fileFlights >> Dest;
        //cout<<"Give me Departu_Time"<<endl;
        fileFlights >> Dep_time;
        //cout<<"Give me Travel Duration"<<endl;
        fileFlights >> Tr_Dur;
        //cout<<"Give me maxnumber A and B"<<endl;
        fileFlights >> max_a >> max_b;
        Venizelos.add_flight(Dest,Dep_time,Tr_Dur,max_a,max_b);
        if (fileFlights.eof())
            break;
    }
    Venizelos.sortFlights();
    Venizelos.distribute_terminal();
    getline(Appfile, line);                 //read the first line which has the types
    string ID,nam,surn,Destin;
    time_t Air_time,Dest_time;
    char Econ_class;

    while (true)
    {
        Appfile >> ID;
        Appfile >> nam >> surn;
        Appfile >> Destin;
        Appfile >> Air_time;
        Appfile >> Dest_time;
        Appfile >> Econ_class;
        Venizelos.add_application(ID,nam,surn,Destin,Air_time,Dest_time,Econ_class);
        if (Appfile.eof())
            break;
    }
    cout<<"<------------------------------------>"<<endl;
    cout<<endl<<"Show timetable"<<endl;     
    cout<<"<------------------------------------>"<<endl;
    Venizelos.show_timetable();
    cout<<"<------------------------------------>"<<endl;
    cout<<endl<<"Show people waiting"<<endl;
    Venizelos.show_people_waiting();
    cout<<"<------------------------------------>"<<endl;
    Venizelos.flow_time(time);
    cout<<endl<<"Show timetable"<<endl;
    cout<<"<------------------------------------>"<<endl;
    Venizelos.show_timetable();
    cout<<"<------------------------------------>"<<endl;
    cout<<endl<<"Show people waiting"<<endl;
    Venizelos.show_people_waiting();
    cout<<"<------------------------------------>"<<endl;
    fileFlights.close();
    Appfile.close();
    return 0;
}
