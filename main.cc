#include "map.h"
#include <ctime>
#include <iomanip>
#include <unistd.h>
#include "Actors.h"
#include "circLinkedList.h"

const int MAX_FPS = 90; //Cap frame rate
const unsigned int TIMEOUT = 10; //Milliseconds to wait for a getch to finish
const int UP = 65; //Key code for up arrow
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;

//Turns on full screen text mode
void turn_on_ncurses() {
    initscr();//Start curses mode
    start_color(); //Enable Colors if possible
    init_pair(1,COLOR_WHITE,COLOR_BLACK); //Set up some color pairs
    init_pair(2,COLOR_CYAN,COLOR_BLACK);
    init_pair(3,COLOR_GREEN,COLOR_BLACK);
    init_pair(4,COLOR_YELLOW,COLOR_BLACK);
    init_pair(5,COLOR_RED,COLOR_BLACK);
    init_pair(6,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(7,COLOR_BLUE,COLOR_BLACK);
    clear();
    noecho();
    cbreak();
    timeout(TIMEOUT); //Set a max delay for key entry
}

//Exit full screen mode - also do this if you ever want to use cout or gtest or something
void turn_off_ncurses() {
    clear();
    endwin(); // End curses mode
    if (system("clear")) {}
}

bool sort_Actors(const shared_ptr<Actors>& lhs, const shared_ptr<Actors>&rhs) {
    return rhs->get_speed() < lhs->get_speed();
}


int main() {
    turn_off_ncurses();
    char userChoice = ' ';
    Map map;
    int x = Map::SIZE / 2, y = Map::SIZE / 2; //Start in middle of the world
    int old_x = -1, old_y = -1;

    cout << "Doth thou wish to starteth a new game or loadeth thy current game?" << endl;
    cout << "Typeth L for load, N for new" << endl;
    while (true) {
        cin >> userChoice;
        if (userChoice == 'N' or userChoice == 'n') {
            map.init_map();
            turn_on_ncurses(); //DON'T DO CIN or COUT WHEN NCURSES MODE IS ON
            break;
        }
        else if (userChoice == 'L' or userChoice == 'l') {
            map.loadMap();
            turn_on_ncurses();
            break;
        }
        else cout << "Please enter a valid choice." << endl;
    }

    //TODO: new game-> have user select heroes to play
          //load game-> load characters
          //save game
    srand(time(0));
    vector<shared_ptr<Actors>> playerVec;
    Warrior w(10);
    Magician m(13);
    Goblin g(8);
    Orc o(12);

    playerVec.push_back(make_shared<Warrior> (w));
    playerVec.push_back(make_shared<Magician> (m));
    playerVec.push_back(make_shared<Goblin> (g));
    playerVec.push_back(make_shared<Orc> (o));

    sort(playerVec.begin(), playerVec.end(), sort_Actors);

    CircLinkedList list;
    for (shared_ptr<Actors> a : playerVec) {
        list.push_back(a.get());
    }

    while (true) {
        int ch = getch(); // Wait for user input, with TIMEOUT delay
        if (ch == 'q' || ch == 'Q') break;
        else if (ch == RIGHT) {
            x++;
            if (x >= Map::SIZE) x = Map::SIZE - 1; //Clamp value
        }
        else if (ch == LEFT) {
            x--;
            if (x < 0) x = 0;
        }
        else if (ch == UP) {
            /* If you want to do cin and cout, turn off ncurses, do your thing, then turn it back on
            turn_off_ncurses();
            string s;
            cin >> s;
            cout << s << endl;
            sleep(1);
            turn_on_ncurses();
            */
            y--;
            if (y < 0) y = 0;
        }
        else if (ch == DOWN) {
            y++;
            if (y >= Map::SIZE) y = Map::SIZE - 1; //Clamp value
        }
        else if (ch == ERR) { //No keystroke
            ; //Do nothing
        }
        //Stop flickering by only redrawing on a change
        if (x != old_x or y != old_y) {
            if (map.get_char(x,y) == Map::TREASURE) {
                map.set_char(x,y,Map::OPEN);
                //TODO: increase money
            }
            else if (map.get_char(x,y) == Map::WALL) {
                x = old_x;
                y = old_y;
            }
            else if (map.get_char(x,y) == Map::SAVE) {
                map.saveMap();
                turn_off_ncurses();
                cout << "Thy game hath saved." << endl;
                usleep(500000);
                turn_on_ncurses();
            }
          
            //enter combat screen
            else if (map.get_char(x,y) == Map::MONSTER) {
                turn_off_ncurses();
                Node currentNode;
                int opponentIndex = 0;
                Actors *attacker = nullptr;
                Actors *opponent = nullptr;
                Combat newCombat;
                string key = "a";
                
                while (true) {
                    //get first attacker from linked list
                    currentNode = list.get_current();
                    attacker = currentNode.a;

                    //print out combat screen
                    newCombat.combatScreen(playerVec, attacker, "\033[1;33m");
                    sleep(1);
                  
                    //print out attacker's stats
                    cout << "\033[1;33mAttacker's Stats\033[0m\n" << endl;
                    attacker->print_stats();
                    sleep(2);
                  
                    //attacker chooses opponent
                    opponentIndex = attacker->choose_opponent(playerVec);
                    opponent = playerVec.at(opponentIndex).get();
                    sleep(2);
                    if (system("clear")) {}
                  
                    //attacker makes move causing damage to choosen opponent
                    if (attacker->make_move(opponent) <= 0) {
                        //if opponent health drops to 0, opponent is marked dead
                        opponent->mark_dead(newCombat);
                        cout << attacker->get_name() << " has defeated the " << opponent->get_name() << "!" << endl << endl;
                        sleep(2);
                        //end combat if all members of either hero or monster team are dead
                        if (newCombat.endCombat()) {
                            newCombat.combatScreen(playerVec, opponent, "\033[1;31m");
                            cout << attacker->get_type() << " team has won!" << endl;
                            sleep(5);
                            break;
                        }
                    }
                  
                    //print out opponent's stats after attack
                    newCombat.combatScreen(playerVec, opponent, "\033[1;31m");
                    cout << "\033[1;31mDamage Report\033[0m" << endl;
                    opponent->print_stats();
                    cout << endl;
                    cout << "Press ENTER to continue." << endl;
                    cin.ignore();
                    if (system("clear")) {}
                    //switch to next attacker in linked list
                    list.set_current();
                }

                turn_on_ncurses();
                map.set_char(x,y,Map::OPEN);
            }

            //clear(); //Put this in if the screen is getting corrupted
            map.draw(x,y);
            mvprintw(Map::DISPLAY+1,0,"X: %i Y: %i\n",x,y);
            refresh();
        }
        old_x = x;
        old_y = y;
        usleep(1'000'000/MAX_FPS);
    }
    turn_off_ncurses();
}
