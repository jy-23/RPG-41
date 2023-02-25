#include "map.h"
#include <ctime>
#include <iomanip>
#include <unistd.h>
#include "Actors.h"
#include "Bridges.h"
#include "CircDLelement.h"

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

//function to insert into linked list (bridges)
CircDLelement<shared_ptr<Actors>> *insertBack(CircDLelement<shared_ptr<Actors>> *tailElement, CircDLelement<shared_ptr<Actors>> *newElement) {
    CircDLelement<shared_ptr<Actors>> *tailNextElement = tailElement->getNext();

    newElement->setNext(tailNextElement);
    newElement->setPrev(tailElement);

    tailNextElement->setPrev(newElement);
    tailElement->setNext(newElement);

    return newElement;
}

bool sort_Actors(const shared_ptr<Actors>& lhs, const shared_ptr<Actors>&rhs) {
    return rhs->get_speed() < lhs->get_speed();
}

//function to save Heroes
fstream heroSave;
void saveHeroes(Actors* hero1, Actors* hero2) {
    heroSave.open("heroSave.txt", ios::out);
    if (heroSave.is_open()) {
        heroSave << hero1->get_health() << endl;
        heroSave << hero1->get_dead() << endl;
        heroSave << hero1->get_speed() << endl;
        heroSave << hero1->get_money() << endl;

        heroSave << hero2->get_health() << endl;
        heroSave << hero2->get_dead() << endl;
        heroSave << hero2->get_speed() << endl;
    }
    heroSave.close();
}

//function to load Heroes
void loadHeroes(Heroes &hero1, Heroes &hero2) {
    int val;
    bool dead;
    string name;
    heroSave.open("heroSave.txt", ios::in);
    if (heroSave.is_open()) {
        heroSave >> val;
        hero1.set_health(val);
        heroSave >> dead;
        hero1.set_dead(dead);
        heroSave >> val;
        hero1.set_speed(val);
        heroSave >> val;
        hero1.set_money(val);

        heroSave >> val;
        hero2.set_health(val);
        heroSave >> dead;
        hero2.set_dead(dead);
        heroSave >> val;
        hero2.set_speed(val);
    }
    heroSave.close();
}


int main(int argc, char **argv) {
    Bridges *bridges = new Bridges(3, "username", "API key");
    bridges->setTitle("RPG-41 Circular Doubly Linked List");

    turn_off_ncurses();
    char userChoice = ' ';
    Map map;
    int x = Map::SIZE / 2, y = Map::SIZE / 2; //Start in middle of the world
    int old_x = -1, old_y = -1;
    
    srand(time(0));
    vector<shared_ptr<Actors>> playerVec;
    Actors *primaryHero;
    Actors *secondaryHero;
    Warrior w(5);
    Magician m(3);
    Goblin g((rand()%9 + 4);
    Orc o(rand()%9 + 4);

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
            loadHeroes(w,m);
            map.loadMap();
            turn_on_ncurses();
            break;
        }
        else cout << "Please enter a valid choice." << endl;
    }

    playerVec.push_back(make_shared<Warrior> (w));
    playerVec.push_back(make_shared<Magician> (m));
    playerVec.push_back(make_shared<Goblin> (g));
    playerVec.push_back(make_shared<Orc> (o));

    for (int i = 0; i < playerVec.size(); i++) {
        if (playerVec.at(i)->get_name() == "WARRIOR") {
            primaryHero = playerVec.at(i).get();
            continue;
        }
        if (playerVec.at(i)->get_name() == "MAGICIAN") {
            secondaryHero = playerVec.at(i).get();
            continue;
        }
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
        else if (ch == 's' or ch == 'S') {
            turn_off_ncurses();
            primaryHero->print_stats();
            cout << endl;
            secondaryHero->print_stats();
            cout << endl << "Enter '0' to exit" << endl;
            int userKey = -1;
            while (true) {
                cin >> userKey;
                if (userKey == 0) break;
            }
            turn_on_ncurses();
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
                saveHeroes(primaryHero, secondaryHero);
                turn_off_ncurses();
                cout << "Thy game hath saved." << endl;
                usleep(500000);
                turn_on_ncurses();
            }
            
            //enter apothecary
            else if (map.get_char(x,y) == Map::APOTHECARY) {
                turn_off_ncurses();
                int potionChoice = -1;
                int heroChoice = -1;
                while (true) {
                    cout << "APOTHECARY" << endl;
                    cout << setw(22) << left << " (1) Health Potion" << "50 coins" << endl;
                    cout << setw(22) << left << " (2) Stamina Potion" << "30 coins" << endl;
                    cout << endl;
                    cout << "Pick a potion to purchase and use" << endl;
                    cout << "Enter '0' to exit" << endl;
                    cin >> potionChoice;
                    if (potionChoice == 1) {
                        if (primaryHero->get_money()-50 <= 0) {
                            cout << "Not enough coins!" << endl;
                            sleep(1);
                            if (system("clear")) {}
                            continue;
                        }
                        primaryHero->set_money(-50);
                        while (true) {
                            if (system("clear")) {}
                            cout << "Select hero to use potion on" << endl;
                            int num = 1;
                            if (primaryHero->get_dead() == false) {
                                cout << " (" << num << ") " << primaryHero->get_name() << endl;
                                num++;
                            }
                            if (secondaryHero->get_dead() == false) {
                                cout << " (" << num << ") " << secondaryHero->get_name() << endl;
                            }
                            cin >> heroChoice;
                            if (heroChoice == 1 and num == 2) {
                                primaryHero->set_health(primaryHero->get_health() + 50);
                                cout << "Warrior health increased to " << primaryHero->get_health() << endl;
                                sleep(1);
                                if (system("clear")) {}
                                break;
                            }
                            else if (heroChoice == 2 or heroChoice == 1) {
                                secondaryHero->set_health(secondaryHero->get_health() + 50);
                                cout << "Magician health increased to " << secondaryHero->get_health() << endl;
                                sleep(1);
                                if (system("clear")) {}
                                break;
                            }
                        }
                    }
                    else if (potionChoice == 2) {
                        if (primaryHero->get_money()-30 <= 0) {
                            cout << "Not enough coins!" << endl;
                            sleep(1);
                            if (system("clear")) {}
                            continue;
                        }
                        primaryHero->set_money(-30);
                        while (true) {
                            if (system("clear")) {}
                            cout << "Select hero to use potion on" << endl;
                            int num = 1;
                            if (primaryHero->get_dead() == false) {
                                cout << " (" << num << ") " << primaryHero->get_name() << endl;
                                num++;
                            }
                            if (secondaryHero->get_dead() == false) {
                                cout << " (" << num << ") " << secondaryHero->get_name() << endl;
                            }
                            cin >> heroChoice;
                            if (heroChoice == 1 and num == 2) {
                                primaryHero->set_speed(primaryHero->get_speed() + 2);
                                cout << "Warrior speed increased to " << primaryHero->get_speed() << endl;
                                sleep(1);
                                if (system("clear")) {}
                                break;
                            }
                            else if (heroChoice == 2 or heroChoice == 1) {
                                secondaryHero->set_speed(secondaryHero->get_speed() + 2);
                                cout << "Magician speed increased to " << secondaryHero->get_speed() << endl;
                                sleep(1);
                                if (system("clear")) {}
                                break;
                            }
                        }
                    }
                    else if (potionChoice == 0) break;
                }
                turn_on_ncurses();
            }
          
            //enter combat screen
            else if (map.get_char(x,y) == Map::MONSTER) {
                turn_off_ncurses();
                int opponentIndex = 0;
                Actors *attacker = nullptr;
                Actors *opponent = nullptr;
                Combat newCombat;
                
                sort(playerVec.begin(), playerVec.end(), sort_Actors);
                
                CircDLelement<shared_ptr<Actors>> *tail = nullptr;
                for (int i = 0; i < playerVec.size(); i++) {
                    CircDLelement<shared_ptr<Actors>> *temp = new CircDLelement<shared_ptr<Actors>>(playerVec.at(i), "");
                    if (i) tail = insertBack(tail, temp);
                    else tail = temp;
                }
                CircDLelement<shared_ptr<Actors>> *current = tail->getNext();
                CircDLelement<shared_ptr<Actors>> * si = tail->getNext();


                for (int i = 0; i < playerVec.size(); i++) {
                    if (playerVec.at(i)->get_dead() == true) newCombat.dec_heroCount();
                }
                cin.ignore();
                
                while (true) {
                    //get first attacker from linked list
                    while (true) {
                        if (current->getValue().get()->get_dead() == true) {
                            current = current->getNext();
                        }
                        else break;
                    }
                    si = tail->getNext();
                    attacker = current->getValue().get();
                    
                    //visualize using bridges
                    do {
                        si->setLabel(si->getValue().get()->get_name());
                        if (si->getValue().get()->get_dead() == true) {
                            si->setColor("red");
                        }
                        else si->setColor("green");

                        si->getLinkVisualizer(si->getNext())->setColor("blue");
                        si->getLinkVisualizer(si->getNext())->setThickness(2.0f);

                        si->getLinkVisualizer(si->getPrev())->setColor("red");
                        si->getLinkVisualizer(si->getPrev())->setThickness(2.0f);

                        si = si->getNext();
                    } while (si != tail->getNext());

                    bridges->setDataStructure(tail->getNext());
                    bridges->visualize();
                    if (system("clear")) {}

                    //print out combat screen
                    newCombat.combatScreen(playerVec, attacker, "\033[1;33m");
                    //print out attacker's stats
                    cout << "\033[1;33mAttacker's Stats\033[0m\n" << endl;
                    attacker->print_stats();
                    sleep(1);
                  
                    //attacker chooses opponent
                    opponentIndex = attacker->choose_opponent(playerVec);
                    opponent = playerVec.at(opponentIndex).get();
                    usleep(600000);
                    if (system("clear")) {}
                  
                    //attacker makes move causing damage to choosen opponent
                    if (attacker->make_move(opponent) <= 0) {
                        //if opponent health drops to 0, opponent is marked dead
                        opponent->mark_dead(newCombat);
                        cout << attacker->get_name() << " has defeated the " << opponent->get_name() << "!" << endl << endl;
                        sleep(1);
                        //end combat if all members of either hero or monster team are dead
                        if (newCombat.endCombat()) {
                            newCombat.combatScreen(playerVec, opponent, "\033[1;31m");
                            cout << attacker->get_type() << " team has won!" << endl;
                            sleep(3);
                            break;
                        }
                    }
                    
                    if (system("clear")) {}
                    //print out opponent's stats after attack
                    newCombat.combatScreen(playerVec, opponent, "\033[1;31m");
                    cout << "\033[1;31mDamage Report\033[0m" << endl;
                    opponent->print_stats();
                    cout << endl;
                    cout << "Press ENTER to continue." << endl;
                    cin.ignore();
                    if (system("clear")) {}
                    //switch to next attacker in linked list
                    current = current->getNext();
                }
                
                for (int i = 0; i < playerVec.size(); i++) {
                    if (playerVec.at(i)->get_type() == "Monster") {
                        playerVec.at(i)->set_health(100);
                        playerVec.at(i)->set_dead(false);
                        playerVec.at(i)->set_speed((rand()%9)+4);
                    }
                }

                turn_on_ncurses();
                map.set_char(x,y,Map::OPEN);
            }

            //clear(); //Put this in if the screen is getting corrupted
            map.draw(x,y);
            mvprintw(Map::DISPLAY+1,0,"X: %i Y: %i\n",x,y);
            printw("Press 's' for stats\n");
            printw("Coins: %i\n",primaryHero->get_money());
            refresh();
        }
        old_x = x;
        old_y = y;
        usleep(1'000'000/MAX_FPS);
    }
    turn_off_ncurses();
}
