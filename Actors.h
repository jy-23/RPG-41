#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class Actors;
class Combat;

struct Item {
    string name = "";
    int damage = 0;
    int count = 1;
};

class Actors {
    protected:
        int speed = 5;
        int health = 100;
        int attack_value = 5;
        int defense_value = 0;
        bool dead = false;
        string type = "";
        string name = "";
    public:
        Actors() {}
        Actors(int n_speed) : speed(n_speed) {}
        vector<Item>inventory = {Item{"NA"}};
        
        //getters
        int get_speed() const { return speed; }
        int get_health() const { return health; }
        int get_attack() const { return attack_value; }
        int get_defense() const { return defense_value; }
        bool get_dead() const { return dead; }
        string get_type() const { return type; }
        string get_name() const { return name; }
        
        //setters
        void set_speed(int new_speed) { speed = new_speed; }
        void set_health(int new_health) {
            if (new_health < 0) health = 0;
            else health = new_health;
        }
        void set_attack(int new_attack) { attack_value = new_attack; }
        void set_defense(int new_defense) { defense_value = new_defense; }
        
        //other functions
        virtual void mark_dead(Combat &newCombat) { dead = true; }
        virtual int choose_opponent(vector<shared_ptr<Actors>> &vec) { return 0; }
        virtual int make_move(Actors *opponent) { return 0; }
        virtual void print_stats() const {
            cout << "   Health: " << health << "/100" << endl;
            cout << "   Attack Value: " << attack_value << endl;
        }
};


class Combat {
    private:
        int hero_count = 2;
        int monster_count = 2;
    public:
        Combat() = default;
        void dec_heroCount() { --hero_count; }
        void dec_monsterCount() { --monster_count; }
        void set_heroCount(int newCount) { hero_count = newCount; }
        void set_monsterCount(int newCount) { monster_count = newCount; }
        
        //ends combat if true
        bool endCombat() {
            if (hero_count == 0 or monster_count == 0) return true;
            else return false;
        }
  
        //function to print out combat screen
        //parameter Actors* a is attacker/opponent and string f is corresponding color code
                //yellow for attacker, red for opponent
        void combatScreen(vector<shared_ptr<Actors>> &vec, Actors* a, string f) {
            cout <<setfill('+') << setw(40) << "" << endl;
            cout << setfill(' ');
            cout << endl << endl;

            //line up monsters on top of combat screen
            for (size_t i = 0; i < vec.size(); i++) {
                if (vec.at(i)->get_type() == "Monster") {
                    if (vec.at(i)->get_name() == a->get_name()) {
                        if (vec.at(i)->get_dead()) cout << "      " << f+"XXXXX\033[0m";
                        else cout << "      " << f+vec.at(i)->get_name()+"\033[0m";
                    }
                    else {
                        if (vec.at(i)->get_dead()) cout << "      " << "XXXXX";
                        else cout << "       " << vec.at(i)->get_name();
                    }
                }
            }
            cout << endl << endl << endl;

            //line up heroes on bottom of combat screen
            for (size_t i = 0; i < vec.size(); i++) {
                if (vec.at(i)->get_type() == "Hero") {
                    if (vec.at(i)->get_name() == a->get_name()) {
                        if (vec.at(i)->get_dead()) cout << "      " << f+"XXXXX\033[0m";
                        else cout << "      " << f+vec.at(i)->get_name()+"\033[0m";
                    }
                    else {
                        if (vec.at(i)->get_dead()) cout << "      " << "XXXXX";
                        else cout << "       " << vec.at(i)->get_name();
                    }
                }
            }

            cout << endl << endl << endl;
            cout << setfill('+') << setw(40) << "" << endl << endl;
        }
};


class Heroes : public Actors {
    protected:
    public:
        Heroes() {}
        Heroes(int n_speed) : Actors(n_speed) {}

        //mark character as dead and decrease respective team count by 1
        void mark_dead(Combat &newCombat) override {
            Actors::mark_dead(newCombat);
            newCombat.dec_heroCount();
        }
  
        //user chooses weapon to use against opponent
        int make_move(Actors *opponent) override {
            cout << "Choose a weapon to use:" << endl;
            for (size_t i = 0; i < inventory.size(); i++) {
                cout << "(" << i+1 << ") " << inventory.at(i).name << " +" << inventory.at(i).damage << " av" << endl;
            }
            int userChoice = 0;
            while (true) {
                cout << endl;
                cout << "Enter number:" << endl;
                cin >> userChoice;
                cin.ignore();
                for (size_t i = 0; i < inventory.size(); i++) {
                    if (userChoice == i+1) {
                        if (system("clear")) {}
                        //attack loading screen
                        cout << this->get_name() << flush;
                        for (size_t i = 0; i < 3; i++) {
                            sleep(1);
                            cout << "  >  " << flush;
                        }
                        sleep(1);
                        cout << opponent->get_name() << endl;
                        sleep(1);
                        if (system("clear")) {}
                        //summarize attack and calculate damage done to opponent
                        cout << "~ " << name << " has used a " << inventory.at(i).name << " against " << opponent->get_name() << " ~" << endl << endl;
                        sleep(2);
                        int damage_taken = inventory.at(i).damage + attack_value - opponent->get_defense();
                        opponent->set_health(opponent->get_health() - damage_taken);
                        return opponent->get_health();
                    }
                }
            }
        }

    
        //user chooses opponent to attack
        int choose_opponent(vector<shared_ptr<Actors>> &vec) override {
            int userChoice = 0;
            //create vector consisting of only opposing team members
            vector<int>monsters;
            for (size_t i = 0; i < vec.size(); i++) {
                if (vec.at(i)->get_type() == "Monster" and !vec.at(i)->get_dead()) {
                    monsters.push_back(i);
                }
            }
            cout << "Choose a monster to attack" << endl;
            for (size_t i = 0; i < monsters.size(); i++) {
                cout << "(" << i+1 << ") " << vec.at(monsters.at(i))->get_name() << endl;
            }
            cout << endl;
            cout << "Enter number:" << endl;
            while (true) {
                cin >> userChoice;
                for (size_t i = 0; i < monsters.size(); i++) {
                    if (userChoice == i+1) return monsters.at(i);
                }
            }
        }

        //print character stats
        void print_stats() const override {
            cout << "--------------------" << endl;
            Actors::print_stats();
            cout << "   Defense Value: " << defense_value << endl;
            cout << "--------------------" << endl << endl;
        }
};

class Monsters : public Actors {
    protected:
    public:
        Monsters() {}
        Monsters(int n_speed) : Actors(n_speed) {}

        void mark_dead(Combat &newCombat) override {
            Actors::mark_dead(newCombat);
            newCombat.dec_monsterCount();
        }

        //computer randomly chooses which weapon monster uses
        int make_move(Actors *opponent) override {
            int choice = rand() % inventory.size();
            //attack loading screen
            cout << this->get_name() << flush;
            for (size_t i = 0; i < 3; i++) {
                sleep(1);
                cout << "  >  " << flush;
            }
            sleep(1);
            cout << opponent->get_name() << endl;
            sleep(1);
            if (system("clear")) {}
            //summarize attack and calculate damage done to opponent
            cout << "~ " << name << " has used a " << inventory.at(choice).name << " against " << opponent->get_name() << " ~" << endl << endl;
            sleep(2);
            int damage_taken = inventory.at(choice).damage + attack_value - opponent->get_defense();
            opponent->set_health(opponent->get_health() - damage_taken);
            return opponent->get_health();
        }

        //computer randomly chooses which hero monster attacks
        int choose_opponent(vector<shared_ptr<Actors>> &vec) override {
            vector<int>heroes;
            for (size_t i = 0; i < vec.size(); i++) {
                if (vec.at(i)->get_type() == "Hero" and !vec.at(i)->get_dead()) {
                    heroes.push_back(i);
                }
            }
            return heroes.at(rand() % heroes.size());
        }
};

class Warrior : public Heroes {
    private:
    public:
        Warrior() {}
        Warrior(int n_speed) : Heroes(n_speed) {
            attack_value = 12;
            defense_value = 9;
            type = "Hero";
            name = "WARRIOR";
            inventory = { Item{"Sword", 30}, Item{"Dagger", 25}, Item{"Crossbow", 28} };
        }
        void print_stats() const override {
            cout << "WARRIOR" << endl;
            Heroes::print_stats();
        }
};

class Magician : public Heroes {
    private:
    public:
        Magician() {}
        Magician(int n_speed) : Heroes(n_speed) {
            attack_value = 9;
            defense_value = 11;
            type = "Hero";
            name = "MAGICIAN";
            inventory = { Item{"Strength Charm", 29}, Item{"Magic Bomb", 24}, Item{"Scepter", 26} };
        }
        void print_stats() const override {
            cout << "MAGICIAN" << endl;
            Heroes::print_stats();
        }
};

class Goblin : public Monsters {
    private:
    public:
        Goblin() {}
        Goblin(int n_speed) : Monsters(n_speed) {
            attack_value = 10;
            defense_value = 11;
            type = "Monster";
            name = "GOBLIN";
            inventory = { Item{"Spear", 29}, Item{"Pike", 26}, Item{"Stink Bomb", 23} };
        }
        void print_stats() const override {
            cout << "GOBLIN" << endl;
            Monsters::print_stats();
        }
};

class Orc : public Monsters {
    private:
    public:
        Orc() {}
        Orc(int n_speed) : Monsters(n_speed) {
            attack_value = 13;
            defense_value = 5;
            type = "Monster";
            name = "ORC";
            inventory = { Item{"Curved Sword", 31}, Item{"Battle Ax", 28}, Item{"Club", 25} };
        }
        void print_stats() const override {
            cout << "ORC" << endl;
            Monsters::print_stats();
        }
};
